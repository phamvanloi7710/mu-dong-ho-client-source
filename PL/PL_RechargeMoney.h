#pragma once
#include "stdafx.h"
#if USING_RECHARGE_MONEY
#include "Protocol.h"

#define MAX_PAYMENT 120

namespace SEASON3B
{
	struct PSBMSG_RECHARGEMONEY_CANCEL_PAYMENT_SEND
	{
		PSBMSG_HEAD header; // C1:F3:F4
		int OrderCode;
	};

	struct GETMYPAYMENT_REQ
	{
		PSBMSG_HEAD header;
		char AccountName[11];
	};

#pragma pack(push, 1) // Ép mọi thứ sát nhau, không chèn byte trống

	struct PSWMSG_RECHARGE_HEAD { // Cấu trúc Header C2 (4 byte)
		BYTE type;
		BYTE size[2];
		BYTE head;
		BYTE sub;
	};

	struct PAYMENT_DATA
	{
		char AccountName[11];
		char TransferContent[16];
		int OrderCode;
		int Amount;
		int AmountPaid;
		int AddBonus;
		int Wcoin;
		int DateCreated;
		int Status;

		PAYMENT_DATA(char* NameAccount, char* CodeTransfer, int CodeOrder, int AmountValue, int PaidAmountValue, int BonusValue, int WcoinValue, int CreateDate, int OrderStatus)
		{
			strncpy_s(
				this->AccountName,
				sizeof(this->AccountName),
				NameAccount,
				_TRUNCATE
			);
			strncpy_s(
				this->TransferContent,
				sizeof(this->TransferContent),
				CodeTransfer,
				_TRUNCATE
			);
			strncpy(this->TransferContent, CodeTransfer, sizeof(CodeTransfer));
			this->OrderCode = CodeOrder;
			this->Amount = AmountValue;
			this->AmountPaid = PaidAmountValue;
			this->AddBonus = BonusValue;
			this->Wcoin = WcoinValue;
			this->DateCreated = CreateDate;
			this->Status = OrderStatus;
		}

	};

	struct MyPaymentData {
		char AccountName[11];
		char TransferContent[16];
		int OrderCode;
		int Amount;
		int AmountPaid;
		int AddBonus;
		int Wcoin;
		int DateCreated;
		int Status;
	};

	// Struct chính gửi từ GS -> Client
	struct PMSG_GETMYPAYMENT {
		PSWMSG_HEAD header;
		int TotalItemCount;
	};

#pragma pack(pop)

	struct MyPaymentData2
	{
		char AccountName[11];
		char TransferContent[16];
		int OrderCode;
		int Amount;
		int AmountPaid;
		int AddBonus;
		int Wcoin;
		int DateCreated;
		int Status;
	};

	struct DGPaymentInfo
	{
		PWMSG_HEAD header;
		MyPaymentData	pData;
	};

	typedef struct
	{
		PBMSG_HEAD header;
		char AccountName[11];
		char TransferContent[16];
		int OrderCode;
		int Amount;
		int AmountPaid;
		int AddBonus;
		int Wcoin;
		int DateCreated;
		int Status;
	} PMSG_QR_CODE_RECV, * LPMSG_QR_CODE_RECV;

	struct PSBMSG_RECHARGEMONEY_CHECK_TRANSACTION_SEND
	{
		PSBMSG_HEAD header;
		char AccountName[11];
		int OrderCode;
	};

	struct GC_PAYMENT_STATUS
	{
		PWMSG_HEAD header;
		int OrderCode;
		int Status;
	};

	struct GC_CANCEL_PAYMENT_SUCCESS
	{
		PWMSG_HEAD header;
	};

	struct PSBMSG_RECHARGEMONEY_CREATE_LINK_SEND
	{
		PSBMSG_HEAD header; // C1:F3:F4
		int amount;
	};

