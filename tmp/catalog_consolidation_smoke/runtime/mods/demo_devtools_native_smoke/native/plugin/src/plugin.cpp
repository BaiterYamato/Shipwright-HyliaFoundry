#include "ExternalModNativeApi.h"

#include <array>
#include <cstring>
#include <fstream>
#include <string>

namespace {

struct SamplePluginState {
    const FoundryHostApiV1* host = nullptr;
    const FoundryRawEngineApiV1* raw = nullptr;
    int32_t frames = 0;
};

constexpr const char* kSampleAssetId = "com.sylian.demo.devtools_native_smoke:sample_payload";

void CopyError(char* errorBuffer, uint32_t errorBufferSize, const char* message) {
    if (errorBuffer == nullptr || errorBufferSize == 0) {
        return;
    }
    errorBuffer[0] = '\0';
    if (message == nullptr) {
        return;
    }
    std::strncpy(errorBuffer, message, static_cast<size_t>(errorBufferSize - 1));
    errorBuffer[errorBufferSize - 1] = '\0';
}

void HostLog(const FoundryHostApiV1* host, int32_t level, const char* message) {
    if (host != nullptr && host->log != nullptr) {
        host->log(host->userData, level, message);
    }
}

} // namespace

#if defined(_WIN32)
#define FOUNDRY_EXPORT extern "C" __declspec(dllexport)
#else
#define FOUNDRY_EXPORT extern "C"
#endif

FOUNDRY_EXPORT int32_t FoundryNative_GetPluginInfo(FoundryNativePluginInfo* outInfo) {
    if (outInfo == nullptr) {
        return 0;
    }
    outInfo->structSize = sizeof(FoundryNativePluginInfo);
    outInfo->abiVersion = FOUNDRY_NATIVE_ABI_VERSION_V1;
    outInfo->apiVersion = 4;
    outInfo->flags = 0;
    outInfo->pluginName = "Demo Devtools Native Smoke Native Sample";
    outInfo->pluginVersion = "0.1.0";
    outInfo->pluginAuthor = "Shipwright";
    outInfo->buildId = "";
    return 1;
}

FOUNDRY_EXPORT int32_t FoundryNative_Load(const FoundryHostApiV1* hostApi, const FoundryRawEngineApiV1* rawApi,
                                          void** outPluginState, char* errorBuffer, uint32_t errorBufferSize) {
    if (hostApi == nullptr || outPluginState == nullptr) {
        CopyError(errorBuffer, errorBufferSize, "hostApi/outPluginState is null");
        return 0;
    }

    auto* state = new SamplePluginState();
    state->host = hostApi;
    state->raw = rawApi;
    *outPluginState = state;

    HostLog(hostApi, FOUNDRY_HOST_LOG_INFO, "Native sample plugin loaded.");
    return 1;
}

FOUNDRY_EXPORT int32_t FoundryNative_Start(void* pluginState, char* errorBuffer, uint32_t errorBufferSize) {
    auto* state = static_cast<SamplePluginState*>(pluginState);
    if (state == nullptr || state->host == nullptr) {
        CopyError(errorBuffer, errorBufferSize, "pluginState is null");
        return 0;
    }

    HostLog(state->host, FOUNDRY_HOST_LOG_INFO, "Native sample plugin started.");
    if (state->host->getAssetInfo != nullptr) {
        std::array<char, 64> kind{};
        std::array<char, 64> format{};
        uint64_t sizeBytes = 0;
        if (state->host->getAssetInfo(state->host->userData, kSampleAssetId, kind.data(),
                                      static_cast<uint32_t>(kind.size()), format.data(),
                                      static_cast<uint32_t>(format.size()), &sizeBytes) != 0) {
            const std::string message = std::string("Sample asset info: id=") + kSampleAssetId + " kind=" + kind.data() +
                                        " format=" + format.data() + " bytes=" + std::to_string(sizeBytes);
            HostLog(state->host, FOUNDRY_HOST_LOG_INFO, message.c_str());
        }
    }
    if (state->host->getAssetPath != nullptr) {
        std::array<char, 1024> assetPath{};
        if (state->host->getAssetPath(state->host->userData, kSampleAssetId, assetPath.data(),
                                      static_cast<uint32_t>(assetPath.size())) != 0) {
            std::ifstream input(assetPath.data(), std::ios::binary);
            if (input.is_open()) {
                std::string firstLine;
                std::getline(input, firstLine);
                const std::string message =
                    std::string("Sample asset path resolved: ") + assetPath.data() + " firstLine=" + firstLine;
                HostLog(state->host, FOUNDRY_HOST_LOG_INFO, message.c_str());
            }
        }
    }
    if (state->host->showNotification != nullptr) {
        state->host->showNotification(state->host->userData, "Native SDK sample is active.");
    }
    return 1;
}

FOUNDRY_EXPORT void FoundryNative_Stop(void* pluginState) {
    auto* state = static_cast<SamplePluginState*>(pluginState);
    if (state != nullptr && state->host != nullptr) {
        HostLog(state->host, FOUNDRY_HOST_LOG_INFO, "Native sample plugin stopped.");
    }
}

FOUNDRY_EXPORT void FoundryNative_Unload(void* pluginState) {
    auto* state = static_cast<SamplePluginState*>(pluginState);
    delete state;
}

FOUNDRY_EXPORT int32_t FoundryNative_BeginFrame(void* pluginState, char* errorBuffer, uint32_t errorBufferSize) {
    auto* state = static_cast<SamplePluginState*>(pluginState);
    if (state == nullptr || state->host == nullptr) {
        CopyError(errorBuffer, errorBufferSize, "pluginState is null");
        return 0;
    }

    state->frames++;
    if (state->frames == 1) {
        const int32_t scene = state->host->getCurrentScene != nullptr ? state->host->getCurrentScene(state->host->userData) : -1;
        const std::string message = "Native sample first frame in scene " + std::to_string(scene) + ".";
        HostLog(state->host, FOUNDRY_HOST_LOG_INFO, message.c_str());
    }
    return 1;
}

FOUNDRY_EXPORT int32_t FoundryNative_OnHook(void* pluginState, int32_t hookType, const char* contextJson,
                                            char* errorBuffer, uint32_t errorBufferSize) {
    auto* state = static_cast<SamplePluginState*>(pluginState);
    if (state == nullptr || state->host == nullptr) {
        CopyError(errorBuffer, errorBufferSize, "pluginState is null");
        return 0;
    }

    (void)contextJson;
    if (hookType == 0) {
        HostLog(state->host, FOUNDRY_HOST_LOG_TRACE, "Received OnLoadGame hook.");
    }
    return 1;
}

