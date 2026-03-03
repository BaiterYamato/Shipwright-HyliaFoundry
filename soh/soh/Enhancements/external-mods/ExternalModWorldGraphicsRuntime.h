#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ExternalModTypes.h"

struct PlayState;

namespace SOH {

class ExternalModManager;

class ExternalModWorldGraphicsRuntime {
  public:
    ExternalModWorldGraphicsRuntime();
    ~ExternalModWorldGraphicsRuntime();

    void OnPlayDrawBegin(ExternalModManager& manager, std::vector<ExternalModPackage>& packages, ::PlayState* play);
    void OnPlayDrawEnd(::PlayState* play);
    void Reset(::PlayState* play);
    std::string BuildInspectorSummary() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

} // namespace SOH
