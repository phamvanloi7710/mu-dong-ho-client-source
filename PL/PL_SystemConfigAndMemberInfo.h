#include "stdafx.h"
#if USING_SYNCHRONIZE_DATA

	struct CG_GET_SYSTEM_CONFIG_INFO_REQ
	{
		PBMSG_HEAD h;
	};
	struct SYSTEM_CONFIG_DATA
	{
		// Thông tin tài khoản game
		int AccountLevel;
		char AccountExpireDate[20];
		char AccountName[11];
		// Thông tin tài khoản ngân hàng nhận tiền
		int BankID;
		char BankName[128];
		char BankAccountName[64];
		char BankAccountNo[64];

		// Thông tin API VietQR
		char VietQRApiUrl[64];
		char VietQRClientID[64];
		char VietQRApiKey[64];

		// Thông tin API PayOS
		char PayOSApiUrl[64];
		char PayOSClientID[64];
		char PayOSApiKey[64];
		char PayOSCheckSumKey[128];

		int RechargeMoneyAddBonus;

		// Thời điểm đặt lại dữ liệu của mốc mạp Ngày/Tuần/Tháng
		int DayHour;
		int DayMinute;
		int DaySecond;
		int WeekHour;
		int WeekMinute;
		int WeekSecond;
		int DayOfWeek;
		int MonthHour;
		int MonthMinute;
		int MonthSecond;
		int DayOfMonth;

		// Bật / tắt các mốc nạp
		int RechargeMoneyGiftFirstEnable;
		int RechargeMoneyGiftTodayEnable;
		int RechargeMoneyGiftWeekEnable;
		int RechargeMoneyGiftMonthEnable;
		int RechargeMoneyGiftTotalEnable;
	};

	typedef struct
	{
		PBMSG_HEADER header;
		SYSTEM_CONFIG_DATA m_SystemConfigData;
	} PMSG_SYSTEM_CONFIG_DATA_RECV, * LPMSG_SYSTEM_CONFIG_DATA_RECV;

	class PLSystemConfigAndMemberInfo
	{
	public:
		PLSystemConfigAndMemberInfo();
		void Init();
		void CGGetSystemConfigInfo();
		void GCSetSystemConfigInfo(BYTE* ReceiveBuffer);
		SYSTEM_CONFIG_DATA m_SystemConfigData;
	};

	extern PLSystemConfigAndMemberInfo gPLSystemConfigAndMemberInfo;
#endif // USING_SYNCHRONIZE_DATA