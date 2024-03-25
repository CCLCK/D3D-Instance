#define _CRT_SECURE_NO_WARNINGS

#include "d3dUtility.h"
#include <winuser.h>
IDirect3DDevice9* Device = 0;
bool Setup()
{
	return true;
}

void Cleanup()
{

}

bool Display(float timeDelta)//通用习惯是接收一个timeDelta
{
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			0x00000000, 1.0f, 0);
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

//HRESULT IDirect3DDevice9::Clear(
//	DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float z, DWORD Stencil
//);

int d3d::EnterMsgLoop(bool (*ptr_display)(float timeDelta))
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	static float lastTime = (float)timeGetTime();
	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime) * 0.001f;
			ptr_display(timeDelta);
			lastTime = currTime;
		}
	}
	return msg.wParam;
}



LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam==VK_ESCAPE)
		{
			::DestroyWindow(hwnd);	
		}
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}




bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width,
	int height,
	bool windowed,
	D3DDEVTYPE deviceType,
	IDirect3DDevice9** device)
{
	// Register the window class.
	

	WNDCLASS wc = { };
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Direct3D9App";

	
	if (!RegisterClass(&wc))
	{
		::MessageBox(0, L"RegistClasss() - FAILED", 0, 0);
		return false;
	}
	// Create the window.

	HWND hwnd = ::CreateWindowW(
		L"Direct3D9App", L"D3D9App", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		::MessageBox(0, L"CrateWindow() - FAILED", 0, 0);
		return false;
	}
	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);//绘制好后会立即显示


	//创建指针
	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);//D3D_SDK_VERSION保证应用程序使用正确的头文件
	if (!d3d9)
	{
		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	/*HRESULT IDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType,
		D3DCAPS9 * pCaps);*/
	D3DCAPS9 caps;//caps是一个存储设备能力的结构体对象
	//获取设备能力
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);//传了结构体地址来填充结构体

	int vp = 0;//看是否支持硬件处理，支持硬件处理的渲染起来不需要CPU，极大提高渲染性能
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)//看是否支持变换和光照
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//设备支持硬件变换和光照就可以支持顶点处理
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	


	//填充D3DPRESENT_PARAMETER参数
	
	//typedef struct _D3DPRESENT_PARAMETERS_ {
	//	UINT BackBufferWidth;
	//	UINT BackBufferHeight;
	//	D3DFORMAT BackBufferFormat; //更高位数的像素格式可以提高图像质量
	//	UINT BackBufferCount;
	//	D3DMULTISAMPLE_TYPE MultiSampleType;//多重采样类型
	//	DWORD MultiSampleQuality;//多重采样的质量水平
	//	D3DSWAPEFFECT SwapEffect;//页面置换方式
	//	HWND hDeviceWindow; //指定窗口句柄
	//	BOOL Windowed;
	//	BOOL EnableAutoDepthStencil;
	//	D3DFORMAT AutoDepthStencilFormat;
	//	DWORD Flags;
	//	UINT FullScreen_RefreshRateInHz;
	//	UINT PresentationInterval;
	//
	//}D3DPRESENT_PARAMETERS;

	D3DPRESENT_PARAMETERS d3dpp; 
	d3dpp.BackBufferWidth = 800;
	d3dpp.BackBufferHeight = 600;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//创建IDirect3DDevice9接口
	/*
	HRESULT IDirect3D9::CreateDevice(
		UINT Adapter,//指定哪块显卡
		D3DDEVTYPE DeviceType,//设备类型
		HWND hFoucusWindow,//和d3dpp.hDeviceWindow为同一个句柄
		DWORD BehaviorFlags,//硬件顶点运算还是软件顶点运算
		D3DPRESENT_PARAMETERS* pPresentationParameters,//指向已初始化的D3DPRESENT_PARAMETERS
		IDirect3DDevice9** ppReturnedDeviceInterface);//返回创建的设备
		*/

	
	HRESULT hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		deviceType,
		hwnd,
		vp,
		&d3dpp,
		device);
	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateDevice() - FAILED", 0, 0);
		return 0;
	}
	/*else
	{
		::MessageBox(0, L"CreateDevice() - SUCCESSED",L"right", 0);
		return 1;
	}*/
	d3d9->Release();
	return true;
	
}




int WINAPI WinMain(
	HINSTANCE hinstance, 
	HINSTANCE prevInstance, 
	PSTR cmdLine, 
	int showCmd)
{
	
	if (!d3d::InitD3D(hinstance, 800, 600, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, L"InitD3D()-FAILED", 0, 0);
		return 0;
	}
	if (!Setup())
	{
		::MessageBox(0, L"Setup()-FAILED", 0, 0);
		return 0;
	}
	d3d::EnterMsgLoop(Display);
	Cleanup();

	Device->Release();
	return 0;
}