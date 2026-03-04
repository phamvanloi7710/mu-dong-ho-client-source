#include "stdafx.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "DSPlaySound.h"
#include "CBInterface.h"
#include "CustomEventTime.h"
#include "NewUIMenuUser.h"
#include "NewUIScrollBar.h" // <-- thêm include cho ScrollBar
#include <CB_HuyDongExc.h>
#if USING_RECHARGE_MONEY
	#include <PL/PL_RechargeMoney.h>
#endif // USING_RECHARGE_MONEY

SEASON3B::CNewUIMenuUser::CNewUIMenuUser()
{
	m_pNewUIMng = nullptr;
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_Button = nullptr;
	for (int i = 0; i < MAX_BUTTON_CREATE; ++i)
	{
		m_ButtonVisible[i] = false;
	}

	WINDOW_WIDTH = 251;
	WINDOW_HEIGHT = 310;

	BUTTON_WIDTH = 108;
	BUTTON_HEIGHT = 29;
}

SEASON3B::CNewUIMenuUser::~CNewUIMenuUser()
{
	Release();
}

bool SEASON3B::CNewUIMenuUser::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_CUSTOM_MENU, this);

		float RenderFrameX = pos_center() + 190;
		float RenderFrameY = PositionY_In_The_Mid(0) + y;

		m_Pos.x = RenderFrameX;
		m_Pos.y = RenderFrameY;

		this->LoadImages();
		
		/*m_Pos.x = x;
		m_Pos.y = y;*/

		InitButtons();

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIMenuUser::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();
	}
}

bool SEASON3B::CNewUIMenuUser::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);
	// --- Vẽ nền ---
	float ScaleW = (225 / WINDOW_WIDTH) / g_fScreenRate_x;
	float ScaleH = (225 / WINDOW_HEIGHT) / g_fScreenRate_y;
	window_backmsg(m_Pos.x, m_Pos.y, WINDOW_WIDTH, WINDOW_HEIGHT);

	// --- Vẽ tiêu đề (không bị cắt) ---
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(CLRDW_WHITE);
	// Dòng tiêu đề "System Menu"
	g_pRenderText->RenderFont(m_Pos.x, m_Pos.y + 7, GlobalText[3450], WINDOW_WIDTH - 10, 0, RT3_SORT_CENTER); // System Menu

	for (int x = 0; x < MAX_BUTTON_CREATE; x++)
	{
		m_Btn[x].Render();
	}

	DisableAlphaBlend();
	return true;
}

void SEASON3B::CNewUIMenuUser::LoadImages()
{
	LoadBitmap("Interface\\HUD\\top_back_3.tga", IMAGE_TOP_BACK3, GL_LINEAR);
}

void SEASON3B::CNewUIMenuUser::UnloadImages()
{
	DeleteBitmap(IMAGE_TOP_BACK3);
}

bool SEASON3B::CNewUIMenuUser::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
			return false;
		}
	}
	return true;
}

void SEASON3B::CNewUIMenuUser::InitButtons()
{
	for (int i = 0; i < MAX_BUTTON_CREATE; ++i)
	{
		//m_ButtonVisible[i] = gProtect->m_MainInfo.MenuButton[i];
		m_ButtonVisible[i] = 1;
	}

	int visibleIndex = 0;

	for (int i = 0; i < MAX_BUTTON_CREATE; ++i)
	{
		if (!m_ButtonVisible[i])
		{
			m_Btn[i].ChangeButtonInfo(0, 0, 0, 0);
			continue;
		}

		int row = visibleIndex / 2;
		bool isLeft = (visibleIndex % 2 == 0);

		int x = isLeft ? (m_Pos.x + 17) : (m_Pos.x + 128);
		int y = m_Pos.y + 35 + row * 30;

		gInterface->SetButtonInfo(
			&m_Btn[i],
			PL_INTERFACE::BUTTON_NORMAL_108_29,
			x,
			y,
			BUTTON_WIDTH,
			BUTTON_HEIGHT,
			true,
			false,
			true,
			true,
			TextGlobal[25 + i],
			"",
			false
		);

		visibleIndex++;
	}
}

bool SEASON3B::CNewUIMenuUser::UpdateMouseEvent()
{
	// Nếu chuột nằm trong vùng khung menu
	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		int visibleIndex = 0;
		for (int i = 0; i < MAX_BUTTON_CREATE; i++)
		{
			int row = visibleIndex / 2;
			bool isLeft = (visibleIndex % 2 == 0);
			int x = isLeft ? (m_Pos.x + 17) : (m_Pos.x + 128);
			int y = m_Pos.y + 35 + row * 30;
			if (m_Btn[i].UpdateMouseEvent())
			{
				// --- Sự kiện
				if (gmProtect->MenuButtonEventTime && i == 0)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_CustomEventTime->OnOffWindow();
						g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// --- Mua VIP
				if (gmProtect->MenuButtonVipShop && i == 1)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						if (gmProtect->WindowsVipShop)
							g_pNewUISystem->Show(INTERFACE_SHOW_VIP);

						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// --- Bảng xếp hạng
				if (gmProtect->MenuButtonRankTop && i == 2)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						if (gmProtect->WindowsRankTop)
							g_pNewUISystem->Show(INTERFACE_RANKING_TOP);

						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// --- Lệnh Ingame
				if (gmProtect->MenuButtonCommand && i == 3)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						//gCBHuyDongExc->OpenWindow();
						if (gmProtect->WindowsCommand)
							g_pNewUISystem->Show(INTERFACE_COMMAND_LIST);

						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// --- Options
				if (gmProtect->MenuButtonOptions && i == 4)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
						g_pNewUISystem->Show(INTERFACE_OPTION);
						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// Mốc Nạp
				if (gmProtect->MenuButtonMocNap && i == 5)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_pNewUIMocNap->Show(true);                  // ✅ Bật UI Mốc Nạp
						g_pNewUIMocNap->IsVisibleWindow();           // ✅ Gửi request + load data
						g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU); // Ẩn menu chính
						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				// Vòng quay
				if (gmProtect->MenuButtonVongQuay && i == 6)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_pCBInterface->OpenWindowVongQuay();        // ✅ Gọi hàm mở vòng quay
						g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU); // Ẩn menu chính
						PlayBuffer(SOUND_CLICK01);                   // Âm thanh click
						return false;
					}
				}


				// --- Lệnh Ingame
				if (gmProtect->MenuButtonHuyExl && i == 7)
				{
					if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
						&& SEASON3B::IsRelease(VK_LBUTTON))
					{
						gCBHuyDongExc->OpenWindow();
						//if (gmProtect->WindowsCommand)
						//	g_pNewUISystem->Show(INTERFACE_COMMAND_LIST);

						PlayBuffer(SOUND_CLICK01);
						return false;
					}
				}

				#if USING_RECHARGE_MONEY
					// --- Nạp tiền
					if (gmProtect->MenuButtonRechargeMoney && i == 8)
					{
						if (SEASON3B::CheckMouseIn(x, y, BUTTON_WIDTH, BUTTON_HEIGHT)
							&& SEASON3B::IsRelease(VK_LBUTTON))
						{
							g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
							g_pNewUISystem->Show(INTERFACE_RECHARGE_MONEY_SYSTEM);
							return false;
						}
					}
				#endif // USING_RECHARGE_MONEY
				
			}
			visibleIndex++;
		}
	}

	return !SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, 214.f, 230.f);
}

bool SEASON3B::CNewUIMenuUser::Update()
{
	return true;
}

float SEASON3B::CNewUIMenuUser::GetLayerDepth()
{
	return 10.0f;
}
