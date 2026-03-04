#include "stdafx.h"
#include "NewUISystem.h"
#include "NewUIRankingTop.h"
#include "ZzzTexture.h"
#include "wsclientinline.h"
#include "CharacterManager.h"
#include "Input.h"

SEASON3B::CNewUIRankingTop::CNewUIRankingTop()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;

	is_request = false;
	listInPage = 0;

	m_bDragging = false;
	m_DragOffsetX = 0;
	m_DragOffsetY = 0;

	CountTab = 0;
	m_iCurTab = 0;
	m_ClassTab = 0;
	selectIndex = -1;
	memset(this->m_RankName, 0, sizeof(this->m_RankName));
}

SEASON3B::CNewUIRankingTop::~CNewUIRankingTop()
{
	if (m_ScrollBar)
		m_ScrollBar->Release();

	SAFE_DELETE(m_ScrollBar);

	Release();
}

bool SEASON3B::CNewUIRankingTop::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RANKING_TOP, this);

		m_ScrollBar = new CNewUIScrollBar();
		m_ScrollBar->Create(x + eMainWidth - 25, y + 85, eMainHeight - 110);

		this->LoadImages();

		this->SetPos(x, y);

		this->SetInfo();

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIRankingTop::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();

	}
}

void SEASON3B::CNewUIRankingTop::SetInfo()
{

}

void SEASON3B::CNewUIRankingTop::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;

	if (m_ScrollBar)
		m_ScrollBar->SetPos(x + eMainWidth - 25, y + 85);
}

void SEASON3B::CNewUIRankingTop::LoadImages()
{
	LoadBitmap("Interface\\newui_tab_72x22.tga", IMAGE_TAB_CLASS, GL_LINEAR);

	LoadBitmap("Interface\\HUD\\top_back_1.tga", IMAGE_TOP_BACK1, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\top_back_2.tga", IMAGE_TOP_BACK2, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\VipLevel1.tga", IMAGE_TOP_LEVEL1, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\HUD\\VipLevel2.tga", IMAGE_TOP_LEVEL2, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\HUD\\VipLevel3.tga", IMAGE_TOP_LEVEL3, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
}

void SEASON3B::CNewUIRankingTop::UnloadImages()
{
	DeleteBitmap(IMAGE_TAB_CLASS);
	DeleteBitmap(IMAGE_TOP_BACK1);
	DeleteBitmap(IMAGE_TOP_BACK2);
	DeleteBitmap(IMAGE_TOP_LEVEL1);
	DeleteBitmap(IMAGE_TOP_LEVEL2);
	DeleteBitmap(IMAGE_TOP_LEVEL3);
}

bool SEASON3B::CNewUIRankingTop::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_RANKING_TOP);
			return false;
		}

		if (SEASON3B::IsRelease(VK_LEFT))
		{
			m_iCurTab--;
			if (m_iCurTab < eRankTotal)
			{
				m_iCurTab = eEndRankTab - 1;
			}
			this->RequestServerRankingInfo(m_iCurTab, m_ClassTab);
			return false;
		}
		if (SEASON3B::IsRelease(VK_RIGHT))
		{
			m_iCurTab++;
			if (m_iCurTab == eEndRankTab)
			{
				m_iCurTab = eRankTotal;
			}
			this->RequestServerRankingInfo(m_iCurTab, m_ClassTab);
			return false;
		}
	}

	return true;
}


bool SEASON3B::CNewUIRankingTop::UpdateMouseEvent()
{
	if (!g_pNewUISystem->IsVisible(INTERFACE_RANKING_TOP))
	{
		return true;
	}

	if (m_ScrollBar)
		m_ScrollBar->UpdateMouseEvent();

	for (int i = 0; i < eEndRankTab; i++)
	{
		if (SEASON3B::CheckMouseIn(m_Pos.x + (57 * i) + 95, m_Pos.y + 40, 56, 22) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (i != m_iCurTab)
			{
				m_iCurTab = i;
				this->RequestServerRankingInfo((m_iCurTab), m_ClassTab);
				return true;
			}
		}
	}

	for (int i = 0; i < MAX_CLASS + 1; i++)
	{
		if (SEASON3B::CheckMouseIn(m_Pos.x + 17, m_Pos.y + (25 * i) + 80, 56, 22) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			selectIndex = -1;
			if (i != m_ClassTab)
			{
				m_ClassTab = i;
				m_ScrollBar->SetCurPos(0);
				m_ScrollBar->Update();
				this->RequestServerRankingInfo(m_iCurTab, m_ClassTab);
				return true;
			}
		}
	}


	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, eMainWidth, eMainHeight))
	{
		m_ScrollBar->MouseWheelWindow = true;
		if (SEASON3B::CheckMouseIn(m_Pos.x + eMainWidth - 33, m_Pos.y, 36.f, 36.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_RANKING_TOP);
		}
		return false;
	}

	return true;
}

