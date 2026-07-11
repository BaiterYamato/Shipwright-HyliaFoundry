#pragma once

#include <stdint.h>

#define FOUNDRY_NATIVE_ABI_VERSION_V1 1u
#define FOUNDRY_NATIVE_FLAG_REQUIRES_RAW 0x1u

typedef enum FoundryHostLogLevel {
    FOUNDRY_HOST_LOG_TRACE = 0,
    FOUNDRY_HOST_LOG_INFO = 1,
    FOUNDRY_HOST_LOG_WARN = 2,
    FOUNDRY_HOST_LOG_ERROR = 3,
} FoundryHostLogLevel;

typedef void (*FoundryHostLogFn)(void* userData, int32_t level, const char* message);
typedef int32_t (*FoundryHostShowNotificationFn)(void* userData, const char* message);
typedef int32_t (*FoundryHostHasPermissionFn)(void* userData, const char* permission);
typedef int32_t (*FoundryHostGetCurrentSceneFn)(void* userData);
typedef int32_t (*FoundryHostGetCurrentRoomFn)(void* userData);
typedef const char* (*FoundryHostGetBuildIdFn)(void* userData);

typedef void* (*FoundryRawGetPlayStateFn)(void* userData);
typedef void* (*FoundryRawGetPlayerFn)(void* userData);
typedef void* (*FoundryRawGetSaveContextFn)(void* userData);
typedef void* (*FoundryRawResolveSymbolFn)(void* userData, const char* symbolName);

typedef struct FoundryHostApiV1 {
    uint32_t structSize;
    uint32_t abiVersion;
    const char* modId;
    const char* runtimeType;
    void* userData;
    FoundryHostLogFn log;
    FoundryHostShowNotificationFn showNotification;
    FoundryHostHasPermissionFn hasPermission;
    FoundryHostGetCurrentSceneFn getCurrentScene;
    FoundryHostGetCurrentRoomFn getCurrentRoom;
    FoundryHostGetBuildIdFn getBuildId;
} FoundryHostApiV1;

typedef struct FoundryRawEngineApiV1 {
    uint32_t structSize;
    uint32_t abiVersion;
    void* userData;
    void* externalModManager;
    FoundryRawGetPlayStateFn getPlayState;
    FoundryRawGetPlayerFn getPlayer;
    FoundryRawGetSaveContextFn getSaveContext;
    FoundryRawResolveSymbolFn resolveSymbol;
} FoundryRawEngineApiV1;

typedef struct FoundryNativePluginInfo {
    uint32_t structSize;
    uint32_t abiVersion;
    uint32_t apiVersion;
    uint32_t flags;
    const char* pluginName;
    const char* pluginVersion;
    const char* pluginAuthor;
    const char* buildId;
} FoundryNativePluginInfo;

typedef int32_t (*FoundryNativeGetPluginInfoFn)(FoundryNativePluginInfo* outInfo);
typedef int32_t (*FoundryNativeLoadFn)(const FoundryHostApiV1* hostApi, const FoundryRawEngineApiV1* rawApi,
                                       void** outPluginState, char* errorBuffer, uint32_t errorBufferSize);
typedef int32_t (*FoundryNativeStartFn)(void* pluginState, char* errorBuffer, uint32_t errorBufferSize);
typedef void (*FoundryNativeStopFn)(void* pluginState);
typedef void (*FoundryNativeUnloadFn)(void* pluginState);
typedef int32_t (*FoundryNativeBeginFrameFn)(void* pluginState, char* errorBuffer, uint32_t errorBufferSize);
typedef int32_t (*FoundryNativeOnHookFn)(void* pluginState, int32_t hookType, const char* contextJson,
                                         char* errorBuffer, uint32_t errorBufferSize);

