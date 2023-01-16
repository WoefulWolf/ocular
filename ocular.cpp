#include "ocular.h"
#include "detours/detours.h"
#pragma comment(lib, "d3d11.lib")

namespace ocular {
	void** swapChain_VTable;
	void** device_VTable;
	void** context_VTable;

	std::unordered_map<VMT, void*> oMethods;

	void Init() {
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetForegroundWindow();
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		IDXGISwapChain* swap;
		ID3D11Device* device;
		ID3D11DeviceContext* context;

		HRESULT res = S_FALSE;
		while (res != S_OK) {
			res = D3D11CreateDeviceAndSwapChain(
				NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				0,
				NULL,
				0,
				D3D11_SDK_VERSION,
				&sd,
				&swap,
				&device,
				nullptr,
				&context
			);
		}

		swapChain_VTable = *reinterpret_cast<void***>(swap);
		device_VTable = *reinterpret_cast<void***>(device);
		context_VTable = *reinterpret_cast<void***>(context);
	}

	void* ResolveFromMethod(VMT method) {
		int methodVal = static_cast<int>(method);

		// SwapChain
		if (methodVal < 0x100)
			return swapChain_VTable[methodVal];

		// Device
		if (methodVal < 0x200)
			return device_VTable[methodVal - 0x100];

		// Context
		return context_VTable[methodVal - 0x200];
	}

	bool CreateHook(void* function, VMT method) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		oMethods[method] = ResolveFromMethod(method);

		LONG attachRes = DetourAttach(&oMethods[method], function);

		LONG commitRes = DetourTransactionCommit();

		if (attachRes == NO_ERROR && commitRes == NO_ERROR)
			return true;
		else
			return false;
	}
}

