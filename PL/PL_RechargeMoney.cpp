#include "stdafx.h"
#if USING_RECHARGE_MONEY
#include <PL/PL_RechargeMoney.h>
#include "GameGuardLoader.h"
#include "TextClien.h"
#include "UIControls.h"
#include "wsclientinline.h"
#include "Protocol.h"
#include <PL/PL_Color.h>
#include <PL/PL_Font.h>
#include <PL/PL_Log.h>
#include "nlohmann/json.hpp"
#if USING_SYNCHRONIZE_DATA
#include "PL_SystemConfigAndMemberInfo.h"
#endif // USING_SYNCHRONIZE_DATA
#include <UIMng.h>
#include <PL/PL_Interface.h>

using namespace SEASON3B;
using namespace std;
using json = nlohmann::json;

int RechargeMoneyPageCount = 1;
int itemPerPage = 20;
int TotalItemCount;
#define MIN_RECHARGE_AMOUNT 10000
#define MAX_RECHARGE_AMOUNT 1000000000  // 1 tỷ
SEASON3B::PLCRechargeMoney::PLCRechargeMoney()
{
	maxItemView = 15;
	Release();
}

bool SEASON3B::PLCRechargeMoney::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RECHARGE_MONEY_SYSTEM, this);
		listInPage = 0;
		m_Pos.y = 50.0;
		m_Pos.x = (MAX_WIN_WIDTH / 2) - (WINDOW_WIDTH / 2);

		m_RechargeMoneyInputAmount = new CUITextInputBox();
		m_RechargeMoneyInputAmount->Init(gwinhandle->GethWnd(), 90, 14, 13, FALSE);
		m_RechargeMoneyInputAmount->SetPosition(m_Pos.x + 26 + 76, m_Pos.y + 70);
		m_RechargeMoneyInputAmount->SetTextColor(255, 255, 255, 255);
		m_RechargeMoneyInputAmount->SetBackColor(0, 255, 255, 255);
		m_RechargeMoneyInputAmount->SetFont(g_hFontBold);
		m_RechargeMoneyInputAmount->SetState(UISTATE_NORMAL);
		m_RechargeMoneyInputAmount->SetOption((UIOPTIONS)UIOPTION_NUMBERONLY);

		m_RechargeMoneyInputCaptCha = new CUITextInputBox();
		m_RechargeMoneyInputCaptCha->Init(gwinhandle->GethWnd(), 90, 14, 4, FALSE);
		m_RechargeMoneyInputCaptCha->SetPosition(m_Pos.x + 26 + 76, m_Pos.y + 148);
		m_RechargeMoneyInputCaptCha->SetTextColor(255, 255, 255, 255);
		m_RechargeMoneyInputCaptCha->SetBackColor(0, 255, 255, 255);
		m_RechargeMoneyInputCaptCha->SetFont(g_hFontBold);
		m_RechargeMoneyInputCaptCha->SetState(UISTATE_NORMAL);
		m_RechargeMoneyInputCaptCha->SetOption((UIOPTIONS)UIOPTION_NUMBERONLY);


		m_pScrollBar.Create(m_Pos.x + WINDOW_WIDTH - 20, m_Pos.y + 54, WINDOW_HEIGHT - 175);
		m_pScrollBar.SetPercent(0.0);

		g_RechargeMoney->Show(false);

		g_RechargeMoney->GenerateRechargeCaptcha();
		Success = true;
	}
	return Success;
}

void SEASON3B::PLCRechargeMoney::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void PLCRechargeMoney::Clear()
{
	m_RechargeMoneyInputAmount = NULL;
	m_RechargeMoneyInputCaptCha = NULL;
}

void SEASON3B::PLCRechargeMoney::GenerateRechargeCaptcha()
{
	BYTE ascii = NULL; int iLen = 0; int aux = 0;
	char Char = 0;
	srand(time(NULL));

	char newCaptcha[5] = { 0, };

	while (iLen < 4)
	{
		ascii = 33 + rand() % (126 - 33);

		Char = (char)ascii;
		if ((Char >= '0' && Char <= '9') /* || (Char >= 'A' && Char <= 'Z') || (Char >= 'a' && Char <= 'z') */)
		{
			newCaptcha[iLen] = Char;
			iLen++;
		}
	}

	for (int i = 0; i < 12; i++)
	{
		if ((i % 2) == 0)
		{
			g_RechargeMoney->rechangeCaptcha2[i] = newCaptcha[aux];
			aux++;
		}
		else
		{
			g_RechargeMoney->rechangeCaptcha2[i] = 32;
		}
	}

	ZeroMemory(g_RechargeMoney->rechangeCaptcha, sizeof(g_RechargeMoney->rechangeCaptcha));
	memcpy(g_RechargeMoney->rechangeCaptcha, newCaptcha, 4);
}

void SEASON3B::PLCRechargeMoney::GetPaymentInformation(PAYMENT_DATA myPaymentData)
{
	if (myPaymentData.DateCreated > 0) {
		// Xóa trắng buffer trước khi copy để tránh rác
		memset(&g_RechargeMoney->paymentDataInfo, 0, sizeof(g_RechargeMoney->paymentDataInfo));

		// Copy an toàn
		memcpy(g_RechargeMoney->paymentDataInfo.AccountName, myPaymentData.AccountName, sizeof(g_RechargeMoney->paymentDataInfo.AccountName) - 1);
		strncpy_s(
			g_RechargeMoney->paymentDataInfo.TransferContent,
			sizeof(g_RechargeMoney->paymentDataInfo.TransferContent),
			myPaymentData.TransferContent,
			_TRUNCATE
		);


		g_RechargeMoney->paymentDataInfo.OrderCode = myPaymentData.OrderCode;
		g_RechargeMoney->paymentDataInfo.Amount = myPaymentData.Amount;
		g_RechargeMoney->paymentDataInfo.AmountPaid = myPaymentData.AmountPaid;
		g_RechargeMoney->paymentDataInfo.AddBonus = myPaymentData.AddBonus;
		g_RechargeMoney->paymentDataInfo.Wcoin = myPaymentData.Wcoin;
		g_RechargeMoney->paymentDataInfo.DateCreated = myPaymentData.DateCreated;
		g_RechargeMoney->paymentDataInfo.Status = myPaymentData.Status;
	}
}