bool SEASON3B::CNewUIRankingTop::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	this->RenderFrame();

	this->RenderTexte();

	DisableAlphaBlend();

	if (gCustomRanking.size() > MAX_RANKING_VIEW)
	{
		if (m_ScrollBar)
			m_ScrollBar->Render();
	}

	return true;
}

bool SEASON3B::CNewUIRankingTop::Update()
{
	if (m_ScrollBar)
	{
		int iMaxPos = gCustomRanking.size();

		if (iMaxPos > MAX_RANKING_VIEW)
		{
			m_ScrollBar->Show(true);

			m_ScrollBar->SetMaxPos((iMaxPos < MAX_RANKING_VIEW) ? 0 : (iMaxPos - MAX_RANKING_VIEW));

			m_ScrollBar->Update();
			this->listInPage = m_ScrollBar->GetCurPos();

		}
		else
		{
			m_ScrollBar->Show(false);
		}
	}

	return true;
}

float SEASON3B::CNewUIRankingTop::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUIRankingTop::OpenningProcess()
{
	is_request = false;
	m_iCurTab = 0;
	m_ClassTab = 0;
	selectIndex = -1;

	m_ScrollBar->SetCurPos(0);

	RequestServerRankingInfo(m_iCurTab, m_ClassTab);

}

void SEASON3B::CNewUIRankingTop::ClosingProcess()
{
	m_iCurTab = 0;
	m_ClassTab = 0;
	selectIndex = -1;

	m_ScrollBar->SetCurPos(0);
}

void SEASON3B::CNewUIRankingTop::RenderFrame()
{
	float StartX = m_Pos.x;
	float StartY = m_Pos.y;

	g_pPCCustomDraw->DrawCustomWindows(&StartX, &StartY, eMainWidth, eMainHeight, INTERFACE_RANKING_TOP, "Bang Xep Hang");

	g_pPCCustomDraw->DrawInfoBox(StartX + 95, StartY + 65, eMainWidth - 120, 10, 0x00000096, 0);

	g_pPCCustomDraw->DrawInfoBox(StartX + 95, StartY + 90, eMainWidth - 120, eMainHeight - 120, 0x00000096, 0);
	//RenderImageF(IMAGE_TOP_BACK2, StartX, StartY, 183.f, 230.f, 0.0, 0.0, 732.0, 917.0); //-- background
	//RenderImageF(IMAGE_TOP_BACK1, StartX, StartY, 183.f, 230.f, 0.0, 0.0, 732.0, 917.0); //-- background


	char* titletab[eEndRankTab] = { "Reset", "Point", "Master", "Tai Phu", "Event","Top Boss" };
	int StartX_Tab = StartX + 100;
	for (int i = 0; i < eEndRankTab; i++)
	{
		bool isActive = (i == m_iCurTab);

		g_pRenderText->SetFont(isActive ? g_hFontBold : g_hFont);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(isActive ? CLRDW_WHITE : CLRDW_GRAY);

		float imageY = isActive ? 22.f : 0.f;
		int textY = StartY + (isActive ? 40 : 42);

		SEASON3B::RenderImage(CNewUIPetInfoWindow::IMAGE_PETINFO_TAB_BUTTON, StartX_Tab, StartY + 40, 56.f, 22.f, 0.0, imageY);
		g_pRenderText->RenderFont(StartX_Tab, textY, titletab[i], 56, 22, RT3_SORT_CENTER);

		StartX_Tab += 57;
	}

	//int Text_TabClass[MAX_CLASS + 1] = { 7,8,9,10,11,12,13,14 };
	char* tabclass[MAX_CLASS + 1] = { "All Class", "DW", "DK","ELF","MG","DL","Sum","RF" };
	int StartY_TabClass = StartY + 80;
	for (int i = 0; i < MAX_CLASS + 1; i++)
	{
		bool isActive = (i == m_ClassTab);

		g_pRenderText->SetFont(isActive ? g_hFontBold : g_hFont);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(isActive ? CLRDW_WHITE : CLRDW_GRAY);

		float imageY = isActive ? 22.f : 0.f;
		int textY = StartY_TabClass + (isActive ? 0 : 2);

		SEASON3B::RenderImage(IMAGE_TAB_CLASS, StartX + 17, StartY_TabClass, 72.f, 22.f, 0.0, imageY);
		g_pRenderText->RenderFont(StartX + 17, textY, tabclass[i], 72, 22, RT3_SORT_CENTER);

		StartY_TabClass += 25;
	}

}

