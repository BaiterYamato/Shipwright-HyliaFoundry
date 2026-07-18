# Provider nativo de atores OoT

`OOT-MODSDK-001` introduz o primeiro recorte do provider `shipwright-native` para a futura API genérica de atores do ShipLua.

## Contrato deste recorte

- somente a thread principal do jogo pode criar, consultar ou destruir atores;
- o mod informa um ID lógico allowlisted, nunca um `ActorId` numérico;
- nenhum `Actor*` sai do provider;
- cada ator recebe um `ShipLua::Handle` com slot, geração, geração de cena e ownership;
- o limite inicial é de 16 atores vivos por mod e 256 no host;
- a dependência de object precisa estar carregada antes do spawn;
- handles são invalidados quando o ator nativo é destruído, o mod é descarregado ou o `PlayState` termina;
- `ACTOR_PLAYER` é rejeitado mesmo que seja incluído acidentalmente na allowlist.

IDs iniciais:

| ID lógico | Ator OoT | Object obrigatório | Params fixos |
|---|---|---|---|
| `en_dog` | `ACTOR_EN_DOG` | `OBJECT_DOG` | `0x8000` |
| `en_torch2` | `ACTOR_EN_TORCH2` | `OBJECT_TORCH2` | `0` |

O provider registra as capabilities experimentais `actor.spawn`, `actor.destroy` e `actor.exists` com versão `0.1.0`. Os bindings `ship.actor.*`, a leitura das permissões genéricas no manifesto e o SDK Lua de alto nível pertencem ao PR seguinte (`MODSDK-005`); até lá, este contrato é consumido apenas pelo host nativo e pelos testes de conformidade.

## Lifecycle

`ShipLuaBootstrap` registra hooks de `OnActorDestroy` e `OnPlayDestroy`. O primeiro invalida imediatamente o handle de um ator removido pelo próprio jogo. O segundo mata os atores ainda pertencentes a mods e avança a geração de cena antes da desmontagem do `PlayState`.

Para hot reload futuro, o host deve chamar `OotActorProvider::ReleaseMod(modId)` antes de destruir o `LuaRuntime` daquele mod.

## Validação

O teste `oot_actor_provider_tests` cobre:

- descritores das capabilities;
- allowlist de `En_Dog` e `En_Torch2`;
- bloqueio de `ACTOR_PLAYER`;
- dependência de object;
- ownership entre mods;
- handles inválidos e gerações obsoletas;
- limite por mod;
- cleanup por ator, mod e cena;
- rejeição fora da thread principal.