void SEASON3B::PLCRechargeMoney::DrawReCaptchaText(int fontSize, int PosX, int PosY, int With, int Align, LPCTSTR Text, ...) {
	HFONT TextFont = CreateFontA(fontSize, 0, 0, 0, 600, 0, 0, 0, 0x1, 0, 0, 3, 0, "Corbel");
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);
	int LineCount = 0;
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char* Line = strtok(Buff, "\n##");
	DWORD BKTextColor = g_pRenderText->GetTextColor();
	DWORD BKBGColor = g_pRenderText->GetBgColor();
	g_pRenderText->SetFont(TextFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->SetBgColor(69, 39, 18, 255);
	while (Line != NULL)
	{
		g_pRenderText->RenderText(PosX, PosY, Line, With, 30, Align, 0);
		PosY += 10;
		Line = strtok(NULL, "\n##");

		LineCount++;
	}
	EnableAlphaTest(false);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(BKTextColor);
	g_pRenderText->SetBgColor(BKBGColor);
	DeleteObject(TextFont);
}

bool SEASON3B::PLCRechargeMoney::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	float MainWidth = WINDOW_WIDTH;
	float MainHeight = WINDOW_HEIGHT;
	float InitialX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (MainHeight / 2);

	g_PLInterface->RenderNormalMain(InitialX, InitialY, MainWidth, MainHeight, "Nạp tiền vào tài khoản");

	int RowCol = (MainWidth / 4);
	float MainCenter = InitialX + RowCol;
	float ButtonX = MainCenter - (float)(29.0 / 2);
	float StartBody = InitialY;
	DWORD Color = 0xFFFFFFB8;
	gInterface->DrawInfoBox(InitialX + 17, InitialY + 40, 180, 150, 0x00000096, 0, 0);
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 19, InitialY + 41, whiteColor, orangeColor, 185, 0, 3, "Tạo lệnh nạp tiền");


	float InputW = 90;
	RenderImage(SEASON3B::CNewUICharacterInfoWindow::IMAGE_LIST::IMAGE_CHAINFO_TEXTBOX, InitialX + 26, InitialY + 62, 170.f, 21.f);
	gPLFont->RenderText(g_hFontBold, InitialX + 26, InitialY + 67, whiteColor, noColor, 77, 0, 3, "Số tiền");
	m_Button.Render();
	m_CloseMainButton.Render();

	// Khuyen mai
#if USING_SYNCHRONIZE_DATA
	int AddBonus = gPLSystemConfigAndMemberInfo.m_SystemConfigData.RechargeMoneyAddBonus;
#else
	int AddBonus = 10;
