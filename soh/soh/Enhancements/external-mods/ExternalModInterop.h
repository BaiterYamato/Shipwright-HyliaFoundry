#pragma once

#include <stdint.h>

struct PlayState;
struct Player;

typedef struct ExternalModsPauseCellInfo {
    int32_t occupied;
    int32_t absoluteIndex;
    const uint8_t* iconRgba32;
    const char* displayName;
    uint8_t assignableButtonsMask;
    uint8_t assignedButtonsMask;
} ExternalModsPauseCellInfo;

typedef enum ExternalModsAimCameraContext {
    EXTERNAL_MODS_AIM_CONTEXT_CUP = 0,
    EXTERNAL_MODS_AIM_CONTEXT_BOW = 1,
    EXTERNAL_MODS_AIM_CONTEXT_HOOKSHOT = 2,
    EXTERNAL_MODS_AIM_CONTEXT_SLINGSHOT = 3,
    EXTERNAL_MODS_AIM_CONTEXT_BOOMERANG = 4,
} ExternalModsAimCameraContext;

typedef enum ExternalModsAimSelectSlotPressResult {
    EXTERNAL_MODS_AIM_SELECT_SLOT_NONE = 0,
    EXTERNAL_MODS_AIM_SELECT_SLOT_ACTIVATED = 1,
    EXTERNAL_MODS_AIM_SELECT_SLOT_DEACTIVATED_CONSUMED = 2,
} ExternalModsAimSelectSlotPressResult;

typedef enum ExternalModsResourceRingFixedAnchor {
    EXTERNAL_MODS_RESOURCE_RING_ANCHOR_LEFT = 0,
    EXTERNAL_MODS_RESOURCE_RING_ANCHOR_RIGHT = 1,
    EXTERNAL_MODS_RESOURCE_RING_ANCHOR_NONE = 2,
} ExternalModsResourceRingFixedAnchor;

typedef enum ExternalModsResourceRingStyleKind {
    EXTERNAL_MODS_RESOURCE_RING_STYLE_RING = 0,
    EXTERNAL_MODS_RESOURCE_RING_STYLE_MAGIC_BAR = 1,
} ExternalModsResourceRingStyleKind;

typedef struct ExternalModsResourceRingView {
    int32_t active;
    int32_t styleKind;
    int32_t showContextual;
    int32_t showFixed;
    int32_t fixedAnchor;
    int32_t fixedStackOrder;
    int32_t fixedStackMagicBarGroup;
    int32_t segmentCount;
    int32_t exhausted;
    int32_t lowPulse;
    int32_t isLow;
    float currentValue;
    float capacityValue;
    float wheelCapacity;
    float scale;
    float opacity;
    float thickness;
    float ringSpacing;
    float fixedStackSpacing;
    float screenOffsetX;
    float screenOffsetY;
    float worldOffsetX;
    float worldOffsetY;
    float worldOffsetZ;
    float lowThresholdPercent;
    uint8_t normalColor[4];
    uint8_t lowColor[4];
    uint8_t exhaustedColor[4];
    uint8_t backgroundColor[4];
    uint8_t segmentColor[4];
    const uint8_t* companionIconRgba32;
    int32_t companionCounterValue;
} ExternalModsResourceRingView;

#ifdef __cplusplus
extern "C" {
#endif

int32_t ExternalMods_GetVirtualInventoryPageCount(void);
int32_t ExternalMods_GetVirtualInventoryCellCount(void);
int32_t ExternalMods_GetVirtualInventoryPageCell(int32_t pageIndex, int32_t cellIndex,
                                                 ExternalModsPauseCellInfo* outInfo);
int32_t ExternalMods_MoveVirtualInventoryCell(int32_t fromAbsoluteIndex, int32_t toAbsoluteIndex);
int32_t ExternalMods_AssignVirtualInventoryCellToButton(int32_t absoluteIndex, int32_t buttonIndex);
void ExternalMods_OnVanillaButtonEquipped(int32_t buttonIndex);
int32_t ExternalMods_TryDrawButtonOverrideIcon(struct PlayState* play, int32_t buttonIndex, int32_t alpha);
int32_t ExternalMods_DrawSurfBoardIfActive(struct PlayState* play, struct Player* player);
int32_t ExternalMods_OnHammerGroundImpact(struct PlayState* play, struct Player* player, float impactX, float impactY,
                                          float impactZ);
int32_t ExternalMods_DrawAimReticleIfActive(struct PlayState* play, struct Player* player, int32_t context);
int32_t ExternalMods_ResolveAimCameraMode(struct PlayState* play, struct Player* player, int32_t defaultMode,
                                          int32_t context);
int32_t ExternalMods_IsAimMouseFireHeld(struct PlayState* play, struct Player* player, int32_t heldItemAction);
int32_t ExternalMods_HandleAimSelectSlotPress(struct PlayState* play, struct Player* player, int32_t buttonIndex,
                                              int32_t itemId);
int32_t ExternalMods_IsAimAttackButtonFireEnabled(struct PlayState* play, struct Player* player);
int32_t ExternalMods_IsAimOverShoulderEnabled(void);
int32_t ExternalMods_ShouldUseAimOverShoulderBattleMovement(struct PlayState* play, struct Player* player,
                                                            int32_t heldItemAction);
int32_t ExternalMods_TryConsumePlayerResourceActionStart(struct PlayState* play, struct Player* player,
                                                        const char* actionTag);
int32_t ExternalMods_TickPlayerResourceAction(struct PlayState* play, struct Player* player, const char* actionTag,
                                             float deltaSeconds);
int32_t ExternalMods_IsPlayerResourceActionInputActive(struct PlayState* play, struct Player* player,
                                                      const char* actionTag, int32_t triggerType);
float ExternalMods_GetPlayerResourceMoveSpeedMultiplier(struct PlayState* play, struct Player* player);
int32_t ExternalMods_IsPlayerResourceSprintBlocked(struct PlayState* play, struct Player* player);
int32_t ExternalMods_GetPlayerResourceRingViewCount(struct PlayState* play, struct Player* player);
int32_t ExternalMods_GetPlayerResourceRingView(struct PlayState* play, struct Player* player, int32_t index,
                                              ExternalModsResourceRingView* outView);
int32_t ExternalMods_TryFillBottleFromWater(struct PlayState* play, struct Player* player, int32_t* outPlaceholderItemId);
int32_t ExternalMods_IsHeldBottleCustomContentActive(struct PlayState* play, struct Player* player);
int32_t ExternalMods_ConsumeHeldBottleCustomContent(struct PlayState* play, struct Player* player);
int32_t ExternalMods_IsHeldInventoryConsumableActive(struct PlayState* play, struct Player* player);
int32_t ExternalMods_ConsumeHeldInventoryConsumable(struct PlayState* play, struct Player* player);
int32_t ExternalMods_TryHandleGrassDrop(struct PlayState* play, float x, float y, float z, int32_t grassType, int32_t dropParams);
int32_t ExternalMods_HasCustomEquippedSlingshotModel(void);
int32_t ExternalMods_DrawCustomEquippedSlingshotModel(struct PlayState* play);
int32_t ExternalMods_IsPlayerFreezeNoDamageActive(struct Player* player);

#ifdef __cplusplus
}
#endif
