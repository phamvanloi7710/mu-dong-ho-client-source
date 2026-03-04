#include "StdAfx.h"
#include "Util.h"
#include "PL_Font.h"

PLFont::PLFont()
{
	
}

PLFont::~PLFont()
{
}

PLFont* PLFont::Instance()
{
	static PLFont s_Instance;
	return &s_Instance;
}

eColor PLFont::SinWaveColor(float frc1, float frc2, float frc3, BYTE phs1, BYTE phs2, BYTE phs3, BYTE center, BYTE width, BYTE step) {
	eColor result;

	result.red = std::sin(frc1 * step + phs1) * width + center;
	result.grn = std::sin(frc2 * step + phs2) * width + center;
	result.blu = std::sin(frc3 * step + phs3) * width + center;
	result.opl = 0xff;

	return result;
}
int PLFont::RenderText(HFONT font, int PosX, int PosY, const ColorRGBA& color, const ColorRGBA& bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	int LineCount = 0;
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char* Line = strtok(Buff, "\n##");
	g_pRenderText->SetFont(font);
	g_pRenderText->SetTextColor(color.r, color.g, color.b, color.a);
	g_pRenderText->SetBgColor(bkcolor.r, bkcolor.g, bkcolor.b, bkcolor.a);
	while (Line != NULL)
	{
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align, 0);
		PosY += 10;
		Line = strtok(NULL, "\n##");
		LineCount++;
	}
	EnableAlphaTest(false);
	return LineCount;
}

int PLFont::DrawFormatWordWrap(ColorRGBA Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, int maxLineLength)
{
	std::string content(Text);
	std::istringstream iss(content);
	std::string word;
	std::string line;

	while (iss >> word)
	{
		// Kiểm tra nếu thêm từ sẽ vượt quá độ dài tối đa
		if (line.length() + word.length() + 1 > (size_t)maxLineLength)
		{
			gPLFont->RenderText(g_hFont, PosX, PosY, Color, noColor, Width, 0, Align, line.c_str());
			PosY += 13;
			line = word;
		}
		else
		{
			if (!line.empty()) line += " ";
			line += word;
		}
	}

	// Vẽ dòng cuối nếu còn
	if (!line.empty())
	{
		gPLFont->RenderText(g_hFont, PosX, PosY, Color, noColor, Width, 0, Align, line.c_str());
		PosY += 13;
	}

	return PosY;
}