#include "Stdafx.h"
#include "NewUIMocNap.h"
#include "WSclientinline.h"
#include "ZzzInventory.h"
#include "Custom/NewUIJewelBank.h"
#include "TextClien.h"

InfoClientRecvSendData g_dataclient;
ItemListClienRecvSend g_mocapclient;

SEASON3B::CNewUIScrollBar* ListMocNap = nullptr;
SEASON3B::CNewUIScrollBar* ListItemNhan = nullptr;

int MaxListItemInPage = 6;

#define LODWORD(qw) \
	((DWORD)(qw))
typedef unsigned __int64 SPK;




void RenderItemSlot(int X, int Y, int Doc, int Ngang, int Use)
{
	float v6;
	float v7;
	GLfloat red;
	GLfloat green;
	int j;
	int i;

	for (i = 0; i < Doc; ++i)
	{
		for (j = 0; j < Ngang; ++j)
		{
			EnableAlphaTest();
			glEnable(GL_ALPHA_TEST);
			glColor4f(0.3, 0.3, 0.3, 0.60000002);
			v7 = (float)(Y + 20 * i);
			v6 = (float)(X + 20 * j);
			RenderColor(LODWORD(v6), LODWORD(v7), 20.0, 20.0, 0.0, 0);
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0, 1.0, 1.0);
			green = (float)(Y + 20 * i);
			red = (float)(X + 20 * j);
			SEASON3B::RenderImage(BITMAP_INTERFACE_NEW_INVENTORY_BASE_BEGIN, LODWORD(red), LODWORD(green), 21.0, 21.0);
			glDisable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			DisableAlphaBlend();
			glColor3f(1.f, 1.f, 1.f);
		}
	}
}



SEASON3B::CNewUIMocNap::CNewUIMocNap()
{
	this->m_pNewUIMng = nullptr;
	this->m_Pos = { 0, 0 };
	ViewIndex = 0;
}



struct SendRequestAction_New
{
	PSBMSG_HEAD header;
	DWORD Action;
};

void KeySendStateMocNap(DWORD c)
{
	SendRequestAction_New pMsg{};
	pMsg.header.set(0xD3, 0x9A, sizeof(pMsg));
	pMsg.Action = g_dataclient.DanhSachMocNap[c].IndexMocNap;
	DataSend((BYTE*)&pMsg, pMsg.header.size);
}


#define SendRequestMocNap1(Action) \
{ \
    CStreamPacketEngine spe; \
    spe.Init( 0xC1, 0xD3); \
	spe << (BYTE)0x9B;\
	spe << (int)Action;\
    spe.Send(); \
}

void SEASON3B::CNewUIMocNap::KeySendStateViewMocNap(DWORD c)
{
	ViewIndex = c;
	SendRequestAction_New pMsg{};
	pMsg.header.set(0xD3, 0x9B, sizeof(pMsg));
	pMsg.Action = g_dataclient.DanhSachMocNap[c].IndexMocNap;
	DataSend((BYTE*)&pMsg, pMsg.header.size);
}





void KeySendStateOpenMocNap(DWORD c)
{
	SendRequestAction_New pMsg{};
	pMsg.header.set(0xD3, 0x9C, sizeof(pMsg));
	pMsg.Action = 1;
	DataSend((LPBYTE)&pMsg, pMsg.header.size);
}


SEASON3B::CNewUIMocNap::~CNewUIMocNap()
{
	Release();
}

bool SEASON3B::CNewUIMocNap::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MOCNAP, this);

	SetPos(x, y);

	Show(false);

	return true;
}

void SEASON3B::CNewUIMocNap::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIMocNap::SetPos(int x, int y)
{
	m_Pos.x = 0;
	m_Pos.y = 0;
}

bool SEASON3B::CNewUIMocNap::Update()
{
	return true;
}

bool SEASON3B::CNewUIMocNap::UpdateMouseEvent()
{
	if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn((m_Pos.x + WINDOW_WIDTH) - 40, m_Pos.y + 5, 16, 16))
	{
		g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_MOCNAP);
	}

	if (CheckMouseIn(m_Pos.x, m_Pos.y, WINDOW_WIDTH - 10, WINDOW_HEIGHT + 70))
		return false;

	return true;
}

