#include "Rendering.h"
#include <d3dx9.h>

extern IDirect3DDevice9* g_pd3dDevice;

extern D3DXMATRIX g_worldMatrix;
extern D3DXMATRIX g_stopMatrix;

static IDirect3DVertexBuffer9* s_pVB = nullptr;
static IDirect3DVertexBuffer9* s_pVB2 = nullptr;
static IDirect3DTexture9* s_pBricksTexture = nullptr;
static IDirect3DTexture9* s_pSFloorTexture = nullptr;
static IDirect3DTexture9* s_pWaterTexture = nullptr;
static IDirect3DTexture9* s_pShadowTexture = nullptr;
static IDirect3DTexture9* s_pStopTexture = nullptr;
static IDirect3DTexture9* s_pMossTexture = nullptr;

static float s_tScl = 3.0f; // Texture scale

static Tex1Vertex s_BoxRoom[]
{
	// Floor
	{-1.0f, -0.5f, 5.0f, 0.0f      , 0.0f      }, // 0
	{ 1.0f, -0.5f, 5.0f, s_tScl * 2, 0.0f      }, // 1
	{ 1.0f, -0.5f, 3.0f, s_tScl * 2, s_tScl * 2}, // 2
	{-1.0f, -0.5f, 3.0f, 0.0f      , s_tScl * 2}, // 3
	// Water layer
	{-1.0f, -0.4f, 5.0f, 0.0f      , 0.0f      }, // 4
	{-0.2f, -0.5f, 5.0f, s_tScl * 2, 0.0f      }, // 5
	{-0.2f, -0.5f, 3.2f, s_tScl * 2, s_tScl * 2}, // 6
	{-1.0f, -0.5f, 3.2f, 0.0f      , s_tScl * 2}, // 7
	// Shadow
	{-0.8f, -0.5f, 4.7f, 0.0f      , 0.0f      }, // 8
	{ 0.2f, -0.5f, 4.7f, 1.0f      , 0.0f      }, // 9
	{ 0.2f, -0.5f, 3.7f, 1.0f      , 1.0f      }, // 10
	{-0.8f, -0.5f, 3.7f, 0.0f      , 1.0f      }, // 11
	// Stop sign					       
	{-0.1f,  0.1f, 0.0f, 0.0f      , 0.0f      }, // 12
	{ 0.1f,  0.1f, 0.0f, 1.0f      , 0.0f      }, // 13
	{ 0.1f, -0.1f, 0.0f, 1.0f      , 1.0f      }, // 14
	{-0.1f, -0.1f, 0.0f, 0.0f      , 1.0f      }  // 15
};

static Tex2Vertex s_BrickWalls[]
{
	// Left wall
	{-1.0f,  0.5f, 3.0f, 0.0f      , 0.0f  , 0.0f      , 0.0f  }, // 0
	{-1.0f,  0.5f, 5.0f, s_tScl * 2, 0.0f  , s_tScl * 2, 0.0f  }, // 1
	{-1.0f, -0.5f, 5.0f, s_tScl * 2, s_tScl, s_tScl * 2, s_tScl}, // 2
	{-1.0f, -0.5f, 3.0f, 0.0f      , s_tScl, 0.0f      , s_tScl}, // 3
	// Front wall				       			       	 
	{-1.0f,  0.5f, 5.0f, 0.0f      , 0.0f  , 0.0f      , 0.0f  }, // 4
	{ 1.0f,  0.5f, 5.0f, s_tScl * 2, 0.0f  , s_tScl * 2, 0.0f  }, // 5
	{ 1.0f, -0.5f, 5.0f, s_tScl * 2, s_tScl, s_tScl * 2, s_tScl}, // 6
	{-1.0f, -0.5f, 5.0f, 0.0f      , s_tScl, 0.0f      , s_tScl}, // 7
	// Right wall				       				       
	{ 1.0f,  0.5f, 5.0f, 0.0f      , 0.0f  , 0.0f      , 0.0f  }, // 8
	{ 1.0f,  0.5f, 3.0f, s_tScl * 2, 0.0f  , s_tScl * 2, 0.0f  }, // 9
	{ 1.0f, -0.5f, 3.0f, s_tScl * 2, s_tScl, s_tScl * 2, s_tScl}, // 10
	{ 1.0f, -0.5f, 5.0f, 0.0f      , s_tScl, 0.0f      , s_tScl}, // 11
};

