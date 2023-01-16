<h1 align="center">
:telescope: ocular :stars:
</h1>

<h3 align="center">
A Simple DX11 Hooking Library using <a href="https://github.com/microsoft/Detours">Detours</a>
</h3>

# Requirements
* Windows SDK

## Usage
* Add `ocular/include/` as an additional include directory.
* Add `ocular.lib` to your linker input.
* `#include <ocular.h>`

```c++
#include <Windows.h>
#include <ocular.h>
#include "tether.h" // https://github.com/WoefulWolf/tether

// Our function we are going to hook with.
HRESULT hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
  printf("Present called!\n");
  // We return the result of the original method.
  // Here we are casting the original method pointer to the appropriate type so that we can pass the parameters.
  return ((ocular::SwapChain::PRESENT)ocular::oMethods[ocular::VMT::Present])(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
  ocular::Init(); // Run Init before creating any hooks.
  ocular::CreateHook(hkPresent, ocular::VMT::Present);
  return TRUE;
}

// Boilerplate
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hModule);
      CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
      Sleep(1000);  // Let sleep to give our program some time to set up.
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
```

## Please Note
* This is not done, not even close to all the DX11 methods have been implemented, I'm adding them as I need them for projects. You are welcome to add others and make a pull request.
* See `ocular::VMT` for currently implemented methods.
* The main priority of the library is simplicity of use, which can sometimes be to the code's detriment.
