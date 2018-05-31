#include "sdk.h"
#include "Draw.h"
#include "xor.h"
#include "GameUtils.h"

CDraw g_Draw;

void CDraw::Init()
{

	font_esp = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_esp, XorStr("gothic"), 12, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	font_espnum = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_espnum, XorStr("Tahoma"), 9, FW_NORMAL, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	font_espinfo = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_espinfo, XorStr("Terminal"), 12, FW_NORMAL, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);

	font_build = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_build, XorStr("Verdana"), 12, FW_NORMAL, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	font_bui2 = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_bui2, XorStr("Verdana"), 34, FW_EXTRABOLD, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	font_icon = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_icon, XorStr("AstriumWep"), 11, FW_NORMAL, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	font_arrows = g_pSurface->SCreateFont();
	g_pSurface->SetFontGlyphSet(font_arrows, XorStr("Untitled1"), 32, FW_NORMAL, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

}

void CDraw::String(int x, int y, int r, int g, int b, const wchar_t* pszText)
{
	if (pszText == NULL)
		return;

	g_pSurface->DrawSetTextPos(x, y);
	g_pSurface->DrawSetTextFont(font_esp);
	g_pSurface->DrawSetTextColor(r, g, b, 255);
	g_pSurface->DrawPrintText(pszText, wcslen(pszText));
}

void CDraw::DrawLine(int x0, int y0, int x1, int y1, Color col)
{
	g_pSurface->SetDrawColor(col);
	g_pSurface->DrawLine(x0, y0, x1, y1);
}

void CDraw::Draw3DBox(Vector* boxVectors, Color color)
{
	Vector boxVectors0, boxVectors1, boxVectors2, boxVectors3,
		boxVectors4, boxVectors5, boxVectors6, boxVectors7;

	if (GameUtils::WorldToScreen(boxVectors[0], boxVectors0) && GameUtils::WorldToScreen(boxVectors[1], boxVectors1) && GameUtils::WorldToScreen(boxVectors[2], boxVectors2) && GameUtils::WorldToScreen(boxVectors[3], boxVectors3) && GameUtils::WorldToScreen(boxVectors[4], boxVectors4) && GameUtils::WorldToScreen(boxVectors[5], boxVectors5) && GameUtils::WorldToScreen(boxVectors[6], boxVectors6) && GameUtils::WorldToScreen(boxVectors[7], boxVectors7)) {

		Vector2D lines[12][2];

		lines[0][0] = { boxVectors0.x, boxVectors0.y };
		lines[0][1] = { boxVectors1.x, boxVectors1.y };
		lines[1][0] = { boxVectors1.x, boxVectors1.y };
		lines[1][1] = { boxVectors2.x, boxVectors2.y };
		lines[2][0] = { boxVectors2.x, boxVectors2.y };
		lines[2][1] = { boxVectors3.x, boxVectors3.y };
		lines[3][0] = { boxVectors3.x, boxVectors3.y };
		lines[3][1] = { boxVectors0.x, boxVectors0.y };

		lines[4][0] = { boxVectors0.x, boxVectors0.y };
		lines[4][1] = { boxVectors6.x, boxVectors6.y };


		lines[5][0] = { boxVectors6.x, boxVectors6.y };
		lines[5][1] = { boxVectors5.x, boxVectors5.y };
		lines[6][0] = { boxVectors5.x, boxVectors5.y };
		lines[6][1] = { boxVectors4.x, boxVectors4.y };
		lines[7][0] = { boxVectors4.x, boxVectors4.y };
		lines[7][1] = { boxVectors7.x, boxVectors7.y };
		lines[8][0] = { boxVectors7.x, boxVectors7.y };
		lines[8][1] = { boxVectors6.x, boxVectors6.y };

		lines[9][0] = { boxVectors5.x, boxVectors5.y };
		lines[9][1] = { boxVectors1.x, boxVectors1.y };

		lines[10][0] = { boxVectors4.x, boxVectors4.y };
		lines[10][1] = { boxVectors2.x, boxVectors2.y };

		lines[11][0] = { boxVectors7.x, boxVectors7.y };
		lines[11][1] = { boxVectors3.x, boxVectors3.y };

		for (int i = 0; i < 12; i++) {
			g_Draw.DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, color);
		}
	}
}

std::wstring CDraw::stringToWide(const std::string& text)
{
	std::wstring wide(text.length(), L' ');
	std::copy(text.begin(), text.end(), wide.begin());

	return wide;
}

