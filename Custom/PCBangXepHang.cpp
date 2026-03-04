#include "stdafx.h"
#include "Custom\PCBangXepHang.h"
#include "DSPlaySound.h"
#include "UIControls.h"
#include "ZzzInterface.h"
#include "NewUICommon.h"
#include "NewUISystem.h"
#include "Input.h"

using namespace SEASON3B;

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;

CNewUIPCDraw::CNewUIPCDraw()
{
}

CNewUIPCDraw::~CNewUIPCDraw()
{
}

bool CNewUIPCDraw::DrawCustomWindows(float* StartX, float* StartY, float CuaSoW, float CuaSoH, int eNumWindow, LPCSTR Text, ...)
{
	if (!GMProtect->CheckWideScreen())
	{
		CuaSoH = (CuaSoH > 429) ? 429 : CuaSoH;
	}
	static char Buff[90];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);
	// ----
	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);
	float x = *StartX;
	float y = *StartY;

	float ScaleW = (225 / CuaSoW) / g_fScreenRate_x;
	float ScaleH = (225 / CuaSoH) / g_fScreenRate_y;

	EnableAlphaTest();
	//glColor3f(1.0, 1.0, 1.0);

	DrawFrame(IMAGE_BCUSTOM_WINDOW_31322, x, y, 225, 225, 0.0, 0.0, 270, 225, ScaleW, ScaleH); //khung goc ben trai tren
	//gInterface.DrawBarForm(x, y + 5, CuaSoW, CuaSoH - 10, 0.1, 0.1, 0.1, 0.9);
	//==== NGang Tren va Duoi
	for (int i = x + 10; i < (x + CuaSoW) - 10; i += 10)
	{
		RenderImage(IMAGE_BCUSTOM_WINDOW_31353, i, y, 10.0, 64.0, 60.0, 0); //khung goc ben trai tren
		RenderImage(IMAGE_BCUSTOM_WINDOW_31357, i, (y + CuaSoH) - 45.0, 10.0, 45.0, 60.0, 0); //khung goc ben trai tren
	}

	//=== Nen va thanh doc 2 trai phai
	for (int doc = y + 10; doc < (y + CuaSoH) - 10; doc += 10)
	{
		RenderImage(IMAGE_BCUSTOM_WINDOW_31355, x, doc, 21.0, 10, 0, 0); //Khung giua ben trai
		RenderImage(IMAGE_BCUSTOM_WINDOW_31356, (x + CuaSoW) - 21.0, doc, 21.0, 10, 0, 0); //Khung giua ben phai
	}

	RenderImage(IMAGE_BCUSTOM_WINDOW_31353, x, y, 60.0, 64.0, 0, 0); //khung goc ben trai tren
	RenderImage(IMAGE_BCUSTOM_WINDOW_31353, (x + CuaSoW) - 60.0, y, 60.0, 64.0, 130.0, 0); //khung goc ben phai tren

	RenderImage(IMAGE_BCUSTOM_WINDOW_31357, x, (y + CuaSoH) - 45.0, 60.0, 45.0, 0, 0); //khung goc ben trai duoi
	RenderImage(IMAGE_BCUSTOM_WINDOW_31357, (x + CuaSoW) - 60.0, (y + CuaSoH) - 45.0, 60.0, 45.0, 130.0, 0); //khung goc ben phai duoi

	//======Close
	int CloseX = x + CuaSoW - 33.0f;
	int CloseY = y;
	RenderImage(IMAGE_BCUSTOM_WINDOW_32453, CloseX, CloseY, 36.0f, 29.0f);
	if (SEASON3B::CheckMouseIn(CloseX, CloseY, 36, 36) == 1)
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			//g_pNewUISystem->Hide(eNumWindow);
			PlayBuffer(25, 0, 0);
		}
		RenderImage(IMAGE_BCUSTOM_WINDOW_32453, CloseX, CloseY, 36, 29, 0, 0, RGBA(255, 204, 20, 130));
		RenderTipText(CloseX + 40, CloseY + 15, "Close");
	}
	//================================================
	TextDraw(g_hFontBold, x, y + 10, 0xFFFFFFFF, 0x0, CuaSoW, 0, 3, Buff); //Buoc Vao
	//DisableAlphaBlend();
	EnableAlphaTest(false);
	return 1;
}

