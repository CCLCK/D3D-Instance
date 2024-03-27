#include "d3dUtility.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <iomanip>
#include <iostream>

IDirect3DDevice9* Device = nullptr;
D3DXMATRIX WorldMatrix;
IDirect3DVertexBuffer9* Triangle = 0;
const int Width = 800;
const int Height = 600;

struct ColorVertex
{
	ColorVertex(){}

	ColorVertex(float x,float y,float z,D3DCOLOR color)
	{
		_x = x;
		_y = y;
		_z = z;
		_color = color;
	}
	float _x, _y, _z;
	D3DCOLOR _color;
	static const DWORD FVF;
};
const DWORD ColorVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

bool Setup()
{
	Device->CreateVertexBuffer(3 * sizeof(ColorVertex),D3DUSAGE_WRITEONLY,ColorVertex::FVF,D3DPOOL_MANAGED,
		&Triangle,0);
	ColorVertex* v;
	Triangle->Lock(0, 0, (void**)&v, 0);
	v[0] = ColorVertex(-1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[1] = ColorVertex(0.0f, 1.0f, 2.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[2] = ColorVertex(1.0f, 0.0f, 2.0f, D3DCOLOR_XRGB(0, 0, 255));
	Triangle->Unlock();

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_LIGHTING, false);
	return true;

	
}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool Display(float timeDelta)
{
	if (Device)
	{
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();
		Device->SetFVF(ColorVertex::FVF);
		Device->SetStreamSource(0, Triangle, 0, sizeof(ColorVertex));

		D3DXMatrixTranslation(&WorldMatrix, -1.25f, 0.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &WorldMatrix);

		Device->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		D3DXMatrixTranslation(&WorldMatrix, 1.25f, 0.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &WorldMatrix);

		Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		Device->EndScene();
		Device->Present(0, 0, 0, 0);

	}
	return true;
}



LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			::DestroyWindow(hwnd);
		}
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(
	HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{

	if (!d3d::InitD3D(hinstance, 640, 480, true, D3DDEVTYPE_HAL, &Device))
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