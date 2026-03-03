# Patch v7.1 - Ambient Occlusion real (SSAO)

## Scope

- Implement SSAO runtime on OpenGL and DX11.
- Add Metal fallback signaling (AO disabled, non-fatal).
- Add data-driven AO fields in `render/pbr_profiles.json`.
- Add global UI/CVar controls for AO.
- Preserve existing world graphics and fog/postfx behavior.

## Deliverables

1. **Contract/runtime**
   - `ExternalModPbrDefinition` extended with `ambientOcclusion`.
   - Strict parser validation/clamps in `TryParsePbrDefinitions`.
   - `ExternalModWorldGraphicsRuntime` resolves AO profile and applies CVar precedence.
2. **Renderer bridge**
   - New `gfx_set_ambient_occlusion_*` C bridge calls.
   - Fallback reason query/clear path.
3. **Backends**
   - OpenGL SSAO generation + blur + composite pass.
   - DX11 SSAO generation + blur + composite pass.
   - Metal fallback reason reporting.
4. **UI**
   - Graphics menu controls:
     - AO enable
     - AO quality
     - AO intensity scale
     - AO debug view

## Acceptance checks

- AO off matches baseline.
- AO low/medium/high visibly affects contact shading.
- Metal backend keeps rendering stable with fallback reason.
- Fog/postfx presets remain functional.