#endif // USING_SYNCHRONIZE_DATA

	char Amount[14] = { 0, };
	m_RechargeMoneyInputAmount->GetText(Amount, 14);

	int iAmount = 0;
	if (Amount[0] != '\0')
	{
		iAmount = strtol(Amount, nullptr, 10);
	}
	int amountOfGcoin = (iAmount + (iAmount / 100 * AddBonus));
	gPLFont->RenderText(g_hFontBold, InitialX + 24, InitialY + 85, whiteColor, noColor, 180, 0, 3, "Quy đổi: %sVNĐ = %sWCoin", gInterface->NumberFormat(iAmount), gInterface->NumberFormat(amountOfGcoin)); // Quy đổi: %s VNĐ = %s WCoin

	g_RechargeMoney->DrawReCaptchaText(30, InitialX + 26, InitialY + 98, 170, 3, rechangeCaptcha);
	RenderImage(SEASON3B::CNewUICharacterInfoWindow::IMAGE_LIST::IMAGE_CHAINFO_TEXTBOX, InitialX + 26, InitialY + 140, 170.f, 21.f);
	gPLFont->RenderText(g_hFontBold, InitialX + 26, InitialY + 145, whiteColor, noColor, 77, 0, 3, "Mã xác nhận");

	// Nút tạo mã QR
	gInterface->SetButtonInfo(&m_Button, PL_INTERFACE::BUTTON_NORMAL_73_27, InitialX + 76, InitialY + 165, 73, 27, true, false, true, true, "Tạo mã QR", "", false);

	// Nút đóng main
	gInterface->SetButtonInfo(&m_CloseMainButton, PL_INTERFACE::BUTTON_CLOSE_MAIN_29_29, InitialX + WINDOW_WIDTH - 40, InitialY - 2, 29, 29, FALSE, FALSE, FALSE, FALSE, "", "Đóng", TRUE);

	gInterface->DrawInfoBox(InitialX + 17, InitialY + 208, 180, 35, 0x00000096, 0, 0);
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 18, InitialY + 208, whiteColor, orangeColor, 186, 0, 3, "Thông tin khuyến mãi");
	gPLFont->RenderText(g_hFontBold, InitialX + 30, InitialY + 228, excellentS15Color, noColor, 160, 15, 3, "Khuyến mãi thêm %d%% phần thưởng", AddBonus); // Khuyến mãi thêm %d%% phần thưởng

	// Danh sách giao dịch
	gInterface->DrawInfoBox(InitialX + 215, InitialY + 40, 375, 203, 0x00000096, 0, 0);
	//gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 18, InitialY + 208, whiteColor, orangeColor, 186, 15, 3, "Thông tin khuyến mãi");

	const int textHeight = 12;
	const int btnW = 54;
	const int btnH = textHeight;
	const int rowHeight = textHeight + 1.725;
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 217, InitialY + 40, whiteColor, headerTableColor, 60, textHeight, 3, "Số tiền"); // Số tiền
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 280, InitialY + 40, whiteColor, headerTableColor, 60, textHeight, 3, "Khuyến mãi"); // Khuyến mãi
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 342, InitialY + 40, whiteColor, headerTableColor, 100, textHeight, 3, "Ngày thực hiện"); // Ngày thực hiện
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 445, InitialY + 40, whiteColor, headerTableColor, 95, textHeight, 3, "Trạng thái"); // Trạng thái
	gPLFont->RenderText((HFONT)g_hFontBold, InitialX + 543, InitialY + 40, whiteColor, headerTableColor, 54, textHeight, 3, "Tùy chọn"); // Tùy chọn

	int posY = 0;
	size_t secure = 0;
	size_t current = 0;
	size_t datalist = paymentData.size();
	if (datalist > (size_t)maxItemView)
	{
		double prev = m_pScrollBar.GetPercent();

		current = (int)((double)(unsigned int)(datalist - maxItemView) * prev);
	}
	if (datalist != 0) {

		int count = 0;
		for (size_t i = current; i < datalist && secure < (size_t)maxItemView; i++, secure++)
			//for (int i = listInPage; i < datalist; i++)
		{
			if (count >= MAX_ITEM_VIEW)
			{
				break;
			}

			int up = i;

			// Hover cho toàn bộ dòng
			const int rowWidth = 590; // Chiều rộng vùng hover (tùy giao diện)
			const int hoverX = InitialX + 217;
			const int hoverY = InitialY + 54 + posY;

			ColorRGBA rowColor = normalColor;

			if (SEASON3B::CheckMouseIn(hoverX, hoverY, rowWidth, rowHeight))
				rowColor = hoverColor;
			//
			// Số tiền
			gPLFont->RenderText(g_hFontBold, InitialX + 217, InitialY + 54 + posY, whiteColor, rowColor, 60, textHeight, 3, "%s", gInterface->NumberFormat(g_RechargeMoney->paymentData[up].Amount));
			// Bonus %
			gPLFont->RenderText(g_hFontBold, InitialX + 280, InitialY + 54 + posY, whiteColor, rowColor, 60, textHeight, 3, "%d%%", g_RechargeMoney->paymentData[up].AddBonus);

			// Ngày tạo
			if (g_RechargeMoney->paymentData[up].DateCreated > 0)
			{
				int mYear = 0, mMonth = 0, mDays = 0, hours = 0, minutes = 0;
				time_t mTime = g_RechargeMoney->paymentData[up].DateCreated;
				tm* lpTime = localtime(&mTime);
				if (lpTime)
				{
					mYear = 1952 + lpTime->tm_year;
					mMonth = 1 + lpTime->tm_mon;
					mDays = lpTime->tm_mday;
					hours = lpTime->tm_hour;
					minutes = lpTime->tm_min;
				}
				gPLFont->RenderText(g_hFontBold, InitialX + 342, InitialY + 54 + posY, whiteColor, rowColor, 100, textHeight, 3, "%02d:%02d %02d/%02d/%d", hours, minutes, mDays, mMonth, mYear);
			}

			// Trạng thái thanh toán
			const char* statusText = "";
			switch (g_RechargeMoney->paymentData[up].Status)
			{
			case 0: statusText = "Chờ thanh toán"; break; // Chờ thanh toán
			case 1: statusText = "Đã thanh toán"; break; // Đã thanh toán
			case 2: statusText = "Thanh toán thiếu"; break; // Thanh toán thiếu
			case 3: statusText = "Đã hết hạn"; break; // Đã hết hạn
			case 4: statusText = "Đang xử lý"; break; // Đang xử lý
			case 5: statusText = "Thất bại"; break; // Thất bại
			case 6: statusText = "Đã hủy"; break; // Đã hủy
			}
			gPLFont->RenderText(g_hFontBold, InitialX + 445, InitialY + 54 + posY, whiteColor, rowColor, 95, textHeight, 3, statusText);

			// Nút "Chi tiết" với hover riêng
			const int btnX = InitialX + 543;

			if (SEASON3B::CheckMouseIn(btnX, InitialY + 54 + posY, btnW, btnH))
			{
				if ((GetKeyState(VK_LBUTTON) & 0x8000) && (GetTickCount() - ClickTick) > 500) // Nếu cho phép click
				{

					g_RechargeMoney->GetPaymentInformation(g_RechargeMoney->paymentData[up]);
					if (g_RechargeMoney->paymentData[up].Status == 0 || g_RechargeMoney->paymentData[up].Status == 2) {
						g_RechargeMoney->QrCode();
					}
					
					g_pNewUISystem->Show(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN);
					ClickTick = GetTickCount();
				}
			}
			gPLFont->RenderText(g_hFontBold, btnX, InitialY + 54 + posY, whiteColor, rowColor, btnW, btnH, 3, "Chi tiết"); // Chi tiết

			posY += rowHeight;
		}
	}
	if (datalist > (size_t)maxItemView)
	{
		m_pScrollBar.Render();
	}

	// Hướng dẫn
	float addY = 15;
	gInterface->DrawInfoBox(InitialX + 15, InitialY + addY + 242, 575, 90, 0x00000096, 0, 0);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 18, InitialY + addY + 242, whiteColor, orangeColor, 579, 0, 3, "Hướng dẫn chi tiết"); // Hướng dẫn chi tiết

	//gInterface.DrawGUI(ePoint, InitialX + 30, InitialY + 258);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 45, InitialY + addY + 258, whiteColor, noColor, 573, 0, 1, "Bước 1: Nhập số tiền muốn nạp, xác nhận mã rồi nhấn TẠO MÃ QR để tạo QR nạp tiền."); // Bước 1: Nhập số tiền muốn nạp, xác nhận mã rồi nhấn TẠO MÃ QR để tạo QR nạp tiền.

	//gInterface.DrawGUI(ePoint, InitialX + 30, InitialY + 273);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 45, InitialY + addY + 273, whiteColor, noColor, 573, 0, 1, "Bước 2: Quét mã QR để tiến hành chuyển khoản."); // Bước 2: Quét mã QR để tiến hành chuyển khoản.

	//gInterface.DrawGUI(ePoint, InitialX + 30, InitialY + 288);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 45, InitialY + addY + 288, whiteColor, noColor, 573, 0, 1, "Lưu ý: Giao dịch trước đó nếu chưa hoàn thành thì bạn không thể tạo lệnh nạp tiền mới."); // Lưu ý: Giao dịch trước đó nếu chưa hoàn thành thì bạn không thể tạo lệnh nạp tiền mới.

	//gInterface.DrawGUI(ePoint, InitialX + 30, InitialY + 303);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 45, InitialY + addY + 303, whiteColor, noColor, 573, 0, 1, "Bạn có thể hủy giao dịch trước đó để tiến hành giao dịch nạp tiền mới."); // Bạn có thể hủy giao dịch trước đó để tiến hành giao dịch nạp tiền mới.

	//gInterface.DrawGUI(ePoint, InitialX + 30, InitialY + 318);
	gPLFont->RenderText((HFONT)g_hFont, InitialX + 45, InitialY + addY + 318, whiteColor, noColor, 573, 0, 1, "Sau khi nạp thành công, trong vòng 5 phút, phần thưởng sẽ được cộng vào tài khoản game của bạn."); // Sau khi nạp thành công, trong vòng 5 phút, phần thưởng sẽ được cộng vào tài khoản game của bạn.



	if (m_RechargeMoneyInputAmount) {
		m_RechargeMoneyInputAmount->Render();
	}

	if (m_RechargeMoneyInputCaptCha) {
		m_RechargeMoneyInputCaptCha->Render();
		m_RechargeMoneyInputAmount->SetTabTarget(m_RechargeMoneyInputCaptCha);
	}
	DisableAlphaBlend();

	return true;
}

