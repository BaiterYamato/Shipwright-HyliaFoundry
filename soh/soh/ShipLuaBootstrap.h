#pragma once

namespace ShipLua {
class ModHost;
}

namespace ShipLuaHost {

class OotHotkeyRegistry;

void Initialize();
void Shutdown();
ShipLua::ModHost* GetModHost();
OotHotkeyRegistry* Hotkeys();

} // namespace ShipLuaHost
