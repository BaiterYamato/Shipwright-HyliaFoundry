// Lado do OoT da ponte. Este arquivo inclui os headers do SoH e NUNCA os do MM
// — ver MmAudioBridge.h para o motivo.

#include "soh/mmaudio/mmseq/MmAudioBridge.h"

#include <string>

#include <spdlog/spdlog.h>
#include <z64.h>

#include "z64audio.h"

#include "soh/ResourceManagerHelpers.h"

namespace {
// Mesmo prefixo que o MmCrossWorldArchive aplica ao montar o mm.o2r.
constexpr const char* kMmNamespace = "mm/";
} // namespace

extern "C" void* MmBridge_LoadSoundFont(const char* pathWithinMm) {
    if (pathWithinMm == nullptr) {
        return nullptr;
    }
    // O factory deriva o namespace das amostras do caminho do próprio recurso
    // (ver AudioSoundFontFactory.cpp), então basta pedir pelo caminho prefixado.
    const std::string full = kMmNamespace + std::string(pathWithinMm);
    return ResourceMgr_LoadAudioSoundFontByName(full.c_str());
}

extern "C" void* MmBridge_LoadSequence(const char* pathWithinMm) {
    if (pathWithinMm == nullptr) {
        return nullptr;
    }
    const std::string full = kMmNamespace + std::string(pathWithinMm);
    return ResourceMgr_LoadSeqPtrByName(full.c_str());
}
