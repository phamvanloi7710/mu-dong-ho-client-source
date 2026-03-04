#include "Stdafx.h"
#include "NewUIRenderNumber.h"
#include "wsclientinline.h"
#include "Custom/NewUIJewelBank.h"

CNewUIJewelBank gJwBanking;

#define MaxY 20


using namespace SEASON3B;


CNewUIJewelBank* CNewUIJewelBank::Instance()
{
	static CNewUIJewelBank s_Instance;
	return &s_Instance;
}


CNewUIJewelBank::CNewUIJewelBank()
{


	//for (int SPK = 0; SPK < 10; SPK++)
	//{
	//	gJwBanking.m_Data.ItemBank[SPK] = 0;
	//}

	gJwBanking.MaxPosition.x = (static_cast<float>((640 - 270)) / 2) ;
	gJwBanking.MaxPosition.y = 0;
	gJwBanking.CurPosition.x = MouseX;
	gJwBanking.CurPosition.y = MouseY;
	gJwBanking.DragItem = false;
	Enable = false;


	D_X = 0;
	D_Y = 0;

}

CNewUIJewelBank::~CNewUIJewelBank()
{

}



void CNewUIJewelBank::GCCustomBankInfoRecv(const BYTE* lpMsg)
{
	auto DataSPK = reinterpret_cast<const PSBMSG_JEWELBANK_RECV*>(lpMsg);

	for (auto SPK = 0; SPK < 10; ++SPK)
	{
		this->m_Data.ItemBank[SPK] = DataSPK->ItemBank[SPK];
	}

	this->JewelEnable = DataSPK->JewelEnable;
	this->JewelMaxCount = DataSPK->JewelMaxCount;

}


void CNewUIJewelBank::Render3DItem()
{
	EndBitmap();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport2(0, 0, WindowWidth, WindowHeight);
	gluPerspective2(1.f, (float)(WindowWidth) / (float)(WindowHeight), RENDER_ITEMVIEW_NEAR, RENDER_ITEMVIEW_FAR);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	GetOpenGLMatrix(CameraMatrix);
	#ifdef USING_SHADER_330
		OGL330MODEL::UpdateCameraMatrix();
	#endif
	EnableDepthTest();
	EnableDepthMask();

	float x = + 80 + 80 - 150;
	float y = + 110; //-- DI chuyển UI Bank lên xuống

	struct ItemRenderInfo
	{
		float offsetX;
		float offsetY;
		int itemType;
	};

	static const ItemRenderInfo itemRenderList[] =
	{
		{ 83.0f,  37.0f,  GET_ITEM(14, 13) },
		{ 83.0f,  56.0f,  GET_ITEM(14, 14) },
		{ 84.7f,  76.0f,  GET_ITEM(14, 16) },
		{ 83.0f,  97.0f,  GET_ITEM(14, 22) },
		{ 83.5f, 118.0f,  GET_ITEM(14, 31) },
		{ 83.0f, 138.0f,  GET_ITEM(14, 41) },
		{ 83.0f, 158.0f,  GET_ITEM(14, 42) },
		{ 83.0f, 176.0f,  GET_ITEM(12, 15) },
		{ 83.0f, 198.0f,  GET_ITEM(14, 43) },
		{ 83.0f, 219.0f,  GET_ITEM(14, 44) }
	};

	for (const auto& item : itemRenderList)
	{
		RenderItem3D(x + item.offsetX, y + item.offsetY, 20, 20, item.itemType, 0, 0, 0, false);
	}

	UpdateMousePositionn();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	BeginBitmap();
	

	// --- FIX PNG BLACK BUG ---
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	// --- END FIX ---
}

