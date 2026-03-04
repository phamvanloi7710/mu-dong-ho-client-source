#include "stdafx.h"
#include "PL_Notification.h"
#include <UIMng.h>
#include <NewUISystem.h>
#include <PL/PL_Font.h>
using namespace SEASON3B;
SEASON3B::PLCNotification::PLCNotification()
{
	Release();
}

bool SEASON3B::PLCNotification::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_NOTIFICATION_MAIN, this);
		m_Pos.y = 80.0;
		m_Pos.x = (MAX_WIN_WIDTH / 2) - (WINDOW_WIDTH / 2);
		gPLNotification->Show(false);
		Success = true;
	}
	return Success;
}

bool SEASON3B::PLCNotification::Render()
{
    EnableAlphaTest();
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	// = 235;
	//WINDOW_HEIGHT = 140;
    // Các hằng số cấu hình
    const float MIN_HEIGHT = 100.0f;
    const float MAX_HEIGHT = 300.0f;
    const float BASE_HEIGHT = 200.0f;
    const float PADDING_TOP = 35.0f;
    const float PADDING_BOTTOM = 20.0f;
    const float LINE_HEIGHT = 20.0f;
    const int CHARS_PER_LINE = 55; // Số ký tự mỗi dòng

    float MainWidth = WINDOW_WIDTH;
    float MainHeight = BASE_HEIGHT;

    if (this->Content[0] != 0) {
        // Tính số dòng đơn giản dựa trên độ dài chuỗi
        int textLength = strlen(this->Content);
        int estimatedLines = (textLength / CHARS_PER_LINE) + 1;

        // Đếm số dòng thực tế có ký tự xuống dòng
        int actualLines = 1;
        for (int i = 0; i < textLength; i++) {
            if (this->Content[i] == '\n') {
                actualLines++;
            }
        }

        // Lấy số dòng lớn hơn
        int totalLines = max(estimatedLines, actualLines);

        // Tính chiều cao
        MainHeight = PADDING_TOP + (totalLines * LINE_HEIGHT) + PADDING_BOTTOM;

        // Giới hạn chiều cao
        if (MainHeight < MIN_HEIGHT) MainHeight = MIN_HEIGHT;
        if (MainHeight > MAX_HEIGHT) MainHeight = MAX_HEIGHT;
    }

    float InitialX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
    float InitialY = (MAX_WIN_HEIGHT / 2) - (MainHeight / 2);

    if (this->Content[0] != 0) {
        // Vẽ popup
        SEASON3B::window_backmsg(InitialX, InitialY, MainWidth, MainHeight);

        // Tiêu đề
        gPLFont->RenderText(g_hFontBold, InitialX, InitialY + 7, eGoldColor, noColor,
            MainWidth, 0, 3, "Thông báo hệ thống");

        // Tính vị trí nội dung để căn giữa theo chiều dọc
        float contentHeight = MainHeight - PADDING_TOP - PADDING_BOTTOM;
        float contentPosY = InitialY + PADDING_TOP;

        // Nếu nội dung ngắn, căn giữa đẹp hơn
        if (contentHeight > 100) {
            contentPosY = InitialY + (MainHeight / 2) - (contentHeight / 2);
        }

        // Vẽ nội dung
        gPLFont->DrawFormatWordWrap(whiteColor, InitialX + 10, contentPosY,
            MainWidth - 20, 3, this->Content, CHARS_PER_LINE);

        // Button
        float buttonWidth = 73.0f;
        float buttonHeight = 27.0f;
        float buttonX = InitialX + (MainWidth / 2) - (buttonWidth / 2);
        float buttonY = InitialY + MainHeight - buttonHeight - 15;

        m_ButtonCloseMain.Render();
        gInterface->SetButtonInfo(&m_ButtonCloseMain, PL_INTERFACE::BUTTON_NORMAL_73_27,
            buttonX, buttonY, buttonWidth, buttonHeight,
            true, false, true, true, "Đóng lại", "", false);
    }

    DisableAlphaBlend();
    return true;
}

void SEASON3B::PLCNotification::Release()
{
	m_pNewUIMng = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	WINDOW_WIDTH = 235;
	WINDOW_HEIGHT = 140;

	BUTTON_WIDTH = 72;
	BUTTON_HEIGHT = 27;
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}

	strncpy(this->Content, "", 0);
}

bool SEASON3B::PLCNotification::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_NOTIFICATION_MAIN);
			return false;
		}
	}
	return true;
}

bool SEASON3B::PLCNotification::UpdateMouseEvent()
{
	float MainWidth = WINDOW_WIDTH;
	float MainHeight = WINDOW_HEIGHT;
	float InitialX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
	float InitialY = (MAX_WIN_HEIGHT / 2) - (MainHeight / 2);
	// Nếu chuột nằm trong vùng khung menu
	if (SEASON3B::CheckMouseIn(InitialX, InitialY, MainWidth, MainHeight))
	{
		// Sự kiện của nút đóng main
		if (m_ButtonCloseMain.UpdateMouseEvent())
		{
			PlayBuffer(SOUND_CLICK01);
			g_pNewUISystem->Hide(INTERFACE_NOTIFICATION_MAIN);
		}
	}

	return !SEASON3B::CheckMouseIn(InitialX, InitialY, MainWidth, MainHeight);
}

