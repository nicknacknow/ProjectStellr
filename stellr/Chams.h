#pragma once
#include "globals.h"
#include "Cheat.h"

#include <D3D11.h>
#include <D3Dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3dcompiler.lib")

HRESULT GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b)
{
	char szCast[] = "struct VS_OUT"
		"{"
		" float4 Position : SV_Position;"
		" float4 Color : COLOR0;"
		"};"

		"float4 main( VS_OUT input ) : SV_Target"
		"{"
		" float4 fake;"
		" fake.a = 1.0f;"
		" fake.r = %f;"
		" fake.g = %f;"
		" fake.b = %f;"
		" return fake;"
		"}";

	ID3D10Blob* pBlob;
	char szPixelShader[1000];

	sprintf_s(szPixelShader, szCast, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

namespace Exploit {
	namespace Visual {
		class Chams : public Cheat::Cheat, public Singleton::Singleton<Chams> {
		private:
			ID3D11DepthStencilState* depthStencilState_OPP = NULL; //depth off
			ID3D11DepthStencilState* depthStencilState_ORIG = NULL; //depth on

			ID3D11PixelShader* pShader = NULL;
		public:
			void OnStep(ID3D11DeviceContext* pContext, UINT indexCount, UINT startIndexLocation, INT baseVertexLocation) {
				if (!this->Enabled) return;
				// https://www.unknowncheats.me/forum/direct3d/349071-d3d11-shader-chams-transparent-walls-tons-info-post.html
				ID3D11Device* device = nullptr;
				pContext->GetDevice(&device);

				if (!pShader)
					GenerateShader(device, &pShader, (209.f / 255.f), (91.f / 255.f), (119.f / 255.f));

				UINT stride = getStride(pContext);
				if (stride == 44 || (AdditionalStride && stride == 52)) {
					if (!this->WallCheck) {
						pContext->OMGetDepthStencilState(&depthStencilState_ORIG, 0);
						pContext->OMSetDepthStencilState(depthStencilState_OPP, 0);

						Hooks::origDrawIndexed(pContext, indexCount, startIndexLocation, baseVertexLocation);

						pContext->OMSetDepthStencilState(depthStencilState_ORIG, 0);
						depthStencilState_ORIG->Release(); depthStencilState_ORIG = nullptr;
					}

					pContext->PSSetShader(pShader, NULL, NULL);
					
					if (this->WireFrame) {
						ID3D11RasterizerState* rState;
						D3D11_RASTERIZER_DESC rDesc;

						pContext->RSGetState(&rState);
						rState->GetDesc(&rDesc);

						rDesc.FillMode = D3D11_FILL_WIREFRAME;

						device->CreateRasterizerState(&rDesc, &rState);

						pContext->RSSetState(rState);
					}
				}
			}
		private:
			UINT getStride(ID3D11DeviceContext* pContext) {
				ID3D11Buffer* veBuffer;
				UINT stride = 0;
				UINT veBufferOffset = 0;
				pContext->IAGetVertexBuffers(0, 1, &veBuffer, &stride, &veBufferOffset);
				return stride;
			}

			// properties
			bool AdditionalStride = false;
			bool WireFrame = true;
			bool WallCheck = false;
		};
	}
}