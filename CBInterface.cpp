#include "stdafx.h"
#include "jpexs.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "CBInterface.h"
#include "ZzzInterface.h"
#include "NewUISystem.h"
#include "NewUICommon.h"
#include <CB_HuyDongExc.h>
#include <UIMng.h>
using namespace SEASON3B;
CB_Interface* g_pCBInterface = CB_Interface::Instance();

CB_Interface* CB_Interface::Instance()
{
	static CB_Interface s_Instance;
	return &s_Instance;
}

CB_Interface::CB_Interface()
{
	this->BLockButtonMoveWindow = false;
	ZeroMemory(&Data, sizeof(Data));
	this->CloseAllWindowCustom();
	BButtonClickTime = 0;
	BLockButtonHover = false;
	BLockMessageBox = false;
	AutoHP = false;
	AutoCtrlPK = false;
	
	this->AutoHP = false;
	this->AutoHPTick = 0;
	
	//==MessBox
	this->MsgBoxCaption.clear();
	this->MsgBoxText.clear();
	this->MsgBoxType = 0;
	this->MsgLine = 0;

	this->vCaptcha = this->generateCaptcha(4);

	
	this->SAU_ActiveMix = false;
	this->SAU_EffectsMix = false;
	this->SetStateDoiItem = false;
	this->RateSAU = false;
	memset(this->SAU_ItemChinh, 0xFF, sizeof(this->SAU_ItemChinh));
	memset(this->SAU_ItemPhu, 0xFF, sizeof(this->SAU_ItemPhu));
	for (int n = 0; n < 6; n++)
	{
		memset(this->SAU_ItemKetQua[n], 0xFF, sizeof(this->SAU_ItemKetQua[n]));
		 
	}
	
}

CB_Interface::~CB_Interface()
{
}


int CB_Interface::DrawMessage(int Mode, LPCSTR Text, ...)
{
	// ----
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);
	// ----
	CreateNotice(Buff, Mode);
	return 1;
}
void CB_Interface::CloseAllWindowCustom()
{
	for (int i = eBeginWindowCustom; i < eEndWindowCustom; i++) this->Data[i].Close(); //Close All window
}
void CB_Interface::SetBlockCur(bool Delay)
{
	if (Delay)
	{
		BLockMouseMoveClick = GetTickCount() + 300;
	}
	else
	{
		BLockMouseMoveClick = 0;
	}

}
bool CB_Interface::BtnProcess()
{
	if (BLockMouseMoveClick > GetTickCount())
	{
		return true;

	}
	else
	{
		BLockMouseMoveClick = 0;
	}
	return false;
}
void CB_Interface::MoveWindows(int eNumWindow)
{
	if (GetKeyState(VK_LBUTTON) & 0x8000 && GetTickCount() - this->Data[eNumWindow].EventTick > 300)
	{
		if (SEASON3B::CheckMouseIn(this->Data[eNumWindow].X, this->Data[eNumWindow].Y, this->Data[eNumWindow].Width - 70, 30))
		{
			if (!this->Data[eNumWindow].OnClick) {
				this->Data[eNumWindow].OnClick = true;
				this->Data[eNumWindow].curX = MouseX;
				this->Data[eNumWindow].curY = MouseY;
			}
		}
		if (this->Data[eNumWindow].OnClick)
		{
			this->BLockButtonMoveWindow = true;
			this->Data[eNumWindow].X += MouseX - this->Data[eNumWindow].curX;
			this->Data[eNumWindow].Y += MouseY - this->Data[eNumWindow].curY;
			this->Data[eNumWindow].curX = MouseX;
			this->Data[eNumWindow].curY = MouseY;
			//===
			if (this->Data[eNumWindow].X < 0)
				this->Data[eNumWindow].X = 0;
			if (this->Data[eNumWindow].Y < 0)
				this->Data[eNumWindow].Y = 0;

			if ((this->Data[eNumWindow].X + this->Data[eNumWindow].Width) > gwinhandle->GetScreenX())
				this->Data[eNumWindow].X = gwinhandle->GetScreenX() - (this->Data[eNumWindow].Width);
			if ((this->Data[eNumWindow].Y + this->Data[eNumWindow].Height + 46) > gwinhandle->GetScreenY())
				this->Data[eNumWindow].Y = gwinhandle->GetScreenY() - (this->Data[eNumWindow].Height + 46);
		}
	}
	else {
		if (this->Data[eNumWindow].OnClick)
		{
			this->Data[eNumWindow].OnClick = false;
			this->BLockButtonMoveWindow = false;
		}
	}
}
void CB_Interface::DrawBarForm(float PosX, float PosY, float Width, float Height, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	EnableAlphaTest(true);
	glColor4f((GLfloat)red, (GLfloat)green, (GLfloat)blue, (float)alpha);
	RenderColor(PosX, PosY, Width, Height, 0.0f, 0);
	EndRenderColor();
	glColor3f(1, 1, 1);
	EnableAlphaTest(false);;
}
int CB_Interface::TextDraw(HFONT font, int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
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
	g_pRenderText->SetTextColor((color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, color & 0x000000ff);
	g_pRenderText->SetBgColor((bkcolor & 0xff000000) >> 24, (bkcolor & 0x00ff0000) >> 16, (bkcolor & 0x0000ff00) >> 8, bkcolor & 0x000000ff);
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

bool CB_Interface::gDrawWindowCustom(float* StartX, float* StartY, float CuaSoW, float CuaSoH, int eNumWindow, LPCSTR Text, ...)
{
	va_list args;
	va_start(args, Text);
	TCHAR Buff[90];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	if (!this->Data[eNumWindow].OnShow)
	{
		return 0;
	}
	if (!this->Data[eNumWindow].FirstLoad)
	{
		this->Data[eNumWindow].Width = CuaSoW;
		this->Data[eNumWindow].Height = CuaSoH;
		this->Data[eNumWindow].X = *StartX;
		this->Data[eNumWindow].Y = *StartY;
		this->Data[eNumWindow].FirstLoad = true;
		// ✅ Thêm dòng này cho phép di chuyển ngay lần đầu
		this->Data[eNumWindow].AllowMove = true;
	}
	else
	{
		(*StartX) = this->Data[eNumWindow].X;
		(*StartY) = this->Data[eNumWindow].Y;
	}


	if (this->Data[eNumWindow].AllowMove)
	{
		MoveWindows(eNumWindow);
	}


	if (SEASON3B::CheckMouseIn(this->Data[eNumWindow].X, this->Data[eNumWindow].Y, CuaSoW, CuaSoH) == 1)
	{
		this->SetBlockCur(TRUE);
		this->Data[eNumWindow].Hover = true;
	}
	else
	{
		this->Data[eNumWindow].Hover = false;
	}
	float ScaleW = (225 / CuaSoW) / g_fScreenRate_x;
	float ScaleH = (225 / CuaSoH) / g_fScreenRate_y;

	EnableAlphaTest();
	window_backmsg(this->Data[eNumWindow].X, this->Data[eNumWindow].Y, CuaSoW, CuaSoH);
	//======Close
	int CloseX = this->Data[eNumWindow].X + CuaSoW - 27.0f;
	int CloseY = this->Data[eNumWindow].Y + 5;
	SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, CloseX, CloseY, 16.0, 16.0, 0.0, 0.0, 15.0, 15.0);
	if (SEASON3B::CheckMouseIn(CloseX, CloseY, 36, 36) == 1)
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000 && GetTickCount() - this->Data[eNumWindow].EventTick > 300 /*&& !BLockButtonHover && !BLockMessageBox*/ && !BLockButtonMoveWindow)
		{
			PlayBuffer(25, 0, 0);
			glColor3f(1.0f, 0.0f, 0.0f);
			BLockMouseMoveClick = GetTickCount() + 500;
			this->Data[eNumWindow].Close();
		}
		SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, CloseX, CloseY, 16.0, 16.0, RGBA(255, 204, 20, 130), 0.0, 0.0, 15.0, 15.0);
		RenderTipText(CloseX + 40, CloseY + 15, "Close");
	}
	//================================================
	this->TextDraw(g_hFontBold, this->Data[eNumWindow].X, this->Data[eNumWindow].Y + 10, 0xFFFFFFFF, 0x0, CuaSoW, 0, 3, Buff); //Buoc Vao

	glColor3f(1, 1, 1);
	EnableAlphaTest(false);

	return 1;
}

