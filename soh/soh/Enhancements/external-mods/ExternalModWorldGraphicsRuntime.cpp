#include "ExternalModWorldGraphicsRuntime.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

#include "ExternalModManager.h"
#include "fast/backends/gfx_rendering_api.h"
#include "libultraship/bridge/consolevariablebridge.h"
#include "ship/Context.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
void gfx_set_force_depth_aware_fog(uint8_t enabled);
void gfx_set_ambient_occlusion_enabled(uint8_t enabled);
void gfx_set_ambient_occlusion_debug_view(uint8_t enabled);
void gfx_set_ambient_occlusion_config(float radius, float intensity, float bias, float power, float maxDistance, int32_t blurPasses,
                                      uint8_t quality);
int32_t gfx_get_ambient_occlusion_fallback_reason();
void gfx_clear_ambient_occlusion_fallback_reason();
void gfx_set_volumetrics_enabled(uint8_t enabled);
void gfx_set_volumetrics_debug_view(uint8_t enabled);
void gfx_set_volumetrics_debug_mode(uint8_t mode);
void gfx_set_volumetrics_appearance(float colorR, float colorG, float colorB, float ambientIntensity);
void gfx_set_volumetrics_config(float density, float anisotropy, float startDistance, float maxDistance, uint8_t quality,
                                uint8_t shadowQuality, float baseHeight, float heightFalloff, uint8_t performanceMode,
                                uint8_t heightFogEnabled, float lightShaftIntensity, float shadowIntensity, float temporalBlend, float jitterScale,
                                float aerialPerspectiveStrength, float macroNoiseStrength, float valleyFogStrength,
                                float edgeHazeStrength, float depthExtinctionStrength, float depthExtinctionExponent,
                                float horizonFogStrength, float skyFallbackStrength, float directionalShadowBias,
                                float directionalShadowNormalBias, float directionalShadowSoftness,
                                uint8_t maxShadowedLights, uint8_t resetHistory);
void gfx_set_volumetrics_camera(const float* currentViewProjection, const float* inverseViewProjection,
                                const float* previousViewProjection, float cameraX, float cameraY, float cameraZ,
                                float nearPlane, float farPlane);
void gfx_set_volumetric_lights(const Fast::GfxVolumetricLight* lights, size_t count);
int32_t gfx_get_volumetrics_fallback_reason();
void gfx_clear_volumetrics_fallback_reason();
void gfx_get_volumetrics_runtime_stats(Fast::GfxVolumetricsRuntimeStats* outStats);
const char* gfx_get_current_backend_name();
}

namespace SOH {

namespace {

constexpr float kPi = 3.14159265358979323846f;

template <typename T>
constexpr T ClampValue(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

bool IsHigherPriorityPackage(const ExternalModPackage* lhs, const ExternalModPackage* rhs) {
    if (lhs == nullptr) {
        return false;
    }
    if (rhs == nullptr) {
        return true;
    }
    if (lhs->manifest.loadPriority != rhs->manifest.loadPriority) {
        return lhs->manifest.loadPriority > rhs->manifest.loadPriority;
    }
    return lhs->manifest.id < rhs->manifest.id;
}

template <typename TDefinition, typename TCollectionSelector>
const TDefinition* FindDefinitionAcrossPackages(const std::vector<ExternalModPackage>& packages, const std::string& id,
                                                TCollectionSelector&& collectionSelector,
                                                const ExternalModPackage** outOwnerPackage = nullptr) {
    const TDefinition* bestDefinition = nullptr;
    const ExternalModPackage* bestPackage = nullptr;

    if (id.empty()) {
        if (outOwnerPackage != nullptr) {
            *outOwnerPackage = nullptr;
        }
        return nullptr;
    }

    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }

        const auto& collection = collectionSelector(package.runtime);
        const auto definitionIt = std::find_if(collection.begin(), collection.end(),
                                               [&id](const TDefinition& definition) { return definition.id == id; });
        if (definitionIt == collection.end()) {
            continue;
        }

        if (!IsHigherPriorityPackage(&package, bestPackage)) {
            continue;
        }

        bestPackage = &package;
        bestDefinition = &(*definitionIt);
    }

    if (outOwnerPackage != nullptr) {
        *outOwnerPackage = bestPackage;
    }
    return bestDefinition;
}

std::optional<int32_t> TryGetIntBlackboardValue(const ExternalModRuntime& runtime, const std::string& key) {
    const auto it = runtime.globalBlackboard.find(key);
    if (it == runtime.globalBlackboard.end()) {
        return std::nullopt;
    }
    try {
        return std::stoi(it->second);
    } catch (...) {
        return std::nullopt;
    }
}

bool MatchesSceneRoom(int16_t currentScene, int16_t currentRoom, const ExternalModRuntime& runtime, const std::string& sceneKey,
                      const std::string& roomKey) {
    const auto sceneValue = TryGetIntBlackboardValue(runtime, sceneKey);
    if (sceneValue.has_value() && static_cast<int16_t>(*sceneValue) != currentScene) {
        return false;
    }

    if (!roomKey.empty()) {
        const auto roomValue = TryGetIntBlackboardValue(runtime, roomKey);
        if (roomValue.has_value() && static_cast<int16_t>(*roomValue) != currentRoom) {
            return false;
        }
    }
    return true;
}

std::array<float, 3> KelvinToRgb(float kelvin) {
    kelvin = ClampValue(kelvin, 1000.0f, 40000.0f) / 100.0f;
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    if (kelvin <= 66.0f) {
        red = 255.0f;
        green = 99.4708025861f * std::log(kelvin) - 161.1195681661f;
        blue = kelvin <= 19.0f ? 0.0f : 138.5177312231f * std::log(kelvin - 10.0f) - 305.0447927307f;
    } else {
        red = 329.698727446f * std::pow(kelvin - 60.0f, -0.1332047592f);
        green = 288.1221695283f * std::pow(kelvin - 60.0f, -0.0755148492f);
        blue = 255.0f;
    }

    red = ClampValue(red, 0.0f, 255.0f);
    green = ClampValue(green, 0.0f, 255.0f);
    blue = ClampValue(blue, 0.0f, 255.0f);
    return { red / 255.0f, green / 255.0f, blue / 255.0f };
}

uint8_t ToByteColor(float linearValue) {
    return static_cast<uint8_t>(ClampValue(linearValue * 255.0f, 0.0f, 255.0f));
}

std::array<float, 3> ToNormalizedRgb(const Color_RGBA8& color) {
    return { ClampValue(static_cast<float>(color.r) / 255.0f, 0.0f, 1.0f),
             ClampValue(static_cast<float>(color.g) / 255.0f, 0.0f, 1.0f),
             ClampValue(static_cast<float>(color.b) / 255.0f, 0.0f, 1.0f) };
}

int16_t ToFogNearFromDensity(float density) {
    const float normalized = ClampValue(density, 0.0f, 1.0f);
    return static_cast<int16_t>(ClampValue(1000.0f - normalized * 850.0f, 0.0f, 1000.0f));
}

int16_t ToFogFarFromDensity(float density) {
    const float normalized = ClampValue(density, 0.0f, 1.0f);
    return static_cast<int16_t>(ClampValue(1000.0f - normalized * 300.0f, 0.0f, 1000.0f));
}

struct ResolvedGraphicsState {
    const ExternalModPackage* sceneProfileSource = nullptr;
    const ExternalModSceneProfileDefinition* sceneProfile = nullptr;
    const ExternalModPackage* roomProfileSource = nullptr;
    const ExternalModRoomProfileDefinition* roomProfile = nullptr;
    const ExternalModPackage* postFxSource = nullptr;
    const ExternalModPostFxPresetDefinition* postFx = nullptr;
    float postFxBlend = 1.0f;
    const ExternalModPackage* skylightSource = nullptr;
    const ExternalModLightProfileDefinition* skylight = nullptr;
    bool fallbackSpotToPoint = false;
};

ResolvedGraphicsState ResolveGraphicsState(const std::vector<ExternalModPackage>& packages, int16_t sceneId, int16_t roomId) {
    ResolvedGraphicsState state{};

    // Action scene override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeSceneProfileId.empty()) {
            continue;
        }
        if (!MatchesSceneRoom(sceneId, roomId, package.runtime, "__world_scene_profile_scene", "")) {
            continue;
        }

        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModSceneProfileDefinition>(
            packages, package.runtime.activeSceneProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModSceneProfileDefinition>& {
                return runtime.sceneProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.sceneProfileSource)) {
            continue;
        }
        state.sceneProfileSource = &package;
        state.sceneProfile = profile;
        if (profileOwner != nullptr) {
            state.sceneProfileSource = profileOwner;
        }
    }

    // Auto scene
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& profile : package.runtime.sceneProfiles) {
            if (profile.sceneId != sceneId) {
                continue;
            }
            if (!IsHigherPriorityPackage(&package, state.sceneProfileSource)) {
                continue;
            }
            state.sceneProfileSource = &package;
            state.sceneProfile = &profile;
        }
    }

    // Action room override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeRoomProfileId.empty()) {
            continue;
        }
        if (!MatchesSceneRoom(sceneId, roomId, package.runtime, "__world_room_profile_scene", "__world_room_profile_room")) {
            continue;
        }

        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModRoomProfileDefinition>(
            packages, package.runtime.activeRoomProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModRoomProfileDefinition>& {
                return runtime.roomProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.roomProfileSource)) {
            continue;
        }
        state.roomProfileSource = &package;
        state.roomProfile = profile;
        if (profileOwner != nullptr) {
            state.roomProfileSource = profileOwner;
        }
    }

    // Auto room
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& profile : package.runtime.roomProfiles) {
            if (profile.roomId != roomId) {
                continue;
            }
            if (profile.sceneId >= 0 && profile.sceneId != sceneId) {
                continue;
            }
            if (!IsHigherPriorityPackage(&package, state.roomProfileSource)) {
                continue;
            }
            state.roomProfileSource = &package;
            state.roomProfile = &profile;
        }
    }

    // Action postfx override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activePostFxPresetId.empty()) {
            continue;
        }

        const ExternalModPackage* presetOwner = nullptr;
        const auto* preset = FindDefinitionAcrossPackages<ExternalModPostFxPresetDefinition>(
            packages, package.runtime.activePostFxPresetId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModPostFxPresetDefinition>& {
                return runtime.postFxPresets;
            },
            &presetOwner);
        if (preset == nullptr || !IsHigherPriorityPackage(&package, state.postFxSource)) {
            continue;
        }
        state.postFxSource = presetOwner != nullptr ? presetOwner : &package;
        state.postFx = preset;
        state.postFxBlend = ClampValue(package.runtime.activePostFxBlend, 0.0f, 1.0f);
    }

    if (state.postFx == nullptr) {
        std::string presetId;
        if (state.roomProfile != nullptr && !state.roomProfile->postFxPresetId.empty()) {
            presetId = state.roomProfile->postFxPresetId;
        } else if (state.sceneProfile != nullptr && !state.sceneProfile->postFxPresetId.empty()) {
            presetId = state.sceneProfile->postFxPresetId;
        }
        if (!presetId.empty()) {
            const ExternalModPackage* presetOwner = nullptr;
            state.postFx = FindDefinitionAcrossPackages<ExternalModPostFxPresetDefinition>(
                packages, presetId,
                [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModPostFxPresetDefinition>& {
                    return runtime.postFxPresets;
                },
                &presetOwner);
            state.postFxSource = presetOwner;
            state.postFxBlend = 1.0f;
        }
    }

    // Action skylight override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeSkylightProfileId.empty()) {
            continue;
        }
        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
            packages, package.runtime.activeSkylightProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                return runtime.lightProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.skylightSource)) {
            continue;
        }
        state.skylightSource = profileOwner != nullptr ? profileOwner : &package;
        state.skylight = profile;
    }

    if (state.skylight == nullptr) {
        std::string profileId;
        if (state.roomProfile != nullptr && !state.roomProfile->skylightProfileId.empty()) {
            profileId = state.roomProfile->skylightProfileId;
        } else if (state.sceneProfile != nullptr && !state.sceneProfile->skylightProfileId.empty()) {
            profileId = state.sceneProfile->skylightProfileId;
        }
        if (!profileId.empty()) {
            const ExternalModPackage* profileOwner = nullptr;
            state.skylight = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
                packages, profileId,
                [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                    return runtime.lightProfiles;
                },
                &profileOwner);
            state.skylightSource = profileOwner;
        }
    }

    return state;
}

uint8_t ParseAmbientOcclusionQuality(const std::string& quality) {
    std::string normalized = quality;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    if (normalized == "low") {
        return 1;
    }
    if (normalized == "medium") {
        return 2;
    }
    if (normalized == "high") {
        return 3;
    }
    return 2;
}

uint8_t ParseVolumetricsQuality(const std::string& quality) {
    return ParseAmbientOcclusionQuality(quality);
}

