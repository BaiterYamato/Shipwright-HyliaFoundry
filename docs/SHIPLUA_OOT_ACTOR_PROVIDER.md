# Provider nativo de atores OoT

`OOT-MODSDK-001` implementa o provider `shipwright-native` para a API genérica de atores do ShipLua 0.4.

## Contrato deste recorte

- somente a thread principal do jogo pode criar, consultar ou destruir atores;
- o mod informa um ID lógico allowlisted, nunca um `ActorId` numérico;
- nenhum `Actor*` sai do provider;
- cada ator recebe um `ShipLua::Handle` com slot, geração, geração de cena e ownership;
- o limite inicial é de 16 atores vivos por mod e 256 no host;
- a dependência de object precisa estar carregada antes do spawn;
- definições externas podem executar um preflight antes de criar o handle ou o ator nativo;
- handles são invalidados quando o ator nativo é destruído, o mod é descarregado ou o `PlayState` termina;
- `ACTOR_PLAYER` é rejeitado mesmo que seja incluído acidentalmente na allowlist.

IDs iniciais:

| ID lógico | Ator no host OoT | Object obrigatório | Params fixos |
|---|---|---|---|
| `oot.en_dog` | `ACTOR_EN_DOG` | `OBJECT_DOG` | `0x8000` |
| `oot.en_torch2` | `ACTOR_EN_TORCH2` (Dark Link em OoT) | `OBJECT_TORCH2` | `0` |
| `compat.mm.elegy_shell.human` | ator dinâmico `En_MmElegyShellHuman` | `OBJECT_GAMEPLAY_KEEP` | `0` |

O provider registra as capabilities experimentais `actor.spawn`, `actor.destroy` e `actor.exists` com versão `0.1.0` e é publicado no `LuaApiHostContext`. Os bindings `ship.actor.spawn`, `ship.actor.destroy` e `ship.actor.exists` vêm do ShipLua 0.4 (`MODSDK-005`, PR `link-span#41`). O manifesto precisa declarar as capabilities, os grants `world.entities.create`, `world.entities.destroy` e `world.entities.read`, além de um limite `actors` maior que zero para spawn.

Posições chegam como números genéricos e são convertidas para `float` somente na chamada nativa. Rotações são expressas em graus pela API portátil e convertidas para o formato binário de ângulo do OoT no limite do host.

## Compatibilidade com a estátua da Elegia de MM

A estátua não é `ACTOR_EN_VM`. Nos decomps, `En_Vm` é o Beamos. A casca criada pela Elegia do Vazio em Majora's Mask é `ACTOR_EN_TORCH2`, que seleciona os display lists humano, Goron, Zora e Deku por parâmetro. Em Ocarina of Time, porém, `ACTOR_EN_TORCH2` já pertence ao Dark Link. Reutilizar diretamente esse ID substituiria o significado do ator no host OoT.

A ponte registra, portanto, um ator OoT dinâmico próprio, `En_MmElegyShellHuman`, e expõe apenas o ID lógico `compat.mm.elegy_shell.human`. O `Draw` desse ator resolve o display list original de MM no resource manager compartilhado:

```text
__OTR__objects/gameplay_keep/gElegyShellHumanDL
```

O spawn só prossegue quando esse recurso está montado e pode ser carregado. Isso demonstra compatibilidade de formato e renderização entre os ports sem misturar IDs nativos dos dois jogos.

Nenhum ativo proprietário de Majora's Mask é armazenado neste repositório. O usuário precisa gerar, a partir de uma cópia legítima do jogo, um bundle mínimo `.o2r`/`.otr` que contenha `gElegyShellHumanDL` e todas as texturas, vértices e display lists referenciados por ele. É preferível montar apenas esse subconjunto, em vez do `gameplay_keep` completo de MM, para evitar colisões de namespace com OoT.

Exemplo de spawn:

```lua
local ship = require("ship")

local statue, err = ship.actor.spawn("compat.mm.elegy_shell.human", {
    position = { x = 0, y = 0, z = 0 },
    rotation = { x = 0, y = 180, z = 0 },
})

if not statue then
    ship.log.warn("Elegy shell spawn failed [" .. err.code .. "]: " .. err.message)
else
    ship.log.info("Majora's Mask Elegy shell rendered by the OoT actor engine")
end
```

As coordenadas são coordenadas mundiais da cena atual. O exemplo completo em `examples/mm-elegy-shell/` usa a tecla **K** e mantém um único handle ativo.

## Lifecycle

`ShipLuaBootstrap` registra hooks de `OnActorDestroy` e `OnPlayDestroy`. O primeiro invalida imediatamente o handle de um ator removido pelo próprio jogo. O segundo mata os atores ainda pertencentes a mods e avança a geração de cena antes da desmontagem do `PlayState`.

Para hot reload futuro, o host deve chamar `OotActorProvider::ReleaseMod(modId)` antes de destruir o `LuaRuntime` daquele mod.

## Validação

O teste `oot_actor_provider_tests` cobre:

- descritores das capabilities;
- allowlist de `En_Dog`, `En_Torch2` e definição compatível;
- bloqueio de `ACTOR_PLAYER`;
- dependência de object;
- preflight de recursos externos antes de criar handle ou ator;
- ownership entre mods;
- handles inválidos e gerações obsoletas;
- limite por mod;
- cleanup por ator, mod e cena;
- rejeição fora da thread principal.

A validação integrada anterior gerou `x64/Release/soh.exe` em Release e executou 57/57 testes aplicáveis do host e do SDK embutido. Este recorte de compatibilidade ainda exige rebuild e smoke test com um bundle de ativos legítimos de MM.
