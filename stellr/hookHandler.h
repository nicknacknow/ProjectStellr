#pragma once
#include "globals.h"
#include "hookVTable.h"
#include "MinHook.h"

#include <vector>

#include <D3D11.h>
#pragma comment(lib, "d3d11.lib")

HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
void WINAPI hkDrawIndexed(ID3D11DeviceContext* pContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation);

namespace Hooks {
	class hookHandler : public Singleton::Singleton<hookHandler> {
	public:
		hookHandler() { initialise(); }

		ID3D11Device* device = nullptr;
		IDXGISwapChain* swapChain = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;

		bool initialise() {
			MH_Initialize();

			HWND target = FindWindow(0, "Roblox");
			if (!target) return false;

			D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_0;
			DXGI_SWAP_CHAIN_DESC swap_chain_desc;
			{ ZeroMemory(&swap_chain_desc, sizeof swap_chain_desc);

			swap_chain_desc.BufferCount = 1;
			swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

			swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swap_chain_desc.OutputWindow = target;
			swap_chain_desc.SampleDesc.Count = 1;

			swap_chain_desc.Windowed = ((GetWindowLongPtr(target, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
			swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			swap_chain_desc.BufferDesc.Width = 1;
			swap_chain_desc.BufferDesc.Height = 1;
			swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
			swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1; }

			if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &level, 1, D3D11_SDK_VERSION, &swap_chain_desc, &swapChain, &device, NULL, &deviceContext)))
				return false;

			HookVTable* hook_vtable = HookVTable::GetSingleton();
			
			Hooks::HookVTable::hook_data deviceContextHook = hook_vtable->hook((uintptr_t)deviceContext, 20);
			Hooks::HookVTable::hook_data swapChainHook = hook_vtable->hook((uintptr_t)swapChain, 18);
			Hooks::HookVTable::hook_data deviceHook = hook_vtable->hook((uintptr_t)device, 43);

			swapChainHook.add_hook((uintptr_t)hkPresent, 8, (uintptr_t*)&origPresent);
			
			MH_CreateHook((LPVOID)deviceContextHook.get_original_func(12), hkDrawIndexed, reinterpret_cast<void**>(&origDrawIndexed));
			MH_EnableHook((LPVOID)deviceContextHook.get_original_func(12)); // must inline hook since DrawIndexed isn't called directly from the vt
		}
	};
}

HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags) {
	Exploit::Aimbot::Aimbot* aimbot = Exploit::Aimbot::Aimbot::GetSingleton();
	aimbot->OnStep();

	return Hooks::origPresent(pSwapChain, syncInterval, flags);
}

void WINAPI hkDrawIndexed(ID3D11DeviceContext* pContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) {
	Exploit::Visual::Chams* chams = Exploit::Visual::Chams::GetSingleton();
	chams->OnStep(pContext, indexCount, startIndexLocation, baseVertexLocation);

	return Hooks::origDrawIndexed(pContext, indexCount, startIndexLocation, baseVertexLocation);
}