int CDraw::getWidht(const char* input, unsigned int font)
{
	int iWide = 0;
	int iTall = 0;
	int iBufSize = MultiByteToWideChar(CP_UTF8, 0x0, input, -1, NULL, 0);

	wchar_t* pszUnicode = new wchar_t[iBufSize];

	MultiByteToWideChar(CP_UTF8, 0x0, input, -1, pszUnicode, iBufSize);

	g_pSurface->GetTextSize(font, pszUnicode, iWide, iTall);

	delete[] pszUnicode;

	return iWide;
}

void CDraw::DrawRect(int x1, int y1, int x2, int y2, Color clr)
{
	g_pSurface->SetDrawColor(clr);
	g_pSurface->DrawFilledRect(x1, y1, x2, y2);
}

void CDraw::StringA(unsigned long Font, bool center, int x, int y, int r, int g, int b, int a, const char* input, ...)
{
	CHAR szBuffer[MAX_PATH];

	if (!input)
		return;

	vsprintf_s(szBuffer, input, (char*)&input + _INTSIZEOF(input));

	if (center)
		x -= getWidht(szBuffer, Font) / 2;

	g_pSurface->DrawSetTextColor(r, g, b, a);
	g_pSurface->DrawSetTextFont(Font);
	g_pSurface->DrawSetTextPos(x, y);

	std::wstring wide = stringToWide(std::string(szBuffer));
	g_pSurface->DrawPrintText(wide.c_str(), wide.length());
}

void CDraw::FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a)
{
	g_pSurface->SetDrawColor(r, g, b, a);
	g_pSurface->DrawFilledRect(x, y, x + w, y + h);
}
void CDraw::FillRGBAOutlined(int x, int y, int w, int h, int r, int g, int b, int a)
{
	FillRGBA(x, y, w, h, r, g, b, a);
	Box(x, y, w, h, 0, 0, 0, a);
}
void CDraw::Box(int x, int y, int w, int h, int r, int g, int b, int a)
{
	g_pSurface->SetDrawColor(r, g, b, a);
	g_pSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void CDraw::LineRGBA(int x1, int y1, int x2, int y2, int r, int g, int b, int a)
{

	g_pSurface->SetDrawColor(r, g, b, a);
	g_pSurface->DrawLine(x1, y1, x2, y2);
}
void TextW(bool cent, unsigned int font, int x, int y, int r, int g, int b, int a, wchar_t* pszString)
{
	if (cent) {
		int wide, tall;
		g_pSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_pSurface->DrawSetTextColor(r, g, b, a);
	g_pSurface->DrawSetTextFont(font);
	g_pSurface->DrawSetTextPos(x, y);
	g_pSurface->DrawPrintText(pszString, (int)wcslen(pszString));
}

void CDraw::Text(int x, int y, int r, int g, int b, int a, bool bCenter, unsigned int font, const char* fmt, ...)
{
	va_list va_alist;
	char szBuffer[1024] = { '\0' };

	va_start(va_alist, fmt);
	vsprintf_s(szBuffer, fmt, va_alist);
	va_end(va_alist);

	wchar_t* pszStringWide = reinterpret_cast<wchar_t*>(malloc((strlen(szBuffer) + 1) * sizeof(wchar_t)));

	size_t outSize;
	size_t size = strlen(szBuffer) + 1;
	mbstowcs_s(&outSize, pszStringWide, size, szBuffer, size - 1);

	TextW(bCenter, font, x, y, r, g, b, a, pszStringWide);

	free(pszStringWide);
}
void CDraw::ColoredCircle(int x, int y, int radius, int r, int g, int b, int a)
{
	g_pSurface->SetDrawColor(r, g, b, a);
	g_pSurface->DrawOutlinedCircle(x, y, radius, 32);
}
void CDraw::DrawRectRainbow(int x, int y, int width, int height, float flSpeed, float& flRainbow)
{
	Color colColor(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f)
		flRainbow = 0.f;

	for (int i = 0; i < width; i++) {
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f)
			hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		FillRGBA(x + i, y, 1, height, colRainbow.r(), colRainbow.g(), colRainbow.b(), colRainbow.a());
	}
}
void CDraw::DrawTextureByteArray(Vector2D p0, Vector2D p1, Vector2D p2, Vector2D p3, const unsigned char* pRawRGBAData)
{
	static unsigned char buffer[4] = { 0, 0, 0, 100 }; 
	Vertex_t verts[4];
	static int Texture = g_pSurface->CreateNewTextureID(true);
	g_pSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_pSurface->SetDrawColor(255, 255, 255, 255);
	g_pSurface->DrawSetTexture(Texture);

	verts[0].Init(p0);
	verts[1].Init(p1);
	verts[2].Init(p2);
	verts[3].Init(p3);
	g_pSurface->DrawTexturedPolygon(4, verts);
}







































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































