	class PLCRechargeMoney : public CNewUIObj
	{
	public:
		PLCRechargeMoney();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		bool Render();
		void Release();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Update();
		float GetLayerDepth();
		void SetPos(float x, float y);
		void GetPaymentInformation(PAYMENT_DATA myPaymentData);
		void GenerateRechargeCaptcha();
		void QRCodeRecv(BYTE* ReceiveBuffer);
		void RecvMyPaymentData(BYTE* ReceiveBuffer);
		void GetMyPayment();
		void QrCode();
		void OpenningProcess();
		void ClosingProcess();
		/*void DrawEventTimePanelWindow();
		void DrawRechargeMoneyWindow();
		void DrawPaymantDetailWindow();
		void cleanFormValue();
		bool RechargeMoneyMouseClick(DWORD ClickMouse);
		void RechargeMoneyMouseClickMovePage(DWORD Event);
		void DrawReCaptchaText(int fontSize, int PosX, int PosY, int With, int Align, LPCTSTR Text, ...);
		bool EventCreateQRCode(DWORD Event);
		void RecvPaymentInfoData(DGPaymentInfo* lpMsg);
		void QRCodeRecv(PMSG_QR_CODE_RECV* lpMsg);
		*/

		/*void ListenPaymentStatusFrom(GC_PAYMENT_STATUS* lpMsg);
		bool textBox(DWORD Event);
		bool EventCancelPayment(DWORD Event);
		bool EventCloseCancelPaymentMain(DWORD Event);*/
		/*void DrawPopupConfirm();
		void processForAutoMoveMain(int& InitialX, int& InitialY, int& WindowW, int& WindowH);
		void processForAutoMoveDetailMain(int& InitialX, int& InitialY, int& WindowW, int& WindowH);
		void processForAutoMovePaymentCancelMain(int& InitialX, int& InitialY, int& WindowW, int& WindowH);
		void ListenPaymentCancelSuccess(GC_CANCEL_PAYMENT_SUCCESS* lpMsg);*/
		//bool PaymentEvent(DWORD Event, DWORD ObjectID);
		//void OpenInput(int index);
		char rechangeCaptcha[5];
		char rechangeCaptcha2[12];
		int TotalItemCount;
		int RechargeMoneyPageNumber;
		void Clear();
		int		RechargeMoneyMaxPage;
		//MyPaymentData paymentData[MAX_PAYMENT];
		std::vector<PAYMENT_DATA> paymentData;
		int listInPage;

		MyPaymentData paymentDataInfo;
		CNewUIButton m_Button;
		CNewUIButton m_CloseMainButton;
		DWORD ClickTick;
		int		itemPerPage;
		int		pageCount;
		CUITextInputBox* m_RechargeMoneyInputAmount;
		CUITextInputBox* m_RechargeMoneyInputCaptCha;
		void DrawReCaptchaText(int fontSize, int PosX, int PosY, int With, int Align, LPCTSTR Text, ...);
		void ClearTextBoxes();
	private:
		CNewUIManager* m_pNewUIMng;
		CNewUIScrollBarHTML m_pScrollBar;
		int maxItemView;
		POINT m_Pos;
		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;

		int BUTTON_WIDTH;
		int BUTTON_HEIGHT;
	};

	class PLCRechargeMoneyDetail : public CNewUIObj
	{
	public:
		PLCRechargeMoneyDetail();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		bool Render();
		void Release();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Update();
		float GetLayerDepth();
		void OpenningProcess();
		void ClosingProcess();
		CNewUIButton m_ButtonCancelPayment;
		CNewUIButton m_CloseMainDetailButton;
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;

		int BUTTON_WIDTH;
		int BUTTON_HEIGHT;
	};

	class PLCRechargeMoneyPaymentCancel : public CNewUIObj
	{
	public:
		PLCRechargeMoneyPaymentCancel();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		bool Render();
		void Release();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Update();
		float GetLayerDepth();
		void OpenningProcess();
		void ClosingProcess();
		CNewUIButton m_CancelPayment;
		CNewUIButton m_CloseCancelPaymentMain;
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;

		int BUTTON_WIDTH;
		int BUTTON_HEIGHT;
	};
}


#endif // #USING_RECHARGE_MONEY