void SEASON3B::PLCRechargeMoney::OpenningProcess()
{
	m_pScrollBar.SetPercent(0);
	g_RechargeMoney->GetMyPayment();
}

void SEASON3B::PLCRechargeMoney::ClosingProcess()
{
	//m_ScrollBar->SetCurPos(0);
}

void SEASON3B::PLCRechargeMoney::Release()
{
	m_pNewUIMng = nullptr;
	m_RechargeMoneyInputAmount = nullptr;
	m_RechargeMoneyInputCaptCha = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	WINDOW_WIDTH = 613;
	WINDOW_HEIGHT = 370;

	BUTTON_WIDTH = 72;
	BUTTON_HEIGHT = 27;
	RechargeMoneyPageNumber = 10;


	//SAFE_DELETE(m_ScrollBar);
	SAFE_DELETE(m_RechargeMoneyInputAmount);
	SAFE_DELETE(m_RechargeMoneyInputCaptCha);
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::PLCRechargeMoney::ClearTextBoxes()
{
	if (m_RechargeMoneyInputAmount) m_RechargeMoneyInputAmount->SetText("");
	if (m_RechargeMoneyInputCaptCha) m_RechargeMoneyInputCaptCha->SetText("");
}

bool SEASON3B::PLCRechargeMoney::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			if (m_RechargeMoneyInputAmount && m_RechargeMoneyInputAmount->HaveFocus()) SetFocus(gwinhandle->GethWnd());
			if (m_RechargeMoneyInputCaptCha && m_RechargeMoneyInputCaptCha->HaveFocus()) SetFocus(gwinhandle->GethWnd());
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_SYSTEM);
			return false;
		}
	}
	return true;
}

bool SEASON3B::PLCRechargeMoney::Update()
{
	if (m_RechargeMoneyInputCaptCha) {
		m_RechargeMoneyInputCaptCha->DoAction();
	}

	if (m_RechargeMoneyInputAmount) {
		m_RechargeMoneyInputAmount->DoAction();
	}

	if (IsVisible())
	{
		m_pScrollBar.Update();
	}
	return true;
}

const char* FormatNumber(int number) {
	static char buffer[20];
	// Format 10.000.000
	sprintf_s(buffer, sizeof(buffer), "%d", number);

	// Có thể thêm logic format dấu chấm/thập phân ở đây
	return buffer;
}

bool SEASON3B::PLCRechargeMoney::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;
	// Nếu chuột nằm trong vùng khung menu
	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		// Sự kiện của nút tạo mã QR
		if (m_Button.UpdateMouseEvent())
		{
			// Thêm các define/const


			char Amount[16] = { 0, };  // Tăng buffer để an toàn
			char CaptchaValue[10] = { 0, };  // Tăng buffer cho captcha

			m_RechargeMoneyInputAmount->GetText(Amount, sizeof(Amount));
			m_RechargeMoneyInputCaptCha->GetText(CaptchaValue, sizeof(CaptchaValue));

			// Kiểm tra input không rỗng
			if (Amount[0] == '\0') {
				gPLNotification->ShowNotification("Vui lòng nhập số tiền cần nạp");
				return true;
			}

			if (CaptchaValue[0] == '\0') {
				gPLNotification->ShowNotification("Vui lòng nhập mã xác nhận");
				return true;
			}

			// Kiểm tra Amount chỉ chứa số
			for (int i = 0; i < strlen(Amount); i++) {
				if (!isdigit(Amount[i])) {
					gPLNotification->ShowNotification("Số tiền chỉ được chứa ký tự số");
					return true;
				}
			}

			// Chuyển đổi số tiền với kiểm tra lỗi
			char* endPtr;
			long iAmount = strtol(Amount, &endPtr, 10);

			// Kiểm tra chuyển đổi thành công
			if (endPtr == Amount || *endPtr != '\0') {
				gPLNotification->ShowNotification("Số tiền không hợp lệ");
				return true;
			}

			// Kiểm tra giới hạn
			if (iAmount < MIN_RECHARGE_AMOUNT) {
				char msg[100];
				sprintf_s(msg, sizeof(msg), "Số tiền nạp tối thiểu là %sđ",
					FormatNumber(MIN_RECHARGE_AMOUNT));
				gPLNotification->ShowNotification(msg);
				return true;
			}

			if (iAmount > MAX_RECHARGE_AMOUNT) {
				char msg[100];
				sprintf_s(msg, sizeof(msg), "Số tiền nạp tối đa là %sđ",
					FormatNumber(MAX_RECHARGE_AMOUNT));
				gPLNotification->ShowNotification(msg);
				return true;
			}

			// Kiểm tra Captcha (giả sử rechangeCaptcha là string)
			if (rechangeCaptcha == nullptr || rechangeCaptcha[0] == '\0') {
				gPLNotification->ShowNotification("Lỗi hệ thống: Không có mã xác nhận");
				return true;
			}

			// Kiểm tra Captcha chỉ chứa số
			for (int i = 0; i < strlen(CaptchaValue); i++) {
				if (!isdigit(CaptchaValue[i])) {
					gPLNotification->ShowNotification("Mã xác nhận chỉ được chứa ký tự số");
					return true;
				}
			}

			// So sánh captcha (FIXED LOGIC)
			if (strcmp(CaptchaValue, rechangeCaptcha) != 0) {
				gPLNotification->ShowNotification("Mã xác nhận không chính xác");
				return true;
			}

			// Tạo và gửi packet
			PSBMSG_RECHARGEMONEY_CREATE_LINK_SEND pMsg;
			memset(&pMsg, 0, sizeof(pMsg));  // Luôn memset để tránh giá trị rác

			// Kiểm tra overflow
			if (iAmount > INT_MAX) {
				gPLNotification->ShowNotification("Số tiền vượt quá giới hạn cho phép");
				return true;
			}

			pMsg.amount = (int)iAmount;
			pMsg.header.set(0x80, 0x00, sizeof(pMsg));

			// Gửi dữ liệu
			DataSend((BYTE*)&pMsg, pMsg.header.size);

			// Optional: Hiển thị thông báo thành công
			//gPLNotification->ShowNotification("Đang xử lý yêu cầu nạp tiền...");
		}

		// Sự kiện của nút đóng main
		if (m_CloseMainButton.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_SYSTEM);
		}

		// Sự kiện của thanh cuộn
		size_t good_count = paymentData.size();

		if (good_count > (size_t)maxItemView)
		{
			double prev = m_pScrollBar.GetPercent();
			if (MouseWheel <= 0)
			{
				if (MouseWheel < 0)
				{
					MouseWheel = 0;
					prev += 0.1;
					if (prev > 1.0)
						prev = 1.0;
					m_pScrollBar.SetPercent(prev);
				}
			}
			else
			{
				MouseWheel = 0;
				prev -= 0.1;
				if (prev < 0.0)
					prev = 0.0;
				m_pScrollBar.SetPercent(prev);
			}

			m_pScrollBar.UpdateMouseEvent();
		}
	}

	return !SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, WINDOW_WIDTH, WINDOW_HEIGHT);
}

