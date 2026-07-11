#pragma once

#include <libultraship/libultraship.h>

namespace SOH {

class ExternalModRenderInspectorWindow final : public Ship::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void Draw() override;
    void InitElement() override {
    }
    void DrawElement() override {
    }
    void UpdateElement() override {
    }
};

} // namespace SOH
