#include "D3D11Hook.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND window;
static WNDPROC oWndProc;
static LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

Present oPresent;
ResizeBuffers oResizeBuffers;
bool init = false;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void SetupRenderTargets(IDXGISwapChain* pSwapchain) {
	std::cout << "[Ocular][D3D11 Hook] Setting up render targets." << std::endl;
	ID3D11Texture2D* pBackBuffer;
	pSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
	pBackBuffer->Release();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
}

// If ResizeBuffer is called, let's setup the render targets again, they might have broken.
HRESULT hkResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	std::cout << "[Ocular][D3D11 Hook] ResizeBuffers called." << std::endl;
	if (mainRenderTargetView) {
		pContext->OMSetRenderTargets(0, 0, 0);
		mainRenderTargetView->Release();
	}
	HRESULT result = oResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	std::cout << "[Ocular][D3D11 Hook] ResizeBuffers -> Resolution: " << Width << ", " << Height << std::endl;

	if (!pDevice) {
		std::cout << "[Ocular][D3D11 Hook] ResizeBuffers -> Device did not exist!" << std::endl;
	}
	else {
		SetupRenderTargets(pThis);
	}
	return result;
}

// We've hooked the Present function, let's setup the render targets and ImGui if this is our first time.
HRESULT __stdcall D3D11Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!init)
	{
		std::cout << "[Ocular][D3D11 Hook] Initializing rendering..." << std::endl;
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;

			SetupRenderTargets(pSwapChain);
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

			InitImGui();
			init = true;
			std::cout << "[Ocular][D3D11 Hook] Rendering initialized!" << std::endl;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	// Let's add our ImGui and return the original Present.
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();

	RenderFrame();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(pSwapChain, SyncInterval, Flags);
}


D3D11Hook::D3D11Hook() {
	if (kiero::bind(8, (void**)&oPresent, hkPresent) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D11 Present." << std::endl;
	if (kiero::bind(13, (void**)&oResizeBuffers, hkResizeBuffers) != kiero::Status::Success) return;
	std::cout << "[Ocular] Kiero bound D3D11 ResizeBuffers." << std::endl;
	hooked = true;
}