#include "ExternalModNativeRuntime.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <utility>

#include <nlohmann/json.hpp>

#include "ExternalModTypes.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace SOH {

namespace {
constexpr size_t kNativeErrorBufferSize = 1024;

std::string SafeCString(const char* value) {
    return value != nullptr ? std::string(value) : std::string{};
}

int32_t CopyCStringToBuffer(const std::string& value, char* outBuffer, uint32_t outBufferSize) {
    if (outBuffer == nullptr || outBufferSize == 0) {
        return 0;
    }
    if (value.size() + 1 > static_cast<size_t>(outBufferSize)) {
        outBuffer[0] = '\0';
        return 0;
    }

    std::memcpy(outBuffer, value.c_str(), value.size() + 1);
    return 1;
}

int32_t CopyStringPayloadToBuffer(const std::string& value, char* outBuffer, uint32_t outBufferSize) {
    if (outBuffer == nullptr || outBufferSize == 0) {
        return value.empty() ? 0 : -2;
    }
    if (value.size() + 1 > static_cast<size_t>(outBufferSize)) {
        outBuffer[0] = '\0';
        return -2;
    }

    std::memcpy(outBuffer, value.c_str(), value.size() + 1);
    return static_cast<int32_t>(value.size());
}

void CopyErrorBuffer(const std::array<char, kNativeErrorBufferSize>& buffer, std::string& outError, const char* fallback) {
    if (buffer[0] != '\0') {
        outError = buffer.data();
    } else {
        outError = fallback != nullptr ? fallback : "Unknown native runtime error";
    }
}

std::string BuildHookContextJson(const ExternalModHookEventContext& context) {
    nlohmann::json json = nlohmann::json::object();
    json["scene"] = context.scene;
    json["actorId"] = context.actorId;
    json["actorCategory"] = context.actorCategory;
    json["actorHandle"] = context.actorHandle;
    json["itemId"] = context.itemId;
    json["flagType"] = context.flagType;
    json["flagId"] = context.flagId;
    json["healthDelta"] = context.healthDelta;
    json["statusId"] = context.statusId;
    json["stateId"] = context.stateId;
    json["settingsKey"] = context.settingsKey;
    json["settingsDomain"] = context.settingsDomain;
    json["settingsApplyMode"] = context.settingsApplyMode;
    json["resourceId"] = context.resourceId;
    json["value"] = context.value;
    json["resourceCurrentValue"] = context.resourceCurrentValue;
    json["resourceCapacityValue"] = context.resourceCapacityValue;
    json["resourcePercent"] = context.resourcePercent;
    json["stackCount"] = context.stackCount;
    return json.dump();
}

} // namespace

struct ExternalModNativeRuntime::Impl {
    ExternalModNativeRuntimeConfig config;
    FoundryHostApiV1 hostApi{};
    FoundryRawEngineApiV1 rawApi{};
    FoundryNativePluginInfo pluginInfo{};

#ifdef _WIN32
    HMODULE libraryHandle = nullptr;
#endif
    FoundryNativeGetPluginInfoFn getPluginInfoFn = nullptr;
    FoundryNativeLoadFn loadFn = nullptr;
    FoundryNativeStartFn startFn = nullptr;
    FoundryNativeStopFn stopFn = nullptr;
    FoundryNativeUnloadFn unloadFn = nullptr;
    FoundryNativeBeginFrameFn beginFrameFn = nullptr;
    FoundryNativeOnHookFn onHookFn = nullptr;
    void* pluginState = nullptr;
    bool loaded = false;
    bool started = false;
    bool requiresRawApi = false;
    std::string libraryPath;
    std::string pluginName;
    std::string pluginVersion;
    std::string pluginAuthor;
    std::string pluginBuildId;

    static Impl* FromUserData(void* userData) {
        return static_cast<Impl*>(userData);
    }

