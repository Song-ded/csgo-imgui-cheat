#include "includes.h"

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color) {
	D3DRECT rect = { x, y, x + w, y + h };
	pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawLine(int x1, int y1, int x2, int y2, int thickness, bool antialias, D3DCOLOR color) {
	ID3DXLine* LineL;
	D3DXCreateLine(pDevice, &LineL);

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->SetAntialias(antialias);
	LineL->Draw(Line, 2, color);
	LineL->Release();
}

void DrawBox(int x, int y, int w, int h, int thikness, bool antialias, D3DCOLOR color) {
	DrawLine(x, y, x + w, y, thikness, antialias, color);
	DrawLine(x, y + h, x + w, y + h, thikness, antialias, color);
	DrawLine(x, y, x, y + h, thikness, antialias, color);;
	DrawLine(x + w, y, x + w, y + h, thikness, antialias, color);
}