uint8_t GetVolumetricsShadowBudget(uint8_t quality, uint8_t shadowQuality) {
    if (shadowQuality == 0 || quality == 0) {
        return 0;
    }
    switch (quality) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 4;
        default:
            return 0;
    }
}

void SetIdentityMatrix(float* outMatrix) {
    std::fill(outMatrix, outMatrix + 16, 0.0f);
    outMatrix[0] = 1.0f;
    outMatrix[5] = 1.0f;
    outMatrix[10] = 1.0f;
    outMatrix[15] = 1.0f;
}

void CopyMatrixToColumnMajor(const MtxF& matrix, float* outMatrix) {
    for (size_t row = 0; row < 4; ++row) {
        for (size_t col = 0; col < 4; ++col) {
            outMatrix[col * 4 + row] = matrix.mf[row][col];
        }
    }
}

std::array<float, 3> NormalizeVector(std::array<float, 3> value, std::array<float, 3> fallback = { { 0.0f, -1.0f, 0.0f } }) {
    const float lengthSq = value[0] * value[0] + value[1] * value[1] + value[2] * value[2];
    if (lengthSq <= 0.000001f) {
        return fallback;
    }
    const float invLength = 1.0f / std::sqrt(lengthSq);
    return { { value[0] * invLength, value[1] * invLength, value[2] * invLength } };
}

std::string GetAmbientOcclusionFallbackReasonName(int32_t code) {
    switch (code) {
        case 1:
            return "AO_BACKEND_UNSUPPORTED_METAL";
        case 2:
            return "AO_SHADER_COMPILE_FAILED";
        case 3:
            return "AO_RUNTIME_UNSUPPORTED";
        default:
            return "";
    }
}

std::string GetVolumetricsFallbackReasonName(int32_t code) {
    switch (code) {
        case 1:
            return "VOLUMETRICS_BACKEND_UNSUPPORTED_METAL";
        case 2:
            return "VOLUMETRICS_SHADER_COMPILE_FAILED";
        case 3:
            return "VOLUMETRICS_RUNTIME_UNSUPPORTED";
        default:
            return "";
    }
}

std::string GetVolumetricsDebugLabel(uint8_t mode) {
    switch (mode) {
        case Fast::GFX_VOLUMETRICS_DEBUG_FOG_DENSITY:
            return "FogDensity";
        case Fast::GFX_VOLUMETRICS_DEBUG_LIGHT_ENERGY:
            return "LightEnergy";
        case Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME:
            return "FinalVolume";
        case Fast::GFX_VOLUMETRICS_DEBUG_TRANSMITTANCE:
            return "Transmittance";
        case Fast::GFX_VOLUMETRICS_DEBUG_SHADOW_OCCLUSION:
            return "ShadowOcclusion";
        default:
            return "Off";
    }
}

std::string GetVolumetricsPerformanceModeLabel(uint8_t mode) {
    switch (mode) {
        case Fast::GFX_VOLUMETRICS_PERF_PERFORMANCE:
            return "Performance";
        case Fast::GFX_VOLUMETRICS_PERF_QUALITY:
            return "Quality";
        case Fast::GFX_VOLUMETRICS_PERF_BALANCED:
        default:
            return "Balanced";
    }
}

std::string GetVolumetricsShadowModeLabel(uint8_t mode) {
    switch (mode) {
        case Fast::GFX_VOLUMETRICS_SHADOW_SCREEN_DEPTH_PROXY:
            return "screenDepthProxy";
        case Fast::GFX_VOLUMETRICS_SHADOW_REAL_SHADOW:
            return "realShadow";
        case Fast::GFX_VOLUMETRICS_SHADOW_OFF:
        default:
            return "off";
    }
}

