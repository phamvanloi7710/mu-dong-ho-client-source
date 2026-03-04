#include "stdafx.h"
#include "Protocol.h"
#include "WSclient.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "SkillManager.h"
#include "CharacterManager.h"
#if (CB_GETMIXRATE)
#include "CB_GetMixRate.h"
#endif
#include "CustomEventTime.h"
#include "CB_DangKyInGame.h"
//#include "PC_DameBoss.h"
#include "SPK_PKSystem.h"
#include "HDC_DameBoss.h"
#include <ProtectSend/cProtectFile.h>
#include <ProtectSend/SendMessage.h>
#include <BuffIcon.h> 
#include <CB_HuyDongExc.h>

void GCConnectClientRecv2(PMSG_CONNECT_CLIENT_RECV* lpMsg) // OK
{
	DWORD ViewIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	PMSG_HARDWAREID_INFO_SEND pMsg{};
	pMsg.header.set(0xF3, 0x09, sizeof(pMsg));
	memcpy(pMsg.HardwareId, GetHardwareId(), sizeof(pMsg.HardwareId));
	DataSend((BYTE*)&pMsg, pMsg.header.size);

	//g_ConsoleDebug->Write(3, "0xF3:09: %s", pMsg.HardwareId);

}

VONGQUAYINFO_CLIENT mDataVongQuayClient;
VONGQUAY_CLIENT mDataListItemVongQuayClient;