bool SEASON3B::PLCNotification::Update()
{
	return true;
}

float SEASON3B::PLCNotification::GetLayerDepth()
{
	return 100;
}

void SEASON3B::PLCNotification::OpenningProcess()
{

}

void SEASON3B::PLCNotification::ClosingProcess()
{

}

void PLCNotification::OpenNotificationWindow(BYTE* ReceiveBuffer)
{
	LPMSG_GC_NOTIFICATION_DATA_RECV lpMsg = (LPMSG_GC_NOTIFICATION_DATA_RECV)ReceiveBuffer;
	memset(this->Content, 0, sizeof(this->Content));
	if ((unsigned char)lpMsg->Content[0] == 0xC1) {
		// Copy từ vị trí thứ 2
		strncpy(this->Content, lpMsg->Content + 1, sizeof(this->Content));
	}
	else {
		strncpy(this->Content, lpMsg->Content, sizeof(this->Content));
	}
	g_pNewUISystem->Show(INTERFACE_NOTIFICATION_MAIN);
}

void PLCNotification::ShowNotification(char Content[255])
{
	if (Content[0] != 0) {
		memset(this->Content, 0, sizeof(this->Content));
		strncpy(this->Content, Content, sizeof(this->Content));
		g_pNewUISystem->Show(INTERFACE_NOTIFICATION_MAIN);
	}
}
 
 
//PLNotification::PLNotification()
//{
//	strncpy(this->Content, "", 0);
//}
//
//void PLNotification::RenderCloseNotificationMainButton(int IMGcode, float x, float y, float w, float h, float uw, float uh, float a8, float a9)
//{
//	if (gInterface.IsCursorInZone((int)x, (int)y, (int)w - 17, (int)h - 15))
//	{
//		if (GetKeyState(VK_LBUTTON) & 0x8000)
//		{
//			RenderBitmap(IMGcode, x, y, w, h, 0.0, uh / a9 * 2.0, uw / a8, uh / a9, 0, 1, 0.0);
//			memset(this->Content, 0, sizeof(this->Content));
//			gInterface.Data[eNotificationMain].OnShow = FALSE;
//		}
//		else
//		{
//			RenderBitmap(IMGcode, x, y, w, h, 0.0, uh / a9, uw / a8, uh / a9, 0, 1, 0.0);
//		}
//		return;
//	}
//	else
//	{
//		RenderBitmap(IMGcode, x, y, w, h, 0.0, 0.0, uw / a8, uh / a9, 0, 1, 0.0);
//	}
//	return;
//}
//
//void PLNotification::OpenNotificationWindow(GC_NOTIFICATION_DATA_RECV* recvData)
//{
//	if (recvData->Content[0] != 0) {
//		gInterface.Data[eNotificationMain].OnShow = TRUE;
//		memset(this->Content, 0, sizeof(this->Content));
//		strncpy(this->Content, recvData->Content, sizeof(this->Content) - 1);
//	}
//}
//
//void PLNotification::ShowNotification(char Content[255])
//{
//	if (Content[0] != 0) {
//		gInterface.Data[eNotificationMain].OnShow = TRUE;
//		memset(this->Content, 0, sizeof(this->Content));
//		strncpy(this->Content, Content, sizeof(this->Content) - 1);
//	}
//}
//
//void PLNotification::DrawNotificationPopup() {
//	if (gInterface.Data[eNotificationMain].OnShow == FALSE)
//	{
//		return;
//	}
//	if (this->CheckNotificationWindow() && this->Content[0] != 0) {
//		int WindowW = 228;
//		int WindowH = 90;
//		int DisplayHeight = (int)((double)pWinHeight / pWinHeightReal);
//		float InitialY = ((DisplayHeight - 51) / 2) - (WindowH / 2);
//#if WIDE_SCREEN
//		float InitialX = Fix_DisplayWinMid - (WindowW / 2);
//#else
//		float InitialX = WindowW / 2;
//#endif
//
//
//		gInterface.DrawIMG(eNotificationMain, InitialX, InitialY, 0.75, 1);
//		gPLNotification.RenderCloseNotificationMainButton(0x7A5B, InitialX + (WindowW / 2) - 20, InitialY + WindowH - 33, 60.0, 36.0, 13.0, 15.0, 16.0, 64.0);
//		float contentPosY = ((InitialY + WindowH / 2) - 20);
//		if (strlen(this->Content) <= 60) {
//			contentPosY = contentPosY + 10;
//		}
//		gInterface.DrawFormatWordWrap(eWhite, InitialX, contentPosY, WindowW, 3, this->Content, 60);
//		pSetCursorFocus = true;
//	}
//}