float SEASON3B::PLCRechargeMoney::GetLayerDepth()
{
	return 10.0f;
}

void PLCRechargeMoney::GetMyPayment() {
	GETMYPAYMENT_REQ pRequest;
	pRequest.header.set(0x80, 0x01, sizeof(pRequest));
	DataSend((LPBYTE)&pRequest, pRequest.header.size);
}

void PLCRechargeMoney::QRCodeRecv(BYTE* ReceiveBuffer)
{
	LPMSG_QR_CODE_RECV lpMsg = (LPMSG_QR_CODE_RECV)ReceiveBuffer;
	g_RechargeMoney->GetMyPayment();
	PAYMENT_DATA myPaymentData(
		lpMsg->AccountName,
		lpMsg->TransferContent,
		lpMsg->OrderCode,
		lpMsg->Amount,
		lpMsg->AmountPaid,
		lpMsg->AddBonus,
		lpMsg->Wcoin,
		lpMsg->DateCreated,
		lpMsg->Status
	);
	g_RechargeMoney->GetPaymentInformation(myPaymentData);
	g_RechargeMoney->QrCode();
	g_pNewUISystem->Show(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN);
}

void PLCRechargeMoney::RecvMyPaymentData(BYTE* ReceiveBuffer)
{
	// Ép kiểu về Struct
	PMSG_GETMYPAYMENT* Data = (PMSG_GETMYPAYMENT*)ReceiveBuffer;

	paymentData.clear();
	g_RechargeMoney->TotalItemCount = Data->TotalItemCount;

	// Điểm bắt đầu của danh sách Item nằm ngay sau PMSG_GETMYPAYMENT
	int offset = sizeof(PMSG_GETMYPAYMENT);

	for (int i = 0; i < Data->TotalItemCount; i++) {
		// Trỏ vào đúng vị trí từng item trong mảng byte
		MyPaymentData2* Data2 = (MyPaymentData2*)(ReceiveBuffer + offset);

		paymentData.push_back(
			PAYMENT_DATA(
				Data2->AccountName,
				Data2->TransferContent,
				Data2->OrderCode,
				Data2->Amount,
				Data2->AmountPaid,
				Data2->AddBonus,
				Data2->Wcoin,
				Data2->DateCreated,
				Data2->Status
			)
		);
		/*g_ConsoleDebug->Write(MCD_NORMAL, "AccountName: %s | TransferContent: %s", Data2->AccountName, Data2->TransferContent);*/
		// Nhảy offset sang item tiếp theo
		offset += sizeof(MyPaymentData2);
	}
	//g_ConsoleDebug->Write(MCD_NORMAL, "MCD_SEND: %d | MCD_RECEIVE: %d | MCD_ERROR: %d | MCD_NORMAL: %d", MCD_SEND, MCD_RECEIVE, MCD_ERROR, MCD_NORMAL);
	g_RechargeMoney->RechargeMoneyMaxPage = g_RechargeMoney->TotalItemCount / g_RechargeMoney->RechargeMoneyPageNumber;
	g_RechargeMoney->RechargeMoneyMaxPage++;
}

void PLCRechargeMoney::QrCode()
{
	#ifdef CONSOLE_DEBUG
		g_ConsoleDebug->Write(MCD_NORMAL,
			"QrCode - TransferContent in paymentDataInfo: '%s' (Length: %d)",
			paymentDataInfo.TransferContent,
			strlen(paymentDataInfo.TransferContent));
	#endif

	json paymentData;
	paymentData["accountNo"] = gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankAccountNo;
	paymentData["acqId"] = gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankID;
	paymentData["amount"] = (paymentDataInfo.Amount - paymentDataInfo.AmountPaid);
	paymentData["addInfo"] = paymentDataInfo.TransferContent;  // Giá trị này sẽ hiển thị trong QR

	#ifdef CONSOLE_DEBUG
		g_ConsoleDebug->Write(MCD_NORMAL,
			"QrCode - JSON addInfo: %s",
			paymentData["addInfo"].dump().c_str());
	#endif

	const char* qrData = gameGuard.BuildQRFromJsonData(paymentData.dump().c_str());
	if (qrData && strlen(qrData) > 0) {

		LogoImage image;
		image = gameGuard.LoadLogoWithWIC("Data/PL/RechargeMoney/logo.png");
		std::string qrDataURL = gameGuard.RenderQRToBase64(qrData, &image);
		gameGuard.LoadTextureFromMemory(qrDataURL.c_str());
		#ifdef CONSOLE_DEBUG
			g_ConsoleDebug->Write(MCD_WARNING, "TEXTURE_QRCODE: %d", gameGuard.TEXTURE_QRCODE);
		#endif
	}
}

SEASON3B::PLCRechargeMoneyDetail::PLCRechargeMoneyDetail()
{
	Release();
}

bool SEASON3B::PLCRechargeMoneyDetail::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN, this);

		m_Pos.y = 80.0;
		m_Pos.x = (MAX_WIN_WIDTH / 2) - (WINDOW_WIDTH / 2);
		g_RechargeMoneyDetail->Show(false);
		Success = true;
	}
	return Success;
}

