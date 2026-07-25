#pragma once

#include <cstdint>

namespace ShipLua {

// Decodifica uma amostra do mm.o2r e a enfileira para tocar uma vez.
// Recebe o caminho COM prefixo: "mm/audio/samples/AdultLinkAttack1_META".
// Chamada da thread do jogo. Devolve false se a amostra não carregou ou o codec
// não é suportado.
bool MmAudio_PlaySampleOneShot(const char* prefixedSamplePath);

// Soma o áudio pendente do MM no buffer que o host vai entregar ao dispositivo.
// Chamada da THREAD DE ÁUDIO (OTRAudio_Thread), formato do SoH: 32 kHz, estéreo,
// s16 intercalado. `frames` é o número de quadros estéreo, não de amostras.
void MmAudio_MixInto(int16_t* buffer, uint32_t frames);

// Há algo tocando? Permite ao host pular a cópia do buffer quando não há nada.
bool MmAudio_HasPending();

} // namespace ShipLua
