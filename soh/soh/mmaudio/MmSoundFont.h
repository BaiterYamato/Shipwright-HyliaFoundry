#pragma once

namespace ShipLua {

// Diagnóstico da Fase 1 do port de áudio (handoff OOT-AUDIO-001): carrega um
// soundfont do MM com e sem o prefixo de namespace e registra quantas amostras
// cada caminho resolveu. Só escreve no log — não toca em nada do jogo.
//
// O carregador em si (LoadMmSoundFont) é interno por enquanto: expô-lo aqui
// exigiria arrastar z64.h/z64audio.h para dentro deste header, já que SoundFont
// é um typedef de struct anônimo e não pode ser declarado adiante. Quando a
// Fase 2 precisar dele, promova junto com os includes certos.
void ProbeMmSoundFonts();

} // namespace ShipLua