    static void HostLog(void* userData, int32_t level, const char* message) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.log) {
            return;
        }
        self->config.log(level, SafeCString(message));
    }

    static int32_t HostShowNotification(void* userData, const char* message) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.showNotification) {
            return 0;
        }
        self->config.showNotification(SafeCString(message));
        return 1;
    }

    static int32_t HostHasPermission(void* userData, const char* permission) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.hasPermission || permission == nullptr) {
            return 0;
        }
        return self->config.hasPermission(permission) ? 1 : 0;
    }

    static int32_t HostGetCurrentScene(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getCurrentScene) {
            return -1;
        }
        return self->config.getCurrentScene();
    }

    static int32_t HostGetCurrentRoom(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getCurrentRoom) {
            return -1;
        }
        return self->config.getCurrentRoom();
    }

    static int32_t HostGetAssetPath(void* userData, const char* assetId, char* outPath, uint32_t outPathSize) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getAssetPath || assetId == nullptr) {
            return 0;
        }

        std::string resolvedPath;
        if (!self->config.getAssetPath(assetId, resolvedPath)) {
            return 0;
        }
        return CopyCStringToBuffer(resolvedPath, outPath, outPathSize);
    }

    static int32_t HostGetAssetInfo(void* userData, const char* assetId, char* outKind, uint32_t outKindSize,
                                    char* outFormat, uint32_t outFormatSize, uint64_t* outSizeBytes) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getAssetInfo || assetId == nullptr) {
            return 0;
        }

        std::string kind;
        std::string format;
        uint64_t sizeBytes = 0;
        if (!self->config.getAssetInfo(assetId, kind, format, sizeBytes)) {
            return 0;
        }
        if (CopyCStringToBuffer(kind, outKind, outKindSize) == 0 ||
            CopyCStringToBuffer(format, outFormat, outFormatSize) == 0) {
            return 0;
        }
        if (outSizeBytes != nullptr) {
            *outSizeBytes = sizeBytes;
        }
        return 1;
    }

    static int32_t HostQueryPublicJson(void* userData, const char* requestJson, char* outJson, uint32_t outJsonSize) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.queryPublicJson || requestJson == nullptr) {
            return -1;
        }

        std::string responseJson;
        if (!self->config.queryPublicJson(requestJson, responseJson)) {
            return -1;
        }
        return CopyStringPayloadToBuffer(responseJson, outJson, outJsonSize);
    }

    static int32_t HostCallService(void* userData, const char* targetModId, const char* serviceId,
                                   const char* requestJson, char* outJson, uint32_t outJsonSize) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.callService || targetModId == nullptr || serviceId == nullptr ||
            requestJson == nullptr) {
            return -1;
        }

        std::string responseJson;
        if (!self->config.callService(targetModId, serviceId, requestJson, responseJson)) {
            return -1;
        }
        return CopyStringPayloadToBuffer(responseJson, outJson, outJsonSize);
    }

    static int32_t HostInvokeActionJson(void* userData, const char* actionJson, char* outJson, uint32_t outJsonSize) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.invokeActionJson || actionJson == nullptr) {
            return -1;
        }

        std::string responseJson;
        if (!self->config.invokeActionJson(actionJson, responseJson)) {
            return -1;
        }
        return CopyStringPayloadToBuffer(responseJson, outJson, outJsonSize);
    }

    static const char* HostGetBuildId(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr) {
            return "";
        }
        return self->config.buildId.c_str();
    }

    static void* RawGetPlayState(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getPlayState) {
            return nullptr;
        }
        return self->config.getPlayState();
    }

    static void* RawGetPlayer(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getPlayer) {
            return nullptr;
        }
        return self->config.getPlayer();
    }

    static void* RawGetSaveContext(void* userData) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.getSaveContext) {
            return nullptr;
        }
        return self->config.getSaveContext();
    }

    static void* RawResolveSymbol(void* userData, const char* symbolName) {
        Impl* self = FromUserData(userData);
        if (self == nullptr || !self->config.resolveSymbol || symbolName == nullptr) {
            return nullptr;
        }
        return self->config.resolveSymbol(symbolName);
    }
};

ExternalModNativeRuntime::ExternalModNativeRuntime() : mImpl(std::make_unique<Impl>()) {
}

ExternalModNativeRuntime::~ExternalModNativeRuntime() {
    Shutdown();
}

