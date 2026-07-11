#pragma once

#include <functional>
#include <string>

#include "ExternalModTypes.h"

namespace SOH {

class ExternalModEffectRuntime {
  public:
    using EffectDispatcher = std::function<bool(const ExternalModEffectGraphDefinition&,
                                                const ExternalModEffectGraphNode&, std::string&)>;

    bool DispatchHit(ExternalModPackage& package, const std::string& trigger, const std::string& itemId,
                     const EffectDispatcher& dispatch, bool& outHandled, std::string& outError) const;

  private:
    static constexpr size_t kMaxExecutedNodes = 64;
    bool ExecuteGraph(const ExternalModEffectGraphDefinition& graph, const EffectDispatcher& dispatch,
                      std::string& outError) const;
};

} // namespace SOH
