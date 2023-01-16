#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <unordered_map>

namespace ocular {
	namespace SwapChain {
		typedef HRESULT(__stdcall* QUERYINTERFACE) (IDXGISwapChain* pSwapChain, REFIID riid, void** ppvObject);
		typedef ULONG(__stdcall* ADDREF) (IDXGISwapChain* pSwapChain);
		typedef ULONG(__stdcall* RELEASE) (IDXGISwapChain* pSwapChain);
		typedef HRESULT(__stdcall* SETPRIVATEDATA) (IDXGISwapChain* pSwapChain, REFGUID Name, UINT DataSize, const void* pData);
		typedef HRESULT(__stdcall* SETPRIVATEDATAINTERFACE) (IDXGISwapChain* pSwapChain, REFGUID Name, const IUnknown* pUnknown);
		typedef HRESULT(__stdcall* GETPRIVATEDATA) (IDXGISwapChain* pSwapChain, REFGUID Name, UINT DataSize, void* pData);
		typedef HRESULT(__stdcall* GETPARENT) (IDXGISwapChain* pSwapChain, REFIID riid, void** ppParent);
		typedef HRESULT(__stdcall* GETDEVICE) (IDXGISwapChain* pSwapChain, REFIID riid, void** ppDevice);
		typedef HRESULT(__stdcall* PRESENT) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

		typedef HRESULT(__stdcall* RESIZEBUFFERS) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	}

	namespace Context {
		typedef void(__stdcall* OMSETRENDERTARGETS) (ID3D11DeviceContext* pContext, UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView);
		typedef void(__stdcall* CLEARDEPTHSTENCILVIEW) (ID3D11DeviceContext* pContext, ID3D11DepthStencilView* pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil);
	}

	enum class VMT {
		// SwapChain
		QueryInterface = 0,
		AddRef = 1,
		Release = 2,
		SetPrivateData = 3,
		SetPrivateDataInterface = 4,
		GetPrivateData = 5,
		GetParent = 6,
		GetDevice = 7,
		Present = 8,

		ResizeBuffers = 13,

		// Device

		// Context
		OMSetRenderTargets = 0x200 + 33,
		ClearDepthStencilView = 0x200 + 53
	};

	extern std::unordered_map<VMT, void*> oMethods;

	void Init();
	bool CreateHook(void* function, VMT method);
}