bool SEASON3B::CNewUIMocNap::UpdateKeyEvent()
{
	// ✅ Toggle F6 mở/tắt UI
	if (SEASON3B::IsRelease(VK_F6))
	{
		bool onShow = gInterface->Data[eWindowMocNap].OnShow;


		if (!gInterface->CheckWindow(CB_Interface::Inventory))
		{
			g_pNewUISystem->Show(SEASON3B::INTERFACE_INVENTORY);
		}

		if (onShow == false)
		{
			gInterface->Data[eWindowMocNap].OnShow = true;
			KeySendStateOpenMocNap(0);
			KeySendStateViewMocNap(1);
			PlayBuffer(SOUND_CLICK01);
		}
		else
		{
			gInterface->Data[eWindowMocNap].OnShow = false;
			PlayBuffer(SOUND_CLICK01);
		}

		return false; // chỉ chặn F6
	}

	// ✅ ESC tắt UI nếu đang mở
	if (gInterface->Data[eWindowMocNap].OnShow)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			gInterface->Data[eWindowMocNap].OnShow = false;
			PlayBuffer(SOUND_CLICK01);
			return false; // chặn ESC chỉ khi UI Mốc Nạp mở
		}
	}

	// ✅ nếu UI đang tắt → không đụng vào ESC, cho hệ thống dùng
	return true;
}

//bool SEASON3B::CNewUIMocNap::UpdateKeyEvent()
//{
//	if (IsVisible())
//	{
//		if (SEASON3B::IsPress(VK_ESCAPE) == true)
//		{
//			g_pNewUIMenuOption->Hide(SEASON3B::INTERFACE_MOCNAP);
//			PlayBuffer(SOUND_CLICK01);
//
//			return false;
//		}
//	}
//	return true;
//}

float SEASON3B::CNewUIMocNap::GetLayerDepth()
{
	return 3.4;
}

float SEASON3B::CNewUIMocNap::GetKeyEventOrder()
{
	return 3.4;
}


void SEASON3B::CNewUIMocNap::Show(bool bShow)
{
	gInterface->Data[eWindowMocNap].OnShow = bShow;
}

bool SEASON3B::CNewUIMocNap::IsVisible() const
{
	return gInterface->Data[eWindowMocNap].OnShow;
}

