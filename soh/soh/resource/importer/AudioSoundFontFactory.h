#pragma once

#include <string>

#include <ship/resource/Resource.h>
#include <ship/resource/ResourceFactoryBinary.h>
#include <ship/resource/ResourceFactoryXML.h>
#include "soh/resource/type/AudioSoundFont.h"

namespace SOH {

// Prefixo de namespace aplicado aos caminhos de amostra ao carregar um soundfont
// do jogo vizinho. Os caminhos gravados dentro do arquivo são relativos à raiz do
// o2r de origem, então carregar "mm/audio/fonts/Soundfont_0" sem isto resolveria
// as amostras para as do OoT de mesmo nome, em silêncio.
// Sempre restaure para "" depois do load (use um RAII).
void SetSoundFontSamplePathPrefix(const std::string& prefix);

class ResourceFactoryBinaryAudioSoundFontV2 final : public Ship::ResourceFactoryBinary {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
};

class ResourceFactoryXMLSoundFontV0 final : public Ship::ResourceFactoryXML {
  public:
    std::shared_ptr<Ship::IResource> ReadResource(std::shared_ptr<Ship::File> file,
                                                  std::shared_ptr<Ship::ResourceInitData> initData) override;
    static int8_t MediumStrToInt(const char* str, const char* file);
    static int8_t CachePolicyToInt(const char* str, const char* file);

  private:
    void ParseDrums(AudioSoundFont* soundFont, tinyxml2::XMLElement* element);
    void ParseInstruments(AudioSoundFont* soundFont, tinyxml2::XMLElement* element);
    void ParseSfxTable(AudioSoundFont* soundFont, tinyxml2::XMLElement* element);
    std::vector<AdsrEnvelope> ParseEnvelopes(AudioSoundFont* soundFont, tinyxml2::XMLElement* element,
                                             unsigned int* count);
};

} // namespace SOH