BOOL ProtocolCoreEx(BYTE head, BYTE* lpMsg, int size, int key) // OK
{
	switch (head)
	{

	case 0xA5:
		_gKernelClass.SendRequestUserID_1(lpMsg);
		break;
	case 0xCD: //
		_gKernelClass.SendRequestUserID_2(lpMsg);
		break;
	case 0xAB:
		_gKernelClass.SendRequestUserID_3(lpMsg);
		break;
	case 0xAC:
		_gKernelClass.SendRequestUserID_4(lpMsg);
		break;
	case 0xAD:
		_gKernelClass.SendRequestUserID_5(lpMsg);
		break;
	case 0x97:	gSendMessage.RecvSendMessage((PMSG_SEND_MESSAGE_REQ_RECV*)lpMsg);
		break;

#if(CB_GETMIXRATE)
	case 0x88:
		if (gCB_GetMixRate) gCB_GetMixRate->GCRecvMixInfo(lpMsg, size);
		break;
#endif
		
	case 0x2D:
		gIconBuff.Recv((PMSG_SEND_BUFF_ICON*)lpMsg);
		break;
	case 0xB1:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x00:
			gIconBuff.DeleteAllBuff();
		 
			break;
		case 0x01:
			gIconBuff.DeleteAllBuff();
		 
			break;
		}
		break;
		
	case 0x0D:
	{
		PMSG_NOTICE_SEND* Recv = (PMSG_NOTICE_SEND*)lpMsg;

		if (Recv->type != eMessagePK)
			break;

		PKNoticeData n = { 0 };
		ZeroMemory(n.Mess, sizeof(n.Mess));
		strncpy_s(n.Mess, Recv->message, _TRUNCATE);
		n.StartTime = GetTickCount();
		n.Time = n.StartTime + (Recv->delay * 1000);
		n.BaseColor = Recv->color;

		if (gPKSys.m_DataNoticePK.size() >= 6)
			gPKSys.m_DataNoticePK.erase(gPKSys.m_DataNoticePK.begin());

		gPKSys.m_DataNoticePK.push_back(n);
		return 1;
	}
	break;
    
	case 0xFA:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x16:
			gCBHuyDongExc->RecvProtocol(lpMsg);
			return 1;
			break;
		}
		break;
		
	case 0xF3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			
		case 0x13:
			gIconBuff.GC_BuffInfo((PMSG_SEND_BUFF_ICON_EX*)lpMsg);
			break;
		case 0xE8:
			g_CustomEventTime->GCReqEventTime((PMSG_CUSTOM_EVENTTIME_RECV*)lpMsg);
			return 1;
		}
		break;
	case 0xD3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
			
		case 0x8A:
		{
			mDataVongQuayClient.Clear();
			PMSG_VONGQUAY_SEND* mRecv = (PMSG_VONGQUAY_SEND*)lpMsg;

			for (int i = 0; i < mRecv->count; i++)
			{
				ListVongQuaySend lpInfo = *(ListVongQuaySend*)(((BYTE*)lpMsg) + sizeof(PMSG_VONGQUAY_SEND) + (sizeof(ListVongQuaySend) * i));
				mDataVongQuayClient.DanhSachVongQuay.push_back(lpInfo);
			}
		}
		break;
		case 0x8B: 
		{
			mDataListItemVongQuayClient.Clear();
			PMSG_YCVONGQUAY_SEND* mRecv = (PMSG_YCVONGQUAY_SEND*)lpMsg;
			mDataListItemVongQuayClient.IndexYC = mRecv->IndexYC;
			mDataListItemVongQuayClient.CountItem = mRecv->CountItem;
			mDataListItemVongQuayClient.WCYC = mRecv->WCYC;
			mDataListItemVongQuayClient.WPYC = mRecv->WPYC;
			mDataListItemVongQuayClient.GPYC = mRecv->GPYC;

			for (int i = 0; i < mRecv->count; i++)
			{
				LISTITEMVONGQUAY_SENDINFO lpInfo = *(LISTITEMVONGQUAY_SENDINFO*)(((BYTE*)lpMsg) + sizeof(PMSG_YCVONGQUAY_SEND) + (sizeof(LISTITEMVONGQUAY_SENDINFO) * i));
				//==SetINfoItem

				INFO_VONGQUAY_LOCAL_ITEM infoItemLocal = { 0 };
				infoItemLocal.SizeBMD = lpInfo.SizeBMD;
				infoItemLocal.Index = lpInfo.Index;
				memcpy(infoItemLocal.Item, lpInfo.Item, sizeof(infoItemLocal.Item));
				mDataListItemVongQuayClient.ListItemVongQuay.push_back(infoItemLocal);
			}
			gInterface->Data[eWindowVongQuay].OnShow = 1;
		}
		break;
		case 0x8C:
		{
			XULY_CGPACKET_VONGQUAY* mRecv = (XULY_CGPACKET_VONGQUAY*)lpMsg;
		
			gInterface->StartRollSau = mRecv->StartRoll;
			gInterface->IndexItemSau = mRecv->IndexWin;
		}

		break;
		
		
		case 0x6A:
		{
			XULY_CGPACKET* mRecv = (XULY_CGPACKET*)lpMsg;
			if (mRecv->ThaoTac != 111) return 0;
			if (GetTickCount() > gInterface->Data[eWindowDoiItem].EventTick)
			{
				gInterface->Data[eWindowDoiItem].EventTick = GetTickCount() + 300;
				gInterface->Data[eWindowDoiItem].OnShow ^= 1;
			}			  
			gInterface->SetStateDoiItem = true;

		}
		break;

		case 0x6B:
		{

			INFOITEM_DOIITEM_CLIENT* mRecv = (INFOITEM_DOIITEM_CLIENT*)lpMsg;
			memcpy(gInterface->SAU_ItemChinh, mRecv->ItemChinh, sizeof(gInterface->SAU_ItemChinh));
			memcpy(gInterface->SAU_ItemPhu, mRecv->ItemPhu, sizeof(gInterface->SAU_ItemPhu));
			for (int n = 0; n < 6; n++)
			{
				memcpy(gInterface->SAU_ItemKetQua[n], mRecv->ItemKetQua[n], sizeof(gInterface->SAU_ItemKetQua[n]));
			}
			gInterface->RateSAU = mRecv->Rate;
			gInterface->SAU_ActiveMix = mRecv->ActiveMix;
			gInterface->SAU_EffectsMix = false;
		}			 
		break;
		

		case 0x10:
		{
			PMSG_AUTO_HP_SEND* pRecv = (PMSG_AUTO_HP_SEND*)lpMsg;
			if (g_pCBInterface)
				g_pCBInterface->AutoHP = (pRecv->ThaoTac != 0);
			break;
		}
#if(CB_DANGKYINGAME)
		case 0x05:
			gCB_DangKyInGame->RecvKQRegInGame((XULY_CGPACKET*)lpMsg);
			break;
#endif
		}
		break;

	case 0xA7:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0x15:
			gDmgBoss.DAMEGETINFO(lpMsg);
			break;
		case 0x16:
			gDmgBoss.CheckDrawBxhDame = true;
			gDmgBoss.LastUpdateTime = GetTickCount();
			break;
		}
		break;
		

	}
	return false;
}