bool SEASON3B::PLCRechargeMoneyDetail::Render()
{
	EnableAlphaTest();
	//EnableAlphaBlend(); // Quan trọng để xử lý độ trong suốt và màu sắc
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Reset màu về trắng nguyên bản để texture không bị ám màu khác
	int WindowW = 0;
	int WindowH = 0;
	if (g_RechargeMoney->paymentDataInfo.Status == 0 || g_RechargeMoney->paymentDataInfo.Status == 2) {
		WindowW = 440;
		WindowH = 300;
	}
	else {
		WindowW = 220;
		WindowH = 190;
	}

	float InitialX = (MAX_WIN_WIDTH / 2) - (WindowW / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (WindowH / 2);

	g_PLInterface->RenderNormalMain(InitialX, InitialY, WindowW, WindowH, "Thông tin giao dịch");




	if (g_RechargeMoney->paymentDataInfo.Status == 0 || g_RechargeMoney->paymentDataInfo.Status == 2) {
		m_CloseMainDetailButton.Render();
		m_ButtonCancelPayment.Render();
		gInterface->DrawInfoBox(InitialX + 215, InitialY + 42, 200, 200, 0x00000096, 0, 0);
		if (gameGuard.TEXTURE_QRCODE != 0) {
			RenderBitmap(gameGuard.TEXTURE_QRCODE, InitialX + 218, InitialY + 43, 203, 203, 0.0, 0.0, 0.95f, 0.95f, 1, 1, 0.0);
		}
		gInterface->DrawInfoBox(InitialX + 17, InitialY + 42, 180, 75, 0x00000096, 0, 0);
		gPLFont->RenderText(g_hFontBold, InitialX + 19, InitialY + 42, whiteColor, redColor, 185, 0, 3, "Thông tin giao dịch");
		//gInterface.DrawGUI(ePoint, InitialX + 20, InitialY + 55);

		if (g_RechargeMoney->paymentDataInfo.Amount > 0) {
			gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 55, whiteColor, noColor, 165, 0, 1, "Số tiền: %sđ", gInterface->NumberFormat(g_RechargeMoney->paymentDataInfo.Amount)); // Số tiền: %sđ
		}

		if (g_RechargeMoney->paymentDataInfo.DateCreated > 0) {
			int mYear = 0, mMonth = 0, mDays = 0, hours = 0, minutes = 0;
			time_t mTime;
			tm* lpTime = NULL;
			mTime = g_RechargeMoney->paymentDataInfo.DateCreated;
			lpTime = localtime(&mTime);
			if (lpTime)
			{
				mYear = 1952 + lpTime->tm_year;
				mMonth = 1 + lpTime->tm_mon;
				mDays = lpTime->tm_mday;
				hours = lpTime->tm_hour;
				minutes = lpTime->tm_min;
			}
			gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 70, whiteColor, noColor, 165, 0, 1, "Ngày tạo: %02d:%02d %02d/%02d/%d", hours, minutes, mDays, mMonth, mYear); // Ngày tạo: %02d:%02d %02d/%02d/%d
		}

		char* strStatus;
		ColorRGBA statusColor;
		if (g_RechargeMoney->paymentDataInfo.Status == 0) {
			strStatus = "Chờ thanh toán"; // Chờ thanh toán
			statusColor = orangeColor;
		}
		else if (g_RechargeMoney->paymentDataInfo.Status == 2) {
			strStatus = "Thanh toán thiếu"; // Thanh toán thiếu
			statusColor = orangeColor;
		}
		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 85, whiteColor, noColor, 55, 0, 1, "Tình trạng:"); // Tình trạng:
		gPLFont->RenderText(g_hFont, InitialX + 35 + 55, InitialY + 85, whiteColor, noColor, 110, 0, 1, "%s", strStatus); // Tình trạng:

		if (g_RechargeMoney->paymentDataInfo.Status == 2 && g_RechargeMoney->paymentDataInfo.AmountPaid > 0) {
			gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 100, whiteColor, noColor, 85, 0, 1, "Số tiền còn thiếu:"); // Số tiền còn thiếu:
			gPLFont->RenderText(g_hFont, InitialX + 35 + 85, InitialY + 100, statusColor, noColor, 80, 0, 3, "%sđ", gInterface->NumberFormat((g_RechargeMoney->paymentDataInfo.Amount - g_RechargeMoney->paymentDataInfo.AmountPaid))); // Số tiền: %sđ
		}
		/*=================================================================================================*/
		gInterface->DrawInfoBox(InitialX + 17, InitialY + 135, 180, 143, 0x00000096, 0, 0);
		gPLFont->RenderText(g_hFontBold, InitialX + 19, InitialY + 135, whiteColor, redColor, 185, 0, 3, "Thông tin chuyển khoản"); //Thông tin chuyển khoản
		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 148, whiteColor, noColor, 50, 0, 1, "Ngân hàng:"); //Ngân hàng:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 50, InitialY + 148, eExcellentS15, noColor, 120, 0, 1, gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankName);

		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 163, whiteColor, noColor, 70, 0, 1, "Ngân hàng:"); //Chủ tài khoản:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 70, InitialY + 163, whiteColor, noColor, 95, 0, 1, gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankAccountName);


		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 178, whiteColor, noColor, 65, 0, 1, "Số tài khoản :"); //Số tài khoản:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 65, InitialY + 178, eExcellentS15, noColor, 100, 0, 1, gPLSystemConfigAndMemberInfo.m_SystemConfigData.BankAccountNo);

		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 193, whiteColor, noColor, 40, 0, 1, "Số tiền:"); // Số tiền:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 40, InitialY + 193, eExcellentS15, noColor, 120, 0, 1, "%dđ", (g_RechargeMoney->paymentDataInfo.Amount - g_RechargeMoney->paymentDataInfo.AmountPaid));

		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 208, whiteColor, noColor, 47, 0, 1, "Nội dung:"); // Nội dung:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 47, InitialY + 208, eExcellentS15, noColor, 113, 0, 1, "%s", g_RechargeMoney->paymentDataInfo.TransferContent);

		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 223, eGoldColor, noColor, 165, 0, 1, "Lưu ý: Sau khi quét mã QR, bạn hãy"); // Lưu ý: Sau khi quét mã QR, bạn hãy
		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 223 + 15, eGoldColor, noColor, 165, 0, 1, "kiểm tra kỹ nội dung nạp tiền và"); // kiểm tra kỹ nội dung nạp tiền và
		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 223 + 30, eGoldColor, noColor, 165, 0, 1, "thông tin chuyển khoản này trùng"); // thông tin chuyển khoản này trùng
		gPLFont->RenderText(g_hFont, InitialX + 35, InitialY + 223 + 45, eGoldColor, noColor, 165, 0, 1, "khớp thì giao dịch này mới hợp lệ."); // khớp thì giao dịch này mới hợp lệ.

		gInterface->SetButtonInfo(&m_ButtonCancelPayment, PL_INTERFACE::BUTTON_NORMAL_73_27, InitialX + 287, InitialY + 258, 73, 27, true, false, true, true, "Hủy giao dịch", "", false);
		gInterface->SetButtonInfo(&m_CloseMainDetailButton, PL_INTERFACE::BUTTON_CLOSE_MAIN_29_29, InitialX + WindowW - 40, InitialY - 2, 29, 29, FALSE, FALSE, FALSE, FALSE, "", "Đóng", TRUE);
		//if (pCheckMouseOver(InitialX + 270, InitialY + 258, 108, 29))
		//{
		//	if (*(BYTE*)0x8793386)
		//	{
		//		gInterface.RenderButton(eCancelPaymentRequest, InitialX + 270, InitialY + 258, eRed, gPLText.RenderText(492)); // Hủy giao dịch
		//		if (GetTickCount() >= ClickTick + 200)
		//		{
		//			if (!gRechargeMoney.CheckPaymentCancelWindow()) {
		//				gRechargeMoney.OpenPaymentCancelWindow();
		//			}
		//			ClickTick = GetTickCount();
		//		}
		//	}
		//	else
		//	{
		//		gInterface.RenderButton(eCancelPaymentRequest, InitialX + 270, InitialY + 258, eRed, gPLText.RenderText(492)); // Hủy giao dịch
		//	}
		//}
		//else
		//{
		//	gInterface.RenderButton(eCancelPaymentRequest, InitialX + 270, InitialY + 258, eRed, gPLText.RenderText(492)); // Hủy giao dịch
		//}
	}
	else {
		m_CloseMainDetailButton.Render();
		gInterface->DrawInfoBox(InitialX + 17, InitialY + 42, 177, 120, 0x00000096, 0, 0);
		char* strStatus;
		ColorRGBA statusColor;
		if (g_RechargeMoney->paymentDataInfo.Status == 1) {
			strStatus = "Đã thanh toán"; // Đã thanh toán
			statusColor = excellentS15Color;
		}
		else if (g_RechargeMoney->paymentDataInfo.Status == 3) {
			strStatus = "Đã hết hạn"; // Đã hết hạn
			statusColor = redColor;
		}
		else if (g_RechargeMoney->paymentDataInfo.Status == 4) {
			strStatus = "Đang xử lý"; // Đang xử lý
			statusColor = eYellowColor;
		}
		else if (g_RechargeMoney->paymentDataInfo.Status == 5) {
			strStatus = "Thất bại"; // Thất bại
			statusColor = redColor;
		}
		else if (g_RechargeMoney->paymentDataInfo.Status == 6) {
			strStatus = "Đã hủy"; // Đã hủy
			statusColor = redColor;
		}

		if (g_RechargeMoney->paymentDataInfo.Amount > 0) {
			gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 50, whiteColor, noColor, 165, 0, 1, "Số tiền: %dđ", g_RechargeMoney->paymentDataInfo.Amount); // Số tiền: %sđ
		}

		gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 65, whiteColor, noColor, 70, 0, 1, "Đã thanh toán:"); // Đã thanh toán:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 70, InitialY + 65, statusColor, noColor, 95, 0, 1, "%dđ", g_RechargeMoney->paymentDataInfo.AmountPaid); // Đã thanh toán:

		string bonusStr = to_string(g_RechargeMoney->paymentDataInfo.AddBonus) + "%";
		const char* AddBonusStr = bonusStr.c_str(); // Lấy con trỏ C-style (read-only)
		gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 80, whiteColor, noColor, 55, 0, 1, "Khuyến mãi:"); // Khuyến mãi:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 55, InitialY + 80, statusColor, noColor, 110, 0, 1, "%s", AddBonusStr); // Khuyến mãi:

		if (g_RechargeMoney->paymentDataInfo.DateCreated > 0) {
			int mYear = 0, mMonth = 0, mDays = 0, hours = 0, minutes = 0;
			time_t mTime;
			tm* lpTime = NULL;
			mTime = g_RechargeMoney->paymentDataInfo.DateCreated;
			lpTime = localtime(&mTime);
			if (lpTime)
			{
				mYear = 1952 + lpTime->tm_year;
				mMonth = 1 + lpTime->tm_mon;
				mDays = lpTime->tm_mday;
				hours = lpTime->tm_hour;
				minutes = lpTime->tm_min;
			}
			gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 95, whiteColor, noColor, 165, 0, 1, "Ngày tạo: %02d:%02d %02d/%02d/%d", hours, minutes, mDays, mMonth, mYear); // Ngày tạo: %02d:%02d %02d/%02d/%d
		}
		gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 110, whiteColor, noColor, 55, 0, 1, "Tình trạng:"); // Tình trạng:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 55, InitialY + 110, statusColor, noColor, 110, 0, 1, "%s", strStatus);


		gPLFont->RenderText(g_hFontBold, InitialX + 35, InitialY + 125, whiteColor, noColor, 70, 0, 1, "Phần thưởng:"); // Phần thưởng:
		gPLFont->RenderText(g_hFontBold, InitialX + 35 + 70, InitialY + 125, statusColor, noColor, 95, 0, 1, "%d WC", g_RechargeMoney->paymentDataInfo.Wcoin);
		gInterface->SetButtonInfo(&m_CloseMainDetailButton, PL_INTERFACE::BUTTON_CLOSE_MAIN_29_29, InitialX + WindowW - 40, InitialY - 2, 29, 29, FALSE, FALSE, FALSE, FALSE, "", "Đóng", TRUE);
	}

	DisableAlphaBlend();

	return true;
}

