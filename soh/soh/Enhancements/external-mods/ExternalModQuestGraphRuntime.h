#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "ExternalModTypes.h"

namespace SOH {

// One gameplay event fed by the manager into the quests.graph.v1 runtime. `event` matches the
// advanceWhen trigger kinds: "flagSet", "sceneEnter", "itemReceive", "enemyDefeat", "bossDefeat".
struct ExternalModQuestEvent {
    std::string event;
    int32_t flagType = -1;
    int32_t flagId = -1;
    int32_t sceneId = -1;
    int32_t itemId = -1;
    int32_t actorId = -1;
};

class ExternalModQuestGraphRuntime {
  public:
    using QuestActionRunner = std::function<void(const std::vector<ExternalModAction>& actions, const char* contextLabel)>;
    using QuestStateNotifier = std::function<void(const std::string& value)>;

    // Objective key inside ExternalModRuntime::narrativeQuests used to mirror the current graph
    // node so the existing narrative persistence (quest:<id>:objective:__node) round-trips it.
    static constexpr const char* kCurrentNodeObjectiveKey = "__node";

    // Starts every quests.graph.v1 quest that has no entry in package.runtime.narrativeQuests
    // (enters the startNode, runs its onEnter actions via runActions, marks the quest "active" and
    // notifies OnQuestStateChanged) and resumes already-active quests by rebuilding
    // package.runtime.questCurrentNode from the persisted "__node" objective. Completed quests are
    // left untouched. Sets persistentStateDirty whenever narrativeQuests changes.
    void StartOrResumeQuests(ExternalModPackage& package, const QuestActionRunner& runActions,
                             const QuestStateNotifier& notifyStateChanged) const;

    // Feeds one gameplay event to every active quests.graph.v1 quest. When any advanceWhen trigger
    // of the quest's current node matches, the quest advances along the first `next` edge (running
    // the entered nodes' onEnter actions and auto-chaining through nodes without triggers). A node
    // with no `next` marks the quest "completed". Mirrors state into narrativeQuests +
    // questCurrentNode, sets persistentStateDirty, and notifies OnQuestStateChanged on every
    // advance/completion.
    void OnEvent(ExternalModPackage& package, const ExternalModQuestEvent& event, const QuestActionRunner& runActions,
                 const QuestStateNotifier& notifyStateChanged) const;
};

} // namespace SOH