void CNewUIPCDraw::DrawInfoBox(float PosX, float PosY, float Width, float Height, DWORD bkcolor, int Arg6, int Arg7)
{
	float Red = (float)((bkcolor & 0xff000000) >> 24) / 255.0f;
	float Green = (float)((bkcolor & 0x00ff0000) >> 16) / 255.0f;
	float Blue = (float)((bkcolor & 0x0000ff00) >> 8) / 255.0f;
	float Alpha = (float)(bkcolor & 0x000000ff) / 255.0f;

	if (bkcolor)
	{
		EnableAlphaTest();
		glColor4f(Red, Green, Blue, Alpha);
		RenderColor(PosX, PosY, Width + 6.5, Height + 5, 0, Arg6); //central
		EndRenderColor();
	}
	float RightX = PosX - 2.0f;
	float LeftX = (PosX + Width) - 2.0f;
	float TopY = PosY - 2.0f;
	float DownY = (PosY + Height) - 2.0f;
	bool horizontal = true;
	bool vertical = true;
	//--
	float IniciarX = PosX - 2.0f;
	float w = 14.0f;
	//--
	float IniciarY = PosY - 2.0f;
	float H = 14.0f;


	float x = PosX - 4;
	float y = PosY - 3.1;
	float width = Width + 16;
	float height = Height + 15.1;
	RenderImage(IMAGE_INFO_TOP_LEFT, x, y, 14.0, 14.0);
	RenderImage(IMAGE_INFO_TOP_RIGHT, (x + width) - 14.f, y, 14.0, 14.0);
	RenderImage(IMAGE_INFO_BOTTOM_LEFT, x, (y + height) - 14.f, 14.0, 14.0);
	RenderImage(IMAGE_INFO_BOTTOM_RIGHT, (x + width) - 14.f, (y + height) - 14.f, 14.0, 14.0);
	RenderImage(IMAGE_INFO_TOP_PIXEL, x + 6.f, y, (width - 12.f), 14.0);
	RenderImage(IMAGE_INFO_RIGHT_PIXEL, (x + width) - 14.f, y + 6.f, 14.0, (height - 14.f));
	RenderImage(IMAGE_INFO_BOTTOM_PIXEL, x + 6.f, (y + height) - 14.f, (width - 12.f), 14.0);
	RenderImage(IMAGE_INFO_LEFT_PIXEL, x, (y + 6.f), 14.0, (height - 14.f));
}


bool CNewUIPCDraw::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_CUSTOM, this);


	SetPos(x, y);
	LoadImages();
	Show(true);


	return true;
}

void CNewUIPCDraw::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void CNewUIPCDraw::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

bool CNewUIPCDraw::UpdateMouseEvent()
{
	return true;
}

bool CNewUIPCDraw::UpdateKeyEvent()
{
	return true;
}

bool CNewUIPCDraw::Update()
{
	return true;
}

bool CNewUIPCDraw::Render() //Work()
{
	return true;
}

float CNewUIPCDraw::GetLayerDepth()
{
	return 9.3f;
}
void CNewUIPCDraw::LoadImages()
{
	//MAx IMAGE_HERO_POSITION_INFO_BASE_WINDOW +5;
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_BCUSTOM_WINDOW_31322, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back01.tga", IMAGE_BCUSTOM_WINDOW_31353, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_BCUSTOM_WINDOW_31357, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_BCUSTOM_WINDOW_31355, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_BCUSTOM_WINDOW_31356, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_cancel.tga", IMAGE_BCUSTOM_WINDOW_32453, GL_LINEAR);
		


}

void CNewUIPCDraw::UnloadImages()
{
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31322);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31353);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31357);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31355);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31356);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_32453);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW_31326);
	DeleteBitmap(IMAGE_BCUSTOM_WINDOW);
}