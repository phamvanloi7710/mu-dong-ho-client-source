#include "stdafx.h"
#if USING_RESET_WITH_GUI
#include "NewUISystem.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include <PL/PL_ResetManager.h>
#include "wsclientinline.h"
#include "PL_Interface.h"
#include "PL_Font.h"


SEASON3B::CPLResetSystem::CPLResetSystem()
{
	Release();
	/*MainWidth = 223;
	MainHeight = 272;

	StartX = 0;
	StartY = 0;*/
}

bool SEASON3B::CPLResetSystem::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RESET_SYSTEM, this);

		m_Pos.y = 80.0;
		m_Pos.x = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);
		//g_PLResetManager->Show(false);
		Success = true;
	}
	return Success;
}

#include "_enum.h"

void SEASON3B::CPLResetSystem::RenderButtons()
{
	if (gmProtect->WindowsResetInfo)
	{
		float iRenderFrameX = GetWindowsX - 110;
		float iRenderFrameY = GetWindowsY - 82;

		/*if ((int)iRenderFrameX > GetScreenWidth()
			|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			|| g_pNewUISystem->IsVisible(INTERFACE_WINDOW_MENU)
			)
		{
			return;
		}*/

		SEASON3B::RenderImageF(IMAGE_BTN_SERVER_INTERFACE, iRenderFrameX, iRenderFrameY, 105, 22, 0.0, 0.0, 223.0, 32.0);

		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetTextColor(CLRDW_YELLOW);
		g_pRenderText->RenderText(iRenderFrameX, iRenderFrameY, "Reset", 105, 22, RT3_SORT_CENTER);

		iRenderFrameX -= 100;


		SEASON3B::RenderImageF(IMAGE_BTN_SERVER_INTERFACE, iRenderFrameX, iRenderFrameY, 105, 22, 0.0, 0.0, 223.0, 32.0);

		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetTextColor(CLRDW_YELLOW);
		g_pRenderText->RenderText(iRenderFrameX, iRenderFrameY, "MasterReset", 105, 22, RT3_SORT_CENTER);
	}
}

void SEASON3B::CPLResetSystem::Release()
{
	
}

bool SEASON3B::CPLResetSystem::Render()
{
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	RenderButtons();
	if (gmProtect->WindowsResetInfo)
	{
		if (g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			)
		{
			return true;
		}

		g_PLInterface->RenderNormalMain(StartX, StartY, MainWidth, MainHeight, "Cài đặt auto Reset");
		gPLFont->RenderText((HFONT)g_hFontBold, StartX + 10, StartY + 42, whiteColor, headerTableColor, 100, 0, 4, "Trạng thái: ");
		if (AutoResetStatus) {
			gPLFont->RenderText((HFONT)g_hFontBold, StartX + 115, StartY + 42, excellentS15Color, headerTableColor, 100, 0, 1, "Đang Bật");
		}
		else {
			gPLFont->RenderText((HFONT)g_hFontBold, StartX + 115, StartY + 42, redColor, headerTableColor, 100, 0, 1, "Đang Tắt");
		}

		gPLFont->RenderText((HFONT)g_hFontBold, StartX + 10, StartY + 65, redColor, headerTableColor, 205, 0, 3, "Chọn kiểu Reset");
	}
	DisableAlphaBlend();
	return true;
}
bool SEASON3B::CPLResetSystem::Update()
{
	
	return true;
}

float SEASON3B::CPLResetSystem::GetLayerDepth()
{
	return 10.0f;
}

bool SEASON3B::CPLResetSystem::UpdateKeyEvent()
{
	return false;
}

bool SEASON3B::CPLResetSystem::UpdateMouseEvent()
{
	return false;
}

void SEASON3B::CPLResetSystem::OpenningProcess()
{
	
}

void SEASON3B::CPLResetSystem::ClosingProcess()
{
	//m_ScrollBar->SetCurPos(0);
}

#endif // USING_RESET_WITH_GUI
