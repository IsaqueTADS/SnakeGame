# CHIP-8 / Snake Game

Este repositório começou como um experimento de CHIP-8, mas a branch atual
`snake-v3` contém um jogo Snake completo, com menu, dificuldades, vidas,
efeitos sonoros e música procedural.

O nome do executável continua sendo `chip8` porque esse nome foi definido no
`build.zig` no início do projeto. Isso não muda o fato de que o programa da
branch atual é o Snake Game.

## Estado da branch

Esta documentação pertence à branch `snake-v3`.

As etapas principais do histórico são:

```text
main -> snake-game -> snake-v2 -> snake-v3
```

Para usar exatamente a versão documentada:

```bash
git fetch origin
git switch snake-v3
```

Se a branch ainda não existir localmente, use:

```bash
git fetch origin
git switch --track origin/snake-v3
```

## Plataforma principal

A configuração atual do `build.zig` é **Linux-first**. A branch foi preparada
para compilar um executável Linux desktop usando:

- raylib compilado diretamente do código incluído em `vendor/raylib`;
- GLFW incluído no código vendorizado do raylib;
- X11 para a janela e entrada no Linux;
- OpenGL para renderização;
- miniaudio para áudio;
- libc, `libm`, pthreads, `libdl` e `librt`.

Não existe um binário Linux versionado no repositório. Dizer que a branch está
"buildada para Linux" significa que o `build.zig` possui um caminho de build
para Linux. O executável deve ser gerado no computador onde o projeto será
executado.

No Linux, o comando padrão é:

```bash
zig build
```

O resultado fica em:

```text
zig-out/bin/chip8
```

Também é possível compilar e executar diretamente com:

```bash
zig build run
```

## Versão do Zig

O projeto não possui `.zig-version`, `build.zig.zon` ou outro arquivo que
registre a versão histórica do Zig usada no CachyOS. Portanto, não é possível
provar pelo Git qual era a versão exata instalada naquela época.

### Recomendação

Use **Zig 0.15.2** para reproduzir o ambiente lembrado durante o
desenvolvimento.

Essa versão é uma boa escolha porque o `build.zig` usa a API moderna do
sistema de build do Zig, incluindo:

- `b.createModule(...)`;
- `root_module` em `b.addExecutable(...)`;
- `b.path(...)`;
- `addCSourceFiles(...)`;
- `linkSystemLibrary(...)`;
- `b.installArtifact(...)`;
- `b.addRunArtifact(...)`.

Essas estruturas existem na linha 0.14 e continuam disponíveis na linha
0.15. No Zig 0.15.2, alguns métodos usados pelo projeto, como
`exe.addCSourceFiles(...)`, `exe.linkLibC()` e `exe.linkSystemLibrary(...)`,
já são considerados APIs legadas, mas ainda funcionam. Isso permite usar o
projeto sem reescrever o `build.zig` imediatamente.

O Ubuntu 26.04 também fornece o pacote `zig0.14` na versão 0.14.1. Essa
versão é uma alternativa compatível caso seja preferível usar o gerenciador de
pacotes da distribuição.

| Versão | Situação | Explicação |
|---|---|---|
| `0.15.2` | Recomendada | É a versão lembrada do ambiente original e mantém a API usada pelo projeto. |
| `0.14.1` | Compatível | É fornecida pelo Ubuntu 26.04 e usa a mesma família de APIs do `build.zig`. |
| `0.16.x` ou mais nova | Não documentada | Pode remover ou alterar os métodos legados usados pelo projeto. Use somente depois de atualizar o `build.zig`. |
| `0.13.x` ou mais antiga | Não recomendada | Pode não reconhecer a combinação atual de `root_module`, `b.path` e `createModule`. |

Sempre verifique a versão antes de compilar:

```bash
zig version
```

Se o resultado for diferente de `0.15.2` ou `0.14.1`, confirme a
compatibilidade antes de continuar. A versão do Zig influencia principalmente
o interpretador do `build.zig`; ela não altera o código C do jogo.

## Links oficiais do Zig

