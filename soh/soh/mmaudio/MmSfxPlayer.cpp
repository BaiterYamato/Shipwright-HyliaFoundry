#include "soh/mmaudio/MmSfxPlayer.h"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

#include <spdlog/spdlog.h>
#include <z64.h>

#include "z64audio.h"

#include "soh/ResourceManagerHelpers.h"

namespace ShipLua {

namespace {

// Formato VADPCM (CODEC_ADPCM) do N64: cada quadro é 1 byte de cabeçalho mais
// 8 bytes de nibbles, produzindo 16 amostras.
constexpr int kFrameBytes = 9;
constexpr int kSamplesPerFrame = 16;
constexpr int kOrder = 2; // o VADPCM do N64 é sempre de ordem 2

int16_t Clamp16(int32_t value) {
    if (value < -0x8000) {
        return -0x8000;
    }
    if (value > 0x7FFF) {
        return 0x7FFF;
    }
    return static_cast<int16_t>(value);
}

// Espelha aADPCMdecImpl de soh/soh/mixer.c:183 — a implementação em software do
// microcódigo de áudio do N64 que o próprio SoH já usa. Reproduzida aqui em vez
// de reaproveitada porque aquela opera sobre o DMEM emulado do RSP, e aqui só
// queremos um decodificador direto de amostra para PCM.
std::vector<int16_t> DecodeVadpcm(const SoundFontSample* sample) {
    std::vector<int16_t> pcm;

    if (sample == nullptr || sample->sampleAddr == nullptr || sample->book == nullptr ||
        sample->book->book == nullptr) {
        return pcm;
    }
    if (sample->codec != CODEC_ADPCM) {
        SPDLOG_WARN("ShipLua/mmaudio: codec {} n\xC3\xA3o suportado nesta fase (s\xC3\xB3 CODEC_ADPCM)",
                    static_cast<uint32_t>(sample->codec));
        return pcm;
    }
    if (sample->book->order != kOrder || sample->book->npredictors <= 0) {
        SPDLOG_WARN("ShipLua/mmaudio: book inesperado (order={} npredictors={})", sample->book->order,
                    sample->book->npredictors);
        return pcm;
    }

    const uint8_t* in = sample->sampleAddr;
    const uint32_t frames = sample->size / kFrameBytes;
    pcm.reserve(static_cast<size_t>(frames) * kSamplesPerFrame);

    int16_t prev1 = 0;
    int16_t prev2 = 0;

    for (uint32_t frame = 0; frame < frames; frame++) {
        const uint8_t header = *in++;
        const int shift = header >> 4;
        const int tableIndex = header & 0xF;

        if (tableIndex >= sample->book->npredictors) {
            in += kFrameBytes - 1; // quadro corrompido: pula sem tentar prever
            continue;
        }

        const int16_t* table = sample->book->book + (tableIndex * kOrder * 8);
        const int16_t* row0 = table;
        const int16_t* row1 = table + 8;

        // Duas metades de 8 amostras; prev1/prev2 ficam fixos dentro de cada
        // metade — a recorrência dentro dela sai do somatório em k.
        for (int half = 0; half < 2; half++) {
            int32_t ins[8];
            for (int j = 0; j < 4; j++) {
                const uint8_t byte = *in++;
                // (x << 28) >> 28 estende o sinal do valor de 4 bits, igual ao mixer.
                ins[j * 2] = ((static_cast<int32_t>(byte >> 4) << 28) >> 28) << shift;
                ins[j * 2 + 1] = ((static_cast<int32_t>(byte & 0xF) << 28) >> 28) << shift;
            }

            int16_t written[8];
            for (int j = 0; j < 8; j++) {
                int32_t acc = row0[j] * prev2 + row1[j] * prev1 + (ins[j] << 11);
                for (int k = 0; k < j; k++) {
                    acc += row1[(j - k) - 1] * ins[k];
                }
                acc >>= 11;
                written[j] = Clamp16(acc);
                pcm.push_back(written[j]);
            }

            prev2 = written[6];
            prev1 = written[7];
        }
    }

    return pcm;
}

struct OneShot {
    std::vector<int16_t> pcm;
    size_t cursor = 0;
};

// A thread do jogo empurra, a thread de áudio consome. Um mutex é aceitável para
// esta fase de prova; a Fase 3 troca por uma fila lock-free, que é o desenho que
// o plano descreve e o que o fork de referência usa.
std::mutex gMutex;
std::vector<OneShot> gActive;
std::atomic<bool> gHasPending{ false };

} // namespace

bool MmAudio_PlaySampleOneShot(const char* prefixedSamplePath) {
    SoundFontSample* sample = ResourceMgr_LoadAudioSample(prefixedSamplePath);
    if (sample == nullptr) {
        SPDLOG_WARN("ShipLua/mmaudio: amostra '{}' n\xC3\xA3o carregou", prefixedSamplePath);
        return false;
    }

    std::vector<int16_t> pcm = DecodeVadpcm(sample);
    if (pcm.empty()) {
        SPDLOG_WARN("ShipLua/mmaudio: amostra '{}' decodificou vazia (size={}B)", prefixedSamplePath, sample->size);
        return false;
    }

    SPDLOG_INFO("ShipLua/mmaudio: tocando '{}' — {}B comprimidos -> {} amostras ({} ms a 32 kHz)", prefixedSamplePath,
                sample->size, pcm.size(), pcm.size() * 1000 / 32000);

    {
        std::lock_guard<std::mutex> lock(gMutex);
        gActive.push_back(OneShot{ std::move(pcm), 0 });
    }
    gHasPending.store(true, std::memory_order_release);
    return true;
}

bool MmAudio_HasPending() {
    return gHasPending.load(std::memory_order_acquire);
}

void MmAudio_MixInto(int16_t* buffer, uint32_t frames) {
    if (buffer == nullptr || frames == 0 || !MmAudio_HasPending()) {
        return;
    }

    std::lock_guard<std::mutex> lock(gMutex);

    // Prova, uma vez só, que a thread de áudio realmente recebeu o material —
    // separa "enfileirou mas nunca misturou" de "misturou mas não se ouve".
    static bool sLoggedFirstMix = false;
    if (!sLoggedFirstMix && !gActive.empty()) {
        sLoggedFirstMix = true;
        SPDLOG_INFO("ShipLua/mmaudio: primeira mixagem na thread de \xC3\xA1udio — {} quadros, {} amostras pendentes",
                    frames, gActive.front().pcm.size());
    }

    for (auto& shot : gActive) {
        const size_t available = shot.pcm.size() - shot.cursor;
        const size_t count = (available < frames) ? available : frames;

        for (size_t i = 0; i < count; i++) {
            // Amostra mono do MM entra nos dois canais; a atenuação evita somar
            // em cima do áudio do jogo e estourar o clamp.
            const int32_t value = shot.pcm[shot.cursor + i] / 2;
            buffer[i * 2] = Clamp16(buffer[i * 2] + value);
            buffer[i * 2 + 1] = Clamp16(buffer[i * 2 + 1] + value);
        }
        shot.cursor += count;
    }

    gActive.erase(std::remove_if(gActive.begin(), gActive.end(),
                                 [](const OneShot& shot) { return shot.cursor >= shot.pcm.size(); }),
                  gActive.end());

    gHasPending.store(!gActive.empty(), std::memory_order_release);
}

} // namespace ShipLua
