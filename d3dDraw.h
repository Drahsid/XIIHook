#ifndef D3DDRAW_H
#define D3DDRAW_H

#include "pch.h"

namespace d3dHook 
{

	class Color4 {
	public:
		int r, g, b, a;
		Color4(int cr = 255, int cg = 255, int cb = 255, int ca = 255) {
			r = cr; g = cg; b = cb; a = ca;
		}
		inline Color4 Color4::operator* (const float lhs) { return Color4(r * lhs, g * lhs, b * lhs, a * lhs); }
		inline Color4 Color4::operator+ (const Color4 lhs) { 
		r += lhs.r;
		g += lhs.g;
		b += lhs.b;
		a += lhs.a;
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;
		return *this;
		}
	};

	class Vector2 {
	public:
		int x, y;
		Vector2(int px = 0, int py = 0) {
			x = px; y = py;
		}
		inline Vector2 Vector2::operator+ (const Vector2&lhs) { return Vector2(x + lhs.x, y + lhs.y); }
		inline Vector2 Vector2::operator- (const Vector2&lhs) { return Vector2(x - lhs.x, y - lhs.y); }
		inline Vector2 Vector2::operator* (const int lhs) { return Vector2(x*lhs, y*lhs); }
		inline Vector2 Vector2::operator/ (const int lhs) { return Vector2(x / lhs, y / lhs); }
	};

	class Box2D {
	public:
		Vector2 Pos;
		Vector2 Size;
		Box2D() {
			Pos = Vector2();
			Size = Vector2();
		}
		Box2D(Vector2&p, Vector2&s) {
			Pos = p;
			Size = s;
		}
		Box2D(int xx, int xy, int xsx, int xsy) {
			Pos = Vector2(xx, xy);
			Size = Vector2(xsx, xsy);
		}
	};

	

	void DrawLine(Vector2&p, Vector2&p0, Color4&c);
	void DrawCursorLine(Vector2&p, Vector2&p0, Color4&c);
	void FillRGB(Box2D&b, Color4&c);
	void DrawBox(Box2D&b, float px, Color4&c);
	void DrawAndFillBox(Box2D&b, Color4&c, Color4&ac, float px);
	void DrawProgressBar(Box2D&b, Color4&c, Color4&ac, Color4&pc, float t, float px0, float px1);
	void DrawTabBox(Box2D&b, Color4&c, Color4&ac, Color4&t, char* title);
	int DrawString(char* String, Vector2&p, Color4&c, ID3DXFont* ifont);


	int DrawString(char* String, Vector2&p, Color4&c, ID3DXFont* ifont)
	{
		RECT ShadowPos;
		ShadowPos.left = p.x + 1;
		ShadowPos.top = p.y + 1;
		RECT FontPos;
		FontPos.left = p.x;
		FontPos.top = p.y;
		ifont->DrawTextA(0, String, strlen(String), &ShadowPos, DT_NOCLIP, D3DCOLOR_ARGB(c.a, c.r / 3, c.g / 3, c.b / 3));
		ifont->DrawTextA(0, String, strlen(String), &FontPos, DT_NOCLIP, D3DCOLOR_ARGB(c.a, c.r, c.g, c.b));
		return 0;
	}
	void FillRGB(Box2D&b, Color4&c)
	{
		D3DXVECTOR2 vLine[2];

		p_Line->SetWidth(b.Size.x);

		vLine[0].x = b.Pos.x + b.Size.x / 2;
		vLine[0].y = b.Pos.y;
		vLine[1].x = b.Pos.x + b.Size.x / 2;
		vLine[1].y = b.Pos.y + b.Size.y;

		p_Line->Begin();
		p_Line->Draw(vLine, 2, D3DCOLOR_RGBA(c.r, c.g, c.b, c.a));
		p_Line->End();
	}
	void DrawLine(Vector2&p, Vector2&p0, Color4&c)
	{
		D3DXVECTOR2 dLine[2];

		p_Line->SetWidth(1);

		dLine[0].x = p.x;
		dLine[0].y = p.y;
		dLine[1].x = p0.x;
		dLine[1].y = p0.y;

		p_Line->Draw(dLine, 2, D3DCOLOR_ARGB(c.a, c.r, c.g, c.b));

	}
	void DrawCursorLine(Vector2&p, Vector2&p0, Color4&c)
	{
		D3DXVECTOR2 dLine[2];

		p_Line->SetWidth(2);

		dLine[0].x = p.x;
		dLine[0].y = p.y;

		dLine[1].x = p0.x;
		dLine[1].y = p0.y;

		p_Line->Draw(dLine, 2, D3DCOLOR_ARGB(c.a, c.r, c.g, c.b));

	}
	void DrawBox(Box2D&b, float px, Color4&c)
	{
		D3DXVECTOR2 points[5];
		points[0] = D3DXVECTOR2(b.Pos.x, b.Pos.y);
		points[1] = D3DXVECTOR2(b.Pos.x + b.Size.x, b.Pos.y);
		points[2] = D3DXVECTOR2(b.Pos.x + b.Size.x, b.Pos.y + b.Size.y);
		points[3] = D3DXVECTOR2(b.Pos.x, b.Pos.y + b.Size.y);
		points[4] = D3DXVECTOR2(b.Pos.x, b.Pos.y);
		p_Line->SetWidth(px);
		p_Line->Draw(points, 5, D3DCOLOR_RGBA(c.r, c.g, c.b, c.a));
	}
	void DrawAndFillBox(Box2D&b, Color4&c, Color4&ac, float px = 1)
	{
		Box2D fill = b;
		fill.Pos.x += 1;
		fill.Pos.y += 1;
		fill.Size.x -= 1;
		fill.Size.y -= 1;
		DrawBox(b, px, c);
		FillRGB(fill, ac);
	}
	void DrawProgressBar(Box2D&b, Color4&c, Color4&ac, Color4&pc, float t, float px0 = 1, float px1 = 1)
	{
		Box2D prog = b;
		prog.Size.x = floor(prog.Size.x * t);	

		FillRGB(b, ac);
		if (prog.Size.x > 0) 
		{
			FillRGB(prog, pc);
			DrawBox(prog, px1, c);
		}
		DrawBox(b, px0, c);
	}
	void DrawTabBox(Box2D&b, Color4&c, Color4&ac, Color4&t, char* title)
	{
		Vector2 sOff = Vector2(b.Pos.x + 3, b.Pos.y);
		Vector2 lOff = Vector2(b.Pos.x, b.Pos.y + 16);
		Vector2 lEnd = Vector2(b.Pos.x + b.Size.x, b.Pos.y + 16);
		FillRGB(b, ac);
		DrawLine(lOff, lEnd, c);
		DrawString(title, sOff, t, pFontSmall);
	}
}

#endif




