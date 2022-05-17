#include "Abstraction.h"
#include "Rendering.h"

IDirect3D9* g_pD3D = nullptr;
IDirect3DDevice9* g_pd3dDevice = nullptr;

D3DXMATRIX g_worldMatrix;
D3DXMATRIX g_stopMatrix;

void AdjustStates(HWND hwnd)
{
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Disables culling, so faces can be seen from both sides
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // Disables lighting, so we can see things without a light source
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); // Enables Z-Buffer

	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // Minification filter
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR); // Minification filter
	// g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // Magnification filter
	// g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); // Magnification filter
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); // Mipmapping filter
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); // Mipmapping filter

	D3DXMATRIX projectionMatrix;

	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);

	D3DXMatrixIdentity(&g_worldMatrix);
	D3DXMatrixIdentity(&g_stopMatrix);
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, D3DX_PI / 8.0f, (FLOAT)windowRect.right / (FLOAT)windowRect.bottom, 1.0f, 100.0f);

	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &projectionMatrix);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
}

void Cleanup() // Frees memory from D3D objects
{
	if (g_pD3D != nullptr)
	{
		g_pD3D->Release();
	}
	if (g_pd3dDevice != nullptr)
	{
		g_pd3dDevice->Release();
	}

	Rendering::Cleanup();
}

HRESULT InitD3D(HWND hwnd)
{
	if (nullptr == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp = {};

	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pd3dDevice
	)))
	{
		return E_FAIL;
	}

	AdjustStates(hwnd);

	Rendering::PrepareRendering();

	return S_OK;
}

void Render()
{
	g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(16, 60, 120), 1.0f, NULL);

	// Camera look at
	{
		static D3DXMATRIX lookAtMatrix;
		
		static D3DXVECTOR3 eye = D3DXVECTOR3(1.0f, 1.5f, 0.0f);
		static D3DXVECTOR3 at = D3DXVECTOR3(-0.5f, 0.0f, 3.5f);
		static D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		D3DXMatrixLookAtLH(&lookAtMatrix, &eye, &at, &up);

		g_pd3dDevice->SetTransform(D3DTS_VIEW, &lookAtMatrix);
	}

	// "stop" == stop sign

	static float mapXOffset = -2.0f;
	static float stopRotation = 0.0f;
	static float stopScale = 1.0f;
	static bool mapRightOffset = false;
	static bool stopGrow = false;

	mapXOffset += mapRightOffset ? -0.0015f : 0.0015f;
	stopRotation += 0.02f;

	if (stopScale < 0.5f)
	{
		stopGrow = true;
	}
	else if (stopScale > 2.0f)
	{
		stopGrow = false;
	}

	if (mapXOffset < -1.5f)
	{
		mapRightOffset = false;
	}
	else if (mapXOffset > 1.5f)
	{
		mapRightOffset = true;
	}

	stopScale += stopGrow ? 0.005f : -0.005f;

	D3DXMATRIX translationMatrix;
	D3DXMATRIX rotationMatrix;
	D3DXMATRIX scaleMatrix;

	D3DXMatrixIdentity(&g_stopMatrix);

	D3DXMatrixScaling(&scaleMatrix, stopScale, stopScale, stopScale);
	D3DXMatrixMultiply(&g_stopMatrix, &g_stopMatrix, &scaleMatrix);
	D3DXMatrixRotationY(&rotationMatrix, stopRotation);
	D3DXMatrixMultiply(&g_stopMatrix, &g_stopMatrix, &rotationMatrix);
	D3DXMatrixTranslation(&translationMatrix, -0.3f + mapXOffset, 0.1f, 4.2f);
	D3DXMatrixMultiply(&g_stopMatrix, &g_stopMatrix, &translationMatrix);

	D3DXMatrixTranslation(&g_worldMatrix, mapXOffset, 0.0f, 0.0f);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		Rendering::RenderBoxRoom();
		Rendering::RenderAlphaBlending();

		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(nullptr, nullptr, NULL, nullptr);
}