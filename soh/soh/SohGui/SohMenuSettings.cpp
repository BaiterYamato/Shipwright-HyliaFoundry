#include "SohMenu.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "SohModals.h"
#include "soh/OTRGlobals.h"
#include <soh/GameVersions.h>
#include "soh/ResourceManagerHelpers.h"
#include "UIWidgets.hpp"
#include <spdlog/fmt/fmt.h>

extern "C" {
#include "include/z64audio.h"
#include "variables.h"
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
extern std::shared_ptr<SohModalWindow> mModalWindow;
using namespace UIWidgets;

static std::map<int32_t, const char*> imguiScaleOptions = {
    { 0, "Small" },
    { 1, "Normal" },
    { 2, "Large" },
    { 3, "X-Large" },
};

static const std::map<int32_t, const char*> menuThemeOptions = {
    { UIWidgets::Colors::Red, "Red" },
    { UIWidgets::Colors::DarkRed, "Dark Red" },
    { UIWidgets::Colors::Orange, "Orange" },
    { UIWidgets::Colors::Green, "Green" },
    { UIWidgets::Colors::DarkGreen, "Dark Green" },
    { UIWidgets::Colors::LightBlue, "Light Blue" },
    { UIWidgets::Colors::Blue, "Blue" },
    { UIWidgets::Colors::DarkBlue, "Dark Blue" },
    { UIWidgets::Colors::Indigo, "Indigo" },
    { UIWidgets::Colors::Violet, "Violet" },
    { UIWidgets::Colors::Purple, "Purple" },
    { UIWidgets::Colors::Brown, "Brown" },
    { UIWidgets::Colors::Gray, "Gray" },
    { UIWidgets::Colors::DarkGray, "Dark Gray" },
};

static const std::map<int32_t, const char*> textureFilteringMap = {
    { Fast::FILTER_THREE_POINT, "Three-Point" },
    { Fast::FILTER_LINEAR, "Linear" },
    { Fast::FILTER_NONE, "None" },
};

static const std::map<int32_t, const char*> volumetricsDebugModeMap = {
    { Fast::GFX_VOLUMETRICS_DEBUG_OFF, "Off" },
    { Fast::GFX_VOLUMETRICS_DEBUG_FOG_DENSITY, "Fog Density" },
    { Fast::GFX_VOLUMETRICS_DEBUG_LIGHT_ENERGY, "Light Energy" },
    { Fast::GFX_VOLUMETRICS_DEBUG_FINAL_VOLUME, "Final Volume" },
    { Fast::GFX_VOLUMETRICS_DEBUG_TRANSMITTANCE, "Transmittance" },
    { Fast::GFX_VOLUMETRICS_DEBUG_SHADOW_OCCLUSION, "Shadow Occlusion" },
};

static const std::map<int32_t, const char*> volumetricsPerformanceModeMap = {
    { Fast::GFX_VOLUMETRICS_PERF_PERFORMANCE, "Performance" },
    { Fast::GFX_VOLUMETRICS_PERF_BALANCED, "Balanced" },
    { Fast::GFX_VOLUMETRICS_PERF_QUALITY, "Quality" },
};

static const std::map<int32_t, const char*> volumetricsTestLightTypeMap = {
    { 0, "Directional" },
    { 1, "Point" },
    { 2, "Spot" },
};

static const std::map<int32_t, const char*> notificationPosition = {
    { 0, "Top Left" }, { 1, "Top Right" }, { 2, "Bottom Left" }, { 3, "Bottom Right" }, { 4, "Hidden" },
};

static const std::map<int32_t, const char*> bootSequenceLabels = {
    { BOOTSEQUENCE_DEFAULT, "Default" },        { BOOTSEQUENCE_AUTHENTIC, "Authentic" },
    { BOOTSEQUENCE_FILESELECT, "File Select" }, { BOOTSEQUENCE_DEBUGWARPSCREEN, "Debug Warp Screen" },
    { BOOTSEQUENCE_WARPPOINT, "Warp Point" },
};

const char* GetGameVersionString(uint32_t index) {
    uint32_t gameVersion = ResourceMgr_GetGameVersion(index);
    switch (gameVersion) {
        case OOT_NTSC_US_10:
            return "NTSC 1.0";
        case OOT_NTSC_US_11:
            return "NTSC 1.1";
        case OOT_NTSC_US_12:
            return "NTSC 1.2";
        case OOT_NTSC_US_GC:
            return "NTSC-U GC";
        case OOT_NTSC_JP_GC:
            return "NTSC-J GC";
        case OOT_NTSC_JP_GC_CE:
            return "NTSC-J GC (Collector's Edition)";
        case OOT_NTSC_US_MQ:
            return "NTSC-U MQ";
        case OOT_NTSC_JP_MQ:
            return "NTSC-J MQ";
        case OOT_PAL_10:
            return "PAL 1.0";
        case OOT_PAL_11:
            return "PAL 1.1";
        case OOT_PAL_GC:
            return "PAL GC";
        case OOT_PAL_MQ:
            return "PAL MQ";
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
            return "PAL GC-D";
        case OOT_PAL_GC_MQ_DBG:
            return "PAL MQ-D";
        case OOT_IQUE_CN:
            return "IQUE CN";
        case OOT_IQUE_TW:
            return "IQUE TW";
        default:
            return "UNKNOWN";
    }
}

#include "message_data_static.h"
extern "C" MessageTableEntry* sNesMessageEntryTablePtr;
extern "C" MessageTableEntry* sGerMessageEntryTablePtr;
extern "C" MessageTableEntry* sFraMessageEntryTablePtr;
extern "C" MessageTableEntry* sJpnMessageEntryTablePtr;

static const std::array<MessageTableEntry**, LANGUAGE_MAX> messageTables = {
    &sNesMessageEntryTablePtr, &sGerMessageEntryTablePtr, &sFraMessageEntryTablePtr, &sJpnMessageEntryTablePtr
};

void SohMenu::UpdateLanguageMap(std::map<int32_t, const char*>& languageMap) {
    for (int32_t i = LANGUAGE_ENG; i < LANGUAGE_MAX; i++) {
        if (*messageTables.at(i) != NULL) {
            if (!languageMap.contains(i)) {
                languageMap.insert(std::make_pair(i, languages.at(i)));
            }
        } else {
            languageMap.erase(i);
        }
    }
}

void SohMenu::AddMenuSettings() {
    // Add Settings Menu
    AddMenuEntry("Settings", CVAR_SETTING("Menu.SettingsSidebarSection"));
    AddSidebarEntry("Settings", "General", 2);
    WidgetPath path = { "Settings", "General", SECTION_COLUMN_1 };

    // General - Settings
    AddWidget(path, "Menu Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Menu Theme", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_SETTING("Menu.Theme"))
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .Tooltip("Changes the Theme of the Menu Widgets.")
                     .ComboMap(menuThemeOptions)
                     .DefaultIndex(Colors::LightBlue));
#if not defined(__SWITCH__) and not defined(__WIIU__)
    AddWidget(path, "Menu Controller Navigation", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_IMGUI_CONTROLLER_NAV)
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(
            "Allows controller navigation of the port menu (Settings, Enhancements,...)\nCAUTION: "
            "This will disable game inputs while the menu is visible.\n\nD-pad to move between "
            "items, A to select, B to move up in scope."));
    AddWidget(path, "Menu Background Opacity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_SETTING("Menu.BackgroundOpacity"))
        .RaceDisable(false)
        .Options(FloatSliderOptions().DefaultValue(0.85f).IsPercentage().Tooltip(
            "Sets the opacity of the background of the port menu."));

    AddWidget(path, "General Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Cursor Always Visible", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("CursorVisibility"))
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetForceCursorVisibility(
                CVarGetInteger(CVAR_SETTING("CursorVisibility"), 0));
        })
        .Options(CheckboxOptions().Tooltip("Makes the cursor always visible, even in full screen."));