bool ExternalModNativeRuntime::Initialize(const std::string& libraryPath, const ExternalModNativeRuntimeConfig& config,
                                          std::string& outError) {
    outError.clear();
    Shutdown();
    mImpl->config = config;
    mImpl->libraryPath = libraryPath;

    mImpl->hostApi.structSize = sizeof(FoundryHostApiV1);
    mImpl->hostApi.abiVersion = FOUNDRY_NATIVE_ABI_VERSION_V1;
    mImpl->hostApi.modId = mImpl->config.modId.c_str();
    mImpl->hostApi.runtimeType = mImpl->config.runtimeType.c_str();
    mImpl->hostApi.userData = mImpl.get();
    mImpl->hostApi.log = &Impl::HostLog;
    mImpl->hostApi.showNotification = &Impl::HostShowNotification;
    mImpl->hostApi.hasPermission = &Impl::HostHasPermission;
    mImpl->hostApi.getCurrentScene = &Impl::HostGetCurrentScene;
    mImpl->hostApi.getCurrentRoom = &Impl::HostGetCurrentRoom;
    mImpl->hostApi.getBuildId = &Impl::HostGetBuildId;
    mImpl->hostApi.getAssetPath = &Impl::HostGetAssetPath;
    mImpl->hostApi.getAssetInfo = &Impl::HostGetAssetInfo;
    mImpl->hostApi.queryPublicJson = &Impl::HostQueryPublicJson;
    mImpl->hostApi.callService = &Impl::HostCallService;
    mImpl->hostApi.invokeActionJson = &Impl::HostInvokeActionJson;

    mImpl->rawApi.structSize = sizeof(FoundryRawEngineApiV1);
    mImpl->rawApi.abiVersion = FOUNDRY_NATIVE_ABI_VERSION_V1;
    mImpl->rawApi.userData = mImpl.get();
    mImpl->rawApi.externalModManager = mImpl->config.externalModManager;
    mImpl->rawApi.getPlayState = &Impl::RawGetPlayState;
    mImpl->rawApi.getPlayer = &Impl::RawGetPlayer;
    mImpl->rawApi.getSaveContext = &Impl::RawGetSaveContext;
    mImpl->rawApi.resolveSymbol = &Impl::RawResolveSymbol;

#ifndef _WIN32
    outError = "native-cpp-v1 is currently supported only on Windows builds";
    return false;
#else
    const std::wstring widePath(libraryPath.begin(), libraryPath.end());
    mImpl->libraryHandle = ::LoadLibraryW(widePath.c_str());
    if (mImpl->libraryHandle == nullptr) {
        outError = "LoadLibraryW failed for native runtime library";
        return false;
    }

    auto resolveProc = [&](const char* exportName) -> FARPROC {
        return ::GetProcAddress(mImpl->libraryHandle, exportName);
    };

    mImpl->getPluginInfoFn = reinterpret_cast<FoundryNativeGetPluginInfoFn>(resolveProc("FoundryNative_GetPluginInfo"));
    mImpl->loadFn = reinterpret_cast<FoundryNativeLoadFn>(resolveProc("FoundryNative_Load"));
    mImpl->startFn = reinterpret_cast<FoundryNativeStartFn>(resolveProc("FoundryNative_Start"));
    mImpl->stopFn = reinterpret_cast<FoundryNativeStopFn>(resolveProc("FoundryNative_Stop"));
    mImpl->unloadFn = reinterpret_cast<FoundryNativeUnloadFn>(resolveProc("FoundryNative_Unload"));
    mImpl->beginFrameFn = reinterpret_cast<FoundryNativeBeginFrameFn>(resolveProc("FoundryNative_BeginFrame"));
    mImpl->onHookFn = reinterpret_cast<FoundryNativeOnHookFn>(resolveProc("FoundryNative_OnHook"));

    if (mImpl->getPluginInfoFn == nullptr || mImpl->loadFn == nullptr || mImpl->startFn == nullptr ||
        mImpl->stopFn == nullptr || mImpl->unloadFn == nullptr) {
        outError = "native runtime library is missing one or more required exports";
        Shutdown();
        return false;
    }

    mImpl->pluginInfo = {};
    mImpl->pluginInfo.structSize = sizeof(FoundryNativePluginInfo);
    if (mImpl->getPluginInfoFn(&mImpl->pluginInfo) == 0) {
        outError = "FoundryNative_GetPluginInfo failed";
        Shutdown();
        return false;
    }
    if (mImpl->pluginInfo.abiVersion != static_cast<uint32_t>(mImpl->config.abiVersion)) {
        outError = "native runtime ABI mismatch";
        Shutdown();
        return false;
    }
    if (mImpl->pluginInfo.apiVersion != static_cast<uint32_t>(mImpl->config.apiVersion)) {
        outError = "native runtime apiVersion mismatch";
        Shutdown();
        return false;
    }

    mImpl->requiresRawApi = (mImpl->pluginInfo.flags & FOUNDRY_NATIVE_FLAG_REQUIRES_RAW) != 0u;
    if (mImpl->requiresRawApi && !mImpl->config.allowRawApi) {
        outError = "native runtime requested raw engine access but native raw permission is not granted";
        Shutdown();
        return false;
    }

    mImpl->pluginName = SafeCString(mImpl->pluginInfo.pluginName);
    mImpl->pluginVersion = SafeCString(mImpl->pluginInfo.pluginVersion);
    mImpl->pluginAuthor = SafeCString(mImpl->pluginInfo.pluginAuthor);
    mImpl->pluginBuildId = SafeCString(mImpl->pluginInfo.buildId);

    std::array<char, kNativeErrorBufferSize> errorBuffer{};
    const FoundryRawEngineApiV1* rawApiPtr = mImpl->config.allowRawApi ? &mImpl->rawApi : nullptr;
    if (mImpl->loadFn(&mImpl->hostApi, rawApiPtr, &mImpl->pluginState, errorBuffer.data(),
                      static_cast<uint32_t>(errorBuffer.size())) == 0) {
        CopyErrorBuffer(errorBuffer, outError, "FoundryNative_Load failed");
        Shutdown();
        return false;
    }

    mImpl->loaded = true;
    return true;
#endif
}

