#include "d3dUtility.h"

#include <cerrno>
#include <cfenv>
#include <cmath>
#include <iomanip>
#include <iostream>

IDirect3DDevice9* Device = nullptr;
IDirect3DVertexBuffer9* VB = 0;
IDirect3DIndexBuffer9* IB = 0;
const int Width = 800;
const int Height = 600;
const double pi = std::acos(-1);
struct Vertex
{
	Vertex(){}
	Vertex(float x, float y, float z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	float _x, _y, _z;
	static const DWORD FVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ;

//画立方体，修改画的图形别忘了修改display中的顶点和图元数量
bool Setup_cube()
{
	Device->CreateVertexBuffer(8 * sizeof(Vertex),D3DUSAGE_WRITEONLY,Vertex::FVF,D3DPOOL_MANAGED,
		&VB,0);
	Device->CreateIndexBuffer(36 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,D3DPOOL_MANAGED,
		&IB,0);
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices,0);

	vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
	vertices[1] = Vertex(-1.0f, 1.0f, -1.0f);
	vertices[2] = Vertex(1.0f, 1.0f, -1.0f);
	vertices[3] = Vertex(1.0f, -1.0f, -1.0f);
	vertices[4] = Vertex(-1.0f, -1.0f, 1.0f);
	vertices[5] = Vertex(-1.0f, 1.0f, 1.0f);
	vertices[6] = Vertex(1.0f, 1.0f, 1.0f);
	vertices[7] = Vertex(1.0f, -1.0f, 1.0f);
	
	VB->Unlock();

	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;


	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 4; indices[10] = 7; indices[11] = 6;


	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;

	indices[18] = 3; indices[19] = 2; indices[20] = 6;
	indices[21] = 3; indices[22] = 6; indices[23] = 7;
	
	indices[24] = 1; indices[25] = 5; indices[26] = 6;
	indices[27] = 1; indices[28] = 6; indices[29] = 2;
	
	indices[30] = 4; indices[31] = 0; indices[32] = 3;
	indices[33] = 4; indices[34] = 3; indices[35] = 7;
	
	IB->Unlock();

	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V,&position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)Width/(float)Height,1.0f,1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	return true;
}


int floorNum = 37;
int level = 50;//五十层的圆柱（蚌
//摄像机坐标
float position_x = 0;
float position_y = 0;
float position_z = -5;
bool Setup()
{
	
	double alpha = 2 * pi / (floorNum-1);
	
	//总的点数=level*(floorNum+1)
	//((floorNum-1)*level+2*(floorNum-1))*3
	Device->CreateVertexBuffer(level*floorNum * sizeof(Vertex), D3DUSAGE_WRITEONLY, Vertex::FVF, D3DPOOL_MANAGED,
		&VB, 0);
	Device->CreateIndexBuffer((floorNum - 1) * (level + (level - 1) * 2)*3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED,
		&IB, 0);
	Vertex* vertices;
	VB->Lock(0, 0, (void**)&vertices, 0);

	//生成这几层的点
	for (int j = 0; j < level; j++)
	{
		vertices[floorNum*j] = Vertex(0.0f, j, 0.0f);
		for (int i = 1; i < floorNum; i++)
		{
			vertices[floorNum * j+i] = Vertex(cos(alpha * (i-1)), j, sin(alpha * (i-1)));
		}
	}
	
	//vertices[1] = Vertex(1.0f, 0.0f, 0.0f);//起点
	//vertices[2] = Vertex(0.707f, 0.0f, -0.707f);
	//vertices[3] = Vertex(0.0f, 0.0f, -1.0f);
	//vertices[4] = Vertex(-0.707f, 0.0f, -0.707f);
	//vertices[5] = Vertex(-1.0f, 0.0f, 0.0f);
	//vertices[6] = Vertex(-0.707f, 0.0f, 0.707f);
	//vertices[7] = Vertex(0.0f, 0.0f, 1.0f);
	//vertices[8] = Vertex(0.707f, 0.0f, 0.707f);
	
	/*for (int i=0;i<9;i++)
	{
		vertices[i + 9] = vertices[i];
		vertices[i + 9]._y = vertices[i]._y + 1.0;
	}*/
	
	VB->Unlock();

	WORD* indices = 0;
	IB->Lock(0, 0, (void**)&indices, 0);

	for (int i=0;i<level;i++)//层数
	{
		float height = vertices[floorNum * i]._y;//中心点的高度
		//这一层的范围是1-8 10-17，即(floorNum*层数+1)-(floorNum*层数+floorNum-1),层数从0开始
		int center = floorNum * i;//中心点的顶点缓存的下标
		/*int start = floorNum * i + 1;*///center+1是这一圈的起始点
		//每一层要花费3*floorNum个indices点，3个一组
		int spend = i * (floorNum-1) * 3;
		for (int j = 0; j < floorNum-1; j++)
		{
			indices[spend+3 * j] = center;
			indices[spend+3 * j+1] = center+1+j;
			indices[spend+3 * j+2] = center+1+j+1;
		}
		//特殊处理一下
		indices[spend + 3 *(floorNum-2) + 2] = center+1;
	}

	int cur = (floorNum - 1) * level * 3;
	//假设只有两层，因为电脑只带的起来两层
	//上面链接下层
	for (int i = 0; i < level-1; i++)
	{
		int center = floorNum * i;//中心点的顶点缓存的下标
		int next_center = floorNum * (i + 1);
		for (int j = 0; j < floorNum-1; j++)
		{
			indices[cur + 3 * j] = center+j+1;
			indices[cur + 3 * j +1] = center + j+2;
			indices[cur + 3 * j + 2] = next_center + j+1;
		}
		indices[cur + 3 * (floorNum - 2) + 1] = center + 1;
		cur += 3 * (floorNum - 1);
	}
	
	//下层链接上层
	for (int i = 0; i < level - 1; i++)
	{
		int center = floorNum * i;//中心点的顶点缓存的下标
		int next_center = floorNum * (i + 1);
		for (int j = 0; j < floorNum - 1; j++)
		{
			indices[cur + 3 * j] = center + j + 1;
			indices[cur + 3 * j + 1] = next_center + j + 2;
			indices[cur + 3 * j + 2] = next_center + j + 1;
		}
		indices[cur + 3 * (floorNum - 2) + 2] = next_center + 1;
		cur += 3 * (floorNum - 1);
	}

	IB->Unlock();

	D3DXVECTOR3 position(position_x, position_y, position_z);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)Width / (float)Height, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
	// 取消背面隐藏
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}
void Cleanup()
{
	d3d::Release<IDirect3DVertexBuffer9*>(VB);
	d3d::Release<IDirect3DIndexBuffer9*>(IB);
}

bool Display(float timeDelta)
{
	if (Device)
	{
		D3DXMATRIX Rx, Ry;
		D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);
		static float y = 0.0f;
		D3DXMatrixRotationX(&Ry, y);
		y += timeDelta;

		if (y >= 6.28f)
		{
			y = 0.0f;
		}

		D3DXMATRIX p = Rx * Ry;
		
		Device->SetTransform(D3DTS_WORLD, &p);

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
		Device->SetIndices(IB);
		Device->SetFVF(Vertex::FVF);//level+(level-1)*2

		Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, level*floorNum, 0, ((floorNum - 1) * (level + (level - 1) * 2)));

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

	if (!d3d::InitD3D(hinstance, 640,480, true, D3DDEVTYPE_HAL, &Device))
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