bool CB_Interface::gDrawWindowCustom2(float* StartX, float* StartY, float CuaSoW, float CuaSoH, int eNumWindow, LPCSTR Text, ...)
{
	va_list args;
	va_start(args, Text);
	TCHAR Buff[90];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	if (!this->Data[eNumWindow].OnShow)
	{
		return 0;
	}
	if (!this->Data[eNumWindow].FirstLoad)
	{
		this->Data[eNumWindow].Width = CuaSoW;
		this->Data[eNumWindow].Height = CuaSoH;
		this->Data[eNumWindow].X = *StartX;
		this->Data[eNumWindow].Y = *StartY;
		this->Data[eNumWindow].FirstLoad = true;
		// ✅ Thêm dòng này cho phép di chuyển ngay lần đầu
		//this->Data[eNumWindow].AllowMove = true;
	}
	else
	{
		(*StartX) = this->Data[eNumWindow].X;
		(*StartY) = this->Data[eNumWindow].Y;
	}


	if (this->Data[eNumWindow].AllowMove)
	{
		//MoveWindows(eNumWindow);
	}


	if (SEASON3B::CheckMouseIn(this->Data[eNumWindow].X, this->Data[eNumWindow].Y, CuaSoW, CuaSoH) == 1)
	{
		this->SetBlockCur(TRUE);
		this->Data[eNumWindow].Hover = true;
	}
	else
	{
		this->Data[eNumWindow].Hover = false;
	}
	float ScaleW = (225 / CuaSoW) / g_fScreenRate_x;
	float ScaleH = (225 / CuaSoH) / g_fScreenRate_y;

	EnableAlphaTest();
	window_backmsg(this->Data[eNumWindow].X, this->Data[eNumWindow].Y, CuaSoW, CuaSoH);
	//======Close
	int CloseX = this->Data[eNumWindow].X + CuaSoW - 27.0f;
	int CloseY = this->Data[eNumWindow].Y + 5;
	SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, CloseX, CloseY, 16.0, 16.0, 0.0, 0.0, 15.0, 15.0);
	if (SEASON3B::CheckMouseIn(CloseX, CloseY, 36, 36) == 1)
	{
		if (GetKeyState(VK_LBUTTON) & 0x8000 && GetTickCount() - this->Data[eNumWindow].EventTick > 300 /*&& !BLockButtonHover && !BLockMessageBox*/ && !BLockButtonMoveWindow)
		{
			PlayBuffer(25, 0, 0);
			glColor3f(1.0f, 0.0f, 0.0f);
			BLockMouseMoveClick = GetTickCount() + 500;
			this->Data[eNumWindow].Close();
		}
		SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, CloseX, CloseY, 16.0, 16.0, RGBA(255, 204, 20, 130), 0.0, 0.0, 15.0, 15.0);
		RenderTipText(CloseX + 40, CloseY + 15, "Close");
	}
	//================================================
	this->TextDraw(g_hFontBold, this->Data[eNumWindow].X, this->Data[eNumWindow].Y + 10, 0xFFFFFFFF, 0x0, CuaSoW, 0, 3, Buff); //Buoc Vao

	glColor3f(1, 1, 1);
	EnableAlphaTest(false);

	return 1;
}

bool CB_Interface::CheckWindow(int WindowID)
{
	return g_pNewUISystem->IsVisible(WindowID);
}

//void CB_Interface::Work()
//{
//	gInterface->DrawWindowVQ();
//
//}
void CB_Interface::Work()
{
	CB_Interface::DrawDoiItem();
	gCBHuyDongExc->DrawWindowHuyExc(); 
	// Vẽ vòng quay
	gInterface->DrawWindowVQ();

	// Thoát bằng ESC
	if (gInterface->Data[eWindowVongQuay].OnShow && SEASON3B::IsPress(VK_ESCAPE))
	{
		gInterface->Data[eWindowVongQuay].Close();
		g_pWindowMgr->HideAllWindow(FALSE);
		PlayBuffer(SOUND_CLICK01);
	}

	if (gInterface->Data[eWindowDoiItem].OnShow && SEASON3B::IsPress(VK_ESCAPE))
	{
		gInterface->Data[eWindowDoiItem].Close();
		PlayBuffer(SOUND_CLICK01);
	}
}

bool CB_Interface::UseHotKey()
{
	if (SEASON3B::IsPress(VK_F9))
	{
		gInterface->OpenWindowVongQuay();
		PlayBuffer(SOUND_CLICK01);
		return true;
	}
	return false;
}

bool CB_Interface::DrawButton(float PosX, float PosY, float SizeButton, int FontSize, const char* ButtonText, float mSizeButtonW, bool IsEnter)
{
	EnableAlphaTest();
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	//============Button
	float SizeButtonW = SizeButton;
	float SizeButtonH = (SizeButton * 20) / 100;

	if (mSizeButtonW != 0)
	{
		SizeButtonW = mSizeButtonW;
	}
	float StartX = PosX;
	float StartY = PosY;
	bool mreturn = false;
	float ScaleX = 0.0, ScaleY = 0.227;
	//=====================Button 1
	if (SEASON3B::CheckMouseIn(StartX, StartY, SizeButtonW, SizeButtonH) == 1 && !BLockButtonHover && !BLockMessageBox && !BLockButtonMoveWindow)
	{
		BLockMouseMoveClick = GetTickCount() + 500;
		ScaleX = ScaleY;
		if (GetTickCount() - BButtonClickTime > 500) //Click
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{

				ScaleX = ScaleY * 2;
				BButtonClickTime = GetTickCount();
				PlayBuffer(25, 0, 0);
				mreturn = true;
			}
		}
	}
	if ((GetKeyState(VK_RETURN) & 0x8000) && IsEnter)
	{
		//g_pBCustomMenuInfo->CloseWindow(ChatWindow);
		PlayBuffer(25, 0, 0);
		mreturn = true;

	}
	//31563
	//RenderBitmap(31563, StartX, StartY, SizeButtonW, SizeButtonH, 0.0, 0.0, 0.64999998, 0.80000001, 1, 1, 0);
	RenderBitmap(BITMAP_btn_empty, StartX, StartY, SizeButtonW, SizeButtonH, 0.0, ScaleX, 0.830, ScaleY, 1, 1, 0);
	TextDraw(g_hFontBold, StartX, (StartY + (SizeButtonH / 2) - (FontSize / 2.7)), 0xE6FCF7FF, 0x0, SizeButtonW, 0, 3, ButtonText); //Buoc Vao
	//=====================================
	//DeleteObject(ButtonText);
	glColor3f(1, 1, 1);
	EnableAlphaTest(false);
	return mreturn;
}

