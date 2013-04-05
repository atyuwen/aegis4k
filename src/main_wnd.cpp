#include "common.hpp"
#include "demo.hpp"
#include "mzk.hpp"

const int g_width  = 640;
const int g_height = 320;

D3DPRESENT_PARAMETERS g_dev_params = {
	g_width, g_height, D3DFMT_A8R8G8B8, 0, D3DMULTISAMPLE_NONE, 0, D3DSWAPEFFECT_DISCARD, NULL, true, true, D3DFMT_D24S8, 0, 0, D3DPRESENT_INTERVAL_IMMEDIATE,
};

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

static HWND InitWindow()
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "aegis4k_wnd";
	RegisterClass(&wc);

	RECT rect = {0, 0, g_width, g_height};
	DWORD wnd_style = WS_OVERLAPPEDWINDOW & (~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX));
	AdjustWindowRect(&rect, wnd_style, FALSE);

	return CreateWindowEx(WS_EX_TOPMOST, "aegis4k_wnd", "AEGIS4K - by atyuwen", wnd_style | WS_VISIBLE,
		(GetSystemMetrics(SM_CXSCREEN) - rect.right + rect.left) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - rect.bottom + rect.top) / 2,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, GetModuleHandle(NULL), NULL);
}

int EntryPoint()
{
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		return 0;
	}

	g_dev_params.hDeviceWindow = InitWindow();
	IDirect3DDevice9* device = NULL;
	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_dev_params.hDeviceWindow, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_dev_params, &device) != S_OK)
	{
		return 0;
	}

	DemoInit(device);

	MSG msg;
	while (GetCurrentSample() < MAX_SAMPLES)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DemoUpdate(device, GetTickCount());
		Sleep(10);
	}

#ifndef RELEASE
	DemoDestroy(device);
	device->Release();
	d3d->Release();
	DestroyWindow(g_dev_params.hDeviceWindow);
	return 0;
#else
	ExitProcess(0);
#endif
}

#ifndef RELEASE
int WINAPI WinMain(HINSTANCE _0, HINSTANCE _1, LPSTR _2, int _3)
{
#ifdef ENABLE_LOG
	AllocConsole();
	SetConsoleTitle("PIANO - console");

	#pragma warning(push)
	#pragma warning(disable: 4996)
	freopen("CONOUT$", "w+t", stdout);
	freopen("CONIN$", "r+t", stdin);
	#pragma warning(pop)
#endif

	return EntryPoint();
}
#endif
