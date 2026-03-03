#include "ExternalModWorldGraphicsRuntime.h"

#include <algorithm>
#include <array>
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

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
}

namespace SOH {

namespace {

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
    std::vector<FrameLightRecord> frameLights;
    std::string lastResolvedSignature;
    std::string inspectorSummary;
};

ExternalModWorldGraphicsRuntime::ExternalModWorldGraphicsRuntime() : mImpl(std::make_unique<Impl>()) {
}

ExternalModWorldGraphicsRuntime::~ExternalModWorldGraphicsRuntime() = default;

void ExternalModWorldGraphicsRuntime::OnPlayDrawBegin(ExternalModManager& manager,
                                                      std::vector<ExternalModPackage>& packages, PlayState* play) {
    if (play == nullptr) {
        return;
    }

    // Defensive cleanup in case End hook was skipped.
    OnPlayDrawEnd(play);

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
    if (resolvedSignature != mImpl->lastResolvedSignature) {
        ExternalModHookEventContext context;
        context.scene = sceneId;
        context.value = resolvedSignature;
        manager.EmitExtendedHook(ExternalModHookType::OnRenderProfileResolved, context, "OnRenderProfileResolved");
        mImpl->lastResolvedSignature = resolvedSignature;
    }

    if (resolvedState.postFx != nullptr) {
        std::array<uint8_t, 3> targetFogColor = {
            ToByteColor(resolvedState.postFx->fogColor[0]), ToByteColor(resolvedState.postFx->fogColor[1]),
            ToByteColor(resolvedState.postFx->fogColor[2]),
        };
        const float blend = ClampValue(resolvedState.postFxBlend, 0.0f, 1.0f);
        for (size_t i = 0; i < 3; ++i) {
            const float sourceFog = static_cast<float>(mImpl->lightFogColor[i]);
            const float blendedFog = sourceFog + (static_cast<float>(targetFogColor[i]) - sourceFog) * blend;
            play->lightCtx.fogColor[i] = static_cast<uint8_t>(ClampValue(blendedFog, 0.0f, 255.0f));
            play->envCtx.lightSettings.fogColor[i] = play->lightCtx.fogColor[i];
        }

        const int16_t targetFogNear = resolvedState.postFx->hasFogNear
                                          ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogNear, 0, 1000))
                                          : ToFogNearFromDensity(resolvedState.postFx->fogDensity);
        const int16_t targetFogFar = resolvedState.postFx->hasFogFar
                                         ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogFar, 0, 1000))
                                         : ToFogFarFromDensity(resolvedState.postFx->fogDensity);
        const float blendedNear =
            static_cast<float>(mImpl->lightFogNear) + (static_cast<float>(targetFogNear - mImpl->lightFogNear) * blend);
        const float blendedFar =
            static_cast<float>(mImpl->lightFogFar) + (static_cast<float>(targetFogFar - mImpl->lightFogFar) * blend);
        play->lightCtx.fogNear = static_cast<int16_t>(ClampValue(blendedNear, 0.0f, 1000.0f));
        play->lightCtx.fogFar = static_cast<int16_t>(ClampValue(blendedFar, 0.0f, 1000.0f));
        play->envCtx.lightSettings.fogNear = play->lightCtx.fogNear;
        play->envCtx.lightSettings.fogFar = play->lightCtx.fogFar;
    }

    if (resolvedState.sceneProfile != nullptr) {
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t ambient = ToByteColor(resolvedState.sceneProfile->ambientColor[i]);
            play->lightCtx.ambientColor[i] = ambient;
            play->envCtx.lightSettings.ambientColor[i] = ambient;
        }
    }

    if (resolvedState.skylight != nullptr) {
        std::array<float, 3> colorLinear = resolvedState.skylight->colorLinear;
        if (resolvedState.skylight->hasKelvin) {
            colorLinear = KelvinToRgb(resolvedState.skylight->kelvin);
        }
        const float intensity = ClampValue(resolvedState.skylight->intensity, 0.0f, 8.0f);
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t colorByte = ToByteColor(colorLinear[i] * intensity);
            play->envCtx.lightSettings.light1Color[i] = colorByte;
            play->envCtx.lightSettings.light2Color[i] = colorByte;
            play->envCtx.lightSettings.ambientColor[i] =
                std::max(play->envCtx.lightSettings.ambientColor[i], static_cast<uint8_t>(colorByte / 2));
            play->lightCtx.ambientColor[i] = play->envCtx.lightSettings.ambientColor[i];
        }
        const auto& direction = resolvedState.skylight->direction;
        play->envCtx.lightSettings.light1Dir[0] = static_cast<int8_t>(ClampValue(direction[0] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[1] = static_cast<int8_t>(ClampValue(direction[1] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[2] = static_cast<int8_t>(ClampValue(direction[2] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light2Dir[0] = -play->envCtx.lightSettings.light1Dir[0];
        play->envCtx.lightSettings.light2Dir[1] = -play->envCtx.lightSettings.light1Dir[1];
        play->envCtx.lightSettings.light2Dir[2] = -play->envCtx.lightSettings.light1Dir[2];
    }

    int32_t maxDynamicLightsTotal = 128;
    int32_t maxDynamicLightsNear = 32;
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& pbrProfile : package.runtime.pbrDefinitions) {
            maxDynamicLightsTotal = std::max(1, pbrProfile.maxDynamicLightsTotal);
            maxDynamicLightsNear = std::max(1, pbrProfile.maxDynamicLightsNear);
        }
    }
    maxDynamicLightsNear = std::min(maxDynamicLightsNear, maxDynamicLightsTotal);

    struct CandidateLight {
        const ExternalModPackage* package = nullptr;
        ExternalModRuntime::DynamicLightState* state = nullptr;
        const ExternalModLightProfileDefinition* profile = nullptr;
        float distanceSq = std::numeric_limits<float>::max();
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
            candidates.push_back(candidate);
        }
    }

    std::sort(candidates.begin(), candidates.end(), [](const CandidateLight& lhs, const CandidateLight& rhs) {
        if (lhs.distanceSq != rhs.distanceSq) {
            return lhs.distanceSq < rhs.distanceSq;
        }
        return lhs.state->handle < rhs.state->handle;
    });

    const int32_t lightCount = std::min<int32_t>(maxDynamicLightsTotal, static_cast<int32_t>(candidates.size()));
    mImpl->frameLights.reserve(static_cast<size_t>(lightCount));

    for (int32_t index = 0; index < lightCount; ++index) {
        const auto& candidate = candidates[static_cast<size_t>(index)];
        auto& frameLight = mImpl->frameLights.emplace_back();
        frameLight.modId = candidate.package->manifest.id;
        frameLight.handle = candidate.state->handle;

        std::array<float, 3> colorLinear = candidate.profile->colorLinear;
        if (candidate.profile->hasKelvin) {
            colorLinear = KelvinToRgb(candidate.profile->kelvin);
        }
        float intensity = ClampValue(candidate.profile->intensity, 0.0f, 8.0f);
        if (candidate.profile->flicker && candidate.profile->flickerAmount > 0.0f) {
            const float phase = static_cast<float>((candidate.state->handle * 37 + static_cast<int32_t>(play->state.frames)) % 89);
            const float pulse = std::sin(phase * 0.321f);
            intensity *= ClampValue(1.0f + pulse * candidate.profile->flickerAmount, 0.0f, 2.0f);
        }

        const int16_t x = static_cast<int16_t>(ClampValue(candidate.state->posX, -32000.0f, 32000.0f));
        const int16_t y = static_cast<int16_t>(ClampValue(candidate.state->posY, -32000.0f, 32000.0f));
        const int16_t z = static_cast<int16_t>(ClampValue(candidate.state->posZ, -32000.0f, 32000.0f));
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
                fallbackContext.actorHandle = candidate.state->handle;
                fallbackContext.value = "LIGHT_TYPE_SPOT_TO_POINT";
                manager.EmitExtendedHook(ExternalModHookType::OnRenderFallbackApplied, fallbackContext,
                                         "OnRenderFallbackApplied");
            }
            Lights_PointNoGlowSetInfo(&frameLight.info, x, y, z, r, g, b, radius);
        }

        frameLight.node = LightContext_InsertLight(play, &play->lightCtx, &frameLight.info);
    }

    std::ostringstream inspector;
    inspector << "scene=" << sceneId << " room=" << roomId << " sceneProfile="
              << (resolvedState.sceneProfile != nullptr ? resolvedState.sceneProfile->id : "none") << " roomProfile="
              << (resolvedState.roomProfile != nullptr ? resolvedState.roomProfile->id : "none") << " postFx="
              << (resolvedState.postFx != nullptr ? resolvedState.postFx->id : "none") << " skylight="
              << (resolvedState.skylight != nullptr ? resolvedState.skylight->id : "none") << " lights="
              << mImpl->frameLights.size() << "/" << maxDynamicLightsTotal << " nearBudget=" << maxDynamicLightsNear;
    mImpl->inspectorSummary = inspector.str();
}

void ExternalModWorldGraphicsRuntime::OnPlayDrawEnd(PlayState* play) {
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
    mImpl->envCaptured = false;
}

void ExternalModWorldGraphicsRuntime::Reset(PlayState* play) {
    OnPlayDrawEnd(play);
    mImpl->lastResolvedSignature.clear();
    mImpl->inspectorSummary.clear();
}

std::string ExternalModWorldGraphicsRuntime::BuildInspectorSummary() const {
    return mImpl->inspectorSummary;
}

} // namespace SOH
