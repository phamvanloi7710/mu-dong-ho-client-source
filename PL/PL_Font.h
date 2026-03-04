#pragma once
#define RT3_SORT_LEFT									1
#define RT3_SORT_LEFT_CLIP								2
#define RT3_SORT_CENTER									3
#define RT3_SORT_RIGHT									4
#define RT3_WRITE_RIGHT_TO_LEFT							7
#define RT3_WRITE_CENTER								8

#define MAX_CHARS										256
#define g_Font											*(HFONT*)0xE8C588
#define g_FontBold										*(HFONT*)0xE8C58C
#define g_hFixFont										*(HFONT*)0xE8C594
#define	ARGB(a,r,g,b)									(((DWORD)(a))<<24 | (DWORD)(r) | ((DWORD)(g))<<8 | ((DWORD)(b))<<16)
#define	CLRDW_WHITE										ARGB(255, 255, 255, 255)
#define CLRDW_BR_GRAY									ARGB(255, 226, 226, 226)
#define CLRDW_GRAY										ARGB(255, 119, 119, 119)
#define CLRDW_YELLOW									ARGB(255, 255, 255, 121)
#define CLRDW_BR_YELLOW									ARGB(255, 255, 238, 193)
#define CLRDW_BR_ORANGE									ARGB(255, 255, 217, 39)
#define CLRDW_ORANGE									ARGB(255, 255, 180, 0)
#define CLRDW_DARKYELLOW								ARGB(255, 255, 255, 0)
// COLORREF Ÿ��.
#define	CLRREF_WHITE									RGB(240, 240, 240)
#define	CLRREF_BR_GRAY									RGB(210, 210, 210)
#define	CLRREF_GRAY										RGB(127, 127, 127)
#define	CLRREF_BLACK									RGB(  0,   0,   0)
#define	CLRREF_RED										RGB(252,  60,  60)
#define	CLRREF_BR_RED									RGB(252, 128, 128)
#define	CLRREF_ORANGE									RGB(220, 170, 100)
#define	CLRREF_YELLOW									RGB(210, 210, 100)
#define	CLRREF_BR_YELLOW								RGB(240, 240, 170)
#define	CLRREF_GREEN									RGB(100, 230, 100)
#define	CLRREF_DK_BLUE									RGB( 50,  40, 255)
#define	CLRREF_BLUE										RGB(101,  89, 255)
#define	CLRREF_BR_BLUE									RGB(168, 148, 255)
#define	CLRREF_VIOLET									RGB(210, 100, 210)
#define	CLRREF_BR_OCHER									RGB(250, 225, 200)

#include "PL_Color.h"
extern HFONT m_hFont;
extern HFONT m_hFontBold;
extern HFONT m_hFontBig;
extern HFONT m_hFontNum;
struct eColor {
	BYTE red;
	BYTE grn;
	BYTE blu;
	BYTE opl;
};

class PLFont
{
public:
	PLFont();
	virtual ~PLFont();
	static PLFont* Instance();
	eColor	SinWaveColor(float frc1, float frc2, float frc3, BYTE phs1, BYTE phs2, BYTE phs3, BYTE center, BYTE width, BYTE step);
	int RenderText(HFONT font, int PosX, int PosY, const ColorRGBA& color, const ColorRGBA& bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...);
	int DrawFormatWordWrap(ColorRGBA Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, int maxLineLength);
};
#define gPLFont			(PLFont::Instance())