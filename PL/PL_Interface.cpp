#include "stdafx.h"
#include "PL_Interface.h"
#include <UIMng.h>
#include <NewUISystem.h>
#include <PL/PL_Font.h>
using namespace SEASON3B;
PLCInterface::PLCInterface()
{

}
PLCInterface* PLCInterface::GetInstance()
{
	static PLCInterface gPLInterface;
	return &gPLInterface;
}

//void SEASON3B::CNewUICharacterInfoWindow::SetButtonInfo()
//{
//	unicode::t_char strText[256];
//	m_pScrollBar.Create(m_Pos.x + 160, m_Pos.y + 155, 216);
//	m_pScrollBar.SetPercent(0.0);
//	m_BtnStat[STAT_STRENGTH].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
//	m_BtnStat[STAT_STRENGTH].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_STRENGTH + 2, 16, 15);
//}

void PLCInterface::RenderNormalMain(float PositionX, float PositionY, float MainWidth, float MainHeight, const char* zText)
{

	int width = (int)(MainWidth / 3.f);
	int hight = (int)(MainHeight / 2.f);
	int inventory_depth = IMAGE_MAIN_BACKGROUND;
	int inventory_border = IMAGE_MAIN_BORDER;

	if (width > 76)
		width = 76;

	if (hight > 126)
		hight = 126;

	int leftover = MainWidth - (2 * width);
	int bothover = MainHeight - (2 * hight);

	int width_depth = MainWidth - 12.f;
	int hight_depth = MainHeight - 10.f;

	float iPos_x = PositionX + (MainWidth - (float)width);
	float iPos_y = PositionY + (MainHeight - (float)hight);

	//-- Background (parte central de la textura de fondo)
	SEASON3B::RenderImageF(inventory_depth, PositionX + 6.f, PositionY + 2.f, (float)width_depth, hight_depth, 0.0, 0.0, (float)width_depth >= 225.f ? 224.f : (float)width_depth, (float)hight_depth >= 512.f ? 512.f : (float)hight_depth);

	//-- border left
	SEASON3B::RenderImageF(inventory_border, PositionX, PositionY, (float)width, (float)hight, 0.0, 0.0, width, (float)hight);

	//-- border right
	SEASON3B::RenderImageF(inventory_border, iPos_x, PositionY, (float)width, hight, (float)(228 - width), 0.0, (float)width, (float)hight);

	//-- border left down
	SEASON3B::RenderImageF(inventory_border, PositionX, iPos_y, (float)width, (float)hight, 0.0, (float)(252 - hight), (float)width, (float)hight);

	//-- border right down
	SEASON3B::RenderImageF(inventory_border, iPos_x, iPos_y, (float)width, (float)hight, (float)(228 - width), (float)(252 - hight), (float)width, (float)hight);


	//-- leftover
	if (leftover > 0)
	{
		int max_width = 164;
		int sections = leftover / max_width;
		int remaining = leftover % max_width;
		float posx = PositionX + (float)width;

		for (int i = 0; i < sections; i++)
		{
			SEASON3B::RenderImageF(inventory_border, posx, PositionY, (float)max_width, hight, 32.f, 0.0, (float)max_width, hight);
			SEASON3B::RenderImageF(inventory_border, posx, iPos_y, (float)max_width, hight, 32.f, (float)(252 - hight), (float)max_width, hight);
			posx += (float)max_width;
		}

		if (remaining > 0)
		{
			SEASON3B::RenderImageF(inventory_border, posx, PositionY, (float)remaining, hight, 32.f, 0.0, (float)remaining, hight);
			SEASON3B::RenderImageF(inventory_border, posx, iPos_y, (float)remaining, hight, 32.f, (float)(252 - hight), (float)remaining, hight);
		}
	}

	if (bothover > 0)
	{
		int max_width = 120;
		int sections = bothover / max_width;
		int remaining = bothover % max_width;
		float posy = PositionY + (float)hight;
		float posx;
		for (int i = 0; i < sections; i++)
		{
			posx = PositionX;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)max_width, 0.f, 72.f, (float)width, max_width);
			posx = iPos_x;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)max_width, (float)(228 - width), 72.f, (float)width, max_width);
			posy += (float)max_width;
		}

		if (remaining > 0)
		{
			posx = PositionX;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)remaining, 0.f, 72.f, (float)width, remaining);
			posx = iPos_x;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)remaining, (float)(228 - width), 72.f, (float)width, remaining);
		}
	}

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 189, 25, 255);
	g_pRenderText->RenderText(PositionX + 10, PositionY + 8, zText, MainWidth - 20, 0, RT3_SORT_CENTER);
}