bool SEASON3B::CNewUIMocNap::Render()
{



	//if (SEASON3B::IsRelease(VK_F6))
	//{
	//	//Enable ^= 1;
	//	CreateNotice("Camera3D : Bật", 1);

	//	if (gInterface->Data[eWindowMocNap].OnShow == false)
	//	{
	//		gInterface->Data[eWindowMocNap].OnShow = true;
	//	}
	//	else
	//	{
	//		gInterface->Data[eWindowMocNap].OnShow = false;
	//	}
	//}


	if (gInterface->Data[eWindowMocNap].OnShow == false)
	{
		return true;
	}

	gInterface->Data[eWindowMocNap].OnShow = true;

	//EnableAlphaTest();
	//glColor4f(1.f, 1.f, 1.f, 1.f);


	//float StartX = StartX;
	//float StartY = StartY;


	float WindowW = 480;
	float WindowH = 250;

	float StartX = (WindowWidth / 2) - (WindowW / 2) - 200;
	float StartY = ((WindowHeight - 51) / 2) - (WindowH / 2) - 50;

	gInterface->gDrawWindowCustom2(&StartX, &StartY, WindowW, WindowH, eWindowMocNap, "Mốc Nạp");

	int TongSoDaNap = g_dataclient.TongNap;
	int MocDaNhan = g_dataclient.NhanMocNap;

	//g_pUIForm->RenderBack(StartX + 8, StartY + 35, int(WINDOW_WIDTH - 33), 20);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 255, 255, 0xFFu);
	char strText[100];
	sprintf_s(strText, gTextClien.TextClien_Khac[10]);
	g_pRenderText->RenderText(StartX + 270, StartY + 30, strText, 190, 0, 3, 0);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 255, 255, 0xFFu);
	sprintf_s(strText, gTextClien.TextClien_Khac[11]);
	g_pRenderText->RenderText(StartX + 270, StartY + 40, strText, 190, 0, 3, 0);


	char szTemp[256];
	wsprintf(szTemp, "Đã Nạp %d", TongSoDaNap);
	TextDraw(g_hFontBold, StartX + 200, StartY + 39, 0xFFFFFFA8, 0x0, 64, 0, 3, szTemp);




	//g_pUIForm->RenderBack(StartX + 8, StartY + 55, int(WINDOW_WIDTH - 300), 150);
	//g_pUIForm->RenderBack(StartX + 186, StartY + 55, int(WINDOW_WIDTH - 374), 150);
	//g_pUIForm->RenderBack(StartX + 292, StartY + 55, int(WINDOW_WIDTH - 317), 150);

	RenderBitmap(BITMAP_QR, StartX + 302, StartY + 57, 150, 150, 0.0, 0.0, 0.63, 0.63, 1, 1, 0);




	int DataListMocNap = g_dataclient.DanhSachMocNap.size();
	int MaxLineData = 10;

	if (ListMocNap == NULL)
	{
		ListMocNap = new CNewUIScrollBar();
		ListMocNap->Create(StartX + 175, StartY + 57, 141);
	}
	if (ListMocNap)
	{
		int maxPos = (DataListMocNap > MaxLineData) ? (DataListMocNap - MaxLineData) : 0;
		ListMocNap->SetMaxPos(maxPos);

		ListMocNap->MouseWheelWindow = CheckMouseIn(StartX, StartY + 60, 170, 141);

		ListMocNap->Render();
		ListMocNap->UpdateMouseEvent();
		ListMocNap->Update();


		// ✅ Thêm dòng này:
		ListMocNap->UpdateWheelEvent();



	}

	int MixItemListPage = ListMocNap->GetCurPos();
	int MaxList = 0;

	if (g_dataclient.DanhSachMocNap.size() == 0)
	{

		g_pRenderText->RenderText(StartX, StartY + 10, "Chưa có mốc nạp !", 270, 0, RT3_SORT_CENTER);
	}
	else
	{
		for (int i = MixItemListPage; i < static_cast<int>(g_dataclient.DanhSachMocNap.size()); i++)
		{
			if (MaxList >= MaxLineData)
				break;

			float TyleNap = (96.0 * TongSoDaNap) / g_dataclient.DanhSachMocNap[i].GiaTriNap;
			if (TyleNap > 96.0)
			{
				TyleNap = 96.0;
			}
			double Tyle = (static_cast<double>(TongSoDaNap) / g_dataclient.DanhSachMocNap[i].GiaTriNap) * 100;

			int PosY1 = StartY + 58 + (MaxList * 14.3);
			int PosY2 = StartY + 60 + (MaxList * 14.3);

			//IsToolKit.ThisFont(StartX + 12, PosY1, 0, 0x000000FF, 160, 12, 1, " ");
			//IsToolKit.ThisFont(StartX + 12, PosY1, 0, 0xCA0E0E69, TyleNap, 12, 1, " ");


			TextDraw(g_hFontBold, StartX + 12, PosY1, 0, 0x000000FF, 160, 12, 1, " ");

			TextDraw(g_hFontBold, StartX + 12, PosY1, 0, 0xCA0E0E69, TyleNap, 12, 1, " ");



			if (CheckMouseIn(StartX + 12, PosY1, 96, 12) == 1)
			{
				//IsToolKit.ThisFont(StartX + 12, PosY1, 0, 0x0064B4FF, 96, 12, 1, " ");

				TextDraw(g_hFontBold, StartX + 12, PosY1, 0, 0x0064B4FF, 96, 12, 1, " ");

				if (SEASON3B::IsRelease(VK_LBUTTON))
				{

					SendRequestMocNap1(i+1);
					//KeySendStateViewMocNap(i);
				}
			}


	
			TextDraw(g_hFontBold, StartX - 80, PosY2 - 2, 0xFFFFFFA8, 0x0, 270, 0, 3, "Mốc %02d: %d", g_dataclient.DanhSachMocNap[i].IndexMocNap, g_dataclient.DanhSachMocNap[i].GiaTriNap);




			if (MocDaNhan < g_dataclient.DanhSachMocNap[i].IndexMocNap)
			{
				if (CheckMouseIn(StartX + 108, PosY2, 64, 12) == 1)
				{
					//IsToolKit.ThisFont(StartX + 108, PosY1, 0, 0x0064B4FF, 64, 12, 1, " ");

					TextDraw(g_hFontBold, StartX + 108, PosY1, 0, 0x0064B4FF, 64, 12, 1, " ");

					if (SEASON3B::IsRelease(VK_LBUTTON) && Tyle >= 100)
					{
						KeySendStateMocNap(i);
					}
				}

				if (Tyle < 100)
				{
					//IsToolKit.ThisFont(StartX + 108, PosY2, HEX_COLOR_WHITE, 0, 64, 0, 3, gConfig.MocNap[5], Tyle);

					TextDraw(g_hFontBold, StartX + 108, PosY2, 0xFFFFFFA8, 0x0, 64, 0, 3, "%0.1f%%", Tyle);
				}
				else
				{
					//IsToolKit.ThisFont(StartX + 108, PosY2, HEX_COLOR_WHITE, 0, 64, 0, 3, gConfig.MocNap[16]);
					TextDraw(g_hFontBold, StartX + 108, PosY2, 0xFFFFFFA8, 0x0, 64, 0, 3, "NHẬN NGAY");

				}
			}
			else
			{
				//IsToolKit.ThisFont(StartX + 108, PosY2, HEX_COLOR_WHITE, 0, 64, 0, 3, gConfig.MocNap[7]);

				TextDraw(g_hFontBold, StartX + 108, PosY2, 0xFFFFFFA8, 0x0, 64, 0, 3, "ĐÃ NHẬN");

			}

			MaxList++;
			if (MaxList >= MaxLineData) break;
		}
	}


	int DataListItem = g_mocapclient.g_listmocnap.size();
	int MaxLineRecv = 6;

	if (ListItemNhan == NULL)
	{
		ListItemNhan = new CNewUIScrollBar();
		ListItemNhan->Create(StartX + 280, StartY + 57, 141);
	}
	if (ListItemNhan)
	{
		int maxPos = (DataListItem > MaxLineRecv) ? (DataListItem - MaxLineRecv) : 0;
		ListItemNhan->SetMaxPos(maxPos);

		ListItemNhan->MouseWheelWindow = CheckMouseIn(StartX + 190, StartY + 60, 90, 141);
		ListItemNhan->Render();
		ListItemNhan->UpdateMouseEvent();
		ListItemNhan->Update();


		// ✅ Thêm dòng này:
		ListItemNhan->UpdateWheelEvent();
	}

	float PosXBoxItem = StartX + 190;
	float PosYBoxItem = StartY + 55;

	float KhoangCach = 40;
	float PosXBoxItemGoc = StartX + 190;
	int CountNgang = 0;
	int CountDoc = 0;
	int ItemListPage = ListItemNhan->GetCurPos();
	int RenderInfoItem = -1;

	for (int n = ItemListPage; n < static_cast<int>(g_mocapclient.g_listmocnap.size()); n++)
	{
		if (CheckMouseIn(PosXBoxItem + 4, PosYBoxItem + 6, 40, 40) == 1)
		{
			//IsToolKit.ThisFont(PosXBoxItem + 4, PosYBoxItem + 6, 0, 0x00A0E8FF, 40, 40, 1, " ");

			//TextDraw(g_hFontBold, PosXBoxItem + 4, PosYBoxItem + 6, 0, 0x00A0E8FF, 40, 40, 1, " ");
		}

		RenderItemSlot(PosXBoxItem + 3, PosYBoxItem + 5, 2, 2, 0);

		g_pNewUISystem->RenderItem3DFree(PosXBoxItem + 8, PosYBoxItem,30,30,g_mocapclient.g_listmocnap[n].Index,	g_mocapclient.g_listmocnap[n].Item.Level,	g_mocapclient.g_listmocnap[n].Item.Option1,		g_mocapclient.g_listmocnap[n].Item.ExtOption,		false, g_mocapclient.g_listmocnap[n].SizeBMD, true);

		if (CheckMouseIn(PosXBoxItem + 4, PosYBoxItem + 6, 40, 40) == 1)
		{
			RenderInfoItem = n;
		}

		PosXBoxItem += KhoangCach;
		CountDoc++;
		CountNgang++;

		// ✅ đổi từ 2 → 4 để có 4 vật phẩm trên 1 hàng
		if (CountNgang >= 2)
		{
			PosXBoxItem = PosXBoxItemGoc;
			PosYBoxItem += KhoangCach;
			CountNgang = 0;
		}

		if (CountDoc >= MaxListItemInPage) break;
	}

	if (RenderInfoItem != -1)
	{
		ITEM tempItem;
		memset(&tempItem, 0, sizeof(ITEM));
		memcpy(&tempItem, &g_mocapclient.g_listmocnap[RenderInfoItem].Item, sizeof(tempItem));




		RenderItemInfo(MouseX, MouseY, &tempItem, false, 0, false);

	
	}

	//EnableAlphaTest();
	//glColor4f(1.f, 1.f, 1.f, 1.f);

	//DisableAlphaBlend();
	return true;
}

