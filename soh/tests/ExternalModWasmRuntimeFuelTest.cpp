#include "ExternalModWasmRuntime.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Binary form of tests/fixtures/wasm_infinite_loop.wat. Keeping the tiny
// fixture inline avoids requiring WABT command-line tools in production builds.
const std::vector<uint8_t> kInfiniteLoopModule = {
    0x00, 0x61, 0x73, 0x6D, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x04, 0x01, 0x60, 0x00, 0x00,
    0x03, 0x02, 0x01, 0x00,
    0x07, 0x08, 0x01, 0x04, 0x73, 0x70, 0x69, 0x6E, 0x00, 0x00,
    0x0A, 0x09, 0x01, 0x07, 0x00, 0x03, 0x40, 0x0C, 0x00, 0x0B, 0x0B,
};

int Fail(const std::string& message) {
    std::cerr << message << '\n';
    return 1;
}

} // namespace

int main() {
    SOH::ExternalModWasmConfig config;
    config.modId = "test.infinite_loop";
    config.maxInstructionsPerCall = 1000;

    SOH::ExternalModWasmRuntime runtime;
    std::string error;
    if (!runtime.Initialize(kInfiniteLoopModule, config, error)) {
        return Fail("fixture initialization failed: " + error);
    }

    if (runtime.InvokeExport("spin", error)) {
        return Fail("infinite export unexpectedly completed");
    }
    if (error.find("instruction fuel exhausted") == std::string::npos ||
        error.find("mod=test.infinite_loop") == std::string::npos || error.find("export=spin") == std::string::npos) {
        return Fail("fuel error lacks mod/export context: " + error);
    }
    if (!runtime.IsQuarantined() || runtime.IsInitialized()) {
        return Fail("runtime was not isolated after fuel exhaustion");
    }
    if (runtime.GetFuelExhaustionsThisFrame() != 1 || runtime.GetInstructionsThisFrame() != 1000) {
        return Fail("per-frame fuel telemetry is incorrect");
    }

    const auto telemetryIt = runtime.GetExportTelemetry().find("spin");
    if (telemetryIt == runtime.GetExportTelemetry().end() || telemetryIt->second.calls != 1 ||
        telemetryIt->second.instructions != 1000 || telemetryIt->second.lastInstructions != 1000 ||
        telemetryIt->second.fuelExhaustions != 1) {
        return Fail("per-export fuel telemetry is incorrect");
    }

    error.clear();
    if (runtime.InvokeExport("spin", error) || error.find("runtime quarantined") == std::string::npos) {
        return Fail("quarantined runtime accepted a second call");
    }

    std::cout << "WASM instruction fuel watchdog passed\n";
    return 0;
}
