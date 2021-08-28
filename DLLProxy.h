#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <iostream>
#include <string>

#include <dinput.h>
#include <xinput.h>
#include <dxgi.h>
#include <dxgi1_3.h>

namespace OcularDLLProxy {
	HMODULE dinput_lib;
	HMODULE xinput_lib;
	HMODULE dxgi_lib;

	enum Result {
		GetModuleHandleExFailure = -1,
		GetModuleFileNameFailure = -2,
		LoadLibraryWFailure = -3,
		DLLFilenameNotRecognized = -4,
		GetSystemDirectoryWFailure = -4,

		Success = 0,
	};

	Result LoadOriginalDLLs();

	inline void Init() {
		Result result = LoadOriginalDLLs();
		if (result != Result::Success) {
			MessageBoxA(GetActiveWindow(), ("LoadOriginalDLLS Error: " + std::to_string(result)).c_str(), "OcularDLLProxy failed!", MB_OK);
			//ExitProcess(0);
		}
	}

	inline Result LoadOriginalDLLs() {
		wchar_t path[MAX_PATH];
		wchar_t filename[FILENAME_MAX];

		wchar_t buffer[MAX_PATH]{ 0 };

		HMODULE hm = NULL;

		// Get a handle to ourself
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&LoadOriginalDLLs, &hm) == 0) {
			int ret = GetLastError();
			std::cout << "GetModuleHandleEx failed!" << ret << std::endl;
			return Result::GetModuleHandleExFailure;
		}

		// Get our filepath
		if (GetModuleFileName(hm, path, MAX_PATH) == 0) {
			int ret = GetLastError();
			std::cout << "GetModuleFileName failed!" << ret << std::endl;
			return Result::GetModuleFileNameFailure;
		}

		// Get our filename
		_wsplitpath(path, NULL, NULL, filename, NULL);

		// Get the system directory.
		if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
			// Load the approprate DLL according to our filename.
			if (wcscmp(filename, L"DINPUT8") == 0 || wcscmp(filename, L"dinput8") == 0) {
				if ((dinput_lib = LoadLibraryW((std::wstring{ buffer } + L"\\dinput8.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"XINPUT1_4") == 0 || wcscmp(filename, L"xinput1_4") == 0) {
				if ((xinput_lib = LoadLibraryW((std::wstring{ buffer } + L"\\xinput1_4.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else if (wcscmp(filename, L"DXGI") == 0 || wcscmp(filename, L"dxgi") == 0) {
				if ((dxgi_lib = LoadLibraryW((std::wstring{ buffer } + L"\\dxgi.dll").c_str())) == NULL) return Result::LoadLibraryWFailure;
			}
			else {
				return Result::DLLFilenameNotRecognized;
			}
		}
		else {
			return Result::GetSystemDirectoryWFailure;
		}

		return Result::Success;
	}
	
	extern "C" {
		// dinput8.dll
		inline __declspec(dllexport) HRESULT WINAPI custom_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter) {
#pragma comment(linker, "/EXPORT:DirectInput8Create=custom_DirectInput8Create")
			return ((decltype(DirectInput8Create)*)GetProcAddress(dinput_lib, "DirectInput8Create"))(hinst, dwVersion, riidltf, ppvOut, punkOuter);
		}

		// xinput1_4.dll
		inline __declspec(dllexport) DWORD WINAPI custom_XInputGetState(DWORD dwUserIndex, XINPUT_STATE *pState) {
#pragma comment(linker, "/EXPORT:XInputGetState=custom_XInputGetState")
			return ((decltype(XInputGetState)*)GetProcAddress(xinput_lib, "XInputGetState"))(dwUserIndex, pState);
		}

		// dxgi.dll
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory(REFIID riid, void** ppFactory) {
#pragma comment(linker, "/EXPORT:CreateDXGIFactory=custom_CreateDXGIFactory")
			return ((decltype(CreateDXGIFactory)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory"))(riid, ppFactory);
		}
		
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory1(REFIID riid, void** ppFactory) {
#pragma comment(linker, "/EXPORT:CreateDXGIFactory1=custom_CreateDXGIFactory1")
			return ((decltype(CreateDXGIFactory1)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory1"))(riid, ppFactory);
		}
		
		inline __declspec(dllexport) DWORD WINAPI custom_CreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory) {
#pragma comment(linker, "/EXPORT:CreateDXGIFactory2=custom_CreateDXGIFactory2")
			return ((decltype(CreateDXGIFactory2)*)GetProcAddress(dxgi_lib, "CreateDXGIFactory2"))(flags, riid, ppFactory);
		}
	}
}