std::string NormalizeBackendPolicy(std::string policy) {
    std::transform(policy.begin(), policy.end(), policy.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return policy;
}

bool IsPbrBackendAllowed(const std::string& backendPolicy, const std::string& backendName) {
    const std::string normalizedPolicy = NormalizeBackendPolicy(backendPolicy);
    std::string normalizedBackend = backendName;
    std::transform(normalizedBackend.begin(), normalizedBackend.end(), normalizedBackend.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    if (normalizedPolicy.empty() || normalizedPolicy == "auto" || normalizedPolicy == "any") {
        return true;
    }
    if (normalizedPolicy == "opengl" || normalizedPolicy == "gl") {
        return normalizedBackend.find("opengl") != std::string::npos;
    }
    if (normalizedPolicy == "d3d11" || normalizedPolicy == "dx11" || normalizedPolicy == "direct3d11") {
        return normalizedBackend.find("direct3d11") != std::string::npos || normalizedBackend.find("d3d11") != std::string::npos;
    }
    if (normalizedPolicy == "metal") {
        return normalizedBackend.find("metal") != std::string::npos;
    }
    return true;
}

std::string GetPbrFallbackReasonName(int32_t code) {
    switch (code) {
        case 1:
            return "PBR_BACKEND_POLICY_BLOCKED";
        case 2:
            return "PBR_BACKEND_UNAVAILABLE";
        default:
            return "";
    }
}

} // namespace

struct ExternalModWorldGraphicsRuntime::Impl {
    struct FrameLightRecord {
        LightInfo info{};
        LightNode* node = nullptr;
        std::string modId;
        int32_t handle = 0;
    };

    bool envCaptured = false;
    std::array<uint8_t, 3> lightAmbientColor{};
    std::array<uint8_t, 3> lightFogColor{};
    int16_t lightFogNear = 0;
    int16_t lightFogFar = 0;
    std::array<uint8_t, 3> envAmbientColor{};
    std::array<uint8_t, 3> envFogColor{};
    std::array<int8_t, 3> envLight1Dir{};
    std::array<int8_t, 3> envLight2Dir{};
    std::array<uint8_t, 3> envLight1Color{};
    std::array<uint8_t, 3> envLight2Color{};
    int16_t envFogNear = 0;
    int16_t envFogFar = 0;
    bool envFillScreen = false;
    std::array<uint8_t, 4> envScreenFillColor{};
    bool forcedFogOverlayActive = false;
    std::array<uint8_t, 4> appliedFogOverlayColor{};
    bool depthAwareFogForced = false;
    bool pbrEnabled = false;
    bool pbrPomEnabled = false;
    int32_t pbrPomSteps = 0;
    float pbrPomMaxDistance = 0.0f;
    int32_t pbrFallbackReason = 0;
    std::string pbrBackendName;
    std::string pbrBackendPolicy;
    std::string pbrSourceProfileId;
    std::string pbrSourceModId;
    size_t pbrBoundMaterialDefinitionCount = 0;
    size_t pbrBoundAlbedoMaterialDefinitionCount = 0;
    size_t pbrActiveMaterialOverrideCount = 0;
    bool aoEnabled = false;
    uint8_t aoQuality = 0;
    int32_t aoFallbackReason = 0;
    std::string aoSourceProfileId;
    std::string aoSourceModId;
    bool volumetricsEnabled = false;
    uint8_t volumetricsQuality = 0;
    uint8_t volumetricsShadowQuality = 0;
    uint8_t volumetricsPerformanceMode = Fast::GFX_VOLUMETRICS_PERF_BALANCED;
    int32_t volumetricsFallbackReason = 0;
    std::string volumetricsSourceProfileId;
    std::string volumetricsSourceModId;
    std::array<float, 3> resolvedVolumetricsColor = { { 0.0f, 0.0f, 0.0f } };
    float resolvedVolumetricsAmbientIntensity = 0.35f;
    uint8_t volumetricsDebugMode = Fast::GFX_VOLUMETRICS_DEBUG_OFF;
    bool customSkylightActive = false;
    bool customTestLightActive = false;
    std::string customTestLightType;
    bool volumetricsDispatched = false;
    bool volumetricsProducedMedium = false;
    bool volumetricsProduced = false;
    bool volumetricsHasActiveLightPayload = false;
    bool volumetricsDepthFogPathActive = false;
    bool volumetricsRaymarchActive = false;
    uint8_t volumetricsShadowMode = Fast::GFX_VOLUMETRICS_SHADOW_OFF;
    uint32_t volumetricsPayloadLightCount = 0;
    bool volumetricsDebugPreviewValid = false;
    uint32_t volumetricsLowWidth = 0;
    uint32_t volumetricsLowHeight = 0;
    uint32_t volumetricsStepCount = 0;
    uint32_t volumetricsRaymarchPixelCount = 0;
    uint32_t volumetricsStatsAgeFrames = 0;
    uint32_t volumetricsMarchedPixelCount = 0;
    bool volumetricsHistoryValid = false;
    float volumetricsAvgDensity = 0.0f;
    float volumetricsAvgLightEnergy = 0.0f;
    float volumetricsAvgFinalVolume = 0.0f;
    float volumetricsAvgTransmittance = 1.0f;
    float volumetricsMaxDensity = 0.0f;
    float volumetricsMaxLightEnergy = 0.0f;
    float volumetricsMaxFinalVolume = 0.0f;
    float volumetricsMaxOpticalDepth = 0.0f;
    uint32_t volumetricsDirectionalShadowMapSize = 0;
    int32_t volumetricsDirectionalShadowCasterIndex = -1;
    size_t volumetricLightCount = 0;
    size_t volumetricShadowLightCount = 0;
    bool customPostFxActive = false;
    float resolvedPostFxExposure = 1.0f;
    float resolvedPostFxBloom = 0.0f;
    float resolvedPostFxSaturation = 1.0f;
    float resolvedPostFxFogDensity = 0.0f;
    std::array<float, 4> resolvedPostFxFogColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    std::array<float, 16> previousViewProjection{};
    bool previousViewProjectionValid = false;
    int16_t previousSceneId = -1;
    int16_t previousRoomId = -1;
    std::vector<FrameLightRecord> frameLights;
    std::string lastResolvedSignature;
    std::string lastVolumetricsStateSignature;
    std::string inspectorSummary;
};

ExternalModWorldGraphicsRuntime::ExternalModWorldGraphicsRuntime() : mImpl(std::make_unique<Impl>()) {
    SetIdentityMatrix(mImpl->previousViewProjection.data());
}

ExternalModWorldGraphicsRuntime::~ExternalModWorldGraphicsRuntime() = default;

void ExternalModWorldGraphicsRuntime::OnPlayDrawBegin(ExternalModManager& manager,
                                                      std::vector<ExternalModPackage>& packages, PlayState* play) {
    if (play == nullptr) {
        return;
    }

    // Defensive cleanup in case End hook was skipped.
    OnPlayDrawEnd(play);
    gfx_set_volumetric_lights(nullptr, 0);

    mImpl->envCaptured = true;
    for (size_t i = 0; i < 3; ++i) {
        mImpl->lightAmbientColor[i] = play->lightCtx.ambientColor[i];
        mImpl->lightFogColor[i] = play->lightCtx.fogColor[i];
        mImpl->envAmbientColor[i] = play->envCtx.lightSettings.ambientColor[i];
        mImpl->envFogColor[i] = play->envCtx.lightSettings.fogColor[i];
        mImpl->envLight1Dir[i] = play->envCtx.lightSettings.light1Dir[i];
        mImpl->envLight2Dir[i] = play->envCtx.lightSettings.light2Dir[i];
        mImpl->envLight1Color[i] = play->envCtx.lightSettings.light1Color[i];
        mImpl->envLight2Color[i] = play->envCtx.lightSettings.light2Color[i];
    }
    mImpl->lightFogNear = play->lightCtx.fogNear;
    mImpl->lightFogFar = play->lightCtx.fogFar;
    mImpl->envFogNear = play->envCtx.lightSettings.fogNear;
    mImpl->envFogFar = play->envCtx.lightSettings.fogFar;
    mImpl->envFillScreen = play->envCtx.fillScreen;
    for (size_t i = 0; i < 4; ++i) {
        mImpl->envScreenFillColor[i] = play->envCtx.screenFillColor[i];
    }
    mImpl->forcedFogOverlayActive = false;
    mImpl->appliedFogOverlayColor.fill(0);
    mImpl->depthAwareFogForced = false;
    mImpl->pbrEnabled = false;
    mImpl->pbrPomEnabled = false;
    mImpl->pbrPomSteps = 0;
    mImpl->pbrPomMaxDistance = 0.0f;
    mImpl->pbrFallbackReason = 0;
    mImpl->pbrBackendName.clear();
    mImpl->pbrBackendPolicy.clear();
    mImpl->pbrSourceProfileId.clear();
    mImpl->pbrSourceModId.clear();
    mImpl->pbrBoundMaterialDefinitionCount = 0;
    mImpl->pbrBoundAlbedoMaterialDefinitionCount = 0;
    mImpl->pbrActiveMaterialOverrideCount = 0;
    mImpl->aoEnabled = false;
    mImpl->aoQuality = 0;
    mImpl->aoFallbackReason = 0;
    mImpl->aoSourceProfileId.clear();
    mImpl->aoSourceModId.clear();
    mImpl->volumetricsEnabled = false;
    mImpl->volumetricsQuality = 0;
    mImpl->volumetricsShadowQuality = 0;
    mImpl->volumetricsPerformanceMode = Fast::GFX_VOLUMETRICS_PERF_BALANCED;
    mImpl->volumetricsFallbackReason = 0;
    mImpl->volumetricsSourceProfileId.clear();
    mImpl->volumetricsSourceModId.clear();
    mImpl->resolvedVolumetricsColor = { { 0.0f, 0.0f, 0.0f } };
    mImpl->resolvedVolumetricsAmbientIntensity = 0.35f;
    mImpl->volumetricsDebugMode = Fast::GFX_VOLUMETRICS_DEBUG_OFF;
    mImpl->customSkylightActive = false;
    mImpl->customTestLightActive = false;
    mImpl->customTestLightType.clear();
    mImpl->volumetricsDispatched = false;
    mImpl->volumetricsProducedMedium = false;
    mImpl->volumetricsProduced = false;
    mImpl->volumetricsHasActiveLightPayload = false;
    mImpl->volumetricsDepthFogPathActive = false;
    mImpl->volumetricsRaymarchActive = false;
    mImpl->volumetricsShadowMode = Fast::GFX_VOLUMETRICS_SHADOW_OFF;
    mImpl->volumetricsPayloadLightCount = 0;
    mImpl->volumetricsDebugPreviewValid = false;
    mImpl->volumetricsLowWidth = 0;
    mImpl->volumetricsLowHeight = 0;
    mImpl->volumetricsStepCount = 0;
    mImpl->volumetricsRaymarchPixelCount = 0;
    mImpl->volumetricsStatsAgeFrames = 0;
    mImpl->volumetricsMarchedPixelCount = 0;
    mImpl->volumetricsHistoryValid = false;
    mImpl->volumetricsAvgDensity = 0.0f;
    mImpl->volumetricsAvgLightEnergy = 0.0f;
    mImpl->volumetricsAvgFinalVolume = 0.0f;
    mImpl->volumetricsAvgTransmittance = 1.0f;
    mImpl->volumetricsMaxDensity = 0.0f;
    mImpl->volumetricsMaxLightEnergy = 0.0f;
    mImpl->volumetricsMaxFinalVolume = 0.0f;
    mImpl->volumetricsMaxOpticalDepth = 0.0f;
    mImpl->volumetricsDirectionalShadowMapSize = 0;
    mImpl->volumetricsDirectionalShadowCasterIndex = -1;
    mImpl->volumetricLightCount = 0;
    mImpl->volumetricShadowLightCount = 0;
    mImpl->customPostFxActive = false;
    mImpl->resolvedPostFxExposure = 1.0f;
    mImpl->resolvedPostFxBloom = 0.0f;
    mImpl->resolvedPostFxSaturation = 1.0f;
    mImpl->resolvedPostFxFogDensity = 0.0f;
    mImpl->resolvedPostFxFogColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };

    const int16_t sceneId = play->sceneNum;
    const int16_t roomId = play->roomCtx.curRoom.num;
    const ResolvedGraphicsState resolvedState = ResolveGraphicsState(packages, sceneId, roomId);

    std::ostringstream signatureBuilder;
    signatureBuilder << sceneId << ":" << roomId << "|"
                     << (resolvedState.sceneProfile != nullptr ? resolvedState.sceneProfile->id : "none") << "|"
                     << (resolvedState.roomProfile != nullptr ? resolvedState.roomProfile->id : "none") << "|"
                     << (resolvedState.postFx != nullptr ? resolvedState.postFx->id : "none") << "|"
                     << (resolvedState.skylight != nullptr ? resolvedState.skylight->id : "none");
    const std::string resolvedSignature = signatureBuilder.str();
    const bool renderProfileChanged = resolvedSignature != mImpl->lastResolvedSignature;
    if (renderProfileChanged) {
        ExternalModHookEventContext context;
        context.scene = sceneId;
        context.value = resolvedSignature;
        manager.EmitExtendedHook(ExternalModHookType::OnRenderProfileResolved, context, "OnRenderProfileResolved");
        mImpl->lastResolvedSignature = resolvedSignature;
    }

    const bool customPostFxActive = CVarGetInteger("gEnhancements.Graphics.PostFx.UseCustom", 0) != 0;
    float resolvedPostFxExposure = resolvedState.postFx != nullptr ? resolvedState.postFx->exposure : 1.0f;
    float resolvedPostFxBloom = resolvedState.postFx != nullptr ? resolvedState.postFx->bloom : 0.0f;
    float resolvedPostFxSaturation = resolvedState.postFx != nullptr ? resolvedState.postFx->saturation : 1.0f;
    std::array<float, 4> resolvedPostFxFogColor =
        resolvedState.postFx != nullptr
            ? resolvedState.postFx->fogColor
            : std::array<float, 4>{ { static_cast<float>(mImpl->lightFogColor[0]) / 255.0f,
                                      static_cast<float>(mImpl->lightFogColor[1]) / 255.0f,
                                      static_cast<float>(mImpl->lightFogColor[2]) / 255.0f, 1.0f } };
    float resolvedPostFxFogDensity = resolvedState.postFx != nullptr ? resolvedState.postFx->fogDensity : 0.0f;
    int16_t resolvedPostFxFogNear =
        resolvedState.postFx != nullptr
            ? (resolvedState.postFx->hasFogNear ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogNear, 0, 1000))
                                                : ToFogNearFromDensity(resolvedState.postFx->fogDensity))
            : mImpl->lightFogNear;
    int16_t resolvedPostFxFogFar =
        resolvedState.postFx != nullptr
            ? (resolvedState.postFx->hasFogFar ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogFar, 0, 1000))
                                               : ToFogFarFromDensity(resolvedState.postFx->fogDensity))
            : mImpl->lightFogFar;

    if (resolvedState.postFx != nullptr && resolvedState.postFx->forceDepthAwareFog) {
        gfx_set_force_depth_aware_fog(1);
        mImpl->depthAwareFogForced = true;
    }

    if (customPostFxActive) {
        const Color_RGBA8 customFogColor =
            CVarGetColor("gEnhancements.Graphics.PostFx.FogColor.Value",
                         { ToByteColor(resolvedPostFxFogColor[0]), ToByteColor(resolvedPostFxFogColor[1]),
                           ToByteColor(resolvedPostFxFogColor[2]), ToByteColor(resolvedPostFxFogColor[3]) });
        resolvedPostFxExposure =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.PostFx.Exposure", resolvedPostFxExposure)), 0.0f,
                       8.0f);
        resolvedPostFxBloom =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.PostFx.Bloom", resolvedPostFxBloom)), 0.0f, 4.0f);
        resolvedPostFxSaturation =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.PostFx.Saturation", resolvedPostFxSaturation)),
                       0.0f, 4.0f);
        resolvedPostFxFogDensity =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.PostFx.FogDensity", resolvedPostFxFogDensity)),
                       0.0f, 1.0f);
        resolvedPostFxFogColor = { { static_cast<float>(customFogColor.r) / 255.0f,
                                     static_cast<float>(customFogColor.g) / 255.0f,
                                     static_cast<float>(customFogColor.b) / 255.0f,
                                     static_cast<float>(customFogColor.a) / 255.0f } };
        resolvedPostFxFogNear = ToFogNearFromDensity(resolvedPostFxFogDensity);
        resolvedPostFxFogFar = ToFogFarFromDensity(resolvedPostFxFogDensity);
    }

    mImpl->customPostFxActive = customPostFxActive;
    mImpl->resolvedPostFxExposure = resolvedPostFxExposure;
    mImpl->resolvedPostFxBloom = resolvedPostFxBloom;
    mImpl->resolvedPostFxSaturation = resolvedPostFxSaturation;
    mImpl->resolvedPostFxFogDensity = resolvedPostFxFogDensity;
    mImpl->resolvedPostFxFogColor = resolvedPostFxFogColor;

    std::array<float, 3> defaultSkylightColor =
        resolvedState.skylight != nullptr
            ? (resolvedState.skylight->hasKelvin ? KelvinToRgb(resolvedState.skylight->kelvin)
                                                 : resolvedState.skylight->colorLinear)
            : std::array<float, 3>{ { static_cast<float>(mImpl->envLight1Color[0]) / 255.0f,
                                      static_cast<float>(mImpl->envLight1Color[1]) / 255.0f,
                                      static_cast<float>(mImpl->envLight1Color[2]) / 255.0f } };
    std::array<float, 3> defaultSkylightDirection =
        resolvedState.skylight != nullptr
            ? NormalizeVector(resolvedState.skylight->direction)
            : NormalizeVector({ { static_cast<float>(mImpl->envLight1Dir[0]) / 127.0f,
                                  static_cast<float>(mImpl->envLight1Dir[1]) / 127.0f,
                                  static_cast<float>(mImpl->envLight1Dir[2]) / 127.0f } });
    const bool customSkylightActive = CVarGetInteger("gEnhancements.Graphics.Skylight.UseCustom", 0) != 0;
    const Color_RGBA8 customSkylightColor =
        CVarGetColor("gEnhancements.Graphics.Skylight.Color.Value",
                     { ToByteColor(defaultSkylightColor[0]), ToByteColor(defaultSkylightColor[1]),
                       ToByteColor(defaultSkylightColor[2]), 255 });
    const float customSkylightIntensity =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Skylight.Intensity",
                                                   resolvedState.skylight != nullptr ? resolvedState.skylight->intensity : 1.0f)),
                   0.0f, 8.0f);
    const float customSkylightVolumetricIntensity =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Skylight.VolumetricIntensity",
                                                   resolvedState.skylight != nullptr
                                                       ? resolvedState.skylight->volumetricIntensity
                                                       : 1.0f)),
                   0.0f, 8.0f);
    const std::array<float, 3> customSkylightDirection = NormalizeVector(
        { { static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Skylight.DirectionX", defaultSkylightDirection[0])),
            static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Skylight.DirectionY", defaultSkylightDirection[1])),
            static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Skylight.DirectionZ", defaultSkylightDirection[2])) } },
        defaultSkylightDirection);

    ExternalModLightProfileDefinition customSkylightProfile{};
    const ExternalModLightProfileDefinition* effectiveSkylight = resolvedState.skylight;
    const ExternalModPackage* effectiveSkylightSource = resolvedState.skylightSource;
    if (customSkylightActive) {
        customSkylightProfile.id = "__custom_skylight";
        customSkylightProfile.lightType = "directional";
        customSkylightProfile.colorLinear = ToNormalizedRgb(customSkylightColor);
        customSkylightProfile.intensity = customSkylightIntensity;
        customSkylightProfile.direction = customSkylightDirection;
        customSkylightProfile.volumetricIntensity = customSkylightVolumetricIntensity;
        customSkylightProfile.castShadows = true;
        customSkylightProfile.shadowResolution = 512;
        customSkylightProfile.shadowBias = 0.01f;
        customSkylightProfile.shadowNormalBias = 0.02f;
        customSkylightProfile.shadowRange = 1200.0f;
        effectiveSkylight = &customSkylightProfile;
        effectiveSkylightSource = nullptr;
    }
    mImpl->customSkylightActive = customSkylightActive;

    const bool customTestLightActive = CVarGetInteger("gEnhancements.Graphics.DebugLight.UseCustom", 0) != 0;
    const int32_t customTestLightTypeIndex =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.DebugLight.Type", 1)), 0, 2);
    const char* customTestLightType =
        customTestLightTypeIndex == 0 ? "directional" : (customTestLightTypeIndex == 1 ? "point" : "spot");
    const Color_RGBA8 customTestLightColor = CVarGetColor("gEnhancements.Graphics.DebugLight.Color.Value",
                                                          { 255, 232, 196, 255 });
    const float customTestLightIntensity =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.Intensity", 2.0f)), 0.0f, 16.0f);
    const float customTestLightVolumetricIntensity = ClampValue(
        static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.VolumetricIntensity", 2.0f)), 0.0f, 8.0f);
    const float customTestLightOffsetX =
        static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.PositionX", 0.0f));
    const float customTestLightOffsetY =
        static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.PositionY", 120.0f));
    const float customTestLightOffsetZ =
        static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.PositionZ", 220.0f));
    const std::array<float, 3> customTestLightDirection = NormalizeVector(
        { { static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.DirectionX", 0.0f)),
            static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.DirectionY", -1.0f)),
            static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.DirectionZ", 0.0f)) } });
    const float customTestLightRadius =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.Radius", 420.0f)), 1.0f, 20000.0f);
    const float customTestLightInnerCone =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.InnerCone", 20.0f)), 0.0f, 180.0f);
    const float customTestLightOuterCone =
        ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.DebugLight.OuterCone", 40.0f)), 0.0f, 180.0f);
    const bool customTestLightCastShadows =
        CVarGetInteger("gEnhancements.Graphics.DebugLight.CastShadows", 1) != 0;
    mImpl->customTestLightActive = customTestLightActive;
    mImpl->customTestLightType = customTestLightType;

    if (resolvedState.postFx != nullptr || customPostFxActive) {
        std::array<uint8_t, 3> targetFogColor = {
            ToByteColor(resolvedPostFxFogColor[0]), ToByteColor(resolvedPostFxFogColor[1]), ToByteColor(resolvedPostFxFogColor[2]),
        };
        const float blend = ClampValue(resolvedState.postFxBlend, 0.0f, 1.0f);
        for (size_t i = 0; i < 3; ++i) {
            const float sourceFog = static_cast<float>(mImpl->lightFogColor[i]);
            const float blendedFog = sourceFog + (static_cast<float>(targetFogColor[i]) - sourceFog) * blend;
            play->lightCtx.fogColor[i] = static_cast<uint8_t>(ClampValue(blendedFog, 0.0f, 255.0f));
            play->envCtx.lightSettings.fogColor[i] = play->lightCtx.fogColor[i];
        }

        const float blendedNear =
            static_cast<float>(mImpl->lightFogNear) + (static_cast<float>(resolvedPostFxFogNear - mImpl->lightFogNear) * blend);
        const float blendedFar =
            static_cast<float>(mImpl->lightFogFar) + (static_cast<float>(resolvedPostFxFogFar - mImpl->lightFogFar) * blend);
        play->lightCtx.fogNear = static_cast<int16_t>(ClampValue(blendedNear, 0.0f, 1000.0f));
        play->lightCtx.fogFar = static_cast<int16_t>(ClampValue(blendedFar, 0.0f, 1000.0f));
        play->envCtx.lightSettings.fogNear = play->lightCtx.fogNear;
        play->envCtx.lightSettings.fogFar = play->lightCtx.fogFar;

        const float overlayStrength =
            (resolvedState.postFx != nullptr)
                ? ClampValue(resolvedState.postFx->fogOverlayStrength * blend, 0.0f, 1.0f)
                : 0.0f;
        if (resolvedState.postFx != nullptr && resolvedState.postFx->forceFogOverlay && overlayStrength > 0.0f &&
            !play->envCtx.fillScreen) {
            play->envCtx.fillScreen = true;
            play->envCtx.screenFillColor[0] = targetFogColor[0];
            play->envCtx.screenFillColor[1] = targetFogColor[1];
            play->envCtx.screenFillColor[2] = targetFogColor[2];
            play->envCtx.screenFillColor[3] = static_cast<uint8_t>(ClampValue(overlayStrength * 255.0f, 0.0f, 255.0f));
            mImpl->forcedFogOverlayActive = true;
            mImpl->appliedFogOverlayColor = { play->envCtx.screenFillColor[0], play->envCtx.screenFillColor[1],
                                              play->envCtx.screenFillColor[2], play->envCtx.screenFillColor[3] };
        }
    }

    if (resolvedState.sceneProfile != nullptr) {
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t ambient = ToByteColor(resolvedState.sceneProfile->ambientColor[i]);
            play->lightCtx.ambientColor[i] = ambient;
            play->envCtx.lightSettings.ambientColor[i] = ambient;
        }
    }

    if (effectiveSkylight != nullptr) {
        std::array<float, 3> colorLinear = effectiveSkylight->colorLinear;
        if (effectiveSkylight->hasKelvin) {
            colorLinear = KelvinToRgb(effectiveSkylight->kelvin);
        }
        const float intensity = ClampValue(effectiveSkylight->intensity, 0.0f, 8.0f);
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t colorByte = ToByteColor(colorLinear[i] * intensity);
            play->envCtx.lightSettings.light1Color[i] = colorByte;
            play->envCtx.lightSettings.light2Color[i] = colorByte;
            play->envCtx.lightSettings.ambientColor[i] =
                std::max(play->envCtx.lightSettings.ambientColor[i], static_cast<uint8_t>(colorByte / 2));
            play->lightCtx.ambientColor[i] = play->envCtx.lightSettings.ambientColor[i];
        }
        const auto& direction = effectiveSkylight->direction;
        play->envCtx.lightSettings.light1Dir[0] = static_cast<int8_t>(ClampValue(direction[0] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[1] = static_cast<int8_t>(ClampValue(direction[1] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[2] = static_cast<int8_t>(ClampValue(direction[2] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light2Dir[0] = -play->envCtx.lightSettings.light1Dir[0];
        play->envCtx.lightSettings.light2Dir[1] = -play->envCtx.lightSettings.light1Dir[1];
        play->envCtx.lightSettings.light2Dir[2] = -play->envCtx.lightSettings.light1Dir[2];
    }

    int32_t maxDynamicLightsTotal = 128;
    int32_t maxDynamicLightsNear = 32;
    const char* backendNameValue = gfx_get_current_backend_name();
    const std::string currentBackendName = (backendNameValue != nullptr && backendNameValue[0] != '\0') ? backendNameValue : "none";
    const ExternalModPbrDefinition* selectedPbrProfile = nullptr;
    const ExternalModPackage* selectedPbrPackage = nullptr;
    const ExternalModPbrDefinition* selectedAoProfile = nullptr;
    const ExternalModPackage* selectedAoPackage = nullptr;
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& pbrProfile : package.runtime.pbrDefinitions) {
            if (pbrProfile.enabled && (selectedPbrPackage == nullptr || IsHigherPriorityPackage(&package, selectedPbrPackage))) {
                selectedPbrPackage = &package;
                selectedPbrProfile = &pbrProfile;
            }
            if (pbrProfile.enabled && pbrProfile.ambientOcclusion.enabled) {
                if (selectedAoPackage == nullptr || IsHigherPriorityPackage(&package, selectedAoPackage)) {
                    selectedAoPackage = &package;
                    selectedAoProfile = &pbrProfile;
                }
            }
        }
    }
    if (selectedPbrProfile != nullptr) {
        maxDynamicLightsTotal = std::max(1, selectedPbrProfile->maxDynamicLightsTotal);
        maxDynamicLightsNear = std::max(1, selectedPbrProfile->maxDynamicLightsNear);
        mImpl->pbrEnabled = true;
        mImpl->pbrPomEnabled = selectedPbrProfile->enablePom;
        mImpl->pbrPomSteps = selectedPbrProfile->pomSteps;
        mImpl->pbrPomMaxDistance = selectedPbrProfile->pomMaxDistance;
        mImpl->pbrBackendName = currentBackendName;
        mImpl->pbrBackendPolicy = selectedPbrProfile->backendPolicy.empty() ? "auto" : selectedPbrProfile->backendPolicy;
        mImpl->pbrSourceProfileId = selectedPbrProfile->id;
        if (selectedPbrPackage != nullptr) {
            mImpl->pbrSourceModId = selectedPbrPackage->manifest.id;
        }
        if (currentBackendName == "none") {
            mImpl->pbrFallbackReason = 2;
        } else if (!IsPbrBackendAllowed(mImpl->pbrBackendPolicy, currentBackendName)) {
            mImpl->pbrFallbackReason = 1;
        }
    }
    manager.GetMaterialRuntimeStats(sceneId, roomId, mImpl->pbrActiveMaterialOverrideCount,
                                    mImpl->pbrBoundMaterialDefinitionCount, mImpl->pbrBoundAlbedoMaterialDefinitionCount);
    maxDynamicLightsNear = std::min(maxDynamicLightsNear, maxDynamicLightsTotal);

    const int32_t aoEnabledCvar = CVarGetInteger("gEnhancements.Graphics.AO.Enabled", 0);
    const int32_t aoQualityCvar =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.AO.Quality", 0)), static_cast<int32_t>(0),
                   static_cast<int32_t>(3));
    const float aoIntensityScale =
        std::clamp(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.AO.IntensityScale", 1.0f)), 0.0f, 8.0f);
    const bool aoDebugView = CVarGetInteger("gEnhancements.Graphics.AO.DebugView", 0) != 0;

    uint8_t resolvedAoQuality = static_cast<uint8_t>(aoQualityCvar);
    float resolvedAoRadius = 0.55f;
    float resolvedAoIntensity = 0.9f;
    float resolvedAoBias = 0.02f;
    float resolvedAoPower = 1.2f;
    float resolvedAoMaxDistance = 1200.0f;
    int32_t resolvedAoBlurPasses = 2;

    if (selectedAoProfile != nullptr) {
        if (resolvedAoQuality == 0) {
            resolvedAoQuality = ParseAmbientOcclusionQuality(selectedAoProfile->ambientOcclusion.quality);
        }
        resolvedAoRadius = selectedAoProfile->ambientOcclusion.radius;
        resolvedAoIntensity = selectedAoProfile->ambientOcclusion.intensity;
        resolvedAoBias = selectedAoProfile->ambientOcclusion.bias;
        resolvedAoPower = selectedAoProfile->ambientOcclusion.power;
        resolvedAoMaxDistance = selectedAoProfile->ambientOcclusion.maxDistance;
        resolvedAoBlurPasses = selectedAoProfile->ambientOcclusion.blurPasses;
    }

    resolvedAoIntensity = std::clamp(resolvedAoIntensity * aoIntensityScale, 0.0f, 3.0f);
    const bool resolvedAoEnabled = aoEnabledCvar != 0 && resolvedAoQuality > 0;
    gfx_set_ambient_occlusion_debug_view(aoDebugView ? 1 : 0);
    gfx_set_ambient_occlusion_config(resolvedAoRadius, resolvedAoIntensity, resolvedAoBias, resolvedAoPower,
                                     resolvedAoMaxDistance, resolvedAoBlurPasses, resolvedAoQuality);
    gfx_set_ambient_occlusion_enabled(resolvedAoEnabled ? 1 : 0);

    mImpl->aoEnabled = resolvedAoEnabled;
    mImpl->aoQuality = resolvedAoQuality;
    if (selectedAoProfile != nullptr) {
        mImpl->aoSourceProfileId = selectedAoProfile->id;
    }
    if (selectedAoPackage != nullptr) {
        mImpl->aoSourceModId = selectedAoPackage->manifest.id;
    }

    const int32_t aoFallbackReason = gfx_get_ambient_occlusion_fallback_reason();
    if (resolvedAoEnabled && aoFallbackReason != 0 && aoFallbackReason != mImpl->aoFallbackReason) {
        ExternalModHookEventContext fallbackContext;
        fallbackContext.scene = sceneId;
        fallbackContext.value = GetAmbientOcclusionFallbackReasonName(aoFallbackReason);
        manager.EmitExtendedHook(ExternalModHookType::OnRenderFallbackApplied, fallbackContext, "OnRenderFallbackApplied");
        mImpl->aoFallbackReason = aoFallbackReason;
    } else if (aoFallbackReason == 0) {
        mImpl->aoFallbackReason = 0;
    }
    gfx_clear_ambient_occlusion_fallback_reason();

    const ExternalModPostFxPresetDefinition* selectedVolumetricsPreset =
        (resolvedState.postFx != nullptr && resolvedState.postFx->hasVolumetrics) ? resolvedState.postFx : nullptr;
    const ExternalModPackage* selectedVolumetricsPackage =
        (selectedVolumetricsPreset != nullptr) ? resolvedState.postFxSource : nullptr;

    const int32_t volumetricsEnabledCvar = CVarGetInteger("gEnhancements.Graphics.Volumetrics.Enabled", 1);
    const int32_t volumetricsQualityCvar =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.Volumetrics.Quality", 0)),
                   static_cast<int32_t>(0), static_cast<int32_t>(3));
    const int32_t volumetricsPerformanceModeCvar =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.Volumetrics.PerformanceMode",
                                                       Fast::GFX_VOLUMETRICS_PERF_BALANCED)),
                   static_cast<int32_t>(Fast::GFX_VOLUMETRICS_PERF_PERFORMANCE),
                   static_cast<int32_t>(Fast::GFX_VOLUMETRICS_PERF_QUALITY));
    const float volumetricsDensityScale =
        std::clamp(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DensityScale", 1.0f)), 0.0f, 8.0f);
    const int32_t volumetricsShadowQualityCvar =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.Volumetrics.ShadowQuality", 0)),
                   static_cast<int32_t>(0), static_cast<int32_t>(3));
    const bool volumetricsDebugView = CVarGetInteger("gEnhancements.Graphics.Volumetrics.DebugView", 0) != 0;
    const bool customVolumetricsActive = CVarGetInteger("gEnhancements.Graphics.Volumetrics.UseCustom", 0) != 0;
    const int32_t customVolumetricsDebugMode =
        std::clamp(static_cast<int32_t>(CVarGetInteger("gEnhancements.Graphics.Volumetrics.DebugMode",
                                                       volumetricsDebugView ? Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME
                                                                            : Fast::GFX_VOLUMETRICS_DEBUG_OFF)),
                   static_cast<int32_t>(Fast::GFX_VOLUMETRICS_DEBUG_OFF),
                   static_cast<int32_t>(Fast::GFX_VOLUMETRICS_DEBUG_SHADOW_OCCLUSION));
    const bool forceUiVolumetricsDebugMode =
        volumetricsDebugView || customVolumetricsDebugMode != static_cast<int32_t>(Fast::GFX_VOLUMETRICS_DEBUG_OFF);
    const uint8_t requestedUiVolumetricsDebugMode = static_cast<uint8_t>(
        customVolumetricsDebugMode != static_cast<int32_t>(Fast::GFX_VOLUMETRICS_DEBUG_OFF)
            ? customVolumetricsDebugMode
            : (volumetricsDebugView ? Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME : Fast::GFX_VOLUMETRICS_DEBUG_OFF));
    const Color_RGBA8 customVolumetricsColor =
        CVarGetColor("gEnhancements.Graphics.Volumetrics.Color.Value",
                     { ToByteColor(resolvedPostFxFogColor[0]), ToByteColor(resolvedPostFxFogColor[1]),
                       ToByteColor(resolvedPostFxFogColor[2]), 255 });
    auto hasActiveVolumetricLightRequest = [&]() -> bool {
        for (const auto& package : packages) {
            if (!package.valid || !package.runtime.enabled) {
                continue;
            }

            for (const auto& lightState : package.runtime.activeDynamicLights) {
                const ExternalModPackage* profileOwner = nullptr;
                const auto* profile = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
                    packages, lightState.profileId,
                    [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                        return runtime.lightProfiles;
                    },
                    &profileOwner);
                if (profile != nullptr && profile->volumetricIntensity > 0.0f && profile->intensity > 0.0f) {
                    return true;
                }
            }
        }
        return false;
    };

    uint8_t resolvedVolumetricsQuality = static_cast<uint8_t>(volumetricsQualityCvar);
    uint8_t resolvedVolumetricsPerformanceMode = static_cast<uint8_t>(volumetricsPerformanceModeCvar);
    float resolvedVolumetricsDensity = 0.03f;
    std::array<float, 3> resolvedVolumetricsColor = { { resolvedPostFxFogColor[0], resolvedPostFxFogColor[1],
                                                         resolvedPostFxFogColor[2] } };
    float resolvedVolumetricsAmbientIntensity = 0.35f;
    float resolvedVolumetricsAnisotropy = 0.2f;
    float resolvedVolumetricsStartDistance = 64.0f;
    float resolvedVolumetricsMaxDistance = 2400.0f;
    bool resolvedVolumetricsHeightFogEnabled = false;
    float resolvedVolumetricsBaseHeight = 0.0f;
    float resolvedVolumetricsHeightFalloff = 0.0025f;
    float resolvedVolumetricsLightShaftIntensity = 1.0f;
    float resolvedVolumetricsShadowIntensity = 0.6f;
    float resolvedVolumetricsTemporalBlend = 0.88f;
    float resolvedVolumetricsJitterScale = 1.0f;
    float resolvedVolumetricsAerialPerspectiveStrength = 1.0f;
    float resolvedVolumetricsMacroNoiseStrength = 1.0f;
    float resolvedVolumetricsValleyFogStrength = 1.0f;
    float resolvedVolumetricsEdgeHazeStrength = 1.0f;
    float resolvedVolumetricsDepthExtinctionStrength = 1.0f;
    float resolvedVolumetricsDepthExtinctionExponent = 1.25f;
    float resolvedVolumetricsHorizonFogStrength = 1.0f;
    float resolvedVolumetricsSkyFallbackStrength = 0.18f;
    float resolvedVolumetricsDirectionalShadowBias = 0.01f;
    float resolvedVolumetricsDirectionalShadowNormalBias = 0.02f;
    float resolvedVolumetricsDirectionalShadowSoftness = 1.0f;
    uint8_t resolvedVolumetricsDebugMode =
        static_cast<uint8_t>(volumetricsDebugView ? Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME : Fast::GFX_VOLUMETRICS_DEBUG_OFF);
    bool resolvedVolumetricsEnabled = volumetricsEnabledCvar != 0;

    if (selectedVolumetricsPreset != nullptr) {
        if (resolvedVolumetricsQuality == 0) {
            resolvedVolumetricsQuality = ParseVolumetricsQuality(selectedVolumetricsPreset->volumetrics.quality);
        }
        resolvedVolumetricsDensity = selectedVolumetricsPreset->volumetrics.density;
        if (selectedVolumetricsPreset->volumetrics.hasColor) {
            resolvedVolumetricsColor = selectedVolumetricsPreset->volumetrics.color;
        }
        resolvedVolumetricsAmbientIntensity = selectedVolumetricsPreset->volumetrics.ambientIntensity;
        resolvedVolumetricsAnisotropy = selectedVolumetricsPreset->volumetrics.anisotropy;
        resolvedVolumetricsStartDistance = selectedVolumetricsPreset->volumetrics.startDistance;
        resolvedVolumetricsMaxDistance = selectedVolumetricsPreset->volumetrics.maxDistance;
        resolvedVolumetricsHeightFogEnabled = selectedVolumetricsPreset->volumetrics.heightFogEnabled;
        resolvedVolumetricsBaseHeight = selectedVolumetricsPreset->volumetrics.baseHeight;
        resolvedVolumetricsHeightFalloff = selectedVolumetricsPreset->volumetrics.heightFalloff;
        resolvedVolumetricsLightShaftIntensity = selectedVolumetricsPreset->volumetrics.lightShaftIntensity;
        resolvedVolumetricsShadowIntensity = selectedVolumetricsPreset->volumetrics.shadowIntensity;
        resolvedVolumetricsTemporalBlend = selectedVolumetricsPreset->volumetrics.temporalBlend;
        resolvedVolumetricsJitterScale = selectedVolumetricsPreset->volumetrics.jitterScale;
        if (selectedVolumetricsPreset->volumetrics.debugView) {
            resolvedVolumetricsDebugMode = Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME;
        }
        resolvedVolumetricsEnabled = resolvedVolumetricsEnabled && selectedVolumetricsPreset->volumetrics.enabled;
    }

    if (customVolumetricsActive) {
        resolvedVolumetricsColor = ToNormalizedRgb(customVolumetricsColor);
        resolvedVolumetricsAmbientIntensity = ClampValue(
            static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.AmbientIntensity",
                                            resolvedVolumetricsAmbientIntensity)),
            0.0f, 8.0f);
        resolvedVolumetricsAnisotropy =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.Anisotropy",
                                                       resolvedVolumetricsAnisotropy)),
                       -0.95f, 0.95f);
        resolvedVolumetricsStartDistance =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.StartDistance",
                                                       resolvedVolumetricsStartDistance)),
                       0.0f, 4000.0f);
        resolvedVolumetricsMaxDistance =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.MaxDistance",
                                                       resolvedVolumetricsMaxDistance)),
                       1.0f, 12000.0f);
        resolvedVolumetricsHeightFogEnabled =
            CVarGetInteger("gEnhancements.Graphics.Volumetrics.HeightFog", resolvedVolumetricsHeightFogEnabled ? 1 : 0) != 0;
        resolvedVolumetricsBaseHeight =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.BaseHeight",
                                                       resolvedVolumetricsBaseHeight)),
                       -20000.0f, 20000.0f);
        resolvedVolumetricsHeightFalloff =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.HeightFalloff",
                                                       resolvedVolumetricsHeightFalloff)),
                       0.0f, 0.05f);
        resolvedVolumetricsLightShaftIntensity =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.LightShaftIntensity",
                                                       resolvedVolumetricsLightShaftIntensity)),
                       0.0f, 8.0f);
        resolvedVolumetricsShadowIntensity =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.ShadowIntensity",
                                                       resolvedVolumetricsShadowIntensity)),
                       0.0f, 1.0f);
        resolvedVolumetricsTemporalBlend =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.TemporalBlend",
                                                       resolvedVolumetricsTemporalBlend)),
                       0.0f, 0.99f);
        resolvedVolumetricsJitterScale =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.JitterScale",
                                                       resolvedVolumetricsJitterScale)),
                       0.0f, 4.0f);
        resolvedVolumetricsAerialPerspectiveStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.AerialPerspectiveStrength",
                                                       resolvedVolumetricsAerialPerspectiveStrength)),
                       0.0f, 4.0f);
        resolvedVolumetricsMacroNoiseStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.MacroNoiseStrength",
                                                       resolvedVolumetricsMacroNoiseStrength)),
                       0.0f, 3.0f);
        resolvedVolumetricsValleyFogStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.ValleyFogStrength",
                                                       resolvedVolumetricsValleyFogStrength)),
                       0.0f, 3.0f);
        resolvedVolumetricsEdgeHazeStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.EdgeHazeStrength",
                                                       resolvedVolumetricsEdgeHazeStrength)),
                       0.0f, 3.0f);
        resolvedVolumetricsDepthExtinctionStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DepthExtinctionStrength",
                                                       resolvedVolumetricsDepthExtinctionStrength)),
                       0.0f, 8.0f);
        resolvedVolumetricsDepthExtinctionExponent =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DepthExtinctionExponent",
                                                       resolvedVolumetricsDepthExtinctionExponent)),
                       0.25f, 6.0f);
        resolvedVolumetricsHorizonFogStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.HorizonFogStrength",
                                                       resolvedVolumetricsHorizonFogStrength)),
                       0.0f, 4.0f);
        resolvedVolumetricsSkyFallbackStrength =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.SkyFallbackStrength",
                                                       resolvedVolumetricsSkyFallbackStrength)),
                       0.0f, 2.0f);
        resolvedVolumetricsDirectionalShadowBias =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DirectionalShadowBias",
                                                       resolvedVolumetricsDirectionalShadowBias)),
                       0.0f, 0.1f);
        resolvedVolumetricsDirectionalShadowNormalBias =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DirectionalShadowNormalBias",
                                                       resolvedVolumetricsDirectionalShadowNormalBias)),
                       0.0f, 0.2f);
        resolvedVolumetricsDirectionalShadowSoftness =
            ClampValue(static_cast<float>(CVarGetFloat("gEnhancements.Graphics.Volumetrics.DirectionalShadowSoftness",
                                                       resolvedVolumetricsDirectionalShadowSoftness)),
                       0.0f, 4.0f);
        resolvedVolumetricsDebugMode = static_cast<uint8_t>(customVolumetricsDebugMode);
    }

    if (forceUiVolumetricsDebugMode) {
        resolvedVolumetricsDebugMode = requestedUiVolumetricsDebugMode;
    }

    const bool customOrDebugVolumetricsDemand = customVolumetricsActive || customSkylightActive || customTestLightActive ||
                                                resolvedVolumetricsDebugMode != Fast::GFX_VOLUMETRICS_DEBUG_OFF;
    if (customOrDebugVolumetricsDemand) {
        resolvedVolumetricsEnabled = true;
    }

    const bool volumetricsAutoDemanded = selectedVolumetricsPreset != nullptr || customVolumetricsActive ||
                                         customSkylightActive || customTestLightActive ||
                                         resolvedVolumetricsDebugMode != Fast::GFX_VOLUMETRICS_DEBUG_OFF ||
                                         hasActiveVolumetricLightRequest();
    if (resolvedVolumetricsEnabled && resolvedVolumetricsQuality == 0 && volumetricsAutoDemanded) {
        resolvedVolumetricsQuality = 2;
    }

    resolvedVolumetricsDensity = std::clamp(resolvedVolumetricsDensity * volumetricsDensityScale, 0.0f, 2.0f);
    resolvedVolumetricsStartDistance = std::max(0.0f, resolvedVolumetricsStartDistance);
    resolvedVolumetricsMaxDistance = std::max(resolvedVolumetricsStartDistance + 1.0f, resolvedVolumetricsMaxDistance);
    if (resolvedVolumetricsQuality == 0) {
        resolvedVolumetricsEnabled = false;
    }

    uint8_t resolvedVolumetricsShadowQuality = static_cast<uint8_t>(volumetricsShadowQualityCvar);
    if (resolvedVolumetricsShadowQuality == 0) {
        resolvedVolumetricsShadowQuality = resolvedVolumetricsEnabled ? resolvedVolumetricsQuality : 0;
    }
    const uint8_t maxShadowedLights =
        GetVolumetricsShadowBudget(resolvedVolumetricsQuality, resolvedVolumetricsShadowQuality);
    std::ostringstream volumetricsStateSignatureBuilder;
    volumetricsStateSignatureBuilder << (customVolumetricsActive ? 1 : 0) << "|"
                                     << resolvedVolumetricsQuality << "|" << static_cast<int32_t>(resolvedVolumetricsPerformanceMode)
                                     << "|" << resolvedVolumetricsDensity << "|"
                                     << resolvedVolumetricsColor[0] << "|" << resolvedVolumetricsColor[1] << "|"
                                     << resolvedVolumetricsColor[2] << "|" << resolvedVolumetricsAmbientIntensity << "|"
                                     << resolvedVolumetricsAnisotropy << "|" << resolvedVolumetricsStartDistance << "|"
                                     << resolvedVolumetricsMaxDistance << "|" << (resolvedVolumetricsHeightFogEnabled ? 1 : 0)
                                     << "|" << resolvedVolumetricsBaseHeight << "|" << resolvedVolumetricsHeightFalloff
                                     << "|" << resolvedVolumetricsLightShaftIntensity << "|" << resolvedVolumetricsShadowIntensity
                                     << "|" << resolvedVolumetricsTemporalBlend << "|" << resolvedVolumetricsJitterScale << "|"
                                     << resolvedVolumetricsAerialPerspectiveStrength << "|"
                                     << resolvedVolumetricsMacroNoiseStrength << "|"
                                     << resolvedVolumetricsValleyFogStrength << "|"
                                     << resolvedVolumetricsEdgeHazeStrength << "|"
                                     << resolvedVolumetricsDepthExtinctionStrength << "|"
                                     << resolvedVolumetricsDepthExtinctionExponent << "|"
                                     << resolvedVolumetricsHorizonFogStrength << "|"
                                     << resolvedVolumetricsSkyFallbackStrength << "|"
                                     << resolvedVolumetricsDirectionalShadowBias << "|"
                                     << resolvedVolumetricsDirectionalShadowNormalBias << "|"
                                     << resolvedVolumetricsDirectionalShadowSoftness << "|"
                                     << static_cast<int32_t>(resolvedVolumetricsDebugMode) << "|"
                                     << (customSkylightActive ? 1 : 0) << "|" << customSkylightIntensity << "|"
                                     << customSkylightVolumetricIntensity << "|" << customSkylightDirection[0] << "|"
                                     << customSkylightDirection[1] << "|" << customSkylightDirection[2] << "|"
                                     << ToNormalizedRgb(customSkylightColor)[0] << "|" << ToNormalizedRgb(customSkylightColor)[1]
                                     << "|" << ToNormalizedRgb(customSkylightColor)[2] << "|"
                                     << (customTestLightActive ? 1 : 0) << "|" << customTestLightTypeIndex << "|"
                                     << customTestLightIntensity << "|" << customTestLightVolumetricIntensity << "|"
                                     << customTestLightOffsetX << "|" << customTestLightOffsetY << "|" << customTestLightOffsetZ
                                     << "|" << customTestLightDirection[0] << "|" << customTestLightDirection[1] << "|"
                                     << customTestLightDirection[2] << "|" << customTestLightRadius << "|"
                                     << customTestLightInnerCone << "|" << customTestLightOuterCone << "|"
                                     << (customTestLightCastShadows ? 1 : 0);
    const std::string volumetricsStateSignature = volumetricsStateSignatureBuilder.str();
    const bool volumetricsStateChanged = volumetricsStateSignature != mImpl->lastVolumetricsStateSignature;
    const bool volumetricsResetHistory =
        volumetricsStateChanged ||
        renderProfileChanged || !mImpl->previousViewProjectionValid || sceneId != mImpl->previousSceneId ||
        roomId != mImpl->previousRoomId;
    mImpl->lastVolumetricsStateSignature = volumetricsStateSignature;

    float currentViewProjection[16];
    float inverseViewProjection[16];
    float previousViewProjection[16];
    int32_t pendingVolumetricsFallbackReason = 0;
    SetIdentityMatrix(currentViewProjection);
    SetIdentityMatrix(inverseViewProjection);
    std::copy(mImpl->previousViewProjection.begin(), mImpl->previousViewProjection.end(), previousViewProjection);

    CopyMatrixToColumnMajor(play->viewProjectionMtxF, currentViewProjection);
    MtxF inverseViewProjectionMtx{};
    const bool haveInverseViewProjection = SkinMatrix_Invert(&play->viewProjectionMtxF, &inverseViewProjectionMtx) == 2;
    if (haveInverseViewProjection) {
        CopyMatrixToColumnMajor(inverseViewProjectionMtx, inverseViewProjection);
    } else {
        resolvedVolumetricsEnabled = false;
        pendingVolumetricsFallbackReason = 3;
    }

    const bool shouldRunVolumetrics = resolvedVolumetricsQuality > 0 &&
                                      (resolvedVolumetricsEnabled || customVolumetricsActive ||
                                       resolvedVolumetricsDebugMode != Fast::GFX_VOLUMETRICS_DEBUG_OFF);

    gfx_set_volumetrics_debug_mode(resolvedVolumetricsDebugMode);
    gfx_set_volumetrics_appearance(resolvedVolumetricsColor[0], resolvedVolumetricsColor[1], resolvedVolumetricsColor[2],
                                   resolvedVolumetricsAmbientIntensity);
    gfx_set_volumetrics_config(resolvedVolumetricsDensity, resolvedVolumetricsAnisotropy, resolvedVolumetricsStartDistance,
                               resolvedVolumetricsMaxDistance, resolvedVolumetricsQuality, resolvedVolumetricsShadowQuality,
                               resolvedVolumetricsBaseHeight, resolvedVolumetricsHeightFalloff,
                               resolvedVolumetricsPerformanceMode, resolvedVolumetricsHeightFogEnabled ? 1 : 0, resolvedVolumetricsLightShaftIntensity,
                               resolvedVolumetricsShadowIntensity, resolvedVolumetricsTemporalBlend,
                               resolvedVolumetricsJitterScale, resolvedVolumetricsAerialPerspectiveStrength,
                               resolvedVolumetricsMacroNoiseStrength, resolvedVolumetricsValleyFogStrength,
                               resolvedVolumetricsEdgeHazeStrength, resolvedVolumetricsDepthExtinctionStrength,
                               resolvedVolumetricsDepthExtinctionExponent, resolvedVolumetricsHorizonFogStrength,
                               resolvedVolumetricsSkyFallbackStrength, resolvedVolumetricsDirectionalShadowBias,
                               resolvedVolumetricsDirectionalShadowNormalBias,
                               resolvedVolumetricsDirectionalShadowSoftness, maxShadowedLights,
                               volumetricsResetHistory ? 1 : 0);
    gfx_set_volumetrics_camera(currentViewProjection, inverseViewProjection, previousViewProjection, play->view.eye.x,
                               play->view.eye.y, play->view.eye.z, play->view.zNear, play->view.zFar);
    gfx_set_volumetrics_enabled(shouldRunVolumetrics ? 1 : 0);

    mImpl->volumetricsEnabled = shouldRunVolumetrics;
    mImpl->volumetricsQuality = resolvedVolumetricsQuality;
    mImpl->volumetricsShadowQuality = resolvedVolumetricsShadowQuality;
    mImpl->volumetricsPerformanceMode = resolvedVolumetricsPerformanceMode;
    mImpl->resolvedVolumetricsColor = resolvedVolumetricsColor;
    mImpl->resolvedVolumetricsAmbientIntensity = resolvedVolumetricsAmbientIntensity;
    mImpl->volumetricsDebugMode = resolvedVolumetricsDebugMode;
    if (selectedVolumetricsPreset != nullptr) {
        mImpl->volumetricsSourceProfileId = selectedVolumetricsPreset->id;
    }
    if (selectedVolumetricsPackage != nullptr) {
        mImpl->volumetricsSourceModId = selectedVolumetricsPackage->manifest.id;
    }

    struct CandidateLight {
        const ExternalModPackage* package = nullptr;
        ExternalModRuntime::DynamicLightState* state = nullptr;
        const ExternalModLightProfileDefinition* profile = nullptr;
        float distanceSq = std::numeric_limits<float>::max();
        float posX = 0.0f;
        float posY = 0.0f;
        float posZ = 0.0f;
        int32_t handle = 0;
        std::string sourceId;
    };
    std::vector<CandidateLight> candidates;
    candidates.reserve(256);

    const Player* player = GET_PLAYER(play);
    Vec3f playerPos{ 0.0f, 0.0f, 0.0f };
    if (player != nullptr) {
        playerPos = player->actor.world.pos;
    }

    auto resolveLightPosition = [&](const ExternalModPackage& package, ExternalModRuntime::DynamicLightState& lightState,
                                    float& outX, float& outY, float& outZ) -> bool {
        if (lightState.attach == "world") {
            if (lightState.hasWorldPos) {
                outX = lightState.posX;
                outY = lightState.posY;
                outZ = lightState.posZ;
                return true;
            }
            return false;
        }

        if (lightState.attach == "actor" || lightState.actorHandle != 0) {
            if (lightState.actorHandle != 0) {
                auto actorIt =
                    std::find_if(package.runtime.actorInstances.begin(), package.runtime.actorInstances.end(),
                                 [&](const ExternalModActorInstance& actorInstance) {
                                     return actorInstance.active && actorInstance.handle == lightState.actorHandle;
                                 });
                if (actorIt != package.runtime.actorInstances.end()) {
                    outX = actorIt->posX;
                    outY = actorIt->posY;
                    outZ = actorIt->posZ;
                    return true;
                }
            }
            if (lightState.actorAddress != 0) {
                const auto* actor = reinterpret_cast<const Actor*>(lightState.actorAddress);
                outX = actor->world.pos.x;
                outY = actor->world.pos.y;
                outZ = actor->world.pos.z;
                return true;
            }
            if (!lightState.follow && lightState.hasWorldPos) {
                outX = lightState.posX;
                outY = lightState.posY;
                outZ = lightState.posZ;
                return true;
            }
            return false;
        }

        if (player != nullptr) {
            outX = player->actor.world.pos.x;
            outY = player->actor.world.pos.y;
            outZ = player->actor.world.pos.z;
            return true;
        }

        if (lightState.hasWorldPos) {
            outX = lightState.posX;
            outY = lightState.posY;
            outZ = lightState.posZ;
            return true;
        }

        return false;
    };

    for (auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }

        for (auto& lightState : package.runtime.activeDynamicLights) {
            const ExternalModPackage* profileOwner = nullptr;
            const auto* profile = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
                packages, lightState.profileId,
                [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                    return runtime.lightProfiles;
                },
                &profileOwner);
            if (profile == nullptr) {
                continue;
            }

            float x = lightState.posX;
            float y = lightState.posY;
            float z = lightState.posZ;
            if (resolveLightPosition(package, lightState, x, y, z)) {
                lightState.posX = x;
                lightState.posY = y;
                lightState.posZ = z;
                lightState.hasWorldPos = true;
            } else if (!lightState.hasWorldPos) {
                continue;
            }

            const float dx = lightState.posX - playerPos.x;
            const float dy = lightState.posY - playerPos.y;
            const float dz = lightState.posZ - playerPos.z;
            CandidateLight candidate;
            candidate.package = &package;
            candidate.state = &lightState;
            candidate.profile = profile;
            candidate.distanceSq = dx * dx + dy * dy + dz * dz;
            candidate.posX = lightState.posX;
            candidate.posY = lightState.posY;
            candidate.posZ = lightState.posZ;
            candidate.handle = lightState.handle;
            candidate.sourceId = package.manifest.id;
            candidates.push_back(candidate);
        }
    }

    ExternalModLightProfileDefinition customTestLightProfile{};
    if (customTestLightActive) {
        customTestLightProfile.id = "__custom_test_light";
        customTestLightProfile.lightType = customTestLightType;
        customTestLightProfile.colorLinear = ToNormalizedRgb(customTestLightColor);
        customTestLightProfile.intensity = customTestLightIntensity;
        customTestLightProfile.radius = customTestLightRadius;
        customTestLightProfile.direction = customTestLightDirection;
        customTestLightProfile.innerConeDeg = customTestLightInnerCone;
        customTestLightProfile.outerConeDeg = std::max(customTestLightInnerCone, customTestLightOuterCone);
        customTestLightProfile.castShadows = (customTestLightTypeIndex == 0) ? true : customTestLightCastShadows;
        customTestLightProfile.shadowResolution = 1024;
        customTestLightProfile.shadowBias = 0.01f;
        customTestLightProfile.shadowNormalBias = 0.02f;
        customTestLightProfile.shadowRange = std::max(customTestLightRadius * 2.0f, 1200.0f);
        customTestLightProfile.volumetricIntensity = customTestLightVolumetricIntensity;

        CandidateLight candidate;
        candidate.package = nullptr;
        candidate.profile = &customTestLightProfile;
        candidate.distanceSq = -1.0f;
        candidate.posX = playerPos.x + customTestLightOffsetX;
        candidate.posY = playerPos.y + customTestLightOffsetY;
        candidate.posZ = playerPos.z + customTestLightOffsetZ;
        candidate.handle = -1;
        candidate.sourceId = "global.debug_light";
        candidates.push_back(candidate);
    }

    std::sort(candidates.begin(), candidates.end(), [](const CandidateLight& lhs, const CandidateLight& rhs) {
        if (lhs.distanceSq != rhs.distanceSq) {
            return lhs.distanceSq < rhs.distanceSq;
        }
        return lhs.handle < rhs.handle;
    });

    const int32_t lightCount = std::min<int32_t>(maxDynamicLightsTotal, static_cast<int32_t>(candidates.size()));
    mImpl->frameLights.reserve(static_cast<size_t>(lightCount));
    std::vector<Fast::GfxVolumetricLight> volumetricLights;
    volumetricLights.reserve(Fast::GFX_MAX_VOLUMETRIC_LIGHTS);
    uint8_t remainingVolumetricShadowBudget = maxShadowedLights;

    auto pushVolumetricLight = [&](const ExternalModLightProfileDefinition& profile, const std::array<float, 3>& colorLinear,
                                   float intensity, float positionX, float positionY, float positionZ, int32_t handle) {
        if (!mImpl->volumetricsEnabled || volumetricLights.size() >= Fast::GFX_MAX_VOLUMETRIC_LIGHTS) {
            return;
        }

        Fast::GfxVolumetricLight volumetricLight{};
        const std::string lightType = profile.lightType;
        if (lightType == "directional") {
            volumetricLight.type = Fast::GFX_VOLUMETRIC_LIGHT_DIRECTIONAL;
        } else if (lightType == "spot") {
            volumetricLight.type = Fast::GFX_VOLUMETRIC_LIGHT_SPOT;
        } else {
            volumetricLight.type = Fast::GFX_VOLUMETRIC_LIGHT_POINT;
        }

        volumetricLight.position[0] = positionX;
        volumetricLight.position[1] = positionY;
        volumetricLight.position[2] = positionZ;

        const auto normalizedDirection = NormalizeVector(profile.direction);
        volumetricLight.direction[0] = normalizedDirection[0];
        volumetricLight.direction[1] = normalizedDirection[1];
        volumetricLight.direction[2] = normalizedDirection[2];
        volumetricLight.color[0] = std::clamp(colorLinear[0], 0.0f, 16.0f);
        volumetricLight.color[1] = std::clamp(colorLinear[1], 0.0f, 16.0f);
        volumetricLight.color[2] = std::clamp(colorLinear[2], 0.0f, 16.0f);
        volumetricLight.intensity = std::clamp(intensity, 0.0f, 64.0f);
        volumetricLight.radius = std::clamp(profile.radius, 0.0f, 20000.0f);
        volumetricLight.volumetricIntensity = std::clamp(profile.volumetricIntensity, 0.0f, 8.0f);
        volumetricLight.innerConeCos =
            std::cos(std::clamp(profile.innerConeDeg, 0.0f, 180.0f) * (kPi / 180.0f));
        volumetricLight.outerConeCos =
            std::cos(std::clamp(profile.outerConeDeg, 0.0f, 180.0f) * (kPi / 180.0f));
        if (volumetricLight.innerConeCos < volumetricLight.outerConeCos) {
            std::swap(volumetricLight.innerConeCos, volumetricLight.outerConeCos);
        }
        volumetricLight.shadowBias = std::clamp(profile.shadowBias, 0.0f, 1.0f);
        volumetricLight.shadowNormalBias = std::clamp(profile.shadowNormalBias, 0.0f, 2.0f);
        volumetricLight.shadowRange = std::clamp(profile.shadowRange, 0.0f, 20000.0f);
        volumetricLight.shadowResolution = static_cast<float>(std::clamp(profile.shadowResolution, 64, 4096));
        volumetricLight.castShadows =
            (profile.castShadows && resolvedVolumetricsShadowQuality > 0 && remainingVolumetricShadowBudget > 0) ? 1 : 0;
        if (volumetricLight.castShadows != 0) {
            remainingVolumetricShadowBudget--;
            mImpl->volumetricShadowLightCount++;
        }
        volumetricLights.push_back(volumetricLight);
        mImpl->volumetricLightCount = volumetricLights.size();
    };

    if (effectiveSkylight != nullptr) {
        std::array<float, 3> skylightColor = effectiveSkylight->colorLinear;
        if (effectiveSkylight->hasKelvin) {
            skylightColor = KelvinToRgb(effectiveSkylight->kelvin);
        }
        pushVolumetricLight(*effectiveSkylight, skylightColor, ClampValue(effectiveSkylight->intensity, 0.0f, 8.0f),
                            play->view.eye.x, play->view.eye.y, play->view.eye.z, 0);
    }

    for (int32_t index = 0; index < lightCount; ++index) {
        const auto& candidate = candidates[static_cast<size_t>(index)];
        auto& frameLight = mImpl->frameLights.emplace_back();
        frameLight.modId = !candidate.sourceId.empty()
                               ? candidate.sourceId
                               : (candidate.package != nullptr ? candidate.package->manifest.id : "global");
        frameLight.handle = candidate.handle;

        std::array<float, 3> colorLinear = candidate.profile->colorLinear;
        if (candidate.profile->hasKelvin) {
            colorLinear = KelvinToRgb(candidate.profile->kelvin);
        }
        float intensity = ClampValue(candidate.profile->intensity, 0.0f, 8.0f);
        if (candidate.state != nullptr && candidate.profile->flicker && candidate.profile->flickerAmount > 0.0f) {
            const float phase = static_cast<float>((candidate.handle * 37 + static_cast<int32_t>(play->state.frames)) % 89);
            const float pulse = std::sin(phase * 0.321f);
            intensity *= ClampValue(1.0f + pulse * candidate.profile->flickerAmount, 0.0f, 2.0f);
        }

        pushVolumetricLight(*candidate.profile, colorLinear, intensity, candidate.posX, candidate.posY, candidate.posZ,
                            candidate.handle);

        const int16_t x = static_cast<int16_t>(ClampValue(candidate.posX, -32000.0f, 32000.0f));
        const int16_t y = static_cast<int16_t>(ClampValue(candidate.posY, -32000.0f, 32000.0f));
        const int16_t z = static_cast<int16_t>(ClampValue(candidate.posZ, -32000.0f, 32000.0f));
        const uint8_t r = ToByteColor(colorLinear[0] * intensity);
        const uint8_t g = ToByteColor(colorLinear[1] * intensity);
        const uint8_t b = ToByteColor(colorLinear[2] * intensity);
        const int16_t radius = static_cast<int16_t>(ClampValue(candidate.profile->radius, -32000.0f, 32000.0f));

        const std::string lightType = candidate.profile->lightType;
        if (lightType == "directional") {
            frameLight.info.type = LIGHT_DIRECTIONAL;
            frameLight.info.params.dir.x =
                static_cast<int8_t>(ClampValue(candidate.profile->direction[0] * 127.0f, -127.0f, 127.0f));
            frameLight.info.params.dir.y =
                static_cast<int8_t>(ClampValue(candidate.profile->direction[1] * 127.0f, -127.0f, 127.0f));
            frameLight.info.params.dir.z =
                static_cast<int8_t>(ClampValue(candidate.profile->direction[2] * 127.0f, -127.0f, 127.0f));
            frameLight.info.params.dir.color[0] = r;
            frameLight.info.params.dir.color[1] = g;
            frameLight.info.params.dir.color[2] = b;
        } else {
            if (lightType == "spot") {
                ExternalModHookEventContext fallbackContext;
                fallbackContext.scene = sceneId;
                fallbackContext.actorHandle = candidate.handle;
                fallbackContext.value = "LIGHT_TYPE_SPOT_TO_POINT";
                manager.EmitExtendedHook(ExternalModHookType::OnRenderFallbackApplied, fallbackContext,
                                         "OnRenderFallbackApplied");
            }
            Lights_PointNoGlowSetInfo(&frameLight.info, x, y, z, r, g, b, radius);
        }

        frameLight.node = LightContext_InsertLight(play, &play->lightCtx, &frameLight.info);
    }

    gfx_set_volumetric_lights(volumetricLights.empty() ? nullptr : volumetricLights.data(), volumetricLights.size());

    if (mImpl->volumetricsEnabled) {
        Fast::GfxVolumetricsRuntimeStats volumetricsStats{};
        gfx_get_volumetrics_runtime_stats(&volumetricsStats);
        mImpl->volumetricsDispatched = volumetricsStats.dispatched;
        mImpl->volumetricsProducedMedium = volumetricsStats.producedMedium;
        mImpl->volumetricsProduced = volumetricsStats.producedFinalVolume;
        mImpl->volumetricsHasActiveLightPayload = volumetricsStats.hasActiveLightPayload;
        mImpl->volumetricsDepthFogPathActive = volumetricsStats.depthFogPathActive;
        mImpl->volumetricsRaymarchActive = volumetricsStats.raymarchActive;
        mImpl->volumetricsPerformanceMode = volumetricsStats.performanceMode;
        mImpl->volumetricsShadowMode = volumetricsStats.shadowMode;
        mImpl->volumetricsPayloadLightCount = volumetricsStats.payloadLightCount;
        mImpl->volumetricsDebugPreviewValid = volumetricsStats.debugPreviewValid;
        mImpl->volumetricsLowWidth = volumetricsStats.lowWidth;
        mImpl->volumetricsLowHeight = volumetricsStats.lowHeight;
        mImpl->volumetricsStepCount = volumetricsStats.stepCount;
        mImpl->volumetricsRaymarchPixelCount = volumetricsStats.raymarchPixelCount;
        mImpl->volumetricsStatsAgeFrames = volumetricsStats.statsAgeFrames;
        mImpl->volumetricsMarchedPixelCount = volumetricsStats.marchedPixelCount;
        mImpl->volumetricsHistoryValid = volumetricsStats.historyValid;
        mImpl->volumetricsAvgDensity = volumetricsStats.avgDensity;
        mImpl->volumetricsAvgLightEnergy = volumetricsStats.avgLightEnergy;
        mImpl->volumetricsAvgFinalVolume = volumetricsStats.avgFinalVolume;
        mImpl->volumetricsAvgTransmittance = volumetricsStats.avgTransmittance;
        mImpl->volumetricsMaxDensity = volumetricsStats.maxDensity;
        mImpl->volumetricsMaxLightEnergy = volumetricsStats.maxLightEnergy;
        mImpl->volumetricsMaxFinalVolume = volumetricsStats.maxFinalVolume;
        mImpl->volumetricsMaxOpticalDepth = volumetricsStats.maxOpticalDepth;
        mImpl->volumetricsDirectionalShadowMapSize = volumetricsStats.directionalShadowMapSize;
        mImpl->volumetricsDirectionalShadowCasterIndex = volumetricsStats.directionalShadowCasterIndex;
    }

    int32_t volumetricsFallbackReason = gfx_get_volumetrics_fallback_reason();
    if (volumetricsFallbackReason == 0) {
        volumetricsFallbackReason = pendingVolumetricsFallbackReason;
    }
    if (mImpl->volumetricsDispatched) {
        volumetricsFallbackReason = 0;
    }
    if (mImpl->volumetricsEnabled && volumetricsFallbackReason != 0 &&
        volumetricsFallbackReason != mImpl->volumetricsFallbackReason) {
        ExternalModHookEventContext fallbackContext;
        fallbackContext.scene = sceneId;
        fallbackContext.value = GetVolumetricsFallbackReasonName(volumetricsFallbackReason);
        manager.EmitExtendedHook(ExternalModHookType::OnRenderFallbackApplied, fallbackContext, "OnRenderFallbackApplied");
        mImpl->volumetricsFallbackReason = volumetricsFallbackReason;
    } else if (volumetricsFallbackReason == 0) {
        mImpl->volumetricsFallbackReason = 0;
    }
    gfx_clear_volumetrics_fallback_reason();

    if (mImpl->volumetricsEnabled && haveInverseViewProjection) {
        std::copy(std::begin(currentViewProjection), std::end(currentViewProjection), mImpl->previousViewProjection.begin());
        mImpl->previousViewProjectionValid = true;
    } else {
        SetIdentityMatrix(mImpl->previousViewProjection.data());
        mImpl->previousViewProjectionValid = false;
    }
    mImpl->previousSceneId = sceneId;
    mImpl->previousRoomId = roomId;

    std::ostringstream inspector;
    const std::string directionalShadowCaster =
        mImpl->volumetricsDirectionalShadowCasterIndex >= 0
            ? (std::string("light#") + std::to_string(mImpl->volumetricsDirectionalShadowCasterIndex))
            : "none";
    const std::string skylightId = customSkylightActive ? "__custom_skylight"
                                                        : (effectiveSkylight != nullptr ? effectiveSkylight->id : "none");
    const std::string skylightModId =
        customSkylightActive ? "global"
                             : (effectiveSkylightSource != nullptr ? effectiveSkylightSource->manifest.id : "none");
    inspector << "backend=" << (!currentBackendName.empty() ? currentBackendName : "none")
              << " scene=" << sceneId << " room=" << roomId << " sceneProfile="
              << (resolvedState.sceneProfile != nullptr ? resolvedState.sceneProfile->id : "none") << "@"
              << (resolvedState.sceneProfileSource != nullptr ? resolvedState.sceneProfileSource->manifest.id : "none")
              << " roomProfile=" << (resolvedState.roomProfile != nullptr ? resolvedState.roomProfile->id : "none") << "@"
              << (resolvedState.roomProfileSource != nullptr ? resolvedState.roomProfileSource->manifest.id : "none")
              << " postFx=" << (resolvedState.postFx != nullptr ? resolvedState.postFx->id : "none") << "@"
              << (resolvedState.postFxSource != nullptr ? resolvedState.postFxSource->manifest.id : "none") << "(custom="
              << (mImpl->customPostFxActive ? "on" : "off") << ",exp=" << mImpl->resolvedPostFxExposure << ",bloom="
              << mImpl->resolvedPostFxBloom << ",sat=" << mImpl->resolvedPostFxSaturation << ",fogDensity="
              << mImpl->resolvedPostFxFogDensity << ",fogColor=" << mImpl->resolvedPostFxFogColor[0] << "/"
              << mImpl->resolvedPostFxFogColor[1] << "/" << mImpl->resolvedPostFxFogColor[2] << ")"
              << " skylight=" << skylightId << "@" << skylightModId << "(custom="
              << (customSkylightActive ? "on" : "off") << ",intensity=" << customSkylightIntensity << ",vol="
              << customSkylightVolumetricIntensity << ",dir=" << customSkylightDirection[0] << "/"
              << customSkylightDirection[1] << "/" << customSkylightDirection[2] << ")"
              << " lights=" << mImpl->frameLights.size() << "/" << maxDynamicLightsTotal << "(testLight="
              << (customTestLightActive ? "on" : "off") << ",type=" << customTestLightType << ",intensity="
              << customTestLightIntensity << ",vol=" << customTestLightVolumetricIntensity << ")" << " nearBudget="
              << maxDynamicLightsNear
              << " fogOverlay=" << (mImpl->forcedFogOverlayActive ? "on" : "off")
              << " depthFog=" << (mImpl->depthAwareFogForced ? "on" : "off")
              << " pbr=" << (mImpl->pbrEnabled ? "on" : "off") << "(profile="
              << (!mImpl->pbrSourceProfileId.empty() ? mImpl->pbrSourceProfileId : "none") << ",mod="
              << (!mImpl->pbrSourceModId.empty() ? mImpl->pbrSourceModId : "none") << ",policy="
              << (!mImpl->pbrBackendPolicy.empty() ? mImpl->pbrBackendPolicy : "auto") << ",backend="
              << (!mImpl->pbrBackendName.empty() ? mImpl->pbrBackendName : "none") << ",pom="
              << (mImpl->pbrPomEnabled ? "on" : "off") << "/" << mImpl->pbrPomSteps << ",pomDist="
              << mImpl->pbrPomMaxDistance << ",bound=" << mImpl->pbrBoundMaterialDefinitionCount << ",boundAlbedo="
              << mImpl->pbrBoundAlbedoMaterialDefinitionCount << ",overrides=" << mImpl->pbrActiveMaterialOverrideCount
              << ",fallback="
              << (mImpl->pbrFallbackReason != 0 ? GetPbrFallbackReasonName(mImpl->pbrFallbackReason) : "none") << ")"
              << " ao=" << (mImpl->aoEnabled ? "on" : "off") << "(q=" << static_cast<int32_t>(mImpl->aoQuality)
              << ",profile=" << (!mImpl->aoSourceProfileId.empty() ? mImpl->aoSourceProfileId : "none")
              << ",mod=" << (!mImpl->aoSourceModId.empty() ? mImpl->aoSourceModId : "none") << ",fallback="
              << (mImpl->aoFallbackReason != 0 ? GetAmbientOcclusionFallbackReasonName(mImpl->aoFallbackReason) : "none")
              << ")"
              << " volumetrics=" << (mImpl->volumetricsEnabled ? "on" : "off")
              << "(master=" << (CVarGetInteger("gEnhancements.Graphics.Volumetrics.Enabled", 1) != 0 ? "on" : "off")
              << ",custom=" << (CVarGetInteger("gEnhancements.Graphics.Volumetrics.UseCustom", 0) != 0 ? "on" : "off")
              << ",perfMode=" << GetVolumetricsPerformanceModeLabel(mImpl->volumetricsPerformanceMode)
              << ",q=" << static_cast<int32_t>(mImpl->volumetricsQuality) << ",shadowQ="
              << static_cast<int32_t>(mImpl->volumetricsShadowQuality) << ",profile="
              << (!mImpl->volumetricsSourceProfileId.empty() ? mImpl->volumetricsSourceProfileId : "none") << ",mod="
              << (!mImpl->volumetricsSourceModId.empty() ? mImpl->volumetricsSourceModId : "none") << ",runtimeLights="
              << mImpl->volumetricLightCount << ",payloadLights=" << mImpl->volumetricsPayloadLightCount
              << ",shadowed=" << mImpl->volumetricShadowLightCount << ",hasActiveLightPayload="
              << (mImpl->volumetricsHasActiveLightPayload ? "yes" : "no") << ",depthFogPath="
              << (mImpl->volumetricsDepthFogPathActive ? "yes" : "no") << ",raymarch="
              << (mImpl->volumetricsRaymarchActive ? "yes" : "no") << ",raymarchPixels="
              << mImpl->volumetricsRaymarchPixelCount << ",shadowMode="
              << GetVolumetricsShadowModeLabel(mImpl->volumetricsShadowMode) << ",statsAge="
              << mImpl->volumetricsStatsAgeFrames << ",dispatched="
              << (mImpl->volumetricsDispatched ? "yes" : "no") << ",medium="
              << (mImpl->volumetricsProducedMedium ? "yes" : "no") << ",producedFinal="
              << (mImpl->volumetricsProduced ? "yes" : "no") << ",debugValid="
              << (mImpl->volumetricsDebugPreviewValid ? "yes" : "no") << ",debug="
              << GetVolumetricsDebugLabel(mImpl->volumetricsDebugMode)
              << ",color=" << mImpl->resolvedVolumetricsColor[0] << "/" << mImpl->resolvedVolumetricsColor[1] << "/"
              << mImpl->resolvedVolumetricsColor[2] << ",ambient=" << mImpl->resolvedVolumetricsAmbientIntensity
              << ",low=" << mImpl->volumetricsLowWidth << "x" << mImpl->volumetricsLowHeight << ",steps="
              << mImpl->volumetricsStepCount << ",marched=" << mImpl->volumetricsMarchedPixelCount << ",history="
              << (mImpl->volumetricsHistoryValid ? "yes" : "no") << ",avgDensity=" << mImpl->volumetricsAvgDensity
              << ",avgLight=" << mImpl->volumetricsAvgLightEnergy << ",avgFinal="
              << mImpl->volumetricsAvgFinalVolume << ",avgTrans=" << mImpl->volumetricsAvgTransmittance
              << ",maxDensity=" << mImpl->volumetricsMaxDensity << ",maxLight="
              << mImpl->volumetricsMaxLightEnergy << ",maxFinal=" << mImpl->volumetricsMaxFinalVolume
              << ",maxOpticalDepth=" << mImpl->volumetricsMaxOpticalDepth << ",shadowMapSize="
              << mImpl->volumetricsDirectionalShadowMapSize << ",shadowCaster=" << directionalShadowCaster
              << ",fallback="
              << (mImpl->volumetricsFallbackReason != 0 ? GetVolumetricsFallbackReasonName(mImpl->volumetricsFallbackReason)
                                                        : "none")
              << ")";
    mImpl->inspectorSummary = inspector.str();
}

