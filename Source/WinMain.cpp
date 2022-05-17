#include "Abstraction.h"

#define CLASS_NAME "WNDCLASS1"

LRESULT WINAPI MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow)
{
	WNDCLASS wc = {};
	wc.hInstance = hInstance;
	wc.lpfnWndProc = MsgProc;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);
	HWND hwnd = CreateWindow(CLASS_NAME, "DirectX Alpha Blending Test by Rubin", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1600, 900, NULL, NULL, hInstance, NULL);

	if (FAILED(InitD3D(hwnd)))
	{
		Cleanup();
		UnregisterClass(CLASS_NAME, hInstance);
		return -1;
	}

	ShowWindow(hwnd, nCmdShow);
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	Cleanup();
	UnregisterClass(CLASS_NAME, hInstance);
	return 0;
}