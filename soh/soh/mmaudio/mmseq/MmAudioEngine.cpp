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

// DESLIGADO POR PADRAO. O interpretador ja roda, mas o script da Sequence_0
// avanca o pc ate sair da regiao valida e derruba o jogo em ~350 ms
// (AudioScript_ScriptReadU8, seqplayer.cpp:554). Enquanto a causa nao estiver
// entendida, o motor nao pode ficar no caminho de audio do usuario.
//
// Ligar com SHIPLUA_MM_SEQ=1 no ambiente para investigar.
bool gEnabled = false;
bool gEnabledChecked = false;

bool EngineEnabled() {
    if (!gEnabledChecked) {
        gEnabledChecked = true;
        const char* env = std::getenv("SHIPLUA_MM_SEQ");
        gEnabled = (env != nullptr && env[0] == '1');
    }
    return gEnabled;
}

bool gReady = false;
bool gInitFailed = false;
bool gPcEscaped = false;
s32 gLastPlayedChannel = -1;
s32 gPeakNotes = 0;
s32 gChannelsOn = 0;
s32 gPlayerAlive = 0;
s32 gPcOffset = -1;
s32 gRenderedSamples = 0;
// Diagnóstico: onde o pc estava quando escapou, relativo ao início da
// sequência. Distingue "nunca foi válido" de "andou e passou do fim".
ptrdiff_t gEscapeOffset = 0;
s32 gTicksBeforeEscape = 0;
s32 gTickCount = 0;

// O pc do script tem que estar dentro do bloco de bytes da sequência. Se sair,
// o decomp continuaria lendo memória arbitrária.
const u8* gSeqStart = nullptr;
size_t gSeqSize = 0;
u8 gSeqHead[8] = { 0 };

bool PcInBounds(const SequencePlayer* sp) {
    if (gSeqStart == nullptr || sp->scriptState.pc == nullptr) {
        return false;
    }
    const u8* pc = sp->scriptState.pc;
    return pc >= gSeqStart && pc < (gSeqStart + gSeqSize);
}

// Posição de leitura por nota. O NoteSampleState do MM descreve O QUE tocar
// (amostra, pitch, volume) mas não ONDE está — isso vive no estado de síntese
// do RSP, que não portamos. Como a síntese é nossa, o cursor também é.
// Estado de SFX por canal, o equivalente do sSfxChannelState do MM. O script
// do canal lê e escreve aqui pelos opcodes 0xA0..0xA3.
u8 gSfxChannelState[16][16] = {};

// Função customizada da sequência (opcode 0xBE). O script a chama para obter
// frequência e bits de stereo da nota; devolver um valor sadio é o suficiente
// para a nota ser montada com o volume e o pitch padrão do efeito.
u32 SfxFreqAndStereo(s8 value, SequenceChannel* channel) {
    (void)channel;
    return (u32)(u8)value;
}

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

    // maxTempo governa o avanço do script: AudioScript_SequencePlayerProcessSequence
    // (seqplayer.c:1899) só avança quando tempoAcc alcança este valor. Zerado —
    // como ficaria pelo memset, já que não portamos o AudioHeap_Init que o
    // preenche — o script correria a cada tick sem respeitar tempo nenhum.
    //
    // Fórmula de heap.c:982:
    //   updatesPerFrame * 2880000 / gTatumsPerBeat / unk_2960
    // gTatumsPerBeat é 48 (TATUMS_PER_BEAT) e unk_2960 é o divisor de refresh,
    // 60 para NTSC.
    gAudioCtx.unk_2960 = 60.0f;
    gAudioCtx.refreshRate = 60;
    gAudioCtx.maxTempo = (u32)(kUpdatesPerFrame * 2880000.0f / 48.0f / 60.0f);
    gAudioCtx.numNotes = kNumNotes;
    gAudioCtx.notes = (Note*)std::calloc(kNumNotes, sizeof(Note));
    gAudioCtx.sampleStateList =
        (NoteSampleState*)std::calloc((size_t)kNumNotes * kUpdatesPerFrame, sizeof(NoteSampleState));

    if (gAudioCtx.notes == nullptr || gAudioCtx.sampleStateList == nullptr) {
        return false;
    }

    // Tabela de decaimento do envelope ADSR. Instrument::adsrDecayIndex indexa
    // nela, então sem ela nenhuma nota tem envelope — e é mais um campo que só
    // o AudioHeap_Init preenche (heap.c:1055), justamente o arquivo que não
    // portamos. Reprodução literal de AudioHeap_InitAdsrDecayTable (heap.c:32).
    static f32 sAdsrDecay[256];
    gAudioCtx.adsrDecayTable = sAdsrDecay;

    auto decay = [](f32 scaleInv) {
        return 256.0f * gAudioCtx.audioBufferParameters.updatesPerFrameInvScaled / scaleInv;
    };
    sAdsrDecay[255] = decay(0.25f);
    sAdsrDecay[254] = decay(0.33f);
    sAdsrDecay[253] = decay(0.5f);
    sAdsrDecay[252] = decay(0.66f);
    sAdsrDecay[251] = decay(0.75f);
    for (s32 i = 128; i < 251; i++) {
        sAdsrDecay[i] = decay((f32)(251 - i));
    }
    for (s32 i = 16; i < 128; i++) {
        sAdsrDecay[i] = decay((f32)(4 * (143 - i)));
    }
    for (s32 i = 1; i < 16; i++) {
        sAdsrDecay[i] = decay((f32)(60 * (23 - i)));
    }
    sAdsrDecay[0] = 0.0f;

    gCursors.assign(kNumNotes, NoteCursor{});
    return true;
}

} // namespace