void SEASON3B::CNewUIRankingTop::RenderTexte()
{
	int x = m_Pos.x;
	int y = m_Pos.y;

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(CLRDW_GOLD);
	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->RenderFont(x + 100, y + 67, "#", 10, 10, RT3_SORT_RIGHT);
	x += 28;
	g_pRenderText->RenderFont(x + 100, y + 67, this->m_Colum1, 100, 10, RT3_SORT_CENTER);
	x += 100;
	g_pRenderText->RenderFont(x + 100, y + 67, this->m_Colum2, 110, 10, RT3_SORT_CENTER);
	x += 110;
	g_pRenderText->RenderFont(x + 100, y + 67, this->m_Colum3, 100, 10, RT3_SORT_CENTER);
	x = m_Pos.x;

	int datalist = gCustomRanking.size();

	int HEIGH_SPACING = 18;

	if (datalist != 0)
	{
		int count = 0;

		for (int i = listInPage; i < datalist; i++)
		{
			if (count >= MAX_RANKING_VIEW)
			{
				break;
			}

			int line = HEIGH_SPACING * count;
			// Kiểm tra mouse hover & select
			if (SEASON3B::CheckMouseIn(x + 100, y + 92 + line, 340, HEIGH_SPACING) || selectIndex == i)
			{
				if (SEASON3B::IsRelease(VK_LBUTTON))
				{
					selectIndex = i;
				}
				EnableAlphaTest(true);
				glColor4ub(79, 86, 100, 255);
				RenderColor(x + 100, y + 92 + line, 335, HEIGH_SPACING);
				EndRenderColor();
			}

			// Cột số thứ tự
			TextDraw(g_hFontBold, x + 100, y + 92 + line, 0xFFFFFFFF, 0x0, 10, HEIGH_SPACING, RT3_SORT_RIGHT, "%d", i + 1);
			x += 12;
			// ================= Top 1 2 3 =================
			if (i + 1 < 4)
			{
				SEASON3B::RenderImage(IMAGE_TOP_LEVEL3 - i, x + 100, y + 92 + line, HEIGH_SPACING, HEIGH_SPACING, 0.0, 0.0, 1.f, 1.f, -1);
			}
			x += 18;
			// ================= Text Name =================
			TextDraw(g_hFontBold, x + 100, y + 92 + line, 0xFFFFFFFF, 0x0, 100, HEIGH_SPACING, RT3_SORT_CENTER, gCustomRanking[i].szName);
			x += 100;

			// ================= Value 2 =================
			TextDraw(g_hFontBold, x + 100, y + 92 + line, 0xFFFFFFFF, 0x0, 110, HEIGH_SPACING, RT3_SORT_CENTER, gCustomRanking[i].value2);
			x += 110;

			// ================= Value 3 =================
			TextDraw(g_hFontBold, x + 100, y + 92 + line, 0xFFFFFFFF, 0x0, 100, HEIGH_SPACING, RT3_SORT_CENTER, gCustomRanking[i].value3);
			x += 100;

			// ================= Online / Offline =================
			if (gCustomRanking[i].CheckOnline)
			{
				RenderBitmap(IMAGE_ONLINE, x + 100, y + 94 + line, 14.0f, 14.0f, 0.f, 0.f, 11.f / 16.f, 11.f / 16.f);
			}
			else
			{
				RenderBitmap(IMAGE_OFFLINE, x + 100, y + 94 + line, 14.0f, 14.0f, 0.f, 0.f, 14.f / 16.f, 14.f / 16.f);
			}
			x += 14;
			// ================= Button View Info =================
			//if (selectIndex == i)
			//{
			//	//m_BtnViewInfo.Render();

			//	m_BtnViewInfo.ChangeButtonImgState(true, IMAGE_VIEW, false, false, true);
			//	m_BtnViewInfo.ChangeButtonInfo(x + 100, y + 92 + line, 18, 18);
			//	m_BtnViewInfo.SetPos(x + 100, y + 92 + line);
			//}
			// Reset X về gốc
			x = m_Pos.x;

			count++;
		}
	}
}

void SEASON3B::CNewUIRankingTop::ReceiveRankingListInfo(BYTE* ReceiveBuffer)
{
	
	PCHEADER_RANKING_LIST* Data = (PCHEADER_RANKING_LIST*)ReceiveBuffer;

	this->m_iCurTab = Data->index;

	memset(this->m_RankName, 0, sizeof(this->m_RankName));
	gCustomRanking.clear();

	strcpy_s(this->m_RankName, Data->rankname);

	strcpy_s(this->m_Colum1, Data->col1);
	strcpy_s(this->m_Colum2, Data->col2);
	strcpy_s(this->m_Colum3, Data->col3);

	int offset = sizeof(PCHEADER_RANKING_LIST);

	for (int n = 0; n < Data->count; n++)
	{
		PCCREATE_RANKING_INFO* Data2 = (PCCREATE_RANKING_INFO*)(ReceiveBuffer + offset);

		gCustomRanking.push_back(CUSTOM_RANKING_DATA(Data2->Name, Data2->value2, Data2->value3, Data2->CheckOnline));

		offset += sizeof(PCCREATE_RANKING_INFO);
	}

	is_request = false;
}


void SEASON3B::CNewUIRankingTop::RequestServerRankingInfo(int Index, int byClass)
{
	if (is_request == false && (Index >= 0 && Index < eEndRankTab))
	{
		is_request = true;
		SendRequestRankingInfoPC(Index, byClass);
	}
}