#endif
    AddWidget(path, "Search In Sidebar", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("Menu.SidebarSearch"))
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            if (CVarGetInteger(CVAR_SETTING("Menu.SidebarSearch"), 0)) {
                mSohMenu->InsertSidebarSearch();
            } else {
                mSohMenu->RemoveSidebarSearch();
            }
        })
        .Options(CheckboxOptions().Tooltip(
            "Displays the Search menu as a sidebar entry in Settings instead of in the header."));
    AddWidget(path, "Search Input Autofocus", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("Menu.SearchAutofocus"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(
            "Search input box gets autofocus when visible. Does not affect using other widgets."));
    AddWidget(path, "Reset Button Combination:", WIDGET_CVAR_BTN_SELECTOR)
        .CVar("gSettings.ResetBtn")
        .Options(BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER2));
    AddWidget(path, "Open App Files Folder", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        })
        .Options(ButtonOptions().Tooltip("Opens the folder that contains the save and mods folders, etc."));

    AddWidget(path, "Boot", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Boot Sequence", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_SETTING("BootSequence"))
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .DefaultIndex(BOOTSEQUENCE_DEFAULT)
                     .LabelPosition(LabelPositions::Far)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .ComboMap(bootSequenceLabels)
                     .Tooltip("Configure what happens when starting or resetting the game.\n\n"
                              "Default: LUS logo -> N64 logo\n"
                              "Authentic: N64 logo only\n"
                              "File Select: Skip to file select menu\n"
                              "Debug Warp Screen: Skip to the debug warp screen\n"
                              "Warp Point: Skip to active warp point (if set), see Dev Tools -> General"));

    AddWidget(path, "Languages", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Translate Title Screen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("TitleScreenTranslation"))
        .RaceDisable(false);
    AddWidget(path, "Language", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_SETTING("Languages"))
        .RaceDisable(false)
        .PreFunc([](WidgetInfo& info) {
            auto options = std::static_pointer_cast<UIWidgets::ComboboxOptions>(info.options);
            SohMenu::UpdateLanguageMap(options->comboMap);
        })
        .Options(ComboboxOptions()
                     .LabelPosition(LabelPositions::Far)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .ComboMap(languages)
                     .DefaultIndex(LANGUAGE_ENG));
    AddWidget(path, "Accessibility", WIDGET_SEPARATOR_TEXT);
