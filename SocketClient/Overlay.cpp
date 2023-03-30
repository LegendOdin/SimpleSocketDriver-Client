#include "D3DOverlay.h"
#include <string>
#include <cstdarg>
#include <d3dtypes.h>
#include <time.h>
#include <vector>

//Main Defines for Overlay
int g_screen_width = 1920;
int g_screen_height = 1080;
LPDIRECT3D9 D3D;
LPDIRECT3DDEVICE9 D3DDEV;
ID3DXLine* d3dline;
MARGINS margin = { 0, 0, g_screen_width, g_screen_height };
LPDIRECT3DVERTEXBUFFER9 g_pVB;
HWND hWnd;
HWND twnd;
LPD3DXFONT d3dFont;
void InitD3D(HWND hWnd) {
	D3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = g_screen_width;
	d3dpp.BackBufferHeight = g_screen_height;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3DDEV);
	D3DXCreateLine(D3DDEV, &d3dline);
	D3DXCreateFontA(D3DDEV, 12, 0, FW_MEDIUM, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Purista-light", &d3dFont);
}

void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color) {
	D3DXVECTOR2 dLine[2];
	d3dline->SetWidth(1.f);
	dLine[0].x = x;
	dLine[0].y = y;
	dLine[1].x = xx;
	dLine[1].y = yy;
	d3dline->Draw(dLine, 2, color);
}

void DrawBox(float x, float y, float w, float h, D3DCOLOR color) {
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + w, y);
	points[2] = D3DXVECTOR2(x + w, y + h);
	points[3] = D3DXVECTOR2(x, y + h);
	points[4] = D3DXVECTOR2(x, y);
	d3dline->SetWidth(1.f);
	d3dline->Draw(points, 5, color);
}

void FillBox(int x, int y, int width, int height, D3DCOLOR color) {
	SD3DVertex pVertex[4] = { { x, y + height, 0.0f, 1.0f, color }, { x, y, 0.0f, 1.0f, color }, { x + width, y + height, 0.0f, 1.0f, color }, { x + width, y, 0.0f, 1.0f, color } };
	D3DDEV->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	D3DDEV->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(SD3DVertex));
}
void BorderBox(int x, int y, int width, int height, int thickness, D3DCOLOR color) {
	FillBox(x, y, width, thickness, color);
	FillBox(x, y, thickness, height, color);
	FillBox(x + width - thickness, y, thickness, height, color);
	FillBox(x, y + height - thickness, width, thickness, color);
}
int FrameRate() {
	static int iFPS, iLastFPS;
	static float flLastTickCount, flTickCount;
	flTickCount = clock() * 0.001f;
	iFPS++;
	if ((flTickCount - flLastTickCount) >= 1.0f) {
		flLastTickCount = flTickCount;
		iLastFPS = iFPS;
		iFPS = 0;
	}
	return iLastFPS;
}
void String(int x, int y, D3DCOLOR color, ID3DXFont* font, bool outlined, const char* text) {
	/*va_list args;
	char cBuffer[256];
	va_start(args, text);
	vsprintf_s(cBuffer, text, args);
	va_end(args);
	RECT pRect;
	if (outlined) {
		pRect.left = x - 1;
		pRect.top = y;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
		pRect.left = x + 1;
		pRect.top = y;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
		pRect.left = x;
		pRect.top = y - 1;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
		pRect.left = x;
		pRect.top = y + 1;
		font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, D3DCOLOR_RGBA(0, 0, 0, 255));
	}
	pRect.left = x;
	pRect.top = y;
	font->DrawTextA(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, color);{}*/
}

void Circle(float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, DWORD color) {
	std::vector<SD3DVertex> circle(resolution + 2);
	float angle = rotate * D3DX_PI / 180;
	float pi;
	if (type == 1) pi = D3DX_PI;
	if (type == 2) pi = D3DX_PI / 2;      // 1/2 circle
	if (type == 3) pi = D3DX_PI / 4;   // 1/4 circle

	for (int i = 0; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(i * (2 * pi / resolution)));
		circle[i].y = (float)(y - radius * sin(i * (2 * pi / resolution)));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}
	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle) * (circle[i].x - x) - sin(angle) * (circle[i].y - y);
		circle[i].y = y + sin(angle) * (circle[i].x - x) + cos(angle) * (circle[i].y - y);
	}
	D3DDEV->CreateVertexBuffer((resolution + 2) * sizeof(SD3DVertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);
	VOID* pVertices;
	g_pVB->Lock(0, (resolution + 2) * sizeof(SD3DVertex), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(SD3DVertex));
	g_pVB->Unlock();
	D3DDEV->SetTexture(0, NULL);
	D3DDEV->SetPixelShader(NULL);
	if (smoothing) {
		D3DDEV->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
		D3DDEV->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
	}
	D3DDEV->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	D3DDEV->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	D3DDEV->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	D3DDEV->SetStreamSource(0, g_pVB, 0, sizeof(SD3DVertex));
	D3DDEV->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	D3DDEV->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
	if (g_pVB != NULL) g_pVB->Release();
}

void DrawBar(int x, int y, int width, int height, D3DCOLOR color, float value) {
	float l, r, g;
	g = value * 2.55;
	r = 255 - g;
	l = value / 3;
	FillRGB(x - (l / 2) - 1, y - 1, l + 2 + 13, 5, 0, 0, 0, 255);
	FillRGB(x - (l / 2), y, l + 13, 3, r, g, 0, 255);
}
void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a) {
	D3DXVECTOR2 vLine[2];

	d3dline->SetWidth(w);

	vLine[0].x = x + w / 2;
	vLine[0].y = y;
	vLine[1].x = x + w / 2;
	vLine[1].y = y + h;

	d3dline->Begin();
	d3dline->Draw(vLine, 2, D3DCOLOR_RGBA(r, g, b, a));
	d3dline->End();
}
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		//render_scene();
		break;
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