void ExternalModWorldGraphicsRuntime::OnPlayDrawEnd(PlayState* play) {
    gfx_set_force_depth_aware_fog(0);
    // Preserve the resolved volumetrics enable state across Play frames so StartFrame() can still force the
    // single-sample/offscreen path required by ApplyVolumetrics(), including debug visualization.

    if (play != nullptr) {
        for (auto& frameLight : mImpl->frameLights) {
            if (frameLight.node != nullptr) {
                LightContext_RemoveLight(play, &play->lightCtx, frameLight.node);
                frameLight.node = nullptr;
            }
        }
    }
    mImpl->frameLights.clear();

    if (!mImpl->envCaptured || play == nullptr) {
        return;
    }

    for (size_t i = 0; i < 3; ++i) {
        play->lightCtx.ambientColor[i] = mImpl->lightAmbientColor[i];
        play->lightCtx.fogColor[i] = mImpl->lightFogColor[i];
        play->envCtx.lightSettings.ambientColor[i] = mImpl->envAmbientColor[i];
        play->envCtx.lightSettings.fogColor[i] = mImpl->envFogColor[i];
        play->envCtx.lightSettings.light1Dir[i] = mImpl->envLight1Dir[i];
        play->envCtx.lightSettings.light2Dir[i] = mImpl->envLight2Dir[i];
        play->envCtx.lightSettings.light1Color[i] = mImpl->envLight1Color[i];
        play->envCtx.lightSettings.light2Color[i] = mImpl->envLight2Color[i];
    }
    play->lightCtx.fogNear = mImpl->lightFogNear;
    play->lightCtx.fogFar = mImpl->lightFogFar;
    play->envCtx.lightSettings.fogNear = mImpl->envFogNear;
    play->envCtx.lightSettings.fogFar = mImpl->envFogFar;
    if (mImpl->forcedFogOverlayActive) {
        const bool overlayStillActive = play->envCtx.fillScreen &&
                                        std::equal(play->envCtx.screenFillColor, play->envCtx.screenFillColor + 4,
                                                   mImpl->appliedFogOverlayColor.begin());
        if (overlayStillActive) {
            play->envCtx.fillScreen = mImpl->envFillScreen;
            for (size_t i = 0; i < 4; ++i) {
                play->envCtx.screenFillColor[i] = mImpl->envScreenFillColor[i];
            }
        }
    }
    mImpl->forcedFogOverlayActive = false;
    mImpl->depthAwareFogForced = false;
    mImpl->pbrEnabled = false;
    mImpl->pbrPomEnabled = false;
    mImpl->pbrPomSteps = 0;
    mImpl->pbrPomMaxDistance = 0.0f;
    mImpl->pbrFallbackReason = 0;
    mImpl->pbrBackendName.clear();
    mImpl->pbrBackendPolicy.clear();
    mImpl->pbrSourceProfileId.clear();
    mImpl->pbrSourceModId.clear();
    mImpl->pbrBoundMaterialDefinitionCount = 0;
    mImpl->pbrBoundAlbedoMaterialDefinitionCount = 0;
    mImpl->pbrActiveMaterialOverrideCount = 0;
    mImpl->aoEnabled = false;
    mImpl->aoQuality = 0;
    mImpl->aoFallbackReason = 0;
    mImpl->aoSourceProfileId.clear();
    mImpl->aoSourceModId.clear();
    mImpl->volumetricsEnabled = false;
    mImpl->volumetricsQuality = 0;
    mImpl->volumetricsShadowQuality = 0;
    mImpl->volumetricsFallbackReason = 0;
    mImpl->volumetricsSourceProfileId.clear();
    mImpl->volumetricsSourceModId.clear();
    mImpl->customSkylightActive = false;
    mImpl->customTestLightActive = false;
    mImpl->customTestLightType.clear();
    mImpl->volumetricsDispatched = false;
    mImpl->volumetricsProducedMedium = false;
    mImpl->volumetricsProduced = false;
    mImpl->volumetricsHasActiveLightPayload = false;
    mImpl->volumetricsPayloadLightCount = 0;
    mImpl->volumetricsDebugPreviewValid = false;
    mImpl->volumetricsLowWidth = 0;
    mImpl->volumetricsLowHeight = 0;
    mImpl->volumetricsStepCount = 0;
    mImpl->volumetricsMarchedPixelCount = 0;
    mImpl->volumetricsHistoryValid = false;
    mImpl->volumetricsAvgDensity = 0.0f;
    mImpl->volumetricsAvgLightEnergy = 0.0f;
    mImpl->volumetricsAvgFinalVolume = 0.0f;
    mImpl->volumetricsAvgTransmittance = 1.0f;
    mImpl->volumetricsMaxDensity = 0.0f;
    mImpl->volumetricsMaxLightEnergy = 0.0f;
    mImpl->volumetricsMaxFinalVolume = 0.0f;
    mImpl->volumetricsMaxOpticalDepth = 0.0f;
    mImpl->volumetricsDirectionalShadowMapSize = 0;
    mImpl->volumetricsDirectionalShadowCasterIndex = -1;
    mImpl->volumetricLightCount = 0;
    mImpl->volumetricShadowLightCount = 0;
    mImpl->envCaptured = false;
}

void ExternalModWorldGraphicsRuntime::Reset(PlayState* play) {
    OnPlayDrawEnd(play);
    mImpl->lastResolvedSignature.clear();
    mImpl->inspectorSummary.clear();
    SetIdentityMatrix(mImpl->previousViewProjection.data());
    mImpl->previousViewProjectionValid = false;
    mImpl->previousSceneId = -1;
    mImpl->previousRoomId = -1;
}

std::string ExternalModWorldGraphicsRuntime::BuildInspectorSummary() const {
    return mImpl->inspectorSummary;
}

bool ExternalModWorldGraphicsRuntime::IsVolumetricsDebugActive() const {
    return mImpl->volumetricsDebugMode != Fast::GFX_VOLUMETRICS_DEBUG_OFF;
}

} // namespace SOH
