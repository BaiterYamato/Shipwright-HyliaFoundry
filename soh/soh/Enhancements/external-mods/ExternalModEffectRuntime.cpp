#include "ExternalModEffectRuntime.h"

#include <algorithm>
#include <unordered_set>
#include <vector>

namespace SOH {

bool ExternalModEffectRuntime::ExecuteGraph(const ExternalModEffectGraphDefinition& graph,
                                            const EffectDispatcher& dispatch, std::string& outError) const {
    std::vector<std::string> pending = { graph.entryNodeId };
    std::unordered_set<std::string> visited;

    while (!pending.empty()) {
        const std::string nodeId = pending.back();
        pending.pop_back();
        if (!visited.insert(nodeId).second) {
            continue;
        }
        if (visited.size() > kMaxExecutedNodes) {
            outError = "effect graph exceeded the 64-node execution budget: " + graph.id;
            return false;
        }

        const auto nodeIt = std::find_if(graph.nodes.begin(), graph.nodes.end(),
                                         [&nodeId](const auto& node) { return node.id == nodeId; });
        if (nodeIt == graph.nodes.end()) {
            outError = "effect graph references unknown node '" + nodeId + "': " + graph.id;
            return false;
        }
        if (!dispatch(graph, *nodeIt, outError)) {
            return false;
        }

        for (auto nextIt = nodeIt->next.rbegin(); nextIt != nodeIt->next.rend(); ++nextIt) {
            pending.push_back(*nextIt);
        }
    }
    return true;
}

bool ExternalModEffectRuntime::DispatchHit(ExternalModPackage& package, const std::string& trigger,
                                           const std::string& itemId, const EffectDispatcher& dispatch,
                                           bool& outHandled, std::string& outError) const {
    outHandled = false;
    std::vector<const ExternalModCombatHitRuleDefinition*> rules;
    for (const auto& rule : package.runtime.combatHitRuleDefinitions) {
        if (rule.trigger == trigger && (rule.itemId.empty() || rule.itemId == itemId)) {
            rules.push_back(&rule);
        }
    }
    std::sort(rules.begin(), rules.end(), [](const auto* lhs, const auto* rhs) {
        if (lhs->priority != rhs->priority) {
            return lhs->priority > rhs->priority;
        }
        return lhs->id < rhs->id;
    });

    for (const auto* rule : rules) {
        const auto graphIt = std::find_if(package.runtime.effectGraphDefinitions.begin(),
                                          package.runtime.effectGraphDefinitions.end(),
                                          [&](const auto& graph) { return graph.id == rule->graphId; });
        if (graphIt == package.runtime.effectGraphDefinitions.end()) {
            outError = "hit rule references unknown effect graph '" + rule->graphId + "': " + rule->id;
            return false;
        }
        if (!ExecuteGraph(*graphIt, dispatch, outError)) {
            return false;
        }
        outHandled = true;
        if (rule->stopPropagation) {
            break;
        }
    }
    return true;
}

} // namespace SOH
