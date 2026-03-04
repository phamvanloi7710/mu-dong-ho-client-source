#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "NewUIMessageBox.h"
#include "NewUIMyInventory.h"
#include "NewUIGuildInfoWindow.h"
#include "WSClient.h"


struct CUSTOM_JEWELBANK_DATA
{
	int ItemBank[10];
};
struct PSBMSG_JEWELBANK_RECV
{
	PSBMSG_HEAD h;

	int ItemBank[10];
	BYTE JewelEnable;
	BYTE JewelMaxCount;
	BYTE MaxPage;        // 1 byte

};



class CNewUIJewelBank
{
public:

	CNewUIJewelBank();
	virtual ~CNewUIJewelBank();

public:
	void Render3DItem();

	void Render();
	bool Enable;

	int D_X;
	int D_Y;

	static CNewUIJewelBank* Instance();

	//Struc
	BYTE JewelEnable;
	BYTE JewelMaxCount;
	DWORD StatusTick;
	CUSTOM_JEWELBANK_DATA m_Data;
	int ItemBank[10];
	void GCCustomBankInfoRecv(const BYTE* lpMsg);

	POINT MaxPosition;
	POINT CurPosition;
	bool  DragItem;
	DWORD ActionTime;
	//bool UpdateKeyEvent();

};

extern CNewUIJewelBank gJwBanking;

#define g_CNewUIJewelBank			(CNewUIJewelBank::Instance())

