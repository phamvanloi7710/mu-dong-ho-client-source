#include "stdafx.h"
#if USING_SYNCHRONIZE_DATA
#include "Protocol.h"
	#include "PL_SystemConfigAndMemberInfo.h"
	#if CONSOLE_LOG
	#include "Console.h"
	#endif // CONSOLE_LOG
	PLSystemConfigAndMemberInfo gPLSystemConfigAndMemberInfo;

	PLSystemConfigAndMemberInfo::PLSystemConfigAndMemberInfo()
	{

	}

	void PLSystemConfigAndMemberInfo::Init()
	{

	}

	void PLSystemConfigAndMemberInfo::CGGetSystemConfigInfo() {
		CG_GET_SYSTEM_CONFIG_INFO_REQ pMsg;
		pMsg.h.set(0x01, sizeof(pMsg));
		DataSend((BYTE*)&pMsg, pMsg.h.size);
	}

	void PLSystemConfigAndMemberInfo::GCSetSystemConfigInfo(BYTE* ReceiveBuffer) {
		LPMSG_SYSTEM_CONFIG_DATA_RECV lpMsg = (LPMSG_SYSTEM_CONFIG_DATA_RECV)ReceiveBuffer;
		// Thông tin tài khoản game
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.AccountLevel = lpMsg->m_SystemConfigData.AccountLevel;
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.AccountExpireDate, lpMsg->m_SystemConfigData.AccountExpireDate, sizeof(lpMsg->m_SystemConfigData.AccountExpireDate));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.AccountName, lpMsg->m_SystemConfigData.AccountName, sizeof(lpMsg->m_SystemConfigData.AccountName));

		// Thông tin tài khoản ngân hàng nhận tiền
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankID = lpMsg->m_SystemConfigData.BankID;
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankName, lpMsg->m_SystemConfigData.BankName, sizeof(lpMsg->m_SystemConfigData.BankName));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankAccountName, lpMsg->m_SystemConfigData.BankAccountName, sizeof(lpMsg->m_SystemConfigData.BankAccountName));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankAccountNo, lpMsg->m_SystemConfigData.BankAccountNo, sizeof(lpMsg->m_SystemConfigData.BankAccountNo));

		// Thông tin API VietQR
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.VietQRApiUrl, lpMsg->m_SystemConfigData.VietQRApiUrl, sizeof(lpMsg->m_SystemConfigData.VietQRApiUrl));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.VietQRClientID, lpMsg->m_SystemConfigData.VietQRClientID, sizeof(lpMsg->m_SystemConfigData.VietQRClientID));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.VietQRApiKey, lpMsg->m_SystemConfigData.VietQRApiKey, sizeof(lpMsg->m_SystemConfigData.VietQRApiKey));

		// Thông tin API PayOS
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.PayOSApiUrl, lpMsg->m_SystemConfigData.PayOSApiUrl, sizeof(lpMsg->m_SystemConfigData.PayOSApiUrl));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.PayOSClientID, lpMsg->m_SystemConfigData.PayOSClientID, sizeof(lpMsg->m_SystemConfigData.PayOSClientID));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.PayOSApiKey, lpMsg->m_SystemConfigData.PayOSApiKey, sizeof(lpMsg->m_SystemConfigData.PayOSApiKey));
		strncpy(gPLSystemConfigAndMemberInfo.m_SystemConfigData.PayOSCheckSumKey, lpMsg->m_SystemConfigData.PayOSCheckSumKey, sizeof(lpMsg->m_SystemConfigData.PayOSCheckSumKey));

		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyAddBonus = lpMsg->m_SystemConfigData.RechargeMoneyAddBonus;

		// Bật / tắt các mốc nạp
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyGiftFirstEnable = lpMsg->m_SystemConfigData.RechargeMoneyGiftFirstEnable;
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyGiftTodayEnable = lpMsg->m_SystemConfigData.RechargeMoneyGiftTodayEnable;
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyGiftWeekEnable = lpMsg->m_SystemConfigData.RechargeMoneyGiftWeekEnable;
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyGiftMonthEnable = lpMsg->m_SystemConfigData.RechargeMoneyGiftMonthEnable;
		gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyGiftTotalEnable = lpMsg->m_SystemConfigData.RechargeMoneyGiftTotalEnable;

	}

#endif // USING_SYNCHRONIZE_DATA