void SEASON3B::PLCRechargeMoneyDetail::Release()
{
	m_pNewUIMng = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	WINDOW_WIDTH = 613;
	WINDOW_HEIGHT = 370;

	BUTTON_WIDTH = 72;
	BUTTON_HEIGHT = 27;
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

bool SEASON3B::PLCRechargeMoneyDetail::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN);
			return false;
		}
	}
	return true;
}

bool SEASON3B::PLCRechargeMoneyDetail::UpdateMouseEvent()
{
	int WindowW = 0;
	int WindowH = 0;
	if (g_RechargeMoney->paymentDataInfo.Status == 0 || g_RechargeMoney->paymentDataInfo.Status == 2) {
		WindowW = 440;
		WindowH = 300;
	}
	else {
		WindowW = 220;
		WindowH = 190;
	}

	float InitialX = (MAX_WIN_WIDTH / 2) - (WindowW / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (WindowH / 2);
	// Nếu chuột nằm trong vùng khung menu
	if (SEASON3B::CheckMouseIn(InitialX, InitialY, WindowW, WindowH))
	{
		// Sự kiện của nút hủy giao dịch
		if (m_ButtonCancelPayment.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			g_pNewUISystem->Show(INTERFACE_RECHARGE_MONEY_CANCEL_PAYMENT_MAIN);
		}

		// Sự kiện của nút đóng main
		if (m_CloseMainDetailButton.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN);
		}
	}

	return !SEASON3B::CheckMouseIn(InitialX, InitialY, WindowW, WindowH);
}