void SEASON3B::CNewUIMocNap::WsClientSend9A(const BYTE* SPK)
{
	g_dataclient.Clear();
	auto* mRecv = (WsClientMocNap*)SPK;
	g_dataclient.NhanMocNap = mRecv->NhanMocNap;
	g_dataclient.TongNap = mRecv->TongNap;
	for (int i = 0; i < mRecv->count; i++)
	{
		ListMocNapSend lpInfo = *(ListMocNapSend*)(((BYTE*)SPK) + sizeof(WsClientMocNap) + (sizeof(ListMocNapSend) * i));
		g_dataclient.DanhSachMocNap.push_back(lpInfo);
	}
}

void SEASON3B::CNewUIMocNap::WsClientSend9B(BYTE* SPK)
{
	g_mocapclient.Clear();
	auto mRecv = reinterpret_cast<ClientListItemSend*>(SPK);
	g_mocapclient.WC = mRecv->WC;
	g_mocapclient.WP = mRecv->WP;
	g_mocapclient.GP = mRecv->GP;

	for (int i = 0; i < mRecv->count; i++)
	{
		auto lpInfo = reinterpret_cast<InfoListMocNapSend*>(reinterpret_cast<BYTE*>(SPK) + sizeof(ClientListItemSend) + (sizeof(InfoListMocNapSend) * i));

		InfoItemLocalSend infoItemLocal = { 0 };
		infoItemLocal.Count = lpInfo->Count;
		infoItemLocal.SizeBMD = lpInfo->SizeBMD;
		infoItemLocal.Index = lpInfo->Index;
		infoItemLocal.GetTypeItem = lpInfo->GetTypeItem;

		auto item = g_pNewItemMng->CreateItem(lpInfo->Item);
		if (item)
		{
			memcpy(&infoItemLocal.Item, item, sizeof(ITEM));
			infoItemLocal.Item.Durability = lpInfo->Dur;

			if (lpInfo->PeriodTime)
			{
				infoItemLocal.Item.bPeriodItem = 1;
				infoItemLocal.Item.lExpireTime = lpInfo->PeriodTime;
			}
			g_mocapclient.g_listmocnap.push_back(infoItemLocal);
		}
	}
}

void SEASON3B::CNewUIMocNap::IsVisibleWindow()
{
	Show(true);
	KeySendStateOpenMocNap(0);
	KeySendStateViewMocNap(1);
}