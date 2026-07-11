#include "ExternalModRenderInspectorWindow.h"

#include <imgui.h>

#include "ExternalModManager.h"
#include "soh/Notification/Notification.h"

namespace SOH {

void ExternalModRenderInspectorWindow::Draw() {
    auto& manager = ExternalModManager::Instance();
    if (!manager.IsRenderInspectorDebugGateActive() || !manager.HasVisibleRenderInspectorOverlay()) {
        return;
    }

    const std::string summary = manager.GetRenderInspectorSummaryText();
    if (summary.empty()) {
        return;
    }

    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGui::SetNextWindowPos(ImVec2(18.0f, 18.0f), ImGuiCond_Always);

    const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking |
                                         ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;

    if (!ImGui::Begin("Render Inspector Overlay", nullptr, windowFlags)) {
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.75f, 0.9f, 1.0f, 1.0f), "Render Inspector");
    ImGui::SameLine();
    if (ImGui::Button("Copy")) {
        ImGui::SetClipboardText(summary.c_str());
        Notification::Emit({
            .message = "Render inspector copied to clipboard.",
            .remainingTime = 2.0f,
        });
    }
    ImGui::TextDisabled("Active while volumetrics debug is enabled.");
    ImGui::Separator();
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 720.0f);
    ImGui::TextUnformatted(summary.c_str());
    ImGui::PopTextWrapPos();
    ImGui::End();
}

} // namespace SOH
