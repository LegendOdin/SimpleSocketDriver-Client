#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>

//Simple ESP Overlay
extern int g_screen_width;
extern int g_screen_height;
extern bool g_show_overlay;
extern LPDIRECT3D9 D3D; // the pointer to our Direct3D interface
extern LPDIRECT3DDEVICE9 D3DDEV;
extern LPD3DXLINE d3dline;
extern LPD3DXFONT D3DFONT;
extern LPD3DXSPRITE D3DSPRITE;
extern HWND g_hwnd;
extern MARGINS margin;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void InitD3D(HWND hWnd);
void RenderScene();
void FillRGB(float x, float y, float w, float h, int r, int g, int b, int a);
void FillBox(int x, int y, int width, int height, D3DCOLOR color);
void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color);
void DrawText(int x, int y, D3DCOLOR color, const char* text);
void DrawText(int x, int y, D3DCOLOR color, const wchar_t* text);
void DrawText(int x, int y, D3DCOLOR color, const char* text, int size);
void DrawText(int x, int y, D3DCOLOR color, const wchar_t* text, int size);
void DrawBox(float x, float y, float w, float h, D3DCOLOR color);
void DrawBox2(int x, int y, int width, int height, D3DCOLOR color, int thickness);
void BorderBox(int x, int y, int width, int height, D3DCOLOR color, int thickness);
void GradientBox(int x, int y, int width, int height, D3DCOLOR color, D3DCOLOR color2, bool horizontal);
void DrawBar(int x, int y, int width, int height, D3DCOLOR color, float value);	
void DrawBar2(int x, int y, int width, int height, D3DCOLOR color, D3DCOLOR color2, float value);
void Circle(float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, DWORD color);
void DrawCircle(int x, int y, int radius, int numSides, D3DCOLOR color);
void DrawCircle2(int x, int y, int radius, int numSides, D3DCOLOR color, int thickness);
void String(int x, int y, D3DCOLOR color, ID3DXFont* font, bool outlined, const char* text);
int String_Width(ID3DXFont* font, char* string);
int FrameRate();
class CVertexList {
public:
	FLOAT X, Y, Z;
	DWORD Color;
};

struct SD3DVertex {
	float x, y, z, rhw;
	DWORD color;
};
