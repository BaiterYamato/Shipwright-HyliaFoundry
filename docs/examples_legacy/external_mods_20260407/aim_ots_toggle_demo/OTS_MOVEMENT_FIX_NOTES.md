# AIM OTS - notas de depuração e correção de movimento

Data: 2026-03-10  
Plano relacionado: `PLN-20260310-0002`

## Objetivo final

Fazer o AIM OTS funcionar como **mira livre em terceira pessoa** com:

- animações e locomoção do conjunto de batalha/Z-lock
- corpo acompanhando a mira
- sem lock-on real sintético
- sem voltar ao state machine de first-person
- mapeamento correto de movimento:
  - `W = frente`
  - `A = esquerda`
  - `D = direita`
  - `S = trás`

## Sintomas encontrados durante a depuração

### 1) OTS visual, mas player ainda em first-person

Sintomas:

- botão `A` mostrava `Return`
- `WASD` ficava travado
- câmera arrastava lentamente

Root cause:

- o AIM OTS estava entrando no fluxo vanilla de first-person (`PLAYER_STATE1_FIRST_PERSON` + `unk_6AD == 2`) mesmo com câmera over-shoulder.

## 2) WASD sendo consumido como input de mira

Sintomas:

- movimento não saía
- câmera sofria bleed/arrasto ao usar `WASD`

Root cause:

- o caminho compartilhado de aim ainda usava o stick de movimento para pitch/yaw em OTS.

## 3) Reentrada contínua no estado de mira

Sintomas:

- OTS parecia rearmar a mira a cada frame
- movimento/câmera voltavam a travar

Root cause:

- `Player_ActionHandler_13(...)` ainda reentrava no fluxo de projectile aim enquanto o OTS third-person já estava ativo.

## 4) Mapeamento de direção embaralhado

Sintomas observados ao longo da depuração:

- `A` e `D` viravam quase frente
- `S` tentava backwalk, mas ainda transladava para frente
- girar a câmera para a esquerda reintroduzia travas

Root causes:

- tentativa de reconstruir movimento a partir de `sControlStickAngle`/yaw
- mistura entre yaw da câmera, yaw da mira e convenção interna do stick
- yaw de movimento sendo sobrescrito pelo yaw de facing da mira

## Arquivos principais envolvidos

### Mod OTS

- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\docs\examples\external_mods\aim_ots_toggle_demo\mod.json`
- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\docs\examples\external_mods\aim_ots_toggle_demo\camera\camera_profiles.json`
- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\docs\examples\external_mods\aim_ots_toggle_demo\behaviors\behaviors.json`

### Gameplay / player

- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\soh\src\overlays\actors\ovl_player_actor\z_player.c`

### Bridge de external mods

- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\soh\soh\Enhancements\external-mods\ExternalModInterop.h`
- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\soh\soh\Enhancements\external-mods\ExternalModManager.h`
- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\soh\soh\Enhancements\external-mods\ExternalModManager.cpp`

## Estratégia final adotada

### 1) OTS passa a ser third-person de verdade

O modo OTS não deve ser tratado como “first-person com câmera deslocada”.  
Ele deve ser tratado como **aim de projétil em terceira pessoa**.

### 2) Reuso do conjunto de batalha/Z-lock

Enquanto o contexto OTS de projétil estiver ativo, o player reutiliza:

- battle ready
- strafe lateral
- backwalk
- transições de idle/move do conjunto de batalha

Sem:

- criar `focusActor`
- criar lock-on hostil/friendly real
- criar target stickiness

### 3) Classificação do pure OTS

Para o caso de OTS puro (sem `focusActor`, sem hostile/friendly lock-on real), a classificação final ficou baseada em:

- `sControlStickWorldYaw`
- quantizado relativamente a `actor.focus.rot.y`

Classes:

- `forward`
- `backward`
- `strafe left`
- `strafe right`
- `idle`

### 4) Separação entre facing de mira e yaw de movimento

Esta foi a correção decisiva.

No OTS puro:

- `actor.shape.rot.y` continua acompanhando a mira
- `actor.focus.rot.y` continua sendo a fonte do aim
- **`this->yaw` não pode ser sobrescrito todo frame pelo facing da mira**

Se `this->yaw` for resetado para frente:

- `A/D` viram diagonal para frente
- `S` entra em animação de trás, mas ainda anda para frente

Por isso, o fix final preserva `this->yaw` como yaw de movimento no OTS puro e só o alinha novamente ao facing quando o player está parado.

## Helpers/áreas mais importantes no `z_player.c`

### Predicados/contexto

- `Player_ShouldUseAimOtsBattleMovement(...)`
- `Player_IsAimOtsThirdPersonAimActive(...)`
- `Player_IsAimOtsPureMovementActive(...)`
- `Player_IsBattleAimMovementActiveWithHostileUpdate(...)`
- `Player_IsFriendlyOrAimOtsBattleMovement(...)`

### Classificação e yaw alvo

- `Player_GetAimOtsPureMovementClass(...)`
- `Player_GetAimOtsBattleMovementSpeedAndYaw(...)`
- `Player_ClassifyAimOtsThirdPersonMovement(...)`

### Facing / sincronização de mira

- `Player_UpdateAimOtsBattleFacing(...)`

### Estados que precisaram ajuste

- `Player_ActionHandler_13(...)`
- `Player_Action_808407CC(...)`
- `Player_Action_80840DE4(...)`
- `Player_Action_808414F8(...)`
- `Player_Action_8084193C(...)`
- `Player_Action_808423EC(...)`
- `func_8084ABD8(...)`
- `Player_UpdateCamAndSeqModes(...)`
- `Player_Draw(...)`

## Correções finais importantes

### Entrada/saída de state machine

- OTS não deve permanecer em `PLAYER_STATE1_FIRST_PERSON`
- ao sair do OTS, o first-person real pode ser restaurado normalmente

### Input

- `WASD`/left stick não devem ser consumidos como aim input no OTS
- aim no OTS fica com mouse/right stick/gyro

### Backward real

Para `S`, o yaw alvo precisa ser o oposto do yaw da mira:

- `parallelYaw + 0x8000`

### Strafe puro

Para `A/D`, o OTS puro faz snap do `this->yaw` para o `yawTarget` lateral correto nas transições e updates de strafe, evitando o viés para frente.

## Resultado esperado

Com OTS ativo em bow/slingshot/hookshot/boomerang:

- `W = ↑`
- `A = ←`
- `D = →`
- `S = ↓`

E:

- câmera continua livre
- corpo acompanha a mira
- `A = Return` não aparece no OTS
- first-person real continua funcionando separado

## Validação feita

Build validado com sucesso:

- `D:\Desenvolvimento\ship of harkinian zelda\Shipwright\x64\Release\soh.exe`

Comando usado:

- `docs/agents/skills/soh-build-test-windows/scripts/build-release.ps1 -RepoRoot D:\Desenvolvimento\ship of harkinian zelda\Shipwright -Config Release -Target soh`

## Observação de manutenção futura

Se o movimento do OTS voltar a quebrar, revisar primeiro:

1. se o player voltou a cair em `PLAYER_STATE1_FIRST_PERSON`
2. se `func_8084ABD8(...)` voltou a consumir o stick de movimento como aim
3. se `Player_ActionHandler_13(...)` voltou a reentrar no projectile aim
4. se `Player_UpdateAimOtsBattleFacing(...)` voltou a sobrescrever `this->yaw` no OTS puro
5. se algum estado de strafe/backwalk voltou a interpolar yaw de movimento a partir do facing da mira