void Rendering::PrepareRendering()
{
	// Create vertex buffer for rendering the box room
	g_pd3dDevice->CreateVertexBuffer(sizeof(s_BoxRoom), D3DUSAGE_WRITEONLY, dwTex1FVF, D3DPOOL_DEFAULT, &s_pVB, nullptr);
	void* pVBOffset;
	s_pVB->Lock(0, 0, (void**)&pVBOffset, 0);
	memcpy(pVBOffset, s_BoxRoom, sizeof(s_BoxRoom));
	s_pVB->Unlock();

	// Create vertex buffer for rendering the walls
	g_pd3dDevice->CreateVertexBuffer(sizeof(s_BrickWalls), D3DUSAGE_WRITEONLY, dwTex2FVF, D3DPOOL_DEFAULT, &s_pVB2, nullptr);
	void* pVBOffset2;
	s_pVB2->Lock(0, 0, (void**)&pVBOffset2, 0);
	memcpy(pVBOffset2, s_BrickWalls, sizeof(s_BrickWalls));
	s_pVB2->Unlock();

	// Create texture for bricks and stone floor
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "Assets\\Textures\\bricks_color.png", &s_pBricksTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\bricks_color.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "Assets\\Textures\\stone_floor_color.png", &s_pSFloorTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\stone_floor_color.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "Assets\\Textures\\water_color.png", &s_pWaterTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\water_color.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "Assets\\Textures\\shadow.png", &s_pShadowTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\shadow.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
	if (FAILED(D3DXCreateTextureFromFileEx(g_pd3dDevice, "Assets\\Textures\\stop.png", 16, 16, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT, D3DCOLOR_XRGB(0, 255, 0), nullptr, nullptr, &s_pStopTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\stop.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "Assets\\Textures\\moss.png", &s_pMossTexture)))
	{
		MessageBox(NULL, "Texture \"Assets\\Textures\\moss.png\" not found!", "Error: Texture not loaded!", MB_OK);
	}
}

void Rendering::Cleanup()
{
	if (s_pVB != nullptr)
	{
		s_pVB->Release();
	}
	if (s_pVB2 != nullptr)
	{
		s_pVB2->Release();
	}
	if (s_pBricksTexture != nullptr)
	{
		s_pBricksTexture->Release();
	}
	if (s_pSFloorTexture != nullptr)
	{
		s_pSFloorTexture->Release();
	}
	if (s_pWaterTexture != nullptr)
	{
		s_pWaterTexture->Release();
	}
	if (s_pShadowTexture != nullptr)
	{
		s_pShadowTexture->Release();
	}
	if (s_pStopTexture != nullptr)
	{
		s_pStopTexture->Release();
	}
	if (s_pMossTexture != nullptr)
	{
		s_pMossTexture->Release();
	}
}

void Rendering::RenderBoxRoom()
{
	g_pd3dDevice->SetStreamSource(0, s_pVB2, 0, sizeof(Tex2Vertex));
	g_pd3dDevice->SetFVF(dwTex2FVF);
	g_pd3dDevice->SetTexture(0, s_pBricksTexture);
	g_pd3dDevice->SetTexture(1, s_pMossTexture);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADDSMOOTH);
	
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 4, 2);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 8, 2);

	g_pd3dDevice->SetStreamSource(0, s_pVB, 0, sizeof(Tex1Vertex));
	g_pd3dDevice->SetFVF(dwTex1FVF);
	g_pd3dDevice->SetTexture(1, nullptr);
	g_pd3dDevice->SetTexture(0, s_pSFloorTexture);
	g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
}

void Rendering::RenderAlphaBlending()
{
	// Alpha blending meshes
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// Shadow
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTexture(0, s_pShadowTexture);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 8, 2);
	// Water layer
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
	g_pd3dDevice->SetTexture(0, s_pWaterTexture);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 4, 2);
	// Stop sign
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_stopMatrix);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetTexture(0, s_pStopTexture);
	g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 12, 2);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &g_worldMatrix);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	// End
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}