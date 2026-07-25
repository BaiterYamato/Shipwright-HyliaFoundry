// Motor de SFX do MM. Este arquivo é do lado do MM da fronteira: inclui os
// tipos do MM e NUNCA um header do OoT (ver MmAudioBridge.h).

#include "soh/mmaudio/mmseq/MmAudioEngine.h"

#include <cstdlib>
#include <cstring>
#include <vector>

#include "soh/mmaudio/mmseq/MmAudioContext.h"
#include "soh/mmaudio/mmseq/MmAudioBridge.h"
#include "soh/mmaudio/mmseq/MmAudioDecls.h"

namespace ShipLua {

namespace {

using namespace mmsfx;

// A sequência 0 do MM é a de efeitos sonoros — é por ela que todo SFX do jogo
// passa. O player 0 é quem a executa.
constexpr s32 kSfxSeqPlayer = 0;
constexpr s32 kNumNotes = 32;         // simultaneidade; o MM usa mais, mas SFX cabe nisto
constexpr s32 kUpdatesPerFrame = 4;   // divisões do tick de áudio por frame de vídeo
constexpr s32 kOutputRate = 32000;

bool gReady = false;
bool gInitFailed = false;

// Posição de leitura por nota. O NoteSampleState do MM descreve O QUE tocar
// (amostra, pitch, volume) mas não ONDE está — isso vive no estado de síntese
// do RSP, que não portamos. Como a síntese é nossa, o cursor também é.
struct NoteCursor {
    const Sample* sample = nullptr;
    std::vector<s16> pcm; // amostra já decodificada
    double position = 0.0;
};
std::vector<NoteCursor> gCursors;

// ---------------------------------------------------------------------------

s16 Clamp16(s32 value) {
    if (value < -0x8000) {
        return -0x8000;
    }
    if (value > 0x7FFF) {
        return 0x7FFF;
    }
    return (s16)value;
}

// Decodificação VADPCM, mesma da Fase 1 (soh/soh/mmaudio/MmSfxPlayer.cpp), aqui
// operando sobre o tipo Sample do MM. Espelha aADPCMdecImpl de mixer.c:183.
std::vector<s16> DecodeSample(const Sample* sample) {
    std::vector<s16> pcm;
    if (sample == nullptr || sample->sampleAddr == nullptr || sample->book == nullptr ||
        sample->book->codeBook == nullptr) {
        return pcm;
    }

    const bool small = (sample->codec == 3); // CODEC_SMALL_ADPCM
    if (sample->codec != 0 && !small) {
        return pcm;
    }
    if (sample->book->order != 2 || sample->book->numPredictors <= 0) {
        return pcm;
    }

    const s32 frameBytes = small ? 5 : 9;
    const u8* in = sample->sampleAddr;
    const u32 frames = sample->size / frameBytes;
    pcm.reserve((size_t)frames * 16);

    s16 prev1 = 0;
    s16 prev2 = 0;

    for (u32 f = 0; f < frames; f++) {
        const u8 header = *in++;
        const s32 shift = header >> 4;
        const s32 tableIndex = header & 0xF;
        if (tableIndex >= sample->book->numPredictors) {
            in += frameBytes - 1;
            continue;
        }
        const s16* row0 = sample->book->codeBook + (tableIndex * 2 * 8);
        const s16* row1 = row0 + 8;

        for (s32 half = 0; half < 2; half++) {
            s32 ins[8];
            if (small) {
                for (s32 j = 0; j < 2; j++) {
                    const u8 b = *in++;
                    ins[j * 4] = (((s32)(b >> 6) << 30) >> 30) << shift;
                    ins[j * 4 + 1] = (((s32)((b >> 4) & 3) << 30) >> 30) << shift;
                    ins[j * 4 + 2] = (((s32)((b >> 2) & 3) << 30) >> 30) << shift;
                    ins[j * 4 + 3] = (((s32)(b & 3) << 30) >> 30) << shift;
                }
            } else {
                for (s32 j = 0; j < 4; j++) {
                    const u8 b = *in++;
                    ins[j * 2] = (((s32)(b >> 4) << 28) >> 28) << shift;
                    ins[j * 2 + 1] = (((s32)(b & 0xF) << 28) >> 28) << shift;
                }
            }

            s16 written[8];
            for (s32 j = 0; j < 8; j++) {
                s32 acc = row0[j] * prev2 + row1[j] * prev1 + (ins[j] << 11);
                for (s32 k = 0; k < j; k++) {
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

// Aloca o que o interpretador espera encontrar pronto no contexto. No MM isto
// vem do heap de áudio do N64 (heap.c), que não portamos.
bool AllocateContext() {
    std::memset(&gAudioCtx, 0, sizeof(gAudioCtx));

    gAudioCtx.audioBufferParameters.samplingFreq = kOutputRate;
    gAudioCtx.audioBufferParameters.aiSamplingFreq = kOutputRate;
    gAudioCtx.audioBufferParameters.resampleRate = 1.0f;
    gAudioCtx.audioBufferParameters.updatesPerFrame = kUpdatesPerFrame;
    // 32000 / 60 fps / 4 updates = ~133 amostras por update.
    gAudioCtx.audioBufferParameters.numSamplesPerUpdate = kOutputRate / 60 / kUpdatesPerFrame;
    gAudioCtx.audioBufferParameters.numSamplesPerUpdateMax =
        gAudioCtx.audioBufferParameters.numSamplesPerUpdate + 8;
    gAudioCtx.audioBufferParameters.numSamplesPerUpdateMin =
        gAudioCtx.audioBufferParameters.numSamplesPerUpdate - 8;
    gAudioCtx.audioBufferParameters.updatesPerFrameInv = 1.0f / kUpdatesPerFrame;
    gAudioCtx.audioBufferParameters.updatesPerFrameInvScaled = (1.0f / kUpdatesPerFrame) / 256.0f;
    gAudioCtx.audioBufferParameters.updatesPerFrameScaled = kUpdatesPerFrame / 4.0f;
    gAudioCtx.audioBufferParameters.numSequencePlayers = 1;

    gAudioCtx.numNotes = kNumNotes;
    gAudioCtx.notes = (Note*)std::calloc(kNumNotes, sizeof(Note));
    gAudioCtx.sampleStateList =
        (NoteSampleState*)std::calloc((size_t)kNumNotes * kUpdatesPerFrame, sizeof(NoteSampleState));

    if (gAudioCtx.notes == nullptr || gAudioCtx.sampleStateList == nullptr) {
        return false;
    }

    gCursors.assign(kNumNotes, NoteCursor{});
    return true;
}

} // namespace

bool MmSeq_IsReady() {
    return gReady;
}

bool MmSeq_Init() {
    if (gReady || gInitFailed) {
        return gReady;
    }

    MmAudio_InitPathTables();

    if (!AllocateContext()) {
        gInitFailed = true;
        return false;
    }

    // O soundfont 0 do MM é o que carrega os SFX. Se ainda não estiver
    // disponível, sair sem marcar falha permite tentar de novo no próximo frame
    // — o mm.o2r pode não ter montado ainda.
    SoundFont* font = ResourceMgr_LoadAudioSoundFontByName(gFontMap[0]);
    if (font == nullptr) {
        return false;
    }
    gAudioCtx.soundFontList = font;

    // A sequência 0 do MM É o motor de SFX: cada efeito é um canal dela. Sem
    // carregar e iniciar isto o player fica desabilitado, ProcessSequences o
    // ignora e nenhum canal existe para receber um id.
    SequenceData* seq = reinterpret_cast<SequenceData*>(MmBridge_LoadSequence(gSequenceMap[0]));
    if (seq == nullptr || seq->seqData == nullptr) {
        return false;
    }

    // Ordem importa: a lista de camadas e a de notas livres precisam existir
    // antes de qualquer canal tentar alocar.
    AudioScript_InitLayerFreelist();
    AudioPlayback_InitNoteFreeList();
    AudioScript_InitSequencePlayers();

    // Start do player, espelhando AudioLoad_SyncInitSeqPlayerInternal
    // (load.c) sem a parte de DMA e cache, que aqui não existe.
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[kSfxSeqPlayer];
    AudioScript_SequencePlayerDisable(seqPlayer);
    AudioScript_ResetSequencePlayer(seqPlayer);

    seqPlayer->seqId = 0;
    seqPlayer->defaultFont = 0;
    seqPlayer->seqData = (u8*)seq->seqData;
    seqPlayer->scriptState.pc = (u8*)seq->seqData;
    seqPlayer->scriptState.depth = 0;
    seqPlayer->delay = 0;
    seqPlayer->finished = false;
    seqPlayer->playerIndex = kSfxSeqPlayer;
    seqPlayer->enabled = true;

    AudioScript_InitSequencePlayerChannels(kSfxSeqPlayer);

    gReady = true;
    return true;
}

bool MmSeq_PlaySfx(uint16_t sfxId) {
    if (!gReady) {
        return false;
    }

    // É assim que o MM dispara um SFX: escreve o id nas portas de IO do canal e
    // liga o canal. A sequência 0 lê essas portas e monta a nota.
    //   porta 0 = enable, 4 = byte baixo do id, 5 = bits altos
    SequencePlayer* seqPlayer = &gAudioCtx.seqPlayers[kSfxSeqPlayer];
    for (s32 ch = 0; ch < SEQ_NUM_CHANNELS; ch++) {
        SequenceChannel* channel = seqPlayer->channels[ch];
        if (channel == nullptr || !IS_SEQUENCE_CHANNEL_VALID(channel)) {
            continue;
        }
        if (channel->seqScriptIO[0] != 0) {
            continue; // canal ocupado
        }
        channel->seqScriptIO[4] = (s8)(sfxId & 0xFF);
        channel->seqScriptIO[5] = (s8)(sfxId >> 8);
        channel->seqScriptIO[0] = 1;
        return true;
    }
    return false; // sem canal livre
}

void MmSeq_RenderInto(int16_t* buffer, uint32_t frames) {
    if (!gReady || buffer == nullptr || frames == 0) {
        return;
    }

    // Um tick de sequência por chamada. O interpretador escreve o que cada nota
    // deve tocar em gAudioCtx.sampleStateList.
    AudioScript_ProcessSequences(0);

    for (s32 i = 0; i < gAudioCtx.numNotes; i++) {
        NoteSampleState* state = &gAudioCtx.sampleStateList[i];
        if (!state->bitField0.enabled || state->tunedSample == nullptr) {
            continue;
        }

        const Sample* sample = state->tunedSample->sample;
        NoteCursor& cursor = gCursors[i];

        // Decodifica uma vez por amostra e reaproveita enquanto a nota tocar.
        if (cursor.sample != sample) {
            cursor.sample = sample;
            cursor.pcm = DecodeSample(sample);
            cursor.position = 0.0;
        }
        if (cursor.pcm.empty()) {
            continue;
        }

        // frequencyFixedPoint é UQ8.8: 0x0100 = velocidade original.
        const double step = state->frequencyFixedPoint / 256.0;
        const double volLeft = state->targetVolLeft / 32768.0;
        const double volRight = state->targetVolRight / 32768.0;

        for (uint32_t f = 0; f < frames; f++) {
            const size_t index = (size_t)cursor.position;
            if (index >= cursor.pcm.size()) {
                break; // amostra acabou
            }
            const s32 value = cursor.pcm[index];
            buffer[f * 2] = Clamp16(buffer[f * 2] + (s32)(value * volLeft));
            buffer[f * 2 + 1] = Clamp16(buffer[f * 2 + 1] + (s32)(value * volRight));
            cursor.position += step;
        }
    }
}

} // namespace ShipLua