bool ExternalModNativeRuntime::Start(std::string& outError) {
    outError.clear();
    if (mImpl == nullptr || !mImpl->loaded || mImpl->startFn == nullptr) {
        outError = "native runtime is not loaded";
        return false;
    }
    if (mImpl->started) {
        return true;
    }

    std::array<char, kNativeErrorBufferSize> errorBuffer{};
    if (mImpl->startFn(mImpl->pluginState, errorBuffer.data(), static_cast<uint32_t>(errorBuffer.size())) == 0) {
        CopyErrorBuffer(errorBuffer, outError, "FoundryNative_Start failed");
        return false;
    }
    mImpl->started = true;
    return true;
}

bool ExternalModNativeRuntime::BeginFrame(std::string& outError) {
    outError.clear();
    if (mImpl == nullptr || !mImpl->loaded || !mImpl->started || mImpl->beginFrameFn == nullptr) {
        return true;
    }

    std::array<char, kNativeErrorBufferSize> errorBuffer{};
    if (mImpl->beginFrameFn(mImpl->pluginState, errorBuffer.data(), static_cast<uint32_t>(errorBuffer.size())) == 0) {
        CopyErrorBuffer(errorBuffer, outError, "FoundryNative_BeginFrame failed");
        return false;
    }
    return true;
}

bool ExternalModNativeRuntime::OnHook(ExternalModHookType hookType, const ExternalModHookEventContext& context,
                                      std::string& outError) {
    outError.clear();
    if (mImpl == nullptr || !mImpl->loaded || !mImpl->started || mImpl->onHookFn == nullptr) {
        return true;
    }

    const std::string hookContextJson = BuildHookContextJson(context);
    std::array<char, kNativeErrorBufferSize> errorBuffer{};
    if (mImpl->onHookFn(mImpl->pluginState, static_cast<int32_t>(hookType), hookContextJson.c_str(), errorBuffer.data(),
                        static_cast<uint32_t>(errorBuffer.size())) == 0) {
        CopyErrorBuffer(errorBuffer, outError, "FoundryNative_OnHook failed");
        return false;
    }
    return true;
}

void ExternalModNativeRuntime::Stop() {
    if (mImpl == nullptr || !mImpl->loaded || !mImpl->started) {
        return;
    }
    if (mImpl->stopFn != nullptr) {
        mImpl->stopFn(mImpl->pluginState);
    }
    mImpl->started = false;
}

void ExternalModNativeRuntime::Shutdown() {
    if (mImpl == nullptr) {
        return;
    }
    Stop();
    if (mImpl->loaded && mImpl->unloadFn != nullptr) {
        mImpl->unloadFn(mImpl->pluginState);
    }
    mImpl->pluginState = nullptr;
    mImpl->loaded = false;
    mImpl->requiresRawApi = false;
    mImpl->pluginName.clear();
    mImpl->pluginVersion.clear();
    mImpl->pluginAuthor.clear();
    mImpl->pluginBuildId.clear();

#ifdef _WIN32
    if (mImpl->libraryHandle != nullptr) {
        ::FreeLibrary(mImpl->libraryHandle);
        mImpl->libraryHandle = nullptr;
    }
#endif
}

bool ExternalModNativeRuntime::IsLoaded() const {
    return mImpl != nullptr && mImpl->loaded;
}

bool ExternalModNativeRuntime::IsStarted() const {
    return mImpl != nullptr && mImpl->started;
}

bool ExternalModNativeRuntime::RequiresRawApi() const {
    return mImpl != nullptr && mImpl->requiresRawApi;
}

std::string ExternalModNativeRuntime::GetPluginName() const {
    return mImpl != nullptr ? mImpl->pluginName : std::string{};
}

std::string ExternalModNativeRuntime::GetPluginVersion() const {
    return mImpl != nullptr ? mImpl->pluginVersion : std::string{};
}

std::string ExternalModNativeRuntime::GetPluginAuthor() const {
    return mImpl != nullptr ? mImpl->pluginAuthor : std::string{};
}

std::string ExternalModNativeRuntime::GetPluginBuildId() const {
    return mImpl != nullptr ? mImpl->pluginBuildId : std::string{};
}

} // namespace SOH