void CBRenderText(char* text, int x, int y, int sx, int sy, DWORD color, DWORD backcolor, int sort)
{
	if (!g_pRenderText) return;
	g_pRenderText->SetFont(g_hFont);

	DWORD backuptextcolor = g_pRenderText->GetTextColor();
	DWORD backuptextbackcolor = g_pRenderText->GetBgColor();

	g_pRenderText->SetTextColor(color);
	g_pRenderText->SetBgColor(backcolor);
	g_pRenderText->RenderText(x, y, text, sx, sy, sort);

	g_pRenderText->SetTextColor(backuptextcolor);
	g_pRenderText->SetBgColor(backuptextbackcolor);
}
#include <cmath>
#include <PL/PL_Font.h>




void* BCacheInfoItemDD;
DWORD CacheTimeShowInfoItem = 0;
bool CB_Interface::ChangeSSendItem(ITEM* ItemSell, int Slot, bool KeyClick)
{
	if (KeyClick && gInterface->Data[eWindowDoiItem].OnShow)// Send Item
	{
 
		if (ItemSell->Type < 0) { return 0; }

		PMSG_ITEM_MOVE_RECV pMsg = { 0 };

		pMsg.h.set(0xD3, 0x6B, sizeof(pMsg));
		pMsg.Target = -1;
		pMsg.sFlag = 0;
		pMsg.tFlag = 0;
		pMsg.Target = 0;
		pMsg.Source = Slot;
		::PlayBuffer(SOUND_GET_ITEM01);
		DataSend((BYTE*)&pMsg, pMsg.h.size);
		char debugMsg[128];
		sprintf(debugMsg, "DEBUG: Sent Slot %d", Slot);
		MessageBoxA(0, debugMsg, "DEBUG", 0);
		 
		return 1;
	}

	return 0;
}
void sfasfDrawInfoItemSet()
{

	if (BCacheInfoItemDD && CacheTimeShowInfoItem > GetTickCount())
	{
		RenderItemInfo(MouseX + 75, MouseY, (ITEM*)BCacheInfoItemDD, 0, 0, false, false);
	}
	else if (BCacheInfoItemDD)
	{
		BCacheInfoItemDD = nullptr;
	}


}
void BackDoiItem(int Type)
{
	if ((GetTickCount() - (gInterface->Data[eWindowDoiItem].EventTick)) > 300)
	{

		XULY_CGPACKET pMsg;
		pMsg.header.set(0xD3, 0x6C, sizeof(pMsg));
		pMsg.ThaoTac = Type; //
		DataSend((LPBYTE)&pMsg, pMsg.header.size);
		gInterface->Data[eWindowDoiItem].EventTick = GetTickCount();
	}
}

