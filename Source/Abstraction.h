#pragma once
#include <d3dx9.h> // Includes d3d9.h and windows.h aswell

void Cleanup();
HRESULT InitD3D(HWND hwnd);
void Render();