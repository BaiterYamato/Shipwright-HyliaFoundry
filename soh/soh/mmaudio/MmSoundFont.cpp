#include "soh/mmaudio/MmSoundFont.h"

#include <string>

#include <spdlog/spdlog.h>
#include <z64.h>

#include "z64audio.h"

#include "soh/ResourceManagerHelpers.h"
#include "soh/resource/importer/AudioSoundFontFactory.h"

namespace ShipLua {

namespace {

// Mesmo prefixo que o MmCrossWorldArchive aplica ao montar o mm.o2r.
// Ver ShipLuaBootstrap.cpp (kMmNamespace).
constexpr const char* kMmNamespace = "mm/";

// Instala o prefixo de amostras enquanto o soundfont é lido e o remove ao sair,
// inclusive por exceção. O caminho normal do OoT nunca vê prefixo.
class ScopedSamplePrefix {
  public:
    explicit ScopedSamplePrefix(const std::string& prefix) {
        SOH::SetSoundFontSamplePathPrefix(prefix);
    }
    ~ScopedSamplePrefix() {
        SOH::SetSoundFontSamplePathPrefix("");
    }
    ScopedSamplePrefix(const ScopedSamplePrefix&) = delete;
    ScopedSamplePrefix& operator=(const ScopedSamplePrefix&) = delete;
};

struct FontStats {
    int total = 0;
    int resolved = 0;
    uint32_t firstSampleSize = 0;
};

// Conta quantas referências de amostra do soundfont apontam para dados de
// verdade. Com o prefixo errado a esmagadora maioria vira nullptr, porque só 15
// dos 682 nomes de amostra do MM existem também no OoT.
void CountSample(const SoundFontSound& sound, FontStats& stats) {
    stats.total++;
    if (sound.sample == nullptr) {
        return;
    }
    stats.resolved++;
    if (stats.firstSampleSize == 0) {
        stats.firstSampleSize = sound.sample->size;
    }
}

FontStats CollectStats(const SoundFont* font) {
    FontStats stats;
    if (font == nullptr) {
        return stats;
    }

    for (uint32_t i = 0; i < font->numSfx; i++) {
        CountSample(font->soundEffects[i], stats);
    }

    for (uint8_t i = 0; i < font->numInstruments; i++) {
        const Instrument* instrument = font->instruments[i];
        if (instrument == nullptr) {
            continue;
        }
        CountSample(instrument->lowNotesSound, stats);
        CountSample(instrument->normalNotesSound, stats);
        CountSample(instrument->highNotesSound, stats);
    }

    for (uint8_t i = 0; i < font->numDrums; i++) {
        const Drum* drum = font->drums[i];
        if (drum == nullptr) {
            continue;
        }
        CountSample(drum->sound, stats);
    }

    return stats;
}

void LogFont(const char* label, const char* path, const SoundFont* font) {
    if (font == nullptr) {
        SPDLOG_INFO("ShipLua/mmaudio: {} '{}' -> NAO CARREGOU", label, path);
        return;
    }

    const FontStats stats = CollectStats(font);
    SPDLOG_INFO("ShipLua/mmaudio: {} '{}' -> inst={} drums={} sfx={} | amostras {}/{} resolvidas | 1a amostra={}B",
                label, path, font->numInstruments, font->numDrums, font->numSfx, stats.resolved, stats.total,
                stats.firstSampleSize);
}

// Carrega um soundfont do mm.o2r resolvendo as amostras dentro do namespace do
// MM. Recebe o caminho SEM prefixo, como está gravado dentro do arquivo:
// "audio/fonts/Soundfont_0".
SoundFont* LoadMmSoundFont(const std::string& pathWithinMm) {
    const std::string fullPath = kMmNamespace + pathWithinMm;
    ScopedSamplePrefix prefix(kMmNamespace);
    return ResourceMgr_LoadAudioSoundFontByName(fullPath.c_str());
}

} // namespace

void ProbeMmSoundFonts() {
    // Alvo: o soundfont que carrega os SFX do MM.
    SoundFont* withPrefix = LoadMmSoundFont("audio/fonts/Soundfont_0");
    LogFont("COM prefixo ", "mm/audio/fonts/Soundfont_0", withPrefix);

    // Controle: outro font, carregado SEM prefixo. Precisa ser um caminho
    // diferente porque o ResourceManager guarda o recurso em cache — recarregar
    // o mesmo caminho devolveria o objeto acima em vez de reexecutar o factory.
    SoundFont* withoutPrefix = ResourceMgr_LoadAudioSoundFontByName("mm/audio/fonts/Soundfont_1");
    LogFont("SEM prefixo ", "mm/audio/fonts/Soundfont_1", withoutPrefix);

    SPDLOG_INFO("ShipLua/mmaudio: se o primeiro resolveu quase tudo e o segundo quase nada, "
                "o prefixo de namespace esta funcionando (OOT-AUDIO-001 Fase 1)");
}

} // namespace ShipLua