bool IsWorkZone(float x, float y, float h, float w)
{
	return MouseX >= x && MouseX <= x + h && MouseY >= y && MouseY <= y + w;
}
void CNewUIJewelBank::Render()
{


	//if (SEASON3B::IsRelease('J') )
	//{
	//	//Enable ^= 1;

	//	if (gInterface->Data[eWindowNganHang].OnShow == false)
	//	{
	//		gInterface->Data[eWindowNganHang].OnShow = true;
	//	}
	//	else
	//	{
	//		gInterface->Data[eWindowNganHang].OnShow = false;
	//	}
	//}


	if (SEASON3B::IsRelease('J'))
	{
		if (gInterface->Data[eWindowNganHang].OnShow == false)
		{
			gInterface->Data[eWindowNganHang].OnShow = true;

			// ✅ Tự mở hòm đồ nếu đang tắt
			if (!gInterface->CheckWindow(CB_Interface::Inventory))
			{
				g_pNewUISystem->Show(SEASON3B::INTERFACE_INVENTORY);
			}
		}
		else
		{
			gInterface->Data[eWindowNganHang].OnShow = false;
		}

		PlayBuffer(SOUND_CLICK01);
	}

	//if(!Enable)
	//{
	//	return;
	//}

	if(gInterface->Data[eWindowNganHang].OnShow == false)
	{
		return;
	}

	//EnableAlphaTest();
	//glColor4f(1.f, 1.f, 1.f, 1.f);




	if (GetKeyState(VK_RBUTTON) & 0x8000 && GetTickCount() > gJwBanking.ActionTime + 1000)
	{
		if (IsWorkZone(gJwBanking.MaxPosition.x, gJwBanking.MaxPosition.y, 270, 270))
		{
			if (!gJwBanking.DragItem)
			{
				gJwBanking.DragItem = true;
				gJwBanking.CurPosition.x = MouseX;
				gJwBanking.CurPosition.y = MouseY;
			}
		}
		if (gJwBanking.DragItem)
		{
			gJwBanking.MaxPosition.x += MouseX - gJwBanking.CurPosition.x;
			gJwBanking.MaxPosition.y += MouseY - gJwBanking.CurPosition.y;
			gJwBanking.CurPosition.x = MouseX;
			gJwBanking.CurPosition.y = MouseY;
		}
	}
	else
	{
		if (gJwBanking.DragItem)
		{
			gJwBanking.DragItem = false;
		}
	}

	if (gJwBanking.MaxPosition.x  < 0)		gJwBanking.MaxPosition.x = -0;
	if (gJwBanking.MaxPosition.y < 0)		gJwBanking.MaxPosition.y = 0;

	float MaxCalc = 477.0f;
	float MaxPos = MaxCalc;

	/*
	if (g_pNewUISystem->IsVisible(INTERFACE_INVENTORY))
	{
		MaxPos = (MaxCalc - 800) - 1;
	}

	if (g_pNewUISystem->IsVisible(INTERFACE_MYSHOP_INVENTORY))
	{
		MaxPos = (MaxCalc - (800 * 2)) - 1;
	}

	if (g_pNewUISystem->IsVisible(INTERFACE_INVENTORY)  && g_pNewUISystem->IsVisible(INTERFACE_MYSHOP_INVENTORY))
	{
		MaxPos = (MaxCalc - (800 * 3)) - 1;
	}
	*/

	if (gJwBanking.MaxPosition.x > MaxPos)	gJwBanking.MaxPosition.x = MaxPos;
	if (gJwBanking.MaxPosition.y > 144)		gJwBanking.MaxPosition.y = 144;

	D_X = gJwBanking.MaxPosition.x + 50 + 80 -150;
	D_Y = gJwBanking.MaxPosition.y + 110; //-- DI chuyển UI Bank lên xuống

	float CangChieuDai = 24;


	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(0xFF00B4FF);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->RenderText(D_X, D_Y + 10, GlobalText[3769], 270, 0, RT3_SORT_CENTER);



	float MainWidth = 370;
	float MainHeight = 315;
	float StartY = 100; //-- DI chuyển UI Bank lên xuống
	float StartX =140 + 80 - 150;

	gInterface->Data[eWindowNganHang].OnShow = true;

	gInterface->gDrawWindowCustom2(&StartX, &StartY, MainWidth, MainHeight, eWindowNganHang, "Ngan Hang Ngoc");


	g_pRenderText->SetTextColor(0xFF00B4FF);

	char ItemCount[255], ItemName[255], GuiAll[255];
	int TypeIDE[10] = { 0, 1, 2, 3, 4 };
	int Counter[5] = { 1, 10, 20, 30, 99 };
	int GetPosX = 152;
	int GetPosY = 21;
	sprintf(GuiAll, "%s", GlobalText[3780]);


	for (int i = 0; i < 10; ++i)
	{
		sprintf(ItemCount, "%d",gJwBanking.m_Data.ItemBank[i]);
		sprintf(ItemName, "%s", GlobalText[3770 + i]);

		g_pRenderText->RenderText(D_X - 110, D_Y + 44 + (i * MaxY), ItemName, 190, 0, RT3_SORT_RIGHT);

		//Nen 
		RenderImage(BITMAP_BOX_BANK, D_X + 105, D_Y + 40 + (i * MaxY),  float(32), float(16));

		g_pRenderText->RenderText(D_X - 57, D_Y + 44 + (i * MaxY), ItemCount, 190, 0, RT3_SORT_RIGHT);





		int posxx = 0;
		int posyy = i;
		for (int m = 0; m < 5; m++)
		{
			if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn(D_X + (GetPosX - 8) + (GetPosY * m), D_Y + 40 + (MaxY * i), 16, 16))
			{
				SendRequestCustomJewelBank(i, Counter[m]);
			}
		}

		if (i < 10)
		{
			for (int j = (i * 5); j < (i * 5) + 5; j++)
			{
				RenderImage(BITMAP_BOX_BANK, D_X + (GetPosX - 8) + (GetPosY * posxx), D_Y + 40 + (MaxY * posyy), float(16), float(16));

				if (posxx == 0) g_RenderNumber->RenderNumber(D_X + GetPosX + (GetPosY * posxx), D_Y + 44 + (MaxY * posyy), 1, 0.78);
				if (posxx == 1) g_RenderNumber->RenderNumber(D_X + GetPosX + (GetPosY * posxx), D_Y + 44 + (MaxY * posyy), 1 * 10, 0.78);
				if (posxx == 2) g_RenderNumber->RenderNumber(D_X + GetPosX + (GetPosY * posxx), D_Y + 44 + (MaxY * posyy), 1 * 20, 0.78);
				if (posxx == 3) g_RenderNumber->RenderNumber(D_X + GetPosX + (GetPosY * posxx), D_Y + 44 + (MaxY * posyy), 1 * 30, 0.78);
				if (posxx == 4) g_RenderNumber->RenderNumber(D_X + GetPosX + (GetPosY * posxx), D_Y + 44 + (MaxY * posyy), gJwBanking.JewelMaxCount, 0.78);
				posxx++;
			}
		}
	}

	//IsToolKit.ThisFont(D_X + 30, D_Y + 255, 0x00A2E8FF, 0, 190, 0, RT3_SORT_LEFT, GuiAll);
	TextDraw(g_hFontBold, D_X + 30, D_Y + 255, 0x00A2E8FF, 0x0, 190, 0, RT3_SORT_LEFT, GuiAll);


	// --- ESC Close ---
	if (SEASON3B::IsPress(VK_ESCAPE))
	{
		if (gInterface->Data[eWindowNganHang].OnShow)
		{
			gInterface->Data[eWindowNganHang].OnShow = false;
			PlayBuffer(SOUND_CLICK01);
			return;
		}
	}

	Render3DItem();


	//DisableAlphaBlend();

	//return;
}

//bool CNewUIJewelBank::UpdateKeyEvent()
//{
//	if (gInterface->Data[eWindowNganHang].OnShow)
//	{
//		if (SEASON3B::IsPress(VK_ESCAPE))
//		{
//			gInterface->Data[eWindowNganHang].OnShow = false;
//			PlayBuffer(SOUND_CLICK01);
//			return false;
//		}
//	}
//	return true;
//}