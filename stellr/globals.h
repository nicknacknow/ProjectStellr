#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

#include "Console.h"
#include "Singleton.h"

#include <D3D11.h>
#pragma comment(lib, "d3d11.lib")

#include "Memory.h"

namespace Hooks {
	typedef HRESULT(WINAPI* tdPresent)(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags);
	tdPresent origPresent = nullptr;
	typedef void(WINAPI* tdDrawIndexed)(ID3D11DeviceContext* pContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation);
	tdDrawIndexed origDrawIndexed = nullptr;
}

class globals : public Singleton::Singleton<globals> {
public:

};

#include "Chams.h"

#include "hookVTable.h"
#include "Aimbot.h"
#include "hookHandler.h"
