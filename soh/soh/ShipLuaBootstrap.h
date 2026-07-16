#pragma once

namespace ShipLua {
class ModHost;
}

namespace ShipLuaHost {

class OotHotkeyRegistry;
class OotWorldAdapter;

void Initialize();
void Shutdown();
ShipLua::ModHost* GetModHost();
OotHotkeyRegistry* Hotkeys();
OotWorldAdapter* WorldAdapter();
void OpenLogWindow();

} // namespace ShipLuaHost