- [Página geral de downloads](https://ziglang.org/download/)
- [Download do Zig 0.15.2](https://ziglang.org/download/0.15.2/)
- [Documentação do Zig 0.15.2](https://ziglang.org/documentation/0.15.2/)
- [Repositório oficial do Zig](https://github.com/ziglang/zig)
- [Release oficial do Zig 0.15.2](https://github.com/ziglang/zig/releases/tag/0.15.2)

Para reproduzir o ambiente em máquinas diferentes, prefira baixar o mesmo
arquivo oficial `0.15.2` em vez de instalar automaticamente a versão mais
recente disponível na distribuição.

## Dependências do Linux

O raylib está dentro do repositório, portanto não é necessário instalar uma
versão externa do raylib. Ainda assim, o compilador precisa encontrar os
headers de X11 e OpenGL fornecidos pelo sistema.

As dependências principais são:

- Git;
- Zig 0.15.2 ou 0.14.1;
- headers de X11;
- headers de OpenGL;
- headers de áudio ALSA quando a distribuição exigir durante a compilação;
- utilitários `curl` e `xz` caso o Zig seja instalado pelo arquivo oficial.

### Ubuntu, Debian e Linux Mint

Links para os pacotes:

- [Pacote Zig no Ubuntu](https://packages.ubuntu.com/search?keywords=zig)
- [Pacote Zig no Debian](https://packages.debian.org/search?keywords=zig)

Instale as ferramentas e bibliotecas do sistema:

```bash
sudo apt update
sudo apt install git curl xz-utils \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
  libxi-dev libxext-dev libgl-dev libasound2-dev
```

O Ubuntu pode disponibilizar o Zig 0.14.1 por meio do pacote padrão:

```bash
sudo apt install zig
zig version
```

Se a saída for `0.14.1`, essa instalação pode ser usada com o projeto.

Para usar especificamente o Zig 0.15.2, não misture duas instalações no
`PATH`. Baixe o arquivo Linux na [página oficial do Zig 0.15.2](https://ziglang.org/download/0.15.2/),
extraia-o e adicione o diretório extraído ao `PATH`:

```bash
mkdir -p "$HOME/.local/opt/zig"
cd "$HOME/.local/opt/zig"
curl -LO https://ziglang.org/download/0.15.2/zig-x86_64-linux-0.15.2.tar.xz
tar -xf zig-x86_64-linux-0.15.2.tar.xz
export PATH="$HOME/.local/opt/zig/zig-x86_64-linux-0.15.2:$PATH"
zig version
```

Para tornar o caminho permanente no Bash, adicione esta linha uma vez ao
`~/.bashrc`:

```bash
export PATH="$HOME/.local/opt/zig/zig-x86_64-linux-0.15.2:$PATH"
```

Depois abra um novo terminal ou execute:

```bash
source ~/.bashrc
zig version
```

Em computadores ARM64, o nome do arquivo oficial muda para a arquitetura
correspondente, como `zig-aarch64-linux-0.15.2.tar.xz`.

### Fedora

Links:

- [Pacote Zig no Fedora](https://packages.fedoraproject.org/pkgs/zig/zig/)
- [Downloads oficiais do Zig](https://ziglang.org/download/0.15.2/)

Instalação pelo repositório do Fedora:

```bash
sudo dnf install git curl xz zig \
  libX11-devel libXrandr-devel libXinerama-devel \
  libXcursor-devel libXi-devel libXext-devel \
  mesa-libGL-devel alsa-lib-devel
```

Confira a versão:

```bash
zig version
```

Se o Fedora fornecer uma versão diferente da desejada, use o arquivo oficial
do Zig 0.15.2 e ajuste o `PATH` como explicado na seção do Ubuntu.

### openSUSE Tumbleweed e Leap

Links:

- [Pacote Zig no openSUSE](https://software.opensuse.org/package/zig)
- [Downloads oficiais do Zig](https://ziglang.org/download/0.15.2/)

Dependências gráficas e de áudio:

```bash
sudo zypper refresh
sudo zypper install git curl tar gzip xz \
  libX11-devel libXrandr-devel libXinerama-devel \
  libXcursor-devel libXi-devel libXext-devel \
  Mesa-libGL-devel alsa-devel
```

Se o pacote estiver disponível no repositório configurado:

```bash
sudo zypper install zig
zig version
```

O openSUSE pode entregar uma versão diferente entre Leap e Tumbleweed. Para
manter exatamente o Zig 0.15.2, use o download oficial em vez do pacote da
distribuição.

### Arch Linux, CachyOS e derivados

CachyOS usa a base e os pacotes do ecossistema Arch. Os nomes abaixo são os
nomes usuais dos pacotes Arch:

- [Pacote Zig no Arch Linux](https://archlinux.org/packages/extra/x86_64/zig/)
- [Downloads oficiais do Zig](https://ziglang.org/download/0.15.2/)

Instalação:

```bash
sudo pacman -Syu
sudo pacman -S --needed zig git libx11 libxrandr libxinerama \
  libxcursor libxi libxext mesa alsa-lib
```

Verifique:

```bash
zig version
```

Como Arch e CachyOS atualizam rapidamente, o pacote pode estar mais novo que
0.15.2. Se aparecer uma versão 0.16 ou posterior, baixe o Zig 0.15.2 na
página oficial e coloque o diretório dele antes dos demais no `PATH`.

### NixOS

Links:

- [Busca de pacotes do NixOS](https://search.nixos.org/packages?query=zig)
- [Downloads oficiais do Zig](https://ziglang.org/download/0.15.2/)

Uma instalação temporária pelo canal configurado pode ser feita com:

```bash
nix-shell -p zig git
zig version
```

Os headers de X11 e OpenGL também precisam estar disponíveis no ambiente Nix
usado para compilar o projeto. Em um `flake.nix`, declare o Zig e as
dependências gráficas explicitamente para obter builds reproduzíveis.

## WSL2 com Ubuntu

O WSL2 é uma forma prática de compilar o alvo Linux no Windows. O WSL não
transforma o programa em um executável Windows: o resultado será um binário
Linux ELF, executado dentro do ambiente Linux.

Links:

- [Instalação oficial do WSL](https://learn.microsoft.com/windows/wsl/install)
- [Projeto WSLg](https://github.com/microsoft/wslg)

No Ubuntu dentro do WSL:

```bash
sudo apt update
sudo apt install git curl xz-utils \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
  libxi-dev libxext-dev libgl-dev libasound2-dev
```

Depois instale o Zig 0.15.2 pelo arquivo oficial ou use o pacote `zig` do
Ubuntu, conforme a seção anterior.

Para confirmar se a parte gráfica do WSL está disponível:

```bash
echo "$DISPLAY"
echo "$WAYLAND_DISPLAY"
```

No Windows 11 atualizado, o WSLg normalmente fornece o suporte necessário
para abrir janelas Linux. Se as variáveis estiverem vazias ou o programa
mostrar erro de display, o build ainda pode funcionar, mas `zig build run` não
conseguirá abrir a janela. Nesse caso, atualize o WSL com:

```powershell
wsl --update
```

Também é possível compilar no WSL e executar o binário em outra máquina Linux
com uma sessão gráfica compatível, desde que as bibliotecas de execução
necessárias estejam presentes.

## Obtendo o código

Clone o projeto e selecione a branch documentada:

```bash
git clone https://github.com/IsaqueTADS/CHIP-8.git
cd CHIP-8
git fetch origin
git switch --track origin/snake-v3
```

Se o clone já tiver a branch local criada, use somente:

```bash
git switch snake-v3
```

Confirme o estado:

```bash
git status --short --branch
zig version
```

## Compilando no Linux

### Build de desenvolvimento

```bash
zig build
```

O modo padrão é `Debug`. O executável será criado em:

```text
zig-out/bin/chip8
```

### Compilar e executar

```bash
zig build run
```

Ou execute o binário já instalado pelo build:

```bash
./zig-out/bin/chip8
```

### Build otimizado

```bash
zig build -Doptimize=ReleaseFast
```

Outras opções comuns são:

```bash
zig build -Doptimize=ReleaseSafe
zig build -Doptimize=ReleaseSmall
```

### Escolher explicitamente o alvo Linux

Quando o Zig está sendo executado em Linux, o alvo nativo já é o padrão. Para
deixar o alvo explícito:

```bash
zig build -Dtarget=x86_64-linux-gnu
```

O alvo deve corresponder à arquitetura da máquina. Em ARM64, por exemplo,
use uma tripla Linux compatível com ARM64.

### Conferir o artefato

Em distribuições Linux, estes comandos ajudam a confirmar o resultado:

```bash
file zig-out/bin/chip8
ldd zig-out/bin/chip8
```

O `file` deve identificar um executável ELF Linux. O `ldd` mostra as
bibliotecas dinâmicas encontradas no sistema.

## Como o build funciona

O `build.zig` não compila um arquivo Zig de aplicação. Ele usa o Zig como
gerenciador de build e compilador C:

1. Cria um executável chamado `chip8`.
2. Adiciona os fontes C do jogo em `src`.
3. Adiciona os módulos C do raylib em `vendor/raylib/src`.
4. Adiciona os fontes do GLFW vendorizado para o sistema operacional escolhido.
5. Usa `-std=c99` nos fontes C.
6. Liga a libc e as bibliotecas gráficas e de sistema do alvo.
7. Instala o executável em `zig-out/bin`.
8. Expõe a etapa `run` para `zig build run`.

Como o raylib e o GLFW estão no repositório, não é necessário instalar
`raylib` globalmente nem baixar submódulos adicionais.

## Situação do Windows

O `build.zig` contém listas para Linux, Windows e macOS, mas a `snake-v3` deve
ser tratada como Linux-first neste momento.

A razão é objetiva: a v3 chama funções de áudio como `InitAudioDevice`,
`PlaySound` e `LoadMusicStreamFromMemory`, implementadas em
`vendor/raylib/src/raudio.c`. No `build.zig` atual, `raudio.c` aparece na lista
de fontes do Linux, mas não na lista de fontes do Windows.

Por isso, um build nativo da v3 no Windows pode compilar os arquivos, mas
falhar na etapa de link com símbolos de áudio não encontrados. Isso é uma
limitação da configuração do repositório, não um motivo para trocar a versão
do Zig.

Enquanto essa configuração não for corrigida, o caminho recomendado no
Windows é:

- usar WSL2 + WSLg para compilar e executar a versão Linux; ou
- usar uma branch sem áudio, como `main`, se a intenção for apenas validar o
  build nativo Windows.

Se o build Windows for necessário para a v3, a correção esperada é incluir
`vendor/raylib/src/raudio.c` também no conjunto de fontes Windows, sem compilar
o mesmo arquivo duas vezes no Linux. Essa alteração deve ser feita no código,
e não mascarada instalando outra versão do Zig.

## Instalação do Zig no Windows

Para instalar a versão recomendada:

1. Abra a [página oficial do Zig 0.15.2](https://ziglang.org/download/0.15.2/).
2. Baixe `zig-windows-x86_64-0.15.2.zip` para Windows 64 bits.
3. Extraia, por exemplo, em `C:\Tools\zig\0.15.2`.
4. Adicione essa pasta ao `Path` do usuário nas variáveis de ambiente do Windows.
5. Abra um novo PowerShell.

Valide:

```powershell
zig version
Get-Command zig
```

Para testar somente na sessão atual do PowerShell, sem alterar o `Path`
permanente:

```powershell
$env:Path = "C:\Tools\zig\0.15.2;$env:Path"
zig version
```

O Zig fornece o compilador C e as ferramentas necessárias para o build. As
bibliotecas `opengl32`, `gdi32` e `winmm` são bibliotecas do próprio Windows.
Ainda assim, a limitação de `raudio.c` descrita acima permanece na branch v3.

## Problemas comuns

### `zig: command not found`

O Zig não está instalado ou o diretório não está no `PATH`:

```bash
which zig
zig version
```

No PowerShell:

```powershell
Get-Command zig
zig version
```

Corrija o `PATH`, abra um novo terminal e tente novamente.

### `X11/Xlib.h: No such file or directory`

Faltam os headers X11. No Ubuntu ou Debian:

```bash
sudo apt install libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxi-dev libxext-dev
```

Use os nomes equivalentes da sua distribuição nas seções acima.

### `GL/gl.h: No such file or directory`

Faltam os headers OpenGL:

```bash
sudo apt install libgl-dev
```

No Fedora, normalmente o pacote correspondente é `mesa-libGL-devel`. No
openSUSE, normalmente é `Mesa-libGL-devel`. No Arch e no CachyOS, normalmente
é fornecido por `mesa`.

### `cannot find -lX11` ou `cannot find -lGL`

O linker não encontrou as bibliotecas de desenvolvimento. Instale os pacotes
X11 e OpenGL da sua distribuição e execute novamente:

```bash
zig build
```

### `Failed to open X display`

O build foi concluído, mas o ambiente gráfico não está disponível. Em WSL,
confirme que o WSLg está instalado e que o WSL está atualizado:

```powershell
wsl --update
```

Em um Linux nativo, confirme que o programa está sendo executado dentro de
uma sessão gráfica X11 ou compatível.

### Erros de `root_module`, `createModule` ou métodos do `Compile`

Confira a versão:

```bash
zig version
```

Use Zig `0.15.2` ou `0.14.1`. Versões antigas podem não reconhecer a API
moderna usada pelo arquivo, enquanto versões novas podem ter removido métodos
legados ainda usados no projeto.

### Símbolos de áudio não encontrados no Windows

Mensagens como `undefined symbol`, `unresolved external symbol` ou referências
a `InitAudioDevice`, `PlaySound` e `LoadMusicStreamFromMemory` indicam que
`raudio.c` não foi incluído na lista de fontes Windows. Isso deve ser corrigido
no `build.zig`; instalar Zig 0.14.1 ou 0.15.2 não resolve esse erro.

## Limpeza do build

Se o cache tiver sido gerado por outra versão do Zig, limpe os artefatos antes
de trocar de versão:

```bash
rm -rf .zig-cache zig-out
zig build
```

Essas pastas são ignoradas pelo Git e podem ser recriadas pelo Zig.

## Resumo rápido

Para Ubuntu, Debian, Mint ou WSL usando o Zig já empacotado:

```bash
sudo apt update
sudo apt install zig git \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev \
  libxi-dev libxext-dev libgl-dev libasound2-dev
git switch snake-v3
zig version
zig build run
```

Para reproduzir exatamente a versão recomendada, instale o Zig 0.15.2 pelo
[download oficial](https://ziglang.org/download/0.15.2/), valide com
`zig version` e então execute:

```bash
zig build
zig build run
```
