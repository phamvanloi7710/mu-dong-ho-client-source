#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "NewUIMessageBox.h"
#include "NewUIMyInventory.h"
#include "NewUIGuildInfoWindow.h"
#include "NewUIJewelBank.h"
namespace SEASON3B
{
	class CNewUIMocNap : public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{

			//IMAGE_BASE_WINDOW_BACK = CNewUIJewelBank::IMAGE_BASE_WINDOW_BACK,				//. newui_msgbox_back.jpg
			//IMAGE_BASE_WINDOW_TOP,
			//IMAGE_BASE_WINDOW_BOTTOM,
			//IMAGE_BASE_WINDOW_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,			//. newui_item_back02-l.tga	(21,320)
			//IMAGE_BASE_WINDOW_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,		//. newui_item_back02-r.tga	(21,320)
			//IMAGE_BASE_WINDOW_BTN_EXIT = CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN,		//. newui_exit_00.tga
			//IMAGE_WINDOW_TAB_BTN = CNewUIGuildInfoWindow::IMAGE_GUILDINFO_TAB_BUTTON,
			//
			//IMAGE_TABLE_TOP_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_LEFT,			//. newui_item_table01(L).tga (14,14)
			//IMAGE_TABLE_TOP_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_RIGHT,		//. newui_item_table01(R).tga (14,14)
			//IMAGE_TABLE_BOTTOM_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_LEFT,	//. newui_item_table02(L).tga (14,14)
			//IMAGE_TABLE_BOTTOM_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_RIGHT,	//. newui_item_table02(R).tga (14,14)
			//IMAGE_TABLE_TOP_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_PIXEL,		//. newui_item_table03(up).tga (1, 14)
			//IMAGE_TABLE_BOTTOM_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_PIXEL,	//. newui_item_table03(dw).tga (1,14)
			//IMAGE_TABLE_LEFT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_LEFT_PIXEL,		//. newui_item_table03(L).tga (14,1)
			//IMAGE_TABLE_RIGHT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_RIGHT_PIXEL,	//. newui_item_table03(R).tga (14,1)
			//
			//IMAGE_IGS_BUTTON = BITMAP_IGS_MSGBOX_BUTTON,//BITMAP_HERO_POSITION_INFO_BEGIN + 10,
		};

		enum PARTY_WINDOW_SIZE
		{
			WINDOW_WIDTH = 480,
			WINDOW_HEIGHT = 150,
		};

		CNewUIManager* m_pNewUIMng;
		POINT					m_Pos;

		CNewUIMocNap();
		virtual ~CNewUIMocNap();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();

		void Show(bool bShow);
		bool IsVisible() const;

		bool Render();
		bool Update();
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();

		float GetLayerDepth();
		float GetKeyEventOrder();

		void SetPos(int x, int y);
		BYTE ViewIndex;
		void KeySendStateViewMocNap(DWORD c);
		void IsVisibleWindow();
		void WsClientSend9A(const BYTE* SPK);
		void WsClientSend9B(BYTE* SPK);

	};

}




struct WsClientMocNap
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
	int NhanMocNap;
	int TongNap;
};

struct ListMocNapSend
{
	int IndexMocNap;
	int GiaTriNap;
};
struct ClientListItemSend
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
	int WC;
	int WP;
	int GP;
};
struct InfoListMocNapSend
{
	float SizeBMD;
	int Count;
	short Index;
	BYTE GetTypeItem;
	BYTE Dur;
	BYTE Item[12];
	int  PeriodTime;
};
//==Struct Client
struct InfoItemLocalSend
{
	float SizeBMD;
	int Count;
	short Index;
	BYTE GetTypeItem;
	ITEM Item;
};
struct ItemListClienRecvSend
{
	int WC;
	int WP;
	int GP;
	std::vector<InfoItemLocalSend> g_listmocnap;

	void Clear()
	{
		WC = 0;
		WP = 0;
		GP = 0;
		g_listmocnap.clear();
	}
};
struct InfoClientRecvSendData
{
	int NhanMocNap;
	int TongNap;
	std::vector<ListMocNapSend> DanhSachMocNap;
	void Clear()
	{
		NhanMocNap = 0;
		TongNap = 0;
		DanhSachMocNap.clear();
	}
};
extern InfoClientRecvSendData g_dataclient;
extern ItemListClienRecvSend g_mocapclient;