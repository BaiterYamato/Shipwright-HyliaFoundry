#pragma once

// Motor de SFX do MM: liga o interpretador de sequência portado ao
// decodificador VADPCM da Fase 1 e à saída de áudio do host.
//
// API de tipos simples de propósito — é a fronteira descrita em
// MmAudioBridge.h. Quem chama daqui nunca vê um tipo do MM.

#include <cstdint>

namespace ShipLua {

// Carrega Soundfont_0 e Sequence_0 do mm.o2r, monta o contexto e dá start no
// player de SFX. Idempotente: pode ser chamada todo frame, o trabalho acontece
// uma vez. Devolve true quando o motor está pronto.
bool MmSeq_Init();

bool MmSeq_IsReady();

// Dispara um sfxId do MM escrevendo nas portas de channel IO da sequência,
// que é como o próprio MM toca seus efeitos.
bool MmSeq_PlaySfx(uint16_t sfxId);

// Avança a sequência e mistura o resultado no buffer de saída.
// Chamada da THREAD DE ÁUDIO. Formato do host: 32 kHz, estéreo, s16
// intercalado; `frames` é o número de quadros estéreo.
void MmSeq_RenderInto(int16_t* buffer, uint32_t frames);

} // namespace ShipLua
