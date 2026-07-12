#include "ExternalModQuestGraphRuntime.h"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace SOH {
namespace {

const ExternalModQuestNode* FindQuestNode(const ExternalModQuestGraphDefinition& graph, const std::string& nodeId) {
    const auto it = std::find_if(graph.nodes.begin(), graph.nodes.end(),
                                 [&nodeId](const ExternalModQuestNode& node) { return node.id == nodeId; });
    return it != graph.nodes.end() ? &(*it) : nullptr;
}

bool TriggerMatchesEvent(const ExternalModQuestTrigger& trigger, const ExternalModQuestEvent& event) {
    if (trigger.event != event.event) {
        return false;
    }
    if (trigger.event == "flagSet") {
        return trigger.hasFlagType && trigger.hasFlagId && trigger.flagType == event.flagType &&
               trigger.flagId == event.flagId;
    }
    if (trigger.event == "sceneEnter") {
        return trigger.hasSceneId && trigger.sceneId == event.sceneId;
    }
    if (trigger.event == "itemReceive") {
        return trigger.hasItemId && trigger.itemId == event.itemId;
    }
    if (trigger.event == "enemyDefeat" || trigger.event == "bossDefeat") {
        return trigger.hasActorId && trigger.actorId == event.actorId;
    }
    return false;
}

void MarkQuestCompleted(ExternalModPackage& package, const ExternalModQuestGraphDefinition& graph,
                        const std::string& finalNodeId,
                        const ExternalModQuestGraphRuntime::QuestStateNotifier& notifyStateChanged) {
    auto& questState = package.runtime.narrativeQuests[graph.id];
    questState.questId = graph.id;
    questState.state = "completed";
    questState.objectives[ExternalModQuestGraphRuntime::kCurrentNodeObjectiveKey] = finalNodeId;
    package.runtime.questCurrentNode.erase(graph.id);
    package.runtime.persistentStateDirty = true;
    if (notifyStateChanged) {
        notifyStateChanged(graph.id);
    }
}

// Enters `node`, runs its onEnter actions, and auto-chains through nodes without advanceWhen
// triggers (following the first `next` edge). Completes the quest when a node has no `next`.
// The graph is validated acyclic at parse time; the visit counter is a second safety net.
void EnterNodeChain(ExternalModPackage& package, const ExternalModQuestGraphDefinition& graph,
                    const ExternalModQuestNode* node,
                    const ExternalModQuestGraphRuntime::QuestActionRunner& runActions,
                    const ExternalModQuestGraphRuntime::QuestStateNotifier& notifyStateChanged) {
    size_t visited = 0;
    while (node != nullptr && visited++ <= graph.nodes.size()) {
        if (!node->onEnter.empty() && runActions) {
            runActions(node->onEnter, graph.id.c_str());
        }

        auto& questState = package.runtime.narrativeQuests[graph.id];
        questState.questId = graph.id;
        questState.state = "active";
        questState.objectives[ExternalModQuestGraphRuntime::kCurrentNodeObjectiveKey] = node->id;
        package.runtime.questCurrentNode[graph.id] = node->id;
        package.runtime.persistentStateDirty = true;
        if (notifyStateChanged) {
            notifyStateChanged(graph.id + ":" + node->id);
        }

        if (node->next.empty()) {
            MarkQuestCompleted(package, graph, node->id, notifyStateChanged);
            return;
        }
        if (!node->advanceWhen.empty()) {
            return; // Wait for a matching gameplay event.
        }
        node = FindQuestNode(graph, node->next.front());
    }
}

} // namespace

void ExternalModQuestGraphRuntime::StartOrResumeQuests(ExternalModPackage& package, const QuestActionRunner& runActions,
                                                       const QuestStateNotifier& notifyStateChanged) const {
    auto& runtime = package.runtime;
    if (!runtime.enabled || runtime.questGraphs.empty()) {
        return;
    }

    for (const auto& graph : runtime.questGraphs) {
        const auto stateIt = runtime.narrativeQuests.find(graph.id);
        if (stateIt != runtime.narrativeQuests.end()) {
            if (stateIt->second.state == "active") {
                // Resume: rebuild the volatile current-node cache from the persisted objective.
                std::string nodeId = graph.startNodeId;
                const auto nodeObjectiveIt = stateIt->second.objectives.find(kCurrentNodeObjectiveKey);
                if (nodeObjectiveIt != stateIt->second.objectives.end() && !nodeObjectiveIt->second.empty() &&
                    FindQuestNode(graph, nodeObjectiveIt->second) != nullptr) {
                    nodeId = nodeObjectiveIt->second;
                } else {
                    SPDLOG_WARN("[ExternalMods] quests.graph.v1 quest={} has no valid persisted node; resuming at start",
                                graph.id);
                }
                runtime.questCurrentNode[graph.id] = nodeId;
            } else {
                runtime.questCurrentNode.erase(graph.id);
            }
            continue;
        }

        // First load for this save: start the quest at its start node.
        const ExternalModQuestNode* startNode = FindQuestNode(graph, graph.startNodeId);
        if (startNode == nullptr) {
            continue; // Unreachable: validated at parse time.
        }
        auto& questState = runtime.narrativeQuests[graph.id];
        questState.questId = graph.id;
        questState.state = "active";
        runtime.persistentStateDirty = true;
        if (notifyStateChanged) {
            notifyStateChanged(graph.id);
        }
        EnterNodeChain(package, graph, startNode, runActions, notifyStateChanged);
    }
}

void ExternalModQuestGraphRuntime::OnEvent(ExternalModPackage& package, const ExternalModQuestEvent& event,
                                           const QuestActionRunner& runActions,
                                           const QuestStateNotifier& notifyStateChanged) const {
    auto& runtime = package.runtime;
    if (!runtime.enabled || runtime.questGraphs.empty()) {
        return;
    }

    for (const auto& graph : runtime.questGraphs) {
        const auto stateIt = runtime.narrativeQuests.find(graph.id);
        if (stateIt == runtime.narrativeQuests.end() || stateIt->second.state != "active") {
            continue;
        }

        // narrativeQuests is the persisted source of truth; questCurrentNode is a volatile cache
        // that may have been dropped by a persistence reload mid-session.
        std::string currentNodeId;
        const auto cacheIt = runtime.questCurrentNode.find(graph.id);
        if (cacheIt != runtime.questCurrentNode.end()) {
            currentNodeId = cacheIt->second;
        } else {
            const auto nodeObjectiveIt = stateIt->second.objectives.find(kCurrentNodeObjectiveKey);
            if (nodeObjectiveIt != stateIt->second.objectives.end()) {
                currentNodeId = nodeObjectiveIt->second;
            }
        }
        if (currentNodeId.empty()) {
            currentNodeId = graph.startNodeId;
        }

        const ExternalModQuestNode* currentNode = FindQuestNode(graph, currentNodeId);
        if (currentNode == nullptr) {
            continue;
        }
        const bool matched =
            std::any_of(currentNode->advanceWhen.begin(), currentNode->advanceWhen.end(),
                        [&event](const ExternalModQuestTrigger& trigger) { return TriggerMatchesEvent(trigger, event); });
        if (!matched) {
            continue;
        }

        if (currentNode->next.empty()) {
            MarkQuestCompleted(package, graph, currentNode->id, notifyStateChanged);
            continue;
        }
        EnterNodeChain(package, graph, FindQuestNode(graph, currentNode->next.front()), runActions, notifyStateChanged);
    }
}

} // namespace SOH