bool SEASON3B::PLCRechargeMoneyDetail::Update()
{
	return true;
}

float SEASON3B::PLCRechargeMoneyDetail::GetLayerDepth()
{
	return 10.1;
}

void SEASON3B::PLCRechargeMoneyDetail::OpenningProcess()
{

}

void SEASON3B::PLCRechargeMoneyDetail::ClosingProcess()
{

}

SEASON3B::PLCRechargeMoneyPaymentCancel::PLCRechargeMoneyPaymentCancel()
{
	Release();
}

bool SEASON3B::PLCRechargeMoneyPaymentCancel::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RECHARGE_MONEY_CANCEL_PAYMENT_MAIN, this);

		m_Pos.y = 80.0;
		m_Pos.x = (MAX_WIN_WIDTH / 2) - (WINDOW_WIDTH / 2);
		g_RechargeMoneyCancelPayment->Show(false);
		Success = true;
	}
	return Success;
}

bool SEASON3B::PLCRechargeMoneyPaymentCancel::Render()
{
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	const int CHARS_PER_LINE = 55; // Số ký tự mỗi dòng

	float MainWidth = WINDOW_WIDTH;
	float MainHeight = WINDOW_HEIGHT;

	float InitialX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (MainHeight / 2);

	// Vẽ popup
	g_PLInterface->RenderNormalMain(InitialX, InitialY, MainWidth, MainHeight, "Thông báo hệ thống");

	// Vẽ nội dung
	gPLFont->DrawFormatWordWrap(redColor, InitialX + 10, InitialY + 48, MainWidth - 20, 3, "Bạn có muốn hủy giao dịch này không?", CHARS_PER_LINE);

	// Button
	float buttonWidth = 73.0f;
	float buttonHeight = 27.0f;
	float buttonX = InitialX + (MainWidth / 2) - (buttonWidth / 2);
	float buttonY = InitialY + MainHeight - buttonHeight - 15;

	m_CancelPayment.Render();
	m_CloseCancelPaymentMain.Render();
	gInterface->SetButtonInfo(&m_CancelPayment, PL_INTERFACE::BUTTON_NORMAL_73_27, buttonX, buttonY, buttonWidth, buttonHeight, true, false, true, true, "Tiến hành hủy", "", false);
	gInterface->SetButtonInfo(&m_CloseCancelPaymentMain, PL_INTERFACE::BUTTON_CLOSE_MAIN_29_29, InitialX + MainWidth - 40, InitialY - 2, 29, 29, FALSE, FALSE, FALSE, FALSE, "", "Đóng", TRUE);

	DisableAlphaBlend();
	return true;
}

void SEASON3B::PLCRechargeMoneyPaymentCancel::Release()
{
	m_pNewUIMng = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	WINDOW_WIDTH = 200;
	WINDOW_HEIGHT = 120;

	BUTTON_WIDTH = 72;
	BUTTON_HEIGHT = 27;
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

bool SEASON3B::PLCRechargeMoneyPaymentCancel::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_CANCEL_PAYMENT_MAIN);
			return false;
		}
	}
	return true;
}

bool SEASON3B::PLCRechargeMoneyPaymentCancel::UpdateMouseEvent()
{
	float MainWidth = WINDOW_WIDTH;
	float MainHeight = WINDOW_HEIGHT;

	float InitialX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (MainHeight / 2);
	// Button
	float buttonWidth = 73.0f;
	float buttonHeight = 27.0f;
	float buttonX = InitialX + (MainWidth / 2) - (buttonWidth / 2);
	float buttonY = InitialY + MainHeight - buttonHeight - 15;

	// Nếu chuột nằm trong vùng khung menu
	if (SEASON3B::CheckMouseIn(InitialX, InitialY, MainWidth, MainHeight))
	{
		// Sự kiện của nút hủy giao dịch
		if (m_CancelPayment.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			PSBMSG_RECHARGEMONEY_CANCEL_PAYMENT_SEND pMsg;
			pMsg.OrderCode = g_RechargeMoney->paymentDataInfo.OrderCode;
			pMsg.header.set(0x80, 0x03, sizeof(pMsg));
			if (pMsg.OrderCode > 0) {
				DataSend((BYTE*)&pMsg, pMsg.header.size);
			}
			g_RechargeMoney->GetMyPayment();
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_DETAIL_MAIN);
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_CANCEL_PAYMENT_MAIN);
		}

		// Sự kiện của nút đóng main
		if (m_CloseCancelPaymentMain.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			g_pNewUISystem->Hide(INTERFACE_RECHARGE_MONEY_CANCEL_PAYMENT_MAIN);
		}
	}

	return !SEASON3B::CheckMouseIn(InitialX, InitialY, MainWidth, MainHeight);
}

bool SEASON3B::PLCRechargeMoneyPaymentCancel::Update()
{
	return true;
}

float SEASON3B::PLCRechargeMoneyPaymentCancel::GetLayerDepth()
{
	return 10.5;
}

void SEASON3B::PLCRechargeMoneyPaymentCancel::OpenningProcess()
{

}

void SEASON3B::PLCRechargeMoneyPaymentCancel::ClosingProcess()
{

}
#endif // #USING_RECHARGE_MONEY