int DoiItem = -1;
void CB_Interface::DrawDoiItem()
{
	if (!gInterface->Data[eWindowDoiItem].OnShow)
	{
		if (gInterface->SetStateDoiItem == 1)
		{
			XULY_CGPACKET pMsg;
			pMsg.header.set(0xD3, 0x6A, sizeof(pMsg));
			pMsg.ThaoTac = 111; //
			DataSend((LPBYTE)&pMsg, pMsg.header.size);
			//gInterface->SetStateDoiItem = false;
			//gInterface->SAU_EffectsMix = false;
			//gInterface->SAU_ActiveMix = false;
			//gInterface->RateSAU = false;

			 this->SAU_ActiveMix = false;
			 this->SAU_EffectsMix = false;
			 this->SetStateDoiItem = false;
			 this->RateSAU = false;

			DoiItem = -1;
		}
		return;
	}
	if (!g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY))
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_INVENTORY);
	}
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER))
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_CHARACTER);
	}

	float StartX = 10;
	float StartY = 20;
	float WindowW = 350;
	float WindowH = 400;
	int CheckItemOK = 0;

	gInterface->gDrawWindowCustom(&StartX, &StartY, WindowW, WindowH, eWindowDoiItem, "Đổi Item Tùy Chọn");



	float BoxItemWH = 35;
	//===Item Chinh
	float PosXGroup1 = StartX + 20;
	float PosYGroup1 = StartY + 40;
	gInterface->DrawInfoBox(PosXGroup1 - 10, PosYGroup1 - 5, WindowW - 30, 240, 0x00000096, 0, 0);

	TextDraw((HFONT)g_hFontBold, PosXGroup1, PosYGroup1 + 5, 0xEBA000FF, 0x0, 0, 0, 1, "Chức năng Đổi Item Class");//
	//===Box Item Chinh
	gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup1 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); // Chinh

	ITEM* CTItemChinh = g_pNewItemMng->CreateItem(gInterface->SAU_ItemChinh);
	if (CTItemChinh->Type != 0x1FFF)
	{
		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2) + 7), (PosYGroup1 + 20) + 7, BoxItemWH, BoxItemWH, CTItemChinh->Type, CTItemChinh->Level, CTItemChinh->Option1, CTItemChinh->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup1 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemChinh;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_RBUTTON) & 0x8000)
			{
				BackDoiItem(0);
			}
		}
	}

	//===Item Phu
	float PosYGroup2 = PosYGroup1 + 85;
	TextDraw((HFONT)g_hFontBold, PosXGroup1, PosYGroup2, 0xEBA000FF, 0x0, 0, 0, 1, "Item Nhận Được");//
	//===Box Item Phu
	if (DoiItem == 1)
	{
		gInterface->DrawInfoBox(PosXGroup1 + 50, PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 1
	}
	gInterface->DrawInfoBox(PosXGroup1 + 50, PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 1
	//====
	ITEM* CTItemPhu1 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[0]);
	if (CTItemPhu1->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + 50) + 7, (PosYGroup2 + 20) + 7, BoxItemWH, BoxItemWH, CTItemPhu1->Type, CTItemPhu1->Level, CTItemPhu1->Option1, CTItemPhu1->ExtOption, 0, 1.4, FALSE);//BMD MOdel

		if (SEASON3B::CheckMouseIn((PosXGroup1 + 50), PosYGroup2 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemPhu1;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 1;
				//BackDoiItem(1);
			}
		}
	}
	//====
	if (DoiItem == 2)
	{
		gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 2
	}
	gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 2
	//====
	ITEM* CTItemPhu2 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[1]);
	if (CTItemPhu2->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2)) + 7, (PosYGroup2 + 20) + 7, BoxItemWH, BoxItemWH, CTItemPhu2->Type, CTItemPhu2->Level, CTItemPhu2->Option1, CTItemPhu2->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn((PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2)), PosYGroup2 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemPhu2;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 2;
				//BackDoiItem(2);
			}
		}
	}
	//====
	if (DoiItem == 3)
	{
		gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 3
	}
	gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup2 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 3
	//====
	ITEM* CTItemPhu3 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[2]);
	if (CTItemPhu3->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + ((WindowW - 40) / 2) + 70) + 5, (PosYGroup2 + 20) + 7, BoxItemWH, BoxItemWH, CTItemPhu3->Type, CTItemPhu3->Level, CTItemPhu3->Option1, CTItemPhu3->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup2 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemPhu3;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 3;
				//BackDoiItem(3);
			}
		}
	}
	//====

	//===Item Ket Qua
	float PosYGroup3 = PosYGroup2 + 85;

	if (DoiItem == 4)
	{
		gInterface->DrawInfoBox(PosXGroup1 + 50, PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 4
	}
	gInterface->DrawInfoBox(PosXGroup1 + 50, PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 4

	ITEM* CTItemKQ1 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[3]);
	if (CTItemKQ1->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + 50) + 7, (PosYGroup3 + 20) + 7, BoxItemWH, BoxItemWH, CTItemKQ1->Type, CTItemKQ1->Level, CTItemKQ1->Option1, CTItemKQ1->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn((PosXGroup1 + 50), PosYGroup3 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemKQ1;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 4;
				//BackDoiItem(4);
			}
		}
	}
	//====
	if (DoiItem == 5)
	{
		gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 5
	}
	gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2), PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 5
	//====
	ITEM* CTItemKQ2 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[4]);
	if (CTItemPhu2->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2)) + 7, (PosYGroup3 + 20) + 7, BoxItemWH, BoxItemWH, CTItemKQ2->Type, CTItemKQ2->Level, CTItemKQ2->Option1, CTItemKQ2->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn((PosXGroup1 + ((WindowW - 40) / 2) - (BoxItemWH / 2)), PosYGroup3 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemKQ2;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 5;
				//BackDoiItem(5);
			}
		}
	}
	//====
	if (DoiItem == 6)
	{
		gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x26E0A596, 0, 0); //Phu 6
	}

	gInterface->DrawInfoBox(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup3 + 20, BoxItemWH, BoxItemWH, 0x00000096, 0, 0); //Phu 6
	//====
	ITEM* CTItemKQ3 = g_pNewItemMng->CreateItem(gInterface->SAU_ItemKetQua[5]);
	if (CTItemKQ3->Type != 0x1FFF)
	{

		g_pNewUISystem->RenderItem3DFree((PosXGroup1 + ((WindowW - 40) / 2) + 70) + 5, (PosYGroup3 + 20) + 7, BoxItemWH, BoxItemWH, CTItemKQ3->Type, CTItemKQ3->Level, CTItemKQ3->Option1, CTItemKQ3->ExtOption, 0, 1.4, FALSE);//BMD MOdel
		if (SEASON3B::CheckMouseIn(PosXGroup1 + ((WindowW - 40) / 2) + 70, PosYGroup3 + 20, 50, 50) == 1)
		{
			BCacheInfoItemDD = CTItemKQ3;
			CacheTimeShowInfoItem = GetTickCount() + 50;
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				DoiItem = 6;
				//BackDoiItem(6);
			}
		}
	}

	gInterface->DrawInfoBox(PosXGroup1 - 10, PosYGroup3 + 80, WindowW - 30, 90, 0x00000096, 0, 0);
	//===Info Text
	float PosYTextInfo = PosYGroup3 + 85;
	int CountText = 0;
	TextDraw((HFONT)g_hFontBold, PosXGroup1, PosYTextInfo + (10 * CountText++), 0xEBA000FF, 0x0, 0, 0, 1, "Đổi Vật Phẩm Theo Tùy Chọn Class");//

	TextDraw((HFONT)g_hFontBold, PosXGroup1, PosYTextInfo + (20 * CountText++), 0xEBA000FF, 0x0, 0, 0, 1, "Click Chuột Trái Vào Item Để Chọn");//

	TextDraw((HFONT)g_hFontBold, PosXGroup1, PosYTextInfo + (20 * CountText++), 0xEBA000FF, 0x0, 0, 0, 1, "WCoin");//

	TextDraw((HFONT)g_hFontBold, PosXGroup1 + 110, PosYTextInfo + (20 * 2), 0x62FF00FF, 0x0, 0, 0, 1, "%s", gInterface->NumberFormat(gInterface->RateSAU));//
	float ButtonW = 115;
	float ButtonH = 25;
	float ButtonX = PosXGroup1 + ((WindowW - 40) / 2) - (ButtonW / 2);
	float ButtonY = (StartY + WindowH) - 20 - ButtonH;
	//if (gElemental.gDrawButton(ButtonX, ButtonY, ButtonW, 12, "Dung Luyện")) //Dung Luyen

	if (gInterface->DrawButton(ButtonX, ButtonY, ButtonW, 12, "Đổi Item"))
	{
		PlayBuffer(25, 0, 0);
		BackDoiItem(DoiItem);
		DoiItem = -1;
		gInterface->Data[eWindowDoiItem].OnShow ^= 1;
	}
	sfasfDrawInfoItemSet();
}



int SelectTypeVQ = 1;
int Chay = -1;
float CountVong = 0.5;
int solan = 1;

void CB_Interface::OpenWindowVongQuay() {
	if (GetTickCount() < gInterface->Data[eWindowVongQuay].EventTick + 500) return;
	gInterface->Data[eWindowVongQuay].EventTick = GetTickCount();

	if (!gInterface->Data[eWindowVongQuay].OnShow) {
		XULY_CGPACKET pMsg;
		pMsg.header.set(0xD3, 0x8C, sizeof(pMsg));
		pMsg.ThaoTac = 1;
		DataSend((LPBYTE)&pMsg, pMsg.header.size);

		pMsg.header.set(0xD3, 0x8B, sizeof(pMsg));
		DataSend((LPBYTE)&pMsg, pMsg.header.size);
	}
	else {
		gInterface->Data[eWindowVongQuay].OnShow = 0;
	}
}
void CB_Interface::DrawWindowVQ()
{
	// ----
	// ----
	
	if (!gInterface->Data[eWindowVongQuay].OnShow)
	{
		 
		if (SelectTypeVQ != 1) SelectTypeVQ = 1;
		gInterface->StartRollSau = -1;
		gInterface->IndexItemSau = -1;


		return;
	}
 
	//gInterface->CheckWindow(ObjWindow::Inventory);

	float WindowW = 420;
	float WindowH = 300;

	float StartY = ((MAX_WIN_HEIGHT - 51) / 2) - (WindowH / 2);
	float StartX = 10 + 100;

	//===Move Window
	if (gInterface->Data[eWindowVongQuay].FirstLoad)
	{
		StartX = gInterface->Data[eWindowVongQuay].X;
		StartY = gInterface->Data[eWindowVongQuay].Y;
	}

	gInterface->gDrawWindowCustom(&StartX, &StartY, WindowW, WindowH, eWindowVongQuay, "Vòng Quay May Mắn");

	{
		 
		glColor3f(1.0, 1.0, 1.0);
	
		//===Info Yeu Cau Moc Nap
		float InfoMocNapX = (StartX + 10) + 3;
		float InfoMocNapY = (StartY + 15);
		float TyleInfoYeuCau = 7.5f;
		float WInfo = (WindowW - 20) / 10;
		float WProcess = (WInfo * (TyleInfoYeuCau - 2.7));
		float WButton = 88;
		float HInfo = WindowH - 160;

		gInterface->DrawInfoBox((StartX + WindowW) - 110, InfoMocNapY + 15, 85, 125, 0x00000096, 0, 0);

		//g_pBCustomMenuInfo->DrawInfoBox((StartX + WindowW) - 110, InfoMocNapY + 160, 85, 105, 0x00000096, 0, 0);

 

		if ((GetTickCount() - gInterface->Data[eTickCount].EventTick) > 6000)
		{

			if (gInterface->DrawButton(StartX + 95, StartY + 185, 100, 16, "Thử Vận May", 100) && (GetTickCount() - gInterface->Data[eTickCount].EventTick) > 6000) //"Nhận"
			{
				XULY_CGPACKET_SOLAN pMsg;
				pMsg.header.set(0xD3, 0x8A, sizeof(pMsg));
				pMsg.ThaoTac = SelectTypeVQ; //
				pMsg.SoLan = 1;
				DataSend((LPBYTE)&pMsg, pMsg.header.size);
				gInterface->Data[eTickCount].EventTick = GetTickCount();
			}
		}
 

		float PosYCoinNhan = StartY + 185;
		gInterface->TextDraw((HFONT)g_hFontBold,  (StartX+ WindowW) - 105, PosYCoinNhan -150 - 5, 0xFFDE26FF, 0x3a4b3978, 90, 0, 3, "Yêu Cầu"); //"Yêu Cầu");
		gInterface->TextDraw((HFONT)g_hFontBold,  (StartX+ WindowW) - 105, PosYCoinNhan -150 + (15 * 1), 0xFF8214FF, 0x0, WindowW, 0, 1, "+WCoin : %s", NumberFormat(mDataListItemVongQuayClient.WCYC));			//Text3 = "+ WCoin : %s
		gInterface->TextDraw((HFONT)g_hFontBold,  (StartX+ WindowW) - 105, PosYCoinNhan -150 + (15 * 2), 0xFF8214FF, 0x0, WindowW, 0, 1, "+WCoinP :%s", NumberFormat(mDataListItemVongQuayClient.WPYC));		  //Text4 = "+ WCoinP : %
		gInterface->TextDraw((HFONT)g_hFontBold,  (StartX+ WindowW) - 105, PosYCoinNhan -150 + (15 * 3), 0xFF8214FF, 0x0, WindowW, 0, 1, "+GobinP :%s", NumberFormat(mDataListItemVongQuayClient.GPYC));		  //Text5 = "+ GobinP : %
  	

		int DataListItem = mDataListItemVongQuayClient.ListItemVongQuay.size();
		float Radius = 105.0f; // Set the radius of the circular layout
		float AngleStep = 2 * 3.14159265358979323846 / 12.0f; // Divide the circle into 12 equal angles
		float WBox = 30;
		int BBShowInfoItem = -1;
		DWORD boxColor = 0x00000096;
		unsigned int timeInterval2 = 10; // 5 seconds
		unsigned int currentTime2 = GetTickCount();
		unsigned int colorIndex2 = (currentTime2 / timeInterval2) % 2;


		if (gInterface->StartRollSau >= 1)
		{
			if (Chay >= 12)
			{
				Chay = 0;
				CountVong += 0.2;
			}

			if (CountVong <= 2.3)
			{
				Chay += CountVong;
			}
			else
			{
				Chay += CountVong * 0.9; // Giam toc do
			}


		}
		else
		{
			CountVong = 1;
		}

		for (int n = 0; n < DataListItem; n++)
		{
			ITEM* pItem = NULL;
			pItem = g_pNewItemMng->CreateItem(mDataListItemVongQuayClient.ListItemVongQuay[n].Item);

			if (NULL != pItem)
			{
				const ITEM_ATTRIBUTE* pItemAttr = &ItemAttribute[pItem->Type];

				float angle = n * AngleStep;
				float PosXBoxItem = StartX + 125 + Radius * cos(angle);
				float PosYBoxItem = StartY + 140 + Radius * sin(angle);

				if (gInterface->StartRollSau >= 1 && n == (int)Chay % 12)
				{
					boxColor = 0xD4966396; // Tô sáng ô đang quay
				}
				else if (gInterface->StartRollSau == 0 && n == gInterface->IndexItemSau)
				{
					boxColor = 0xD4966396; // Khi dừng, tô màu ô trúng
				}
				else
				{
					boxColor = 0x00000096;
				}

				gInterface->DrawInfoBox(PosXBoxItem, PosYBoxItem, WBox, WBox, boxColor, 0, 0);
				if (gInterface->StartRollSau == 0 && n == gInterface->IndexItemSau)
				{
				}
				g_pNewUISystem->RenderItem3DFree(
					PosXBoxItem, PosYBoxItem, WBox, WBox,
					mDataListItemVongQuayClient.ListItemVongQuay[n].Index,
					pItem->Level, pItem->Option1, pItem->ExtOption, 1,
					mDataListItemVongQuayClient.ListItemVongQuay[n].SizeBMD,
					0);
				if (SEASON3B::CheckMouseIn(PosXBoxItem, PosYBoxItem, WBox, WBox))
				{
					BBShowInfoItem = n;
				}
			}
			}

		if (SEASON3B::CheckMouseIn(InfoMocNapX + 112, StartY + 140, WBox, WBox))
		{
			BBShowInfoItem = gInterface->IndexItemSau;
		}
			
		//===Coin


		if (BBShowInfoItem != -1)
		{
			ITEM* CTItem = g_pNewItemMng->CreateItem(mDataListItemVongQuayClient.ListItemVongQuay[BBShowInfoItem].Item);
			RenderItemInfo(MouseX + 75, MouseY, CTItem, 0, 0, false, false);
		}

		glColor3f(1.0, 1.0, 1.0);
	}
	}
	
	
int CB_Interface::DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...)
{
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	//CBRenderText(Buff, PosX, PosY, Width, 0, Color, 0, Align);
	//return 1;

	int LineCount = 0;

	char* Line = strtok(Buff, "\n");

	while (Line != NULL)
	{
		CBRenderText(Line, PosX, PosY, Width, 0, Color, 0, Align);

		PosY += 10;
		Line = strtok(NULL, "\n");
	}

	return PosY;
}


char* CB_Interface::NumberFormat(int Number)
{


	if (Number < 0) { return "0"; }

	char OutPut[15];

	if (Number < 1000) {
		sprintf(OutPut, "%d", Number);
		return strdup(OutPut);
	}
	else if (Number < 1000000) {
		int part1 = Number % 1000;
		int part2 = (Number - part1) / 1000;
		sprintf(OutPut, "%d,%03d", part2, part1);
		return strdup(OutPut);
	}
	else if (Number < 1000000000) {
		int part1 = Number % 1000;
		Number = (Number - part1) / 1000;
		int part2 = Number % 1000;
		Number = (Number - part2) / 1000;
		int part3 = Number % 1000;

		sprintf(OutPut, "%d,%03d,%03d", part3, part2, part1);
		return strdup(OutPut);
	}
	else {
		int part1 = Number % 1000;
		Number = (Number - part1) / 1000;
		int part2 = Number % 1000;
		Number = (Number - part2) / 1000;
		int part3 = Number % 1000;
		int part4 = (Number - part3) / 1000;

		sprintf(OutPut, "%d,%03d,%03d,%03d", part4, part3, part2, part1);
		return strdup(OutPut);
	}
	return "0";
}


bool CB_Interface::DrawButtonGUI(int IDGUID, float PosX, float PosY, float SizeW, float SizeH, int TypeButton)
{
	EnableAlphaTest();
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	bool mreturn = false;
	//=====================Button 1

	if (TypeButton == 3)
	{
		if (SEASON3B::CheckMouseIn(PosX, PosY, SizeW, SizeH) == 1)
		{
			BLockMouseMoveClick = GetTickCount() + 500;

			if ((GetKeyState(VK_LBUTTON) & 0x8000) && GetTickCount() - gInterface->Data[eTIME].EventTick > 500 && !BLockButtonHover && !BLockMessageBox && !BLockButtonMoveWindow)
			{
				gInterface->Data[eTIME].EventTick = GetTickCount();
				PlayBuffer(25, 0, 0);
				mreturn = true;
				RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, SizeH * 2);
			}
			else {
				RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, SizeH * 1);
			}

		}
		else {
			RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, 0.0);
		}
	}
	else if (TypeButton == 2)
	{
		if (SEASON3B::CheckMouseIn(PosX, PosY, SizeW, SizeH) == 1)
		{
			BLockMouseMoveClick = GetTickCount() + 500;

			if ((GetKeyState(VK_LBUTTON) & 0x8000) && GetTickCount() - gInterface->Data[eTIME].EventTick > 500 && !BLockButtonHover && !BLockMessageBox && !BLockButtonMoveWindow)
			{
				gInterface->Data[eTIME].EventTick = GetTickCount();
				PlayBuffer(25, 0, 0);
				mreturn = true;
				RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, SizeH * 1);
			}
			else {
				RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, 0.0);
			}

		}
		else {
			RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0.0, 0.0);
		}
	}

	else
	{
		if (SEASON3B::CheckMouseIn(PosX, PosY, SizeW, SizeH) == 1)
		{
			BLockMouseMoveClick = GetTickCount() + 500;
			if (GetTickCount() - gInterface->Data[eTIME].EventTick > 500 && !BLockButtonHover && !BLockMessageBox && !BLockButtonMoveWindow) //Click
			{
				if ((GetKeyState(VK_LBUTTON) & 0x8000))
				{
					gInterface->Data[eTIME].EventTick = GetTickCount();
					PlayBuffer(25, 0, 0);
					mreturn = true;
				}
			}
			RenderImage(IDGUID, PosX, PosY, SizeW, SizeH, 0, 0, RGBA(255, 228, 107, 255));
		}
		else
		{
			RenderImage(IDGUID, PosX, PosY, SizeW, SizeH);
		}
	}
	glColor3f(1, 1, 1);
	EnableAlphaTest(false);
	return mreturn;
}



void CB_Interface::DrawInfoBox(float PosX, float PosY, float Width, float Height, DWORD bkcolor, int Arg6, int Arg7)
{
	float Red = (float)((bkcolor & 0xff000000) >> 24) / 255.0f;
	float Green = (float)((bkcolor & 0x00ff0000) >> 16) / 255.0f;
	float Blue = (float)((bkcolor & 0x0000ff00) >> 8) / 255.0f;
	float Alpha = (float)(bkcolor & 0x000000ff) / 255.0f;
	EnableAlphaTest();
	if (bkcolor)
	{
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


	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


	while (vertical || horizontal)
	{
		if (vertical)
		{
			IniciarX += 14.0f;

			if ((IniciarX + w) > LeftX)
			{
				vertical = false;
				w = LeftX - IniciarX;
			}
			RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_PIXEL, IniciarX, TopY, w, 14.0f); //Ngang

			RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_PIXEL, IniciarX, DownY, w, 14.0f); //Ngang 2
		}
		//-- abajo
		if (horizontal)
		{
			IniciarY += 14.0f;

			if ((IniciarY + H) > DownY)
			{
				horizontal = false;
				H = DownY - IniciarY;
			}

			RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_RIGHT_PIXEL, LeftX, IniciarY, 14.0f, H); //Doc 2

			RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_LEFT_PIXEL, RightX, IniciarY, 14.0f, H); // Doc 1


		}
	}

	RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_LEFT, RightX, TopY, 14.0f, 14.0f); //Goc tren ben trai

	RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_RIGHT, LeftX, TopY, 14.0f, 14.0f); //Goc tren ben phai

	RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_LEFT, RightX, DownY, 14.0f, 14.0f);  //Goc duoi trai

	RenderImage(CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_RIGHT, LeftX, DownY, 14.0f, 14.0f);  //Goc Duoi phai
	glColor3f(1, 1, 1);
	EnableAlphaTest(false);
}
bool CB_Interface::RenderCheckBox(float PosX, float PosY, DWORD TextColor, bool Checkis, LPCSTR Text, ...)
{
	bool mreturn = false;
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	if (Checkis)
	{
		RenderImage(CNewUIOptionWindow::IMAGE_OPTION_BTN_CHECK, PosX, PosY, 15.0, 15.0, 0.0, 0.0);
	}
	else
	{
		RenderImage(CNewUIOptionWindow::IMAGE_OPTION_BTN_CHECK, PosX, PosY, 15.0, 15.0, 0.0, 15.0);
	}
	TextDraw(g_hFont, PosX + 20, PosY + 3, TextColor, 0x0, 0, 0, 1, Text);

	if (SEASON3B::CheckMouseIn(PosX, PosY, 15, 15) == 1)
	{
		if ((GetKeyState(VK_LBUTTON) & 0x8000) && GetTickCount() - gInterface->Data[eTIME].EventTick > 500)
		{
			gInterface->Data[eTIME].EventTick = GetTickCount();
			PlayBuffer(25, 0, 0);
			BButtonClickTime = GetTickCount();
			mreturn = true;
		}
	}
	return mreturn;
}




bool CB_Interface::DrawSelectBar(int X, int Y, int W, int H, int* SelectNum, std::vector<std::string> Data)
{
	if (Data.empty()) return 0;
	//==31422 Select Bar
	static int ShowBarSelect = 0;
	int SelectBarHover = 0;
	DWORD SelectBarHoverBG = 0x0;
	float SelectBarY = Y;
	bool KQClick = false;
	RenderBitmap(CNewUIGuildMakeWindow::IMAGE_GUILDMAKE_EDITBOX, X - 2, Y + 37, W, H, 0.0, 0.0, 0.82, 0.71, 1, 1, 0.0);
	if (SEASON3B::CheckMouseIn(X - 2, Y + 37, W, H))
	{

		SelectBarHover = 1;
		SelectBarHoverBG = 0x66646450;
		if (GetTickCount() - gInterface->Data[eTIME].EventTick > 500) //Click
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				SelectBarHover = 2;
				gInterface->Data[eTIME].EventTick = GetTickCount();
				PlayBuffer(25, 0, 0);
				ShowBarSelect ^= 1;
			}
		}
		this->SetBlockCur(TRUE);
	}
	RenderBitmap(CNewUICastleWindow::IMAGE_CASTLEWINDOW_SCROLL_DOWN_BTN, X + 2, Y + 39.5, 13.5, 13.5, 0.0, 0.21 * SelectBarHover, 0.91, 0.21, 1, 1, 0.0);
	TextDraw(g_hFont, X + 15, Y + 41, 0xFFFFFFFF, SelectBarHoverBG, W - 25, 0, 3, Data[*SelectNum].c_str());// 
	if (ShowBarSelect)
	{
		int CountSelectBar = 1;
		gInterface->DrawBarForm(X + 15, Y + 41 + (15), W - 25, (H - 5) * (Data.size()), 0.0, 0.0, 0.0, 0.8);
		for (size_t i = 0; i < Data.size(); i++)
		{
			DWORD SelectHover = 0x0;
			if (SEASON3B::CheckMouseIn(X + 15, Y + 41 + (15 * CountSelectBar), W + 20, 15))
			{
				SelectHover = 0x66646450;
				if (GetTickCount() - gInterface->Data[eTIME].EventTick > 500) //Click
				{
					if (GetKeyState(VK_LBUTTON) & 0x8000)
					{
						*SelectNum = i;
						gInterface->Data[eTIME].EventTick = GetTickCount();
						PlayBuffer(25, 0, 0);
						ShowBarSelect ^= 1;
						KQClick = true;

					}
				}
				this->SetBlockCur(TRUE);
			}
			TextDraw(g_hFont, X + 15, Y + 41 + (15 * CountSelectBar), 0xFFFFFFFF, SelectHover, W - 25, 0, 3, Data[i].c_str());// 
			CountSelectBar++;
		}
	}
	return KQClick;
}




bool CB_Interface::RenderInputBox(float PosX, float PosY, float W, float H, char* TextSet, CUITextInputBox*& Input, UIOPTIONS UiOption, int MaxChar, bool isPass)
{
	// Input Box
	if (!Input)
	{
		Input = new CUITextInputBox;
		Input->Init(gwinhandle->GethWnd(), W, H, MaxChar, isPass);
		Input->SetPosition(PosX, PosY);
		/*Input->SetTextColor(255, 255, 230, 230);
		Input->SetBackColor(0, 0, 0, 25);*/
		Input->SetBackColor(0, 0, 0, 255);
		Input->SetTextColor(255, 189, 25, 255);
		Input->SetFont(g_hFont);
		Input->SetState(UISTATE_NORMAL);
		Input->SetOption(UiOption);
		Input->SetText(TextSet);
	}
	else
	{
		Input->Render();
		if (SEASON3B::CheckMouseIn(PosX - 3, PosY - 2.5, W + 6, H + 5) == 1)
		{

			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				Input->GiveFocus(1);
			}
		}
		return  1;
	}
	return 0;
}


void CB_Interface::DrawMessageBox()
{
	if (!gInterface->Data[eWindowMessageBox].OnShow)
	{
		return;
	}
	float CuaSoW = 220;
	float CuaSoH = 80 + (this->MsgLine * 10);

	float StartX = (MAX_WIN_WIDTH / 2) - (CuaSoW / 2);
	float StartY = 15.0;

	gInterface->gDrawWindowCustom(&StartX, &StartY, CuaSoW, CuaSoH, eWindowMessageBox, this->MsgBoxCaption.c_str());
	EnableAlphaTest(true);
	glColor3f(1.0, 1.0, 1.0);
	gInterface->SetBlockCur(TRUE);
	this->MsgLine = TextDraw((HFONT)g_hFont, StartX, StartY + 35, 0xFFFFFFFF, 0x0, CuaSoW, 0, 3, this->MsgBoxText.c_str());
	if (gInterface->Data[eWindowMessageBox].Height < (80 + (this->MsgLine * 10))) gInterface->Data[eWindowMessageBox].Height = (float)(80 + (this->MsgLine * 10));

	if (this->MsgBoxType == 0)
	{
		float BtnHeight = 20.0;
		float BtnWidth = 120.0;
		if (gInterface->DrawButton(StartX + CuaSoW / 2 - (BtnWidth / 2), StartY + CuaSoH - BtnHeight - 10, BtnWidth, 12, "Close"))
		{
			gInterface->Data[eWindowMessageBox].OnShow = 0;
		}
	}
	else if (this->MsgBoxType == 1)
	{
		float BtnHeight = 20.0;
		float BtnWidth = 55.0;
		if (gInterface->DrawButton(StartX + CuaSoW / 2 - (BtnWidth + 7.5), StartY + CuaSoH - BtnHeight - 10, BtnWidth, 12, "OK"))
		{
			this->MsgBoxCallback(this);
			this->MsgBoxCallback = 0;
			gInterface->Data[eWindowMessageBox].OnShow = 0;
		}

		if (gInterface->DrawButton(StartX + CuaSoW / 2 + (7.5), StartY + CuaSoH - BtnHeight - 10, BtnWidth, 12, "Close"))
		{
			gInterface->Data[eWindowMessageBox].OnShow = 0;
		}
	}
	DisableAlphaBlend();
	EnableAlphaTest(0);

}

void CB_Interface::OpenMessageBox(char* caption, char* Format, ...)
{
	if (gInterface->Data[eWindowMessageBox].OnShow)
	{
		return;
	}

	char text[1024] = { 0 };
	va_list va;
	va_start(va, Format);
	vsprintf_s(text, Format, va);
	va_end(va);
	this->MsgLine = 0;
	this->MsgBoxCaption.clear();
	this->MsgBoxText.clear();
	this->MsgBoxCaption = caption;
	this->MsgBoxText = text;

	this->MsgLine = 1;
	gInterface->Data[eWindowMessageBox].OnShow = true;
	this->MsgBoxType = 0;
}

void CB_Interface::OpenMessageBoxOkCancel(PUSHEVENT_CALLBACK_LPVOID pCallbackFunc, char* caption, char* Format, ...)
{
	if (gInterface->Data[eWindowMessageBox].OnShow)
	{
		return;
	}

	char text[1024] = { 0 };
	va_list va;
	va_start(va, Format);
	vsprintf_s(text, Format, va);
	va_end(va);

	this->MsgLine = 0;
	this->MsgBoxCaption.clear();
	this->MsgBoxText.clear();
	this->MsgBoxCaption = caption;
	this->MsgBoxText = Format;
	this->MsgLine = 1;
	gInterface->Data[eWindowMessageBox].OnShow = true;
	this->MsgBoxType = 1;
	this->MsgBoxCallback = pCallbackFunc;
}


std::string CB_Interface::generateCaptcha(int n)
{
	time_t t;
	srand((unsigned)time(&t));
	char* required_chars = "0123456789";
	std::string captcha = "";
	while (n--)
		captcha.push_back(required_chars[rand() % sizeof(required_chars)]);
	return captcha;
}



bool CB_Interface::ActionSendItemGS(ITEM* ItemSell, int Slot)
{
	if (ChangeSSendItem(ItemSell, Slot, GetKeyState(VK_RBUTTON))) return 1;
	return 0;
}



