#pragma once

#include <functional>
#include <memory>
#include <string>

#include "ExternalModNativeApi.h"

namespace SOH {

enum class ExternalModHookType;
struct ExternalModHookEventContext;

struct ExternalModNativeRuntimeConfig {
    std::string modId;
    std::string runtimeType;
    std::string buildId;
    int32_t apiVersion = 4;
    int32_t abiVersion = static_cast<int32_t>(FOUNDRY_NATIVE_ABI_VERSION_V1);
    bool allowRawApi = false;
    void* externalModManager = nullptr;
    std::function<void(int32_t, const std::string&)> log;
    std::function<void(const std::string&)> showNotification;
    std::function<bool(const std::string&)> hasPermission;
    std::function<int32_t()> getCurrentScene;
    std::function<int32_t()> getCurrentRoom;
    std::function<bool(const std::string&, std::string&)> getAssetPath;
    std::function<bool(const std::string&, std::string&, std::string&, uint64_t&)> getAssetInfo;
    std::function<bool(const std::string&, std::string&)> queryPublicJson;
    std::function<bool(const std::string&, const std::string&, const std::string&, std::string&)> callService;
    std::function<bool(const std::string&, std::string&)> invokeActionJson;
    std::function<void*()> getPlayState;
    std::function<void*()> getPlayer;
    std::function<void*()> getSaveContext;
    std::function<void*(const std::string&)> resolveSymbol;
};

class ExternalModNativeRuntime {
  public:
    ExternalModNativeRuntime();
    ~ExternalModNativeRuntime();

    bool Initialize(const std::string& libraryPath, const ExternalModNativeRuntimeConfig& config, std::string& outError);
    bool Start(std::string& outError);
    bool BeginFrame(std::string& outError);
    bool OnHook(ExternalModHookType hookType, const ExternalModHookEventContext& context, std::string& outError);
    void Stop();
    void Shutdown();

    bool IsLoaded() const;
    bool IsStarted() const;
    bool RequiresRawApi() const;
    std::string GetPluginName() const;
    std::string GetPluginVersion() const;
    std::string GetPluginAuthor() const;
    std::string GetPluginBuildId() const;

  private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

} // namespace SOH
