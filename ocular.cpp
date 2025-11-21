#include "ocular.h"
#include "detours/detours.h"
#pragma comment(lib, "d3d11.lib")

namespace ocular {
	constexpr int MAX_INIT_RETRIES = 100;

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

		IDXGISwapChain* swap = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;

		HRESULT res = S_FALSE;
		int retries = 0;
		while (res != S_OK && retries < MAX_INIT_RETRIES) {
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
			retries++;
		}

		if (res != S_OK) {
			// Failed to initialize after MAX_INIT_RETRIES attempts
			return;
		}

		swapChain_VTable = *reinterpret_cast<void***>(swap);
		device_VTable = *reinterpret_cast<void***>(device);
		context_VTable = *reinterpret_cast<void***>(context);

		// Clean up temporary D3D objects
		swap->Release();
		device->Release();
		context->Release();
	}

	void* ResolveFromMethod(VMT method) {
		int methodVal = static_cast<int>(method);

		// SwapChain
		if (methodVal < OCULAR_DEVICE_OFFSET)
			return swapChain_VTable[methodVal];

		// Device
		if (methodVal < OCULAR_CONTEXT_OFFSET)
			return device_VTable[methodVal - OCULAR_DEVICE_OFFSET];

		// Context
		return context_VTable[methodVal - OCULAR_CONTEXT_OFFSET];
	}

	bool CreateHook(void* function, VMT method) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		oMethods[method] = ResolveFromMethod(method);

		LONG attachRes = DetourAttach(&oMethods[method], function);

		LONG commitRes = DetourTransactionCommit();

		return (attachRes == NO_ERROR && commitRes == NO_ERROR);
	}
}