bool MmSeq_IsReady() {
    return gReady;
}

bool MmSeq_PcEscaped() {
    return gPcEscaped;
}

void MmSeq_GetScriptStats(int* channelsOn, int* playerAlive, int* pcOffset) {
    if (channelsOn != nullptr) { *channelsOn = (int)gChannelsOn; }
    if (playerAlive != nullptr) { *playerAlive = (int)gPlayerAlive; }
    if (pcOffset != nullptr) { *pcOffset = (int)gPcOffset; }
}

void MmSeq_GetRenderStats(int* peakNotes, int* renderedSamples) {
    if (peakNotes != nullptr) {
        *peakNotes = (int)gPeakNotes;
    }
    if (renderedSamples != nullptr) {
        *renderedSamples = (int)gRenderedSamples;
    }
}

int MmSeq_LastChannel() {
    return (int)gLastPlayedChannel;
}

void MmSeq_GetSeqHead(unsigned char* out8) {
    for (s32 i = 0; i < 8; i++) {
        out8[i] = gSeqHead[i];
    }
}

void MmSeq_GetEscapeInfo(long long* offset, int* ticks, unsigned int* seqSize) {
    if (offset != nullptr) {
        *offset = (long long)gEscapeOffset;
    }
    if (ticks != nullptr) {
        *ticks = (int)gTicksBeforeEscape;
    }
    if (seqSize != nullptr) {
        *seqSize = (unsigned int)gSeqSize;
    }
}

bool MmSeq_Init() {
    if (gReady || gInitFailed) {
        return gReady;
    }
    if (!EngineEnabled()) {
        gInitFailed = true; // nao tenta de novo todo frame
        return false;
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
    gSeqStart = (const u8*)seq->seqData;
    gSeqSize = (size_t)seq->seqDataSize;
    seqPlayer->seqData = (u8*)seq->seqData;
    seqPlayer->scriptState.pc = (u8*)seq->seqData;
    seqPlayer->scriptState.depth = 0;
    seqPlayer->delay = 0;
    seqPlayer->finished = false;
    seqPlayer->playerIndex = kSfxSeqPlayer;
    seqPlayer->enabled = true;

    AudioScript_InitSequencePlayerChannels(kSfxSeqPlayer);

    // Guarda os primeiros bytes para o host poder logar: se a sequência não
    // for bytes de script de verdade, o pc se perde já no primeiro comando.
    for (s32 i = 0; i < 8 && i < (s32)gSeqSize; i++) {
        gSeqHead[i] = gSeqStart[i];
    }

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
        // Porta livre é SEQ_IO_VAL_NONE (-1), não 0 — AudioScript_InitSequenceChannel
        // (seqplayer.c:312) inicializa as oito portas com esse valor. Testar
        // contra 0 descartava TODOS os canais e a chamada sempre devolvia
        // "recusado".
        if (channel->seqScriptIO[0] != SEQ_IO_VAL_NONE) {
            continue; // canal ocupado
        }
        // Contrato das portas de IO da sequência de SFX do MM:
        //   0 = enable   1 = "pronto" (o script escreve de volta)
        //   2 = volume   4 = byte baixo do sfxId   5 = bits altos
        //
        // A porta 2 importa: sem ela o canal fica com SEQ_IO_VAL_NONE (-1) e o
        // script não monta a nota. Escrever só 4/5/0 fazia o sfx ser aceito e
        // não produzir som nenhum — os 16 canais ligavam, o player seguia vivo
        // e notas(pico) ficava em 0.
        channel->seqScriptIO[2] = 127; // volume cheio (s8)
        channel->seqScriptIO[4] = (s8)(sfxId & 0xFF);
        channel->seqScriptIO[5] = (s8)(sfxId >> 8);
        channel->seqScriptIO[0] = 1; // enable por último: destrava a leitura
        gLastPlayedChannel = ch;
        return true;
    }
    return false; // sem canal livre
}

