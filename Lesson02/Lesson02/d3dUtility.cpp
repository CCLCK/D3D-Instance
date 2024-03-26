#include "d3dUtility.h"
#include <winuser.h>



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
	::UpdateWindow(hwnd);//���ƺú��������ʾ


	//����ָ��
	IDirect3D9* d3d9 = 0;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);//D3D_SDK_VERSION��֤Ӧ�ó���ʹ����ȷ��ͷ�ļ�
	if (!d3d9)
	{
		::MessageBox(0, L"Direct3DCreate9() - FAILED", 0, 0);
		return false;
	}

	/*HRESULT IDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType,
		D3DCAPS9 * pCaps);*/
	D3DCAPS9 caps;//caps��һ���洢�豸�����Ľṹ�����
	//��ȡ�豸����
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);//���˽ṹ���ַ�����ṹ��

	int vp = 0;//���Ƿ�֧��Ӳ������֧��Ӳ���������Ⱦ��������ҪCPU�����������Ⱦ����
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)//���Ƿ�֧�ֱ任�͹���
	{
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//�豸֧��Ӳ���任�͹��վͿ���֧�ֶ��㴦��
	}
	else
	{
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}



	//���D3DPRESENT_PARAMETER����

	//typedef struct _D3DPRESENT_PARAMETERS_ {
	//	UINT BackBufferWidth;
	//	UINT BackBufferHeight;
	//	D3DFORMAT BackBufferFormat; //����λ�������ظ�ʽ�������ͼ������
	//	UINT BackBufferCount;
	//	D3DMULTISAMPLE_TYPE MultiSampleType;//���ز�������
	//	DWORD MultiSampleQuality;//���ز���������ˮƽ
	//	D3DSWAPEFFECT SwapEffect;//ҳ���û���ʽ
	//	HWND hDeviceWindow; //ָ�����ھ��
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

	//����IDirect3DDevice9�ӿ�
	/*
	HRESULT IDirect3D9::CreateDevice(
		UINT Adapter,//ָ���Ŀ��Կ�
		D3DDEVTYPE DeviceType,//�豸����
		HWND hFoucusWindow,//��d3dpp.hDeviceWindowΪͬһ�����
		DWORD BehaviorFlags,//Ӳ���������㻹�������������
		D3DPRESENT_PARAMETERS* pPresentationParameters,//ָ���ѳ�ʼ����D3DPRESENT_PARAMETERS
		IDirect3DDevice9** ppReturnedDeviceInterface);//���ش������豸
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




