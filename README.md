<h1 align="center">:telescope: ocular :stars:</h1>
<h3 align="center">A Simple Graphical Hook + ImGui Implementation &amp; DLL Proxy For Games.</h2>

## Supports

| Graphical API                       | | DLL Proxy     |
| ----------------------------------- |-| ------------- |
| :heavy_multiplication_x: DirectX 9  | | dinput8.dll   |
| :heavy_multiplication_x: DirectX 10 | | xinput1_4.dll <sub><sup>(Untested)</sup></sub> |
| :heavy_check_mark: DirectX 11       | | dxgi.dll  |
| :heavy_multiplication_x: DirectX 12 | |   |
| :heavy_multiplication_x: OpenGL     | |   |
| :heavy_multiplication_x: Vulkan     | |   |

## Dependencies
* [kiero](https://github.com/Rebzzel/kiero/) :heavy_check_mark:
  * [Windows SDK](https://www.microsoft.com/en-us/download/details.aspx?id=8279) :heavy_exclamation_mark:
  * [DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812) :heavy_exclamation_mark:
  * [MinHook](https://github.com/TsudaKageyu/minhook) :heavy_check_mark:
* [ImGui](https://github.com/ocornut/imgui/) :heavy_check_mark:

<sub><sup>:heavy_check_mark: - Included Submodule &emsp; :heavy_exclamation_mark: - Requires Installation</sup></sub>

## Cloning
```
git clone --recursive --recurse-submodules https://github.com/WoefulWolf/ocular/
```

## Installation
1. [Clone](https://github.com/WoefulWolf/ocular/#cloning) the repository.
2. Install necessary [dependencies](https://github.com/WoefulWolf/ocular/#dependencies).
3. Move the following files up one directory from `ocular/imgui/backends/` to `ocular/imgui/`:
```
imgui_impl_dx11.cpp
imgui_impl_dx11.h
imgui_impl_win32.cpp
imgui_impl_win32.h
```
4. Set the following macro definitions in `ocular/kiero/kiero.h` to `1`:
```c++
#define KIERO_INCLUDE_D3D11  1
#define KIERO_USE_MINHOOK    1
```
5. `#include` `ocular/ocular.h` and/or `ocular/DLLProxy.h` in your project!

## Important Notes
This is still very early in development. A lot of it is a bit spaghetti.
It is actually taken from another WIP project, but I thought this might be handy to have here to use in other projects.
I have tested very few games thus far, and I am certain some will not work.
Some games that I have tested that work include;
 * NieR:Automata
 * NieR Replicant ver.1.22474487139
 * Dark Souls III
 * Factorio
 * Final Fantasy XIV - A Realm Reborn
 * Kenshi

## Usage Example
```c++
// Include ocular
#include "ocular/ocular.h"
#include "ocular/DLLProxy.h"

// Just so we can see some logs and output.
void ConsoleSetup() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdout, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdout, "CONOUT$", "r", stdin);
}

// Our custom GUI function, gets run every frame (You can include ImGui functions).
void OurCustomRenderFunction() {
    ImGui::Begin("ocular");
    ImGui::Text("Hello World!");
    ImGui::End();
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
    OcularDLLProxy::Init();   // Let's use the proxy (do this first or very early).
    ConsoleSetup();           // Create our console.

    OcularHook oHook = Ocular::Init(); // Initialize ocular.
    oHook.BindRenderFunction(OurCustomRenderFunction);  // Add our own render function.

    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        Sleep(1000);  // Not necessarily required, but let's sleep to allow everything to be setup.
        break;
    case DLL_PROCESS_DETACH:
        Ocular::Shutdown();
        break;
    }
    return TRUE;
}

```
After building the DLL, rename the file to one of the supported [DLL proxies](https://github.com/WoefulWolf/ocular/#supports) and place it adjacent to your game's executable.

![Example of ImGui Window](https://i.imgur.com/NpRoK55.png)

## Visual Studio Tips For Newbies
 * Make sure you are building a Dynamic Library (.dll)!
 * Here is a list of the minimum files to *"Include In Project"*:
```
ocular/ocular.h
ocular/ocular.cpp
ocular/DLLProxy.h

ocular/hooks/                      // Entire direcory.
ocular/imgui/                      // All source files inside EXCLUDING sub-directories.

ocular/kiero/kiero.h
ocular/kiero/kiero.cpp
ocular/kiero/minhook/include/      // Entire direcory.
ocular/kiero/minhook/scr/          // Entire direcory.
```
 * A handy extension I like: https://github.com/reloZid/vc-fileutils