bool CB_Interface::check_Captcha(std::string& captcha, std::string& user_input)
{
	return captcha.compare(user_input) == 0;
}
void CB_Interface::RenderCaptchaNumber(float PosX, float PosY, CUITextInputBox* a6, LPCSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);
	// ----
	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);


	gInterface->DrawBarForm(PosX, PosY + 3.5, 120, 17, 0.0, 0.0, 0.0, 1.0);
	gInterface->DrawBarForm(PosX + 2, PosY + 4, 50, 15, 1.0, 0.2167, 0.0, 1.0);
	TextDraw((HFONT)g_hFontBold, PosX + 2, PosY + 4 + 2, 0xFFFFFFB8, 0x0, 50, 0, 3, Buff); //Ma Xac Nhan
	a6->SetPosition(PosX + 60, PosY + 6.5);
	a6->Render();

	if (SEASON3B::CheckMouseIn(PosX - 5, PosY - 5, 120, 30) == 1)
	{

		if (GetKeyState(VK_LBUTTON) & 0x8000)
		{
			a6->GiveFocus(1);
			PlayBuffer(25, 0, 0);
		}
	}
	//gCentral.DrawStringInput(PosX + 60, PosY + 5, 50, 13, 3, a6, 0x57575750, 0x0, 0xFFFFFFFF);

}


/*bool CB_Interface::NutHienThi()
{
	if (gInterface->CheckWindow(gInterface->Warehouse) //-- 8
		|| gInterface->CheckWindow(gInterface->Shop) //-- 12
		|| gInterface->CheckWindow(gInterface->ChaosBox) //-- 9
		|| gInterface->CheckWindow(gInterface->Trade) //-- 7
		|| gInterface->CheckWindow(gInterface->Store) //-- 14
		|| gInterface->CheckWindow(gInterface->OtherStore) //-- 15
		|| gInterface->CheckWindow(gInterface->LuckyCoin1) //-- 60
		|| gInterface->CheckWindow(gInterface->NPC_ChaosMix)
		|| gInterface->CheckWindow(gInterface->MoveList)
		|| gInterface->CheckWindow(gInterface->CashShop)
		|| gInterface->CheckWindow(gInterface->ChatWindow)
		|| gInterface->CheckWindow(SEASON3B::INTERFACE_LIST::INTERFACE_MASTER_LEVEL)

		)
	{
		return false;
	}

	float CuaSoW = 20;
	float CuaSoH = 20;
	float StartX = 67;
	float StartY = 27;

	//Nút HP
	if (SEASON3B::CheckMouseIn(StartX, StartY, CuaSoW, CuaSoH))
	{
		RenderTipText(StartX, StartY + 30, "Auto HP");
		//RenderTipText(CloseX + 40, CloseY + 15, "Close");
		if (AutoHP)
		{
			RenderBitmap(10104, StartX, StartY, CuaSoW, CuaSoH, 0.0f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
		else
		{
			RenderBitmap(10104, StartX, StartY, CuaSoW, CuaSoH, 0.0f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}

		if (GetTickCount() - gInterface->Data[eAutoHP].EventTick > 300)
		{
			gInterface->SetBlockCur(TRUE);
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				AutoHP = !AutoHP;


				gInterface->Data[eAutoHP].EventTick = GetTickCount();
			}
		}
	}
	else
	{
		if (AutoHP)
		{
			RenderBitmap(10104, StartX, StartY, CuaSoW, CuaSoH, 0.0f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
		else
		{
			RenderBitmap(10104, StartX, StartY, CuaSoW, CuaSoH, 0.0f / 256.f, 0.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
	}

	//Nút PK
	if (SEASON3B::CheckMouseIn(StartX + (22 * 1), StartY, CuaSoW, CuaSoH))
	{
		RenderTipText(StartX + (22 * 1), StartY + 30, "Auto PK");


		if (AutoCtrlPK)
		{
			RenderBitmap(10104, StartX + (22 * 1), StartY, CuaSoW, CuaSoH, 28.5f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
		else
		{
			RenderBitmap(10104, StartX + (22 * 1), StartY, CuaSoW, CuaSoH, 28.5f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}

		if (GetTickCount() - gInterface->Data[eAutoPK].EventTick > 300)
		{
			gInterface->SetBlockCur(TRUE);
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				AutoCtrlPK = !AutoCtrlPK;

				if (AutoCtrlPK)
				{
					gInterface->DrawMessage(2, "Ctrl PK (Bật)");
				}
				else
				{
					gInterface->DrawMessage(2, "Ctrl PK (Tắt)");
				}

				gInterface->Data[eAutoPK].EventTick = GetTickCount();

			}
		}
	}
	else
	{
		if (AutoCtrlPK)
		{
			RenderBitmap(10104, StartX + (22 * 1), StartY, CuaSoW, CuaSoH, 28.5f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
		else
		{
			RenderBitmap(10104, StartX + (22 * 1), StartY, CuaSoW, CuaSoH, 28.5f / 256.f, 0.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
	}
	// Nút Facebook
	static DWORD ClickTick = 0;


		if (SEASON3B::CheckMouseIn(StartX + (66 * 1), StartY, CuaSoW, CuaSoH))
		{
			RenderTipText(StartX + (60 * 1), StartY + 30, "Facebook");

			RenderBitmap(10104, StartX + (66 * 1), StartY, CuaSoW, CuaSoH, 86.5f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);

			if (GetTickCount() - ClickTick > 300)
			{
				gInterface->SetBlockCur(TRUE);
				if (GetKeyState(VK_LBUTTON) & 0x8000)
				{
					ShellExecuteA(NULL, "open", "https://www.facebook.com/Phongcom99", NULL, NULL, SW_SHOWNORMAL);

					ClickTick = GetTickCount();
				}
			}
		}
		else
		{
			RenderBitmap(10104, StartX + (66 * 1), StartY, CuaSoW, CuaSoH, 86.5f / 256.f, 0.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
	

		//Nút Zalo
		if (SEASON3B::CheckMouseIn(StartX + (88 * 1), StartY, CuaSoW, CuaSoH))
		{
			RenderTipText(StartX + (90 * 1), StartY + 30, "Zalo");

			RenderBitmap(10104, StartX + (88 * 1), StartY, CuaSoW, CuaSoH, 116.0f / 256.f, 29.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);

			if (GetTickCount() - ClickTick > 300)
			{
				gInterface->SetBlockCur(TRUE);
				if (GetKeyState(VK_LBUTTON) & 0x8000)
				{
					ShellExecuteA(NULL, "open", "https://www.facebook.com/Phongcom99", NULL, NULL, SW_SHOWNORMAL);

					ClickTick = GetTickCount();
				}
			}
		}
		else
		{
			RenderBitmap(10104, StartX + (88 * 1), StartY, CuaSoW, CuaSoH, 116.0f / 256.f, 0.f / 128.f, 30.f / 256.f, 29.f / 128.f, 1, 1, 0.0);
		}
	}*/


void CB_Interface::SetButtonInfo(CNewUIButton* m_Btn, int imgindex, int x, int y, int sx, int sy, bool overflg, bool isimgwidth, bool bClickEffect, bool MoveTxt, unicode::t_string btname, unicode::t_string tooltiptext, bool istoppos)
{
	m_Btn->ChangeButtonImgState(1, imgindex, overflg, isimgwidth, bClickEffect);
	m_Btn->ChangeButtonInfo(x, y, sx, sy);

	m_Btn->ChangeText(btname);
	m_Btn->ChangeToolTipText(tooltiptext, istoppos);

	if (MoveTxt)
	{
		m_Btn->MoveTextPos(0, -1);
	}
}