void MmSeq_RenderInto(int16_t* buffer, uint32_t frames) {
    if (!gReady || buffer == nullptr || frames == 0) {
        return;
    }

    // Cadência do MM (AudioSynth_Update, synthesis.c:227): por quadro de áudio,
    // ProcessSequences roda updatesPerFrame vezes com o índice descendo até 0.
    // Chamar uma vez por bloco de render, como a primeira versão fazia, avança a
    // sequência ~12x devagar demais.
    const s32 samplesPerAudioFrame = kOutputRate / 60;
    const s32 audioFrames = (s32)(frames / samplesPerAudioFrame) + 1;

    for (s32 af = 0; af < audioFrames; af++) {
        for (s32 rev = kUpdatesPerFrame; rev > 0; rev--) {
            // Guarda: se o pc saiu do bloco da sequência, o script se perdeu.
            // Sem isto o decomp lê memória inválida e derruba o processo — foi
            // o crash de 25/07 (AudioScript_ScriptReadU8, seqplayer.cpp:554).
            SequencePlayer* sp = &gAudioCtx.seqPlayers[kSfxSeqPlayer];
            if (sp->enabled && !PcInBounds(sp)) {
                sp->enabled = false;
                gPcEscaped = true;
                gEscapeOffset = (const u8*)sp->scriptState.pc - gSeqStart;
                gTicksBeforeEscape = gTickCount;
                break;
            }
            gTickCount++;
            AudioScript_ProcessSequences(rev - 1);
        }
    }

    // O interpretador escreve no slot indicado por sampleStateOffset, que é
    // (updatesPerFrame - arg0 - 1) * numNotes. Com o último arg0 = 0, o bloco
    // válido é o do índice updatesPerFrame-1 — ler a partir de 0, como a
    // primeira versão fazia, pegaria slots que ninguém preencheu.
    NoteSampleState* states = &gAudioCtx.sampleStateList[(kUpdatesPerFrame - 1) * kNumNotes];

    // Diagnóstico: quantas notas o interpretador realmente habilitou. Separa
    // "a sequência não gerou nota" de "gerou mas o render não a tocou" — sem
    // isto, silêncio é ambíguo entre as duas coisas.
    // Conta canais habilitados e se o player ainda esta vivo. Se o sfx foi
    // aceito mas nenhum canal liga, o problema esta no script da sequencia
    // antes de chegar a nota — nao no envelope nem na amostra.
    {
        SequencePlayer* sp = &gAudioCtx.seqPlayers[kSfxSeqPlayer];
        s32 chOn = 0;
        for (s32 c = 0; c < SEQ_NUM_CHANNELS; c++) {
            SequenceChannel* ch = sp->channels[c];
            if (ch != nullptr && IS_SEQUENCE_CHANNEL_VALID(ch) && ch->enabled) {
                chOn++;
            }
        }
        gChannelsOn = chOn;
        gPlayerAlive = sp->enabled ? 1 : 0;
        gPcOffset = (gSeqStart != nullptr && sp->scriptState.pc != nullptr)
                        ? (s32)((const u8*)sp->scriptState.pc - gSeqStart)
                        : -1;
    }

    s32 enabledCount = 0;
    for (s32 i = 0; i < gAudioCtx.numNotes; i++) {
        if (gAudioCtx.sampleStateList[(kUpdatesPerFrame - 1) * kNumNotes + i].bitField0.enabled) {
            enabledCount++;
        }
    }
    if (enabledCount > gPeakNotes) {
        gPeakNotes = enabledCount;
    }

    for (s32 i = 0; i < gAudioCtx.numNotes; i++) {
        NoteSampleState* state = &states[i];
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
            gRenderedSamples++;
        }
    }
}

} // namespace ShipLua