#if defined(_WIN32) || defined(__APPLE__) || defined(ESPEAK)
    AddWidget(path, "Text to Speech", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("A11yTTS"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Enables text to speech for in game dialog"));
#endif
    AddWidget(path, "Disable Idle Camera Re-Centering", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("A11yDisableIdleCam"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Disables the automatic re-centering of the camera when idle."));
    AddWidget(path, "Disable Screen Flash for Finishing Blow", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("A11yNoScreenFlashForFinishingBlow"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Disables the white screen flash on enemy kill."));
    AddWidget(path, "EXPERIMENTAL", WIDGET_SEPARATOR_TEXT).Options(TextOptions().Color(Colors::Orange));
    AddWidget(path, "ImGui Menu Scaling", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_SETTING("ImGuiScale"))
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .ComboMap(imguiScaleOptions)
                     .Tooltip("Changes the scaling of the ImGui menu elements.")
                     .DefaultIndex(1)
                     .ComponentAlignment(ComponentAlignments::Right)
                     .LabelPosition(LabelPositions::Far))
        .Callback([](WidgetInfo& info) { OTRGlobals::Instance->ScaleImGui(); });

    // General - About
    path.column = SECTION_COLUMN_2;

    AddWidget(path, "About", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Ship Of Harkinian", WIDGET_TEXT);
    if (gGitCommitTag[0] != 0) {
        AddWidget(path, gBuildVersion, WIDGET_TEXT);
    } else {
        AddWidget(path, ("Branch: " + std::string(gGitBranch)), WIDGET_TEXT);
        AddWidget(path, ("Commit: " + std::string(gGitCommitHash)), WIDGET_TEXT);
    }
    for (uint32_t i = 0; i < ResourceMgr_GetNumGameVersions(); i++) {
        AddWidget(path, GetGameVersionString(i), WIDGET_TEXT);
    }

    // Audio Settings
    path.sidebarName = "Audio";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", "Audio", 3);

    AddWidget(path, "Master Volume: %d %%", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("Volume.Master"))
        .RaceDisable(false)
        .Options(IntSliderOptions().Min(0).Max(100).DefaultValue(40).ShowButtons(true).Format(""));
    AddWidget(path, "Main Music Volume: %d %%", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("Volume.MainMusic"))
        .RaceDisable(false)
        .Options(IntSliderOptions().Min(0).Max(100).DefaultValue(100).ShowButtons(true).Format(""))
        .Callback([](WidgetInfo& info) {
            Audio_SetGameVolume(SEQ_PLAYER_BGM_MAIN,
                                ((float)CVarGetInteger(CVAR_SETTING("Volume.MainMusic"), 100) / 100.0f));
        });
    AddWidget(path, "Sub Music Volume: %d %%", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("Volume.SubMusic"))
        .RaceDisable(false)
        .Options(IntSliderOptions().Min(0).Max(100).DefaultValue(100).ShowButtons(true).Format(""))
        .Callback([](WidgetInfo& info) {
            Audio_SetGameVolume(SEQ_PLAYER_BGM_SUB,
                                ((float)CVarGetInteger(CVAR_SETTING("Volume.SubMusic"), 100) / 100.0f));
        });
    AddWidget(path, "Fanfare Volume: %d %%", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("Volume.Fanfare"))
        .RaceDisable(false)
        .Options(IntSliderOptions().Min(0).Max(100).DefaultValue(100).ShowButtons(true).Format(""))
        .Callback([](WidgetInfo& info) {
            Audio_SetGameVolume(SEQ_PLAYER_FANFARE,
                                ((float)CVarGetInteger(CVAR_SETTING("Volume.Fanfare"), 100) / 100.0f));
        });
    AddWidget(path, "Sound Effects Volume: %d %%", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("Volume.SFX"))
        .RaceDisable(false)
        .Options(IntSliderOptions().Min(0).Max(100).DefaultValue(100).ShowButtons(true).Format(""))
        .Callback([](WidgetInfo& info) {
            Audio_SetGameVolume(SEQ_PLAYER_SFX, ((float)CVarGetInteger(CVAR_SETTING("Volume.SFX"), 100) / 100.0f));
        });
    AddWidget(path, "Audio API (Needs reload)", WIDGET_AUDIO_BACKEND).RaceDisable(false);

    // Graphics Settings
    static int32_t maxFps = 360;
    const char* tooltip = "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is "
                          "purely visual and does not impact game logic, execution of glitches etc.\n\nA higher target "
                          "FPS than your monitor's refresh rate will waste resources, and might give a worse result.";
    path.sidebarName = "Graphics";
    AddSidebarEntry("Settings", "Graphics", 3);
    AddWidget(path, "Graphics Options", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Fullscreen", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) { Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen(); })
        .Options(ButtonOptions().Tooltip("Toggles Fullscreen On/Off."));
    AddWidget(path, "Internal Resolution", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_INTERNAL_RESOLUTION)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(
                CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
        })
        .PreFunc([](WidgetInfo& info) {
            if (mSohMenu->disabledMap.at(DISABLE_FOR_ADVANCED_RESOLUTION_ON).active &&
                mSohMenu->disabledMap.at(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_ADVANCED_RESOLUTION_ON);
                info.activeDisables.push_back(DISABLE_FOR_VERTICAL_RES_TOGGLE_ON);
            } else if (mSohMenu->disabledMap.at(DISABLE_FOR_LOW_RES_MODE_ON).active) {
                info.activeDisables.push_back(DISABLE_FOR_LOW_RES_MODE_ON);
            }
        })
        .Options(
            FloatSliderOptions()
                .Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective "
                         "form of anti-aliasing.")
                .ShowButtons(false)
                .IsPercentage()
                .Min(0.5f)
                .Max(2.0f));
#ifndef __WIIU__
    AddWidget(path, "Anti-aliasing (MSAA)", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_MSAA_VALUE)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
        })
        .Options(
            IntSliderOptions()
                .Tooltip("Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of "
                         "rendered geometry.\n"
                         "Higher sample count will result in smoother edges on models, but may reduce performance.")
                .Min(1)
                .Max(8)
                .DefaultValue(1));
#endif
    auto fps = CVarGetInteger(CVAR_SETTING("InterpolationFPS"), 20);
    const char* fpsFormat = fps == 20 ? "Original (%d)" : "%d";
    AddWidget(path, "Current FPS", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SETTING("InterpolationFPS"))
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            auto options = std::static_pointer_cast<IntSliderOptions>(info.options);
            int32_t defaultValue = options->defaultValue;
            if (CVarGetInteger(info.cVar, defaultValue) == defaultValue) {
                options->format = "Original (%d)";
            } else {
                options->format = "%d";
            }
        })
        .PreFunc([](WidgetInfo& info) {
            if (mSohMenu->disabledMap.at(DISABLE_FOR_MATCH_REFRESH_RATE_ON).active)
                info.activeDisables.push_back(DISABLE_FOR_MATCH_REFRESH_RATE_ON);
        })
        .Options(IntSliderOptions().Tooltip(tooltip).Min(20).Max(maxFps).DefaultValue(20).Format(fpsFormat));
    AddWidget(path, "Match Refresh Rate", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SETTING("MatchRefreshRate"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Matches interpolation value to the refresh rate of your display."));
    AddWidget(path, "Renderer API (Needs reload)", WIDGET_VIDEO_BACKEND).RaceDisable(false);
    AddWidget(path, "Enable Vsync", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_VSYNC_ENABLED)
        .RaceDisable(false)
        .PreFunc([](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_VSYNC).active; })
        .Options(CheckboxOptions()
                     .Tooltip("Removes tearing, but clamps your max FPS to your displays refresh rate.")
                     .DefaultValue(true));
    AddWidget(path, "Windowed Fullscreen", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SDL_WINDOWED_FULLSCREEN)
        .RaceDisable(false)
        .PreFunc([](WidgetInfo& info) {
            info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_WINDOWED_FULLSCREEN).active;
        })
        .Options(CheckboxOptions().Tooltip("Enables Windowed Fullscreen Mode."));
    AddWidget(path, "Allow multi-windows", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENABLE_MULTI_VIEWPORTS)
        .RaceDisable(false)
        .PreFunc(
            [](WidgetInfo& info) { info.isHidden = mSohMenu->disabledMap.at(DISABLE_FOR_NO_MULTI_VIEWPORT).active; })
        .Options(CheckboxOptions()
                     .Tooltip("Allows multiple windows to be opened at once. Requires a reload to take effect.")
                     .DefaultValue(true));
    AddWidget(path, "Texture Filter (Needs reload)", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_TEXTURE_FILTER)
        .RaceDisable(false)
        .Options(ComboboxOptions().Tooltip("Sets the applied Texture Filtering.").ComboMap(textureFilteringMap));

    path.column = SECTION_COLUMN_2;
    AddWidget(path, "Advanced Graphics Options", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Ambient Occlusion (SSAO)", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.AO.Enabled")
        .RaceDisable(false)
        .Options(CheckboxOptions()
                     .Tooltip("Enables screen-space ambient occlusion (SSAO). Uses extra GPU processing.")
                     .DefaultValue(false));
    AddWidget(path, "Ambient Occlusion Quality", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.AO.Quality")
        .RaceDisable(false)
        .Options(IntSliderOptions()
                     .Tooltip("0=Off, 1=Low, 2=Medium, 3=High.")
                     .Min(0)
                     .Max(3)
                     .DefaultValue(0));
    AddWidget(path, "Ambient Occlusion Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.AO.IntensityScale")
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("Global multiplier for AO intensity. 1.0 = profile default.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Ambient Occlusion Debug View", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.AO.DebugView")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Displays AO buffer instead of compositing it.").DefaultValue(false));
    AddWidget(path, "Volumetrics", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.Enabled")
        .RaceDisable(false)
        .Options(CheckboxOptions()
                     .Tooltip("Enables real volumetric fog/lighting when a render profile or dynamic lights request it.")
                     .DefaultValue(true));
    AddWidget(path, "Volumetrics Quality", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.Volumetrics.Quality")
        .RaceDisable(false)
        .Options(IntSliderOptions()
                     .Tooltip("0=Auto, 1=Low, 2=Medium, 3=High.")
                     .Min(0)
                     .Max(3)
                     .DefaultValue(0));
    AddWidget(path, "Volumetrics Performance Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.PerformanceMode")
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .Tooltip("Performance keeps the cheap depth fog path; Balanced limits raymarch cost; Quality enables heavier lighting.")
                     .ComboMap(volumetricsPerformanceModeMap)
                     .DefaultIndex(Fast::GFX_VOLUMETRICS_PERF_BALANCED));
    AddWidget(path, "Performance Fog Preset", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.PerformanceMode", Fast::GFX_VOLUMETRICS_PERF_PERFORMANCE);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.Quality", 1);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.ShadowQuality", 0);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DensityScale", 0.85f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.AmbientIntensity", 0.28f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DepthExtinctionStrength", 1.65f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.HorizonFogStrength", 0.45f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.SkyFallbackStrength", 0.08f);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        })
        .Options(ButtonOptions().Tooltip("Fast depth fog/aerial perspective preset. Best when FPS matters.").Size(Sizes::Inline));
    AddWidget(path, "Balanced Atmosphere Preset", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.PerformanceMode", Fast::GFX_VOLUMETRICS_PERF_BALANCED);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.Quality", 2);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.ShadowQuality", 0);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DensityScale", 1.0f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.AmbientIntensity", 0.35f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DepthExtinctionStrength", 2.1f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.HorizonFogStrength", 0.6f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.SkyFallbackStrength", 0.1f);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        })
        .Options(ButtonOptions().Tooltip("Balanced depth fog plus limited raymarch when a real volumetric light exists.").Size(Sizes::Inline));
    AddWidget(path, "Quality Volumetrics Preset", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.PerformanceMode", Fast::GFX_VOLUMETRICS_PERF_QUALITY);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.Quality", 3);
            CVarSetInteger("gEnhancements.Graphics.Volumetrics.ShadowQuality", 1);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DensityScale", 1.0f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.AmbientIntensity", 0.35f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.DepthExtinctionStrength", 2.4f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.HorizonFogStrength", 0.7f);
            CVarSetFloat("gEnhancements.Graphics.Volumetrics.SkyFallbackStrength", 0.12f);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        })
        .Options(ButtonOptions().Tooltip("Higher quality raymarch budget. Shadows stay conservative to avoid the old 1 FPS path.").Size(Sizes::Inline));
    AddWidget(path, "Volumetrics Density", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DensityScale")
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("Global multiplier for volumetric density. 1.0 = profile default.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Shadow Quality", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.Volumetrics.ShadowQuality")
        .RaceDisable(false)
        .Options(IntSliderOptions()
                     .Tooltip("0=Auto, 1=Low, 2=Medium, 3=High.")
                     .Min(0)
                     .Max(3)
                     .DefaultValue(0));
    AddWidget(path, "Volumetrics Debug View", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.DebugView")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Displays the volumetric buffer instead of the final composite.")
                     .DefaultValue(false));
    AddWidget(path, "Advanced Volumetrics Overrides", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Custom Volumetrics", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.UseCustom")
        .RaceDisable(false)
        .Options(CheckboxOptions()
                     .Tooltip("Overrides the active volumetrics preset with the advanced values below.")
                     .DefaultValue(false));
    AddWidget(path, "Volumetrics Color", WIDGET_CVAR_COLOR_PICKER)
        .CVar("gEnhancements.Graphics.Volumetrics.Color")
        .RaceDisable(false)
        .Options(ColorPickerOptions()
                     .Tooltip("Base fog color used by volumetrics when custom overrides are enabled.")
                     .Color(Colors::Blue)
                     .DefaultValue({ 184, 214, 255, 255 })
                     .ShowReset());
    AddWidget(path, "Volumetrics Ambient Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.AmbientIntensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How strongly the medium fog shows up even without bright dynamic lights.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(0.35f)
                     .Step(0.01f));
    AddWidget(path, "Volumetrics Anisotropy", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.Anisotropy")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Scattering phase bias: negative spreads evenly, positive pushes forward shafts.")
                     .Min(-0.95f)
                     .Max(0.95f)
                     .DefaultValue(0.2f)
                     .Step(0.01f));
    AddWidget(path, "Volumetrics Start Distance", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.StartDistance")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How far from the camera the volumetric fog starts.")
                     .Min(0.0f)
                     .Max(1024.0f)
                     .DefaultValue(64.0f)
                     .Step(1.0f));
    AddWidget(path, "Volumetrics Max Distance", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.MaxDistance")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Maximum distance marched by the volumetrics pass.")
                     .Min(64.0f)
                     .Max(6000.0f)
                     .DefaultValue(2400.0f)
                     .Step(10.0f));
    AddWidget(path, "Volumetrics Height Fog", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.HeightFog")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Enables exponential height falloff when custom overrides are enabled.")
                     .DefaultValue(false));
    AddWidget(path, "Volumetrics Base Height", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.BaseHeight")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Height reference used by the custom volumetric height fog.")
                     .Min(-4000.0f)
                     .Max(4000.0f)
                     .DefaultValue(0.0f)
                     .Step(10.0f));
    AddWidget(path, "Volumetrics Height Falloff", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.HeightFalloff")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How quickly the custom height fog fades above the base height.")
                     .Min(0.0f)
                     .Max(0.02f)
                     .DefaultValue(0.0025f)
                     .Step(0.0001f)
                     .Format("%.4f"));
    AddWidget(path, "Volumetrics Light Shaft Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.LightShaftIntensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Multiplier for volumetric light scattering.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Shadow Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.ShadowIntensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How much shadowed volumetric occlusion darkens the medium.")
                     .Min(0.0f)
                     .Max(1.0f)
                     .DefaultValue(0.6f)
                     .Step(0.01f));
    AddWidget(path, "Volumetrics Temporal Blend", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.TemporalBlend")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Blend factor used when reprojection history is valid.")
                     .Min(0.0f)
                     .Max(0.99f)
                     .DefaultValue(0.88f)
                     .Step(0.01f));
    AddWidget(path, "Volumetrics Jitter Scale", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.JitterScale")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Noise jitter amplitude used by the volumetric raymarch.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Aerial Perspective", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.AerialPerspectiveStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Strength of depth-driven haze that makes far geometry lose contrast and readability.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Macro Noise", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.MacroNoiseStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Strength of large world-space fog billows and pockets.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Valley Fog", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.ValleyFogStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How strongly the fog pools in lower areas and hugs the terrain.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Edge Haze", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.EdgeHazeStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Extra haze applied around distant depth transitions and silhouettes.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Depth Extinction", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DepthExtinctionStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Strength of depth-driven extinction that fades distant geometry into fog.")
                     .Min(0.0f)
                     .Max(8.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Depth Exponent", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DepthExtinctionExponent")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Curve of the depth extinction response across near to far geometry.")
                     .Min(0.25f)
                     .Max(6.0f)
                     .DefaultValue(1.25f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Horizon Fog", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.HorizonFogStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Extra horizon haze for broad outdoor depth and map-scale fog layers.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Sky Fallback", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.SkyFallbackStrength")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Fallback fog strength for sky and background when no scene depth is available.")
                     .Min(0.0f)
                     .Max(2.0f)
                     .DefaultValue(0.18f)
                     .Step(0.01f));
    AddWidget(path, "Directional Shadow Bias", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DirectionalShadowBias")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Depth bias for the main directional volumetric shadow.")
                     .Min(0.0f)
                     .Max(0.1f)
                     .DefaultValue(0.01f)
                     .Step(0.001f));
    AddWidget(path, "Directional Shadow Normal Bias", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DirectionalShadowNormalBias")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Normal-based bias that helps reduce shadow acne in the volumetric pass.")
                     .Min(0.0f)
                     .Max(0.2f)
                     .DefaultValue(0.02f)
                     .Step(0.002f));
    AddWidget(path, "Directional Shadow Softness", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Volumetrics.DirectionalShadowSoftness")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Softens the main directional volumetric shadow filtering.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Volumetrics Debug Mode", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.Volumetrics.DebugMode")
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .Tooltip("Advanced debug visualization for the volumetric pass.")
                     .ComboMap(volumetricsDebugModeMap)
                     .DefaultIndex(Fast::GFX_VOLUMETRICS_DEBUG_OFF));
    AddWidget(path, "Advanced PostFX Overrides", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Custom PostFX", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.PostFx.UseCustom")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Overrides the resolved postfx preset values below in real time.")
                     .DefaultValue(false));
    AddWidget(path, "PostFX Exposure", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.PostFx.Exposure")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Global postfx exposure override.")
                     .Min(0.0f)
                     .Max(4.0f)
                     .DefaultValue(1.0f)
                     .Step(0.01f));
    AddWidget(path, "PostFX Bloom", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.PostFx.Bloom")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Global bloom override from the active postfx preset.")
                     .Min(0.0f)
                     .Max(2.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "PostFX Saturation", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.PostFx.Saturation")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Global saturation override from the active postfx preset.")
                     .Min(0.0f)
                     .Max(3.0f)
                     .DefaultValue(1.0f)
                     .Step(0.01f));
    AddWidget(path, "PostFX Fog Color", WIDGET_CVAR_COLOR_PICKER)
        .CVar("gEnhancements.Graphics.PostFx.FogColor")
        .RaceDisable(false)
        .Options(ColorPickerOptions()
                     .Tooltip("Fog color override used by postfx and as the default volumetric fog tint.")
                     .Color(Colors::LightBlue)
                     .DefaultValue({ 171, 204, 255, 255 })
                     .ShowReset());
    AddWidget(path, "PostFX Fog Density", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.PostFx.FogDensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Density override used to derive near/far fog when custom postfx is active.")
                     .Min(0.0f)
                     .Max(1.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "Advanced Skylight Overrides", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Custom Skylight", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.Skylight.UseCustom")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Overrides the resolved skylight with the custom values below for validation.")
                     .DefaultValue(false));
    AddWidget(path, "Skylight Color", WIDGET_CVAR_COLOR_PICKER)
        .CVar("gEnhancements.Graphics.Skylight.Color")
        .RaceDisable(false)
        .Options(ColorPickerOptions()
                     .Tooltip("Custom skylight color used for both the environment light and volumetrics.")
                     .Color(Colors::LightBlue)
                     .DefaultValue({ 255, 255, 255, 255 })
                     .ShowReset());
    AddWidget(path, "Skylight Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Skylight.Intensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Intensity multiplier for the custom skylight.")
                     .Min(0.0f)
                     .Max(8.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Skylight Volumetric Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Skylight.VolumetricIntensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("How strongly the custom skylight contributes to volumetric scattering.")
                     .Min(0.0f)
                     .Max(8.0f)
                     .DefaultValue(1.0f)
                     .Step(0.05f));
    AddWidget(path, "Skylight Direction X", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Skylight.DirectionX")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("X direction of the custom skylight.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "Skylight Direction Y", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Skylight.DirectionY")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Y direction of the custom skylight.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(-1.0f)
                     .Step(0.01f));
    AddWidget(path, "Skylight Direction Z", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.Skylight.DirectionZ")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Z direction of the custom skylight.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "Advanced Volumetric Test Light", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Custom Test Light", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DebugLight.UseCustom")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Injects a guaranteed test light for validating volumetrics even when the scene has no dynamic lights.")
                     .DefaultValue(false));
    AddWidget(path, "Test Light Type", WIDGET_CVAR_COMBOBOX)
        .CVar("gEnhancements.Graphics.DebugLight.Type")
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .Tooltip("Selects the light archetype used by the custom volumetric test light.")
                     .ComboMap(volumetricsTestLightTypeMap)
                     .DefaultIndex(1));
    AddWidget(path, "Test Light Color", WIDGET_CVAR_COLOR_PICKER)
        .CVar("gEnhancements.Graphics.DebugLight.Color")
        .RaceDisable(false)
        .Options(ColorPickerOptions()
                     .Tooltip("Color of the custom volumetric test light.")
                     .Color(Colors::Orange)
                     .DefaultValue({ 255, 232, 196, 255 })
                     .ShowReset());
    AddWidget(path, "Test Light Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.Intensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Intensity multiplier for the custom test light.")
                     .Min(0.0f)
                     .Max(16.0f)
                     .DefaultValue(2.0f)
                     .Step(0.05f));
    AddWidget(path, "Test Light Volumetric Intensity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.VolumetricIntensity")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Extra volumetric contribution of the custom test light.")
                     .Min(0.0f)
                     .Max(8.0f)
                     .DefaultValue(2.0f)
                     .Step(0.05f));
    AddWidget(path, "Test Light Position X", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.PositionX")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("X offset of the custom test light relative to the player.")
                     .Min(-2000.0f)
                     .Max(2000.0f)
                     .DefaultValue(0.0f)
                     .Step(5.0f));
    AddWidget(path, "Test Light Position Y", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.PositionY")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Y offset of the custom test light relative to the player.")
                     .Min(-2000.0f)
                     .Max(2000.0f)
                     .DefaultValue(120.0f)
                     .Step(5.0f));
    AddWidget(path, "Test Light Position Z", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.PositionZ")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Z offset of the custom test light relative to the player.")
                     .Min(-2000.0f)
                     .Max(2000.0f)
                     .DefaultValue(220.0f)
                     .Step(5.0f));
    AddWidget(path, "Test Light Direction X", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.DirectionX")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("X direction of the custom test light.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "Test Light Direction Y", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.DirectionY")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Y direction of the custom test light.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(-1.0f)
                     .Step(0.01f));
    AddWidget(path, "Test Light Direction Z", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.DirectionZ")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Z direction of the custom test light.")
                     .Min(-1.0f)
                     .Max(1.0f)
                     .DefaultValue(0.0f)
                     .Step(0.01f));
    AddWidget(path, "Test Light Radius", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.Radius")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Radius used by the custom point or spot light.")
                     .Min(1.0f)
                     .Max(4000.0f)
                     .DefaultValue(420.0f)
                     .Step(5.0f));
    AddWidget(path, "Test Light Inner Cone", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.InnerCone")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Inner cone angle used when the test light type is Spot.")
                     .Min(0.0f)
                     .Max(180.0f)
                     .DefaultValue(20.0f)
                     .Step(1.0f));
    AddWidget(path, "Test Light Outer Cone", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.DebugLight.OuterCone")
        .RaceDisable(false)
        .Options(FloatSliderOptions().Tooltip("Outer cone angle used when the test light type is Spot.")
                     .Min(0.0f)
                     .Max(180.0f)
                     .DefaultValue(40.0f)
                     .Step(1.0f));
    AddWidget(path, "Test Light Cast Shadows", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.DebugLight.CastShadows")
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip("Allows the custom test light to consume one volumetric shadow slot when supported.")
                     .DefaultValue(false));
    AddWidget(path, "External Mod Resource Bars", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Use Centered Resource Bar Position", WIDGET_CVAR_CHECKBOX)
        .CVar("gEnhancements.Graphics.ExternalResourceBars.UseCenteredPosition")
        .RaceDisable(false)
        .Options(CheckboxOptions()
                     .Tooltip("Moves stacked external magic-style resource bars with centered screen coordinates. 0,0 is the middle of the screen.")
                     .DefaultValue(false));
    AddWidget(path, "Resource Bars Position X", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.ExternalResourceBars.PosX")
        .RaceDisable(false)
        .Options(IntSliderOptions()
                     .Tooltip("Horizontal position for the stacked external resource bars when centered positioning is enabled. 0 is screen center.")
                     .Min(-300)
                     .Max(300)
                     .DefaultValue(0));
    AddWidget(path, "Resource Bars Position Y", WIDGET_CVAR_SLIDER_INT)
        .CVar("gEnhancements.Graphics.ExternalResourceBars.PosY")
        .RaceDisable(false)
        .Options(IntSliderOptions()
                     .Tooltip("Vertical position for the stacked external resource bars when centered positioning is enabled. 0 is screen center.")
                     .Min(-220)
                     .Max(220)
                     .DefaultValue(0));
    AddWidget(path, "Resource Bars Stack Spacing", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar("gEnhancements.Graphics.ExternalResourceBars.StackSpacing")
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("Vertical distance between stacked external magic-style resource bars.")
                     .Min(12.0f)
                     .Max(24.0f)
                     .DefaultValue(17.0f)
                     .Step(0.5f));

    // Controls
    path.sidebarName = "Controls";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", "Controls", 2);
    AddWidget(path, "Clear Devices", WIDGET_BUTTON)
        .Callback([](WidgetInfo& info) {
            SohGui::mModalWindow->RegisterPopup(
                "Clear Config",
                "This will completely erase the controls config, including registered devices.\nContinue?", "Clear",
                "Cancel",
                []() {
                    Ship::Context::GetInstance()->GetConsoleVariables()->ClearBlock(CVAR_PREFIX_SETTING ".Controllers");
                    uint8_t bits = 0;
                    Ship::Context::GetInstance()->GetControlDeck()->Init(&bits);
                },
                nullptr);
        })
        .Options(ButtonOptions().Size(Sizes::Inline));
    AddWidget(path, "Controller Bindings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Bindings Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ControllerConfiguration"))
        .RaceDisable(false)
        .WindowName("Configure Controller")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip("Enables the separate Bindings Window."));

    // Input Viewer
    path.sidebarName = "Input Viewer";
    AddSidebarEntry("Settings", path.sidebarName, 3);
    AddWidget(path, "Input Viewer", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Toggle Input Viewer", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("InputViewer"))
        .RaceDisable(false)
        .WindowName("Input Viewer")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip("Toggles the Input Viewer.").EmbedWindow(false));

    AddWidget(path, "Input Viewer Settings", WIDGET_SEPARATOR_TEXT);
    AddWidget(path, "Popout Input Viewer Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("InputViewerSettings"))
        .RaceDisable(false)
        .WindowName("Input Viewer Settings")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip("Enables the separate Input Viewer Settings Window."));

    // Notifications
    path.sidebarName = "Notifications";
    path.column = SECTION_COLUMN_1;
    AddSidebarEntry("Settings", path.sidebarName, 3);
    AddWidget(path, "Position", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_SETTING("Notifications.Position"))
        .RaceDisable(false)
        .Options(ComboboxOptions()
                     .Tooltip("Which corner of the screen notifications appear in.")
                     .ComboMap(notificationPosition)
                     .DefaultIndex(3));
    AddWidget(path, "Duration (seconds):", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_SETTING("Notifications.Duration"))
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("How long notifications are displayed for.")
                     .Format("%.1f")
                     .Step(0.1f)
                     .Min(3.0f)
                     .Max(30.0f)
                     .DefaultValue(10.0f));
    AddWidget(path, "Background Opacity", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_SETTING("Notifications.BgOpacity"))
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("How opaque the background of notifications is.")
                     .DefaultValue(0.5f)
                     .IsPercentage());
    AddWidget(path, "Size:", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_SETTING("Notifications.Size"))
        .RaceDisable(false)
        .Options(FloatSliderOptions()
                     .Tooltip("How large notifications are.")
                     .Format("%.1f")
                     .Step(0.1f)
                     .Min(1.0f)
                     .Max(5.0f)
                     .DefaultValue(1.8f));
    AddWidget(path, "Test Notification", WIDGET_BUTTON)
        .RaceDisable(false)
        .Callback([](WidgetInfo& info) {
            Notification::Emit({
                .itemIcon = "__OTR__textures/icon_item_24_static/gQuestIconGoldSkulltulaTex",
                .prefix = "This",
                .message = "is a",
                .suffix = "test.",
            });
        })
        .Options(ButtonOptions().Tooltip("Displays a test notification."));

    // Mod Menu
    path.sidebarName = "Mod Menu";
    AddSidebarEntry("Settings", path.sidebarName, 1);
    AddWidget(path, "Popout Mod Menu Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ModMenu"))
        .WindowName("Mod Menu")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip("Enables the separate Mod Menu Window."));
}

} // namespace SohGui
