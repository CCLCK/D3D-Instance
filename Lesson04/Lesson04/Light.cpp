#include "d3dUtility.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <iomanip>
#include <iostream>

IDirect3DDevice9* Device = nullptr;
D3DXMATRIX WorldMatrix;
IDirect3DVertexBuffer9* Pyramid = 0;
const int Width = 800;
const int Height = 600;

struct Vertex
{
	struct Vertex(float x, float y, float z, float nx, float ny, float nz )
	{
		_x = x;
		_y = y;
		_z = z;
		_nx = nx;
		_ny = ny;
		_nz = nz;
	};
	float _x, _y, _z;
	float _nx, _ny, _nz;
	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

bool Setup()
{
	
	Device->CreateVertexBuffer(12 * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED, &Pyramid, 0);
	Vertex* v;
	Pyramid->Lock(0, 0, (void**)&v, 0);

	v[0] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
	v[1] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, -0.707f);
	v[2] = Vertex(1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);

	v[3] = Vertex(-1.0f, 0.0f, 1.0f, -0.707f, 0.707f, 0.0f);
	v[4] = Vertex(0.0f, 1.0f, 0.0f, -0.707f, 0.707f, 0.0f);
	v[5] = Vertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);


	v[6] = Vertex(1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
	v[7] = Vertex(0.0, 1.0f, 0.0f, 0.707f, 0.707f, 0.0f);
	v[8] = Vertex(1.0f, 0.0f, 1.0f, 0.707f, 0.707f, 0.0f);


	v[9] = Vertex(1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	v[10] = Vertex(0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
	v[11] = Vertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, 0.707f);
	
	Pyramid->Unlock();


	D3DMATERIAL9 mtrl;
	mtrl.Ambient = d3d::WHITE;
	mtrl.Diffuse = d3d::WHITE;
	mtrl.Specular = d3d::WHITE;
	mtrl.Emissive = d3d::BLACK;
	mtrl.Power = 5.0;
	Device->SetMaterial(&mtrl);

	D3DLIGHT9 dir;
	::ZeroMemory(&dir, sizeof(dir));
	dir.Type = D3DLIGHT_DIRECTIONAL;
	dir.Diffuse = d3d::WHITE;
	dir.Specular = d3d::WHITE * 0.3f;
	dir.Ambient = d3d::WHITE * 0.6f;
	dir.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	Device->SetLight(0, &dir);
	Device->LightEnable(0, true);
	
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	

	D3DXMATRIX view_matrix;

	D3DXVECTOR3 pos(0.0f, 1.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&view_matrix, &pos, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &view_matrix);

	// set the projection matrix

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)Width / Height, 1.0f, 1000.0f);

	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;

}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(Pyramid);
}

bool Display(float timeDelta)
{
	if (!Device)
	{
		MessageBox(0, L"Display Err", 0, 0);
		return false;
	}
	Device->SetRenderState(D3DRS_LIGHTING, true);
	// 设置世界变换矩阵，使得金字塔旋转
	D3DXMATRIX Ry;
	static float y = 0.0f;
	D3DXMatrixRotationY(&Ry, y);
	y += timeDelta * 0.5;
	if (y >= 6.28f)
	{
		y = 0.0f;
	}
	Device->SetTransform(D3DTS_WORLD, &Ry);

	// 清空缓冲区
	Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	Device->BeginScene();

	// 设置顶点缓冲区和灵活顶点格式
	Device->SetStreamSource(0, Pyramid, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	// 绘制金字塔
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 4);

	// 结束场景渲染
	Device->EndScene();

	// 将渲染结果呈现到屏幕上
	Device->Present(0, 0, 0, 0);
	Device->SetRenderState(D3DRS_LIGHTING, false);
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