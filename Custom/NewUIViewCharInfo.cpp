#include "StdAfx.h"
#include "Custom/NewUIViewCharInfo.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "CharacterManager.h"
#include "MonkSystem.h"
#include "ZzzInventory.h"



SEASON3B::CNewUIViewCharInfo::CNewUIViewCharInfo()
{
	m_pNewUIMng = NULL;
	m_pNewUI3DRenderMng = NULL;

	m_Pos.x = 0;
	m_Pos.y = 0;

	m_bDragging = false;
	m_DragOffsetX = 0;
	m_DragOffsetY = 0;

	m_iPointedSlot = -1;
	m_iBackPointed = -1;

	memset(&m_Equipment, 0, sizeof(m_Equipment));
	m_ViewCharInfo = CHARVIEW_INFO();

	for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
	{
		m_Equipment[i].index = -1;
		m_Equipment[i].CheckRender = true;

		if (((i == EQUIPMENT_ERING_L || i == EQUIPMENT_ERING_R) && !gmProtect->shutdown_item_earring)
			|| (i == EQUIPMENT_WING_CORE && !gmProtect->shutdown_item_core)
			|| (i == EQUIPMENT_FLAG_NAT && !gmProtect->shutdown_item_flag)
			)
		{
			m_Equipment[i].CheckRender = false;
		}
		else
		{
			EquipmentInven.push_back(i);
		}
	}
	this->Init();
}

SEASON3B::CNewUIViewCharInfo:: ~CNewUIViewCharInfo()
{
	Release();
}

void SEASON3B::CNewUIViewCharInfo::Init()
{
	m_PhotoView.Init(0);
	m_PhotoView.SetSize(125, 130);
	//m_PhotoView.CopyPlayer();
	m_PhotoView.SetAutoupdatePlayer(TRUE);
	m_PhotoView.SetAnimation(AT_STAND1);
	m_PhotoView.SetAngle(90);
	m_PhotoView.SetZoom(0.8);
	m_PhotoView.SetPosition(0, 0);
	m_PhotoView.SetOption(UIPHOTOVIEWER_CANCONTROL);
}

bool SEASON3B::CNewUIViewCharInfo::Create(CNewUIManager* pNewUIMng, CNewUI3DRenderMng* pNewUI3DRenderMng, float x, float y)
{
	if (NULL == pNewUIMng || NULL == pNewUI3DRenderMng || NULL == g_pNewItemMng)
	{
		return false;
	}

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(INTERFACE_VIEW_CHARINFO, this);

	m_pNewUI3DRenderMng = pNewUI3DRenderMng;
	m_pNewUI3DRenderMng->Add3DRenderObj(this, INVENTORY_CAMERA_Z_ORDER);
	m_pNewUI3DRenderMng->Add3DRenderObj(this, TOOLTIP_CAMERA_Z_ORDER);

	this->LoadImages();

	this->SetPos(x, y);

	this->SetInfo();

	this->Show(false);

	SetEquipmentSlotInfo();

	return true;
}

void SEASON3B::CNewUIViewCharInfo::Release()
{
	if (m_pNewUI3DRenderMng)
		m_pNewUI3DRenderMng->DeleteUI2DEffectObject(UI2DEffectCallback);

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		this->UnloadImages();
	}

	if (m_pNewUI3DRenderMng)
	{
		m_pNewUI3DRenderMng->Remove3DRenderObj(this);
		m_pNewUI3DRenderMng = NULL;
	}
	//memset(&m_ViewCharInfo, 0, sizeof(m_ViewCharInfo));
	EquipmentInven.clear();
}

void SEASON3B::CNewUIViewCharInfo::SetInfo()
{
	float StartX = m_Pos.x + 190;
	float StartY = m_Pos.y + 50;


}

void SEASON3B::CNewUIViewCharInfo::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;

	SetEquipmentSlotInfo();

	m_PhotoView.SetPosition(m_Pos.x + 190, m_Pos.y + 50);

}

void SEASON3B::CNewUIViewCharInfo::LoadImages()
{

}

void SEASON3B::CNewUIViewCharInfo::UnloadImages()
{

}

bool SEASON3B::CNewUIViewCharInfo::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_VIEW_CHARINFO) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_VIEW_CHARINFO);
			return false;
		}
	}

	return true;
}

bool SEASON3B::CNewUIViewCharInfo::UpdateMouseEvent()
{
	if (!g_pNewUISystem->IsVisible(INTERFACE_VIEW_CHARINFO))
	{
		return true;
	}


	int dragAreaX = m_Pos.x;
	int dragAreaY = m_Pos.y;
	int dragAreaW = eMainWidth;
	int dragAreaH = 30;


	m_PhotoView.DoMouseAction();

	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, eMainWidth, eMainHeight))
	{
		//m_ScrollBar->MouseWheelWindow = true;
		if (SEASON3B::CheckMouseIn(m_Pos.x + eMainWidth - 33, m_Pos.y, 36.f, 36.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_VIEW_CHARINFO);
		}
		return false;
	}

	return true;
}

bool SEASON3B::CNewUIViewCharInfo::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	this->RenderFrame();

	this->RenderText();

	RenderEquippedItem();

	m_PhotoView.Render();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIViewCharInfo::UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB)
{
	if (pClass)
	{
		g_pViewCharInfo->RenderItemToolTip(dwParamA);
	}
}

bool SEASON3B::CNewUIViewCharInfo::Update()
{
	if (this->IsVisible())
	{
		m_iPointedSlot = -1;

		for (int i : EquipmentInven)
		{
			if (m_Equipment[i].index == -1 || !m_Equipment[i].CheckRender)
				continue;

			if (CheckMouseIn(m_Equipment[i].x + 1, m_Equipment[i].y, m_Equipment[i].width - 4, m_Equipment[i].height - 4))
			{
				m_iPointedSlot = m_Equipment[i].index;
				break;
			}
		}
	}
	else
	{
		m_iPointedSlot = -1;
	}

	return true;
}

float SEASON3B::CNewUIViewCharInfo::GetLayerDepth()
{
	return 3.1f;
}

void SEASON3B::CNewUIViewCharInfo::OpenningProcess()
{
	for (int i : EquipmentInven)
	{
		this->SetEquipementRender(i, true);
	}
	//m_PhotoView.CopyPlayer();
}

void SEASON3B::CNewUIViewCharInfo::ClosingProcess()
{

}

void SEASON3B::CNewUIViewCharInfo::RenderFrame()
{
	float StartX = m_Pos.x;
	float StartY = m_Pos.y;

	g_pThangCustomDraw->DrawCustomWindows(&StartX, &StartY, eMainWidth, eMainHeight, INTERFACE_VIEW_CHARINFO, "Thong Tin Nhan Vat");
	g_pThangCustomDraw->DrawInfoBox(StartX + 190, StartY + 50, 125, 130); // box 3d
	g_pThangCustomDraw->DrawInfoBox(StartX + 15, StartY + 205, eMainWidth - 40, 125, 0x00000096); // box info


}

void SEASON3B::CNewUIViewCharInfo::RenderText()
{
	float StartX = m_Pos.x + 30;
	float StartY = m_Pos.y + 210;

	int heighText = 18;
	TextDraw(g_hFontBig, StartX, StartY, 0xF5E720FF, 0x0, eMainWidth - 40, heighText, RT3_SORT_CENTER, "[Tên Nhân Vật: %s]", curName);
	StartY += heighText + 5;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Class: %s", gCharacterManager.GetCharacterClassText((m_ViewCharInfo.Class / 16)));
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Tổng Điểm: %d", m_ViewCharInfo.TotalPoint);
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Guild: [%s]", m_ViewCharInfo.GuildName);
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Chức Vụ: %s", gCharacterManager.GetGuildStatusText(m_ViewCharInfo.GuildStatus));

	StartY = m_Pos.y + 210 + heighText + 5;
	StartX += 160;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Lvl: %d", m_ViewCharInfo.Level);
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Lvl Master: %d", m_ViewCharInfo.MasterLevel);
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Reset: %d", m_ViewCharInfo.Reset);
	StartY += heighText;
	TextDraw(g_hFontBold, StartX, StartY, 0xFFFFFFFF, 0x0, 130, heighText, RT3_SORT_LEFT, "Reset Master: %d", m_ViewCharInfo.MasterReset);
	//g_ConsoleDebug->Write(MCD_NORMAL, "GuildStatus: %s", gCharacterManager.GetGuildStatusText(m_ViewCharInfo.GuildStatus));
}

void SEASON3B::CNewUIViewCharInfo::SetEquipmentInfo(EQUIPMENT_ITEM* pEquipment, int index, int imageindex, int x, int y, int width, int height)
{
	pEquipment->index = index;
	if (x != -1)
		pEquipment->x = x;
	if (y != -1)
		pEquipment->y = y;
	pEquipment->width = width;
	pEquipment->height = height;
	pEquipment->dwBgImage = imageindex;
}

void SEASON3B::CNewUIViewCharInfo::SetEquipmentSlotInfo()
{

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_HELPER], EQUIPMENT_HELPER, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_FAIRY, m_Pos.x + 15, m_Pos.y + 44, 46, 46);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_HELM], EQUIPMENT_HELM, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_HELM, m_Pos.x + 75, m_Pos.y + 44, 46, 46);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_WING], EQUIPMENT_WING, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_WING, m_Pos.x + 120, m_Pos.y + 44, 61, 46);
	//--
	SetEquipmentInfo(&m_Equipment[EQUIPMENT_WEAPON_RIGHT], EQUIPMENT_WEAPON_RIGHT, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_RIGHT, m_Pos.x + 15, m_Pos.y + 87, 46, 66);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_ARMOR], EQUIPMENT_ARMOR, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_ARMOR, m_Pos.x + 75, m_Pos.y + 87, 46, 66);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_WEAPON_LEFT], EQUIPMENT_WEAPON_LEFT, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_LEFT, m_Pos.x + 135, m_Pos.y + 87, 46, 66);
	//--
	SetEquipmentInfo(&m_Equipment[EQUIPMENT_GLOVES], EQUIPMENT_GLOVES, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_GLOVES, m_Pos.x + 15, m_Pos.y + 150, 46, 46);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_PANTS], EQUIPMENT_PANTS, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_PANTS, m_Pos.x + 75, m_Pos.y + 150, 46, 46);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_BOOTS], EQUIPMENT_BOOTS, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_BOOT, m_Pos.x + 135, m_Pos.y + 150, 46, 46);
	//--
	SetEquipmentInfo(&m_Equipment[EQUIPMENT_RING_LEFT], EQUIPMENT_RING_LEFT, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_RING, m_Pos.x + 116, m_Pos.y + 150, 24, 24);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_AMULET], EQUIPMENT_AMULET, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_NECKLACE, m_Pos.x + 56, m_Pos.y + 87, 24, 24);

	SetEquipmentInfo(&m_Equipment[EQUIPMENT_RING_RIGHT], EQUIPMENT_RING_RIGHT, CNewUIMyInventory::IMAGE_INVENTORY_ITEM_RING, m_Pos.x + 56, m_Pos.y + 150, 24, 24);


	EQUIPMENT_ITEM* pEquipement = &m_Equipment[EQUIPMENT_ARMOR];

	//if (gmProtect->shutdown_item_core)
	//{
	//	if (g_pInventoryCore)
	//		SetEquipmentInfo(&m_Equipment[EQUIPMENT_WING_CORE], EQUIPMENT_WING_CORE, CNewUIMyInventory::IMAGE_ITEM_CORE, -1, -1, 61, 46);
	//	else
	//		SetEquipmentInfo(&m_Equipment[EQUIPMENT_WING_CORE], EQUIPMENT_WING_CORE, CNewUIMyInventory::IMAGE_ITEM_CORE, m_Pos.x + 31, pEquipement->y, 61, 46);
	//}

	if (gmProtect->shutdown_item_flag)
	{
		pEquipement = &m_Equipment[EQUIPMENT_ARMOR];

		SetEquipmentInfo(&m_Equipment[EQUIPMENT_FLAG_NAT], EQUIPMENT_FLAG_NAT, CNewUIMyInventory::IMAGE_ITEM_FLAG, pEquipement->x, pEquipement->y, pEquipement->width, pEquipement->height);
	}

	if (gmProtect->shutdown_item_earring)
	{
		pEquipement = &m_Equipment[EQUIPMENT_RING_LEFT];

		int _RenderFrameY = m_Equipment[EQUIPMENT_AMULET].y + pEquipement->height + 2;

		SetEquipmentInfo(&m_Equipment[EQUIPMENT_ERING_L], EQUIPMENT_ERING_L, CNewUIMyInventory::IMAGE_ITEM_EARRING, pEquipement->x, _RenderFrameY, pEquipement->width, pEquipement->height);

		pEquipement = &m_Equipment[EQUIPMENT_RING_RIGHT];

		SetEquipmentInfo(&m_Equipment[EQUIPMENT_ERING_R], EQUIPMENT_ERING_R, CNewUIMyInventory::IMAGE_ITEM_EARRING, pEquipement->x, _RenderFrameY, pEquipement->width, pEquipement->height);
	}

	if (gmProtect->shutdown_item_muun)
	{
		pEquipement = &m_Equipment[EQUIPMENT_RING_RIGHT];

		int _RenderFrameY = m_Equipment[EQUIPMENT_AMULET].y - pEquipement->height + 2;

		SetEquipmentInfo(&m_Equipment[EQUIPMENT_MUUN], EQUIPMENT_MUUN, CNewUIMyInventory::IMAGE_ITEM_MUUN, pEquipement->x, _RenderFrameY, pEquipement->width, pEquipement->height);
	}

	if (gmProtect->shutdown_item_eagle)
	{
		pEquipement = &m_Equipment[EQUIPMENT_RING_LEFT];

		int _RenderFrameY = pEquipement->y + pEquipement->height;

		SetEquipmentInfo(&m_Equipment[EQUIPMENT_EAGLE], EQUIPMENT_EAGLE, CNewUIMyInventory::IMAGE_ITEM_MUUN, pEquipement->x, _RenderFrameY, pEquipement->width, pEquipement->height);
	}
}

void SEASON3B::CNewUIViewCharInfo::RenderEquippedItem()
{
	for (int i : EquipmentInven)
	{
		if (i == EQUIPMENT_HELM && m_ViewCharInfo.Class / 16 == Magic_Gladiator)
		{
			continue;
		}

		if (i == EQUIPMENT_GLOVES && m_ViewCharInfo.Class / 16 == Rage_Fighter)
		{
			continue;
		}

		if (m_Equipment[i].index == -1 || !m_Equipment[i].CheckRender)
		{
			continue;
		}

		EnableAlphaTest();

		EQUIPMENT_ITEM* info = &m_Equipment[i];

		RenderImageF(info->dwBgImage, info->x, info->y, info->width, info->height);

		DisableAlphaBlend();

	}

	if (m_iPointedSlot != -1 && m_pNewUI3DRenderMng)
	{
		if (m_iPointedSlot != m_iBackPointed)
		{
			if (m_iPointedSlot == EQUIPMENT_HELPER)
			{
				if (CharacterMachine->Equipment[EQUIPMENT_HELPER].Type == ITEM_HELPER + 4
					|| CharacterMachine->Equipment[EQUIPMENT_HELPER].Kind3 == ItemKind3::PET_GAIN_EXPERIENCE)
				{
					SendRequestPetInfo(PET_TYPE_DARK_HORSE, 0, EQUIPMENT_HELPER);
				}
			}
			if (m_iPointedSlot == EQUIPMENT_WEAPON_LEFT)
			{
				if (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type == ITEM_HELPER + 5)
				{
					SendRequestPetInfo(PET_TYPE_DARK_SPIRIT, 0, EQUIPMENT_WEAPON_LEFT);
				}
			}
		}
		m_pNewUI3DRenderMng->RenderUI2DEffect(TOOLTIP_CAMERA_Z_ORDER, UI2DEffectCallback, this, m_iPointedSlot, 0);
	}
	m_iBackPointed = m_iPointedSlot;
}


void SEASON3B::CNewUIViewCharInfo::Render3D()
{
	for (int i : EquipmentInven)
	{
		if (m_Equipment[i].index == -1 || !m_Equipment[i].CheckRender)
			continue;

		this->RenderItemEquipment(&m_Equipment[i], m_ViewCharInfo.ItemEquipment[i]);
	}

	if (gmProtect->shutdown_item_core && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE))
	{
		EQUIPMENT_ITEM equipment = m_Equipment[EQUIPMENT_WING_CORE];

		equipment.y -= 85;

		this->RenderItemEquipment(&equipment, m_ViewCharInfo.ItemEquipment[EQUIPMENT_WING]);
	}

}

bool SEASON3B::CNewUIViewCharInfo::IsVisible() const
{
	return CNewUIObj::IsVisible();
}

void SEASON3B::CNewUIViewCharInfo::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, m_Pos.y);
}

void SEASON3B::CNewUIViewCharInfo::RenderItemEquipment(EQUIPMENT_ITEM* equipment, ITEM* pEquippedItem)
{
	if (pEquippedItem && pEquippedItem->Type >= 0 && equipment->index != -1)
	{
		float posy = 0.0;
		float posx = equipment->x + 1;
		float wmodel = (equipment->width - 4);
		float hmodel = (equipment->height - 4);
		float renderposx = equipment->x + 1;
		float renderposy = equipment->y + 1;
		float rendersizex = (equipment->width - 4);
		float rendersizey = (equipment->height - 4);

		if (equipment->index == EQUIPMENT_ARMOR)
			posy = equipment->y - 10.f;
		else
			posy = equipment->y;

		glColor4f(1.f, 1.f, 1.f, 1.f);

		Script_Item* item_info = GMItemMng->find(pEquippedItem->Type);

		if (item_info->Type != -1)
		{
			int itemwidth = (item_info->Width * 20);
			int itemhight = (item_info->Height * 20);
			posx += (wmodel - itemwidth) / 2;
			posy += (hmodel - itemhight) / 2;
			wmodel = itemwidth;
			hmodel = itemhight;
		}

		RenderItem3D(posx, posy, wmodel, hmodel, pEquippedItem->Type, pEquippedItem->Level, pEquippedItem->Option1, pEquippedItem->ExtOption, false, renderposx, renderposy, rendersizex, rendersizey);
		//g_ConsoleDebug->Write(2, "RenderItemEquipment() Slot:%d Type:%d Level:%d", equipment->index, pEquippedItem->Type, pEquippedItem->Level);
	}
}

bool SEASON3B::CNewUIViewCharInfo::CheckEquipementIndex(int index)
{
	return std::find(EquipmentInven.begin(), EquipmentInven.end(), index) != EquipmentInven.end();
}

void SEASON3B::CNewUIViewCharInfo::SetEquipementPosition(int index, int x, int y)
{
	if (this->CheckEquipementIndex(index) && m_Equipment[index].index != -1)
	{
		m_Equipment[index].x = x;
		m_Equipment[index].y = y;
	}
}

void SEASON3B::CNewUIViewCharInfo::SetEquipementRender(int index, bool bRender)
{
	if (this->CheckEquipementIndex(index) && m_Equipment[index].index != -1)
	{
		m_Equipment[index].CheckRender = bRender;
	}
}

void SEASON3B::CNewUIViewCharInfo::RenderItemToolTip(int iSlotIndex)
{
	if (m_iPointedSlot != -1 && CNewUIInventoryCtrl::GetPickedItem() == NULL)
	{
		ITEM* pEquipmentItemSlot = m_ViewCharInfo.ItemEquipment[iSlotIndex];

		if (pEquipmentItemSlot->Type != -1)
		{
			int iTargetX = m_Equipment[iSlotIndex].x + m_Equipment[iSlotIndex].width / 2;
			int iTargetY = m_Equipment[iSlotIndex].y + m_Equipment[iSlotIndex].height / 2;

			pEquipmentItemSlot->bySelectedSlotIndex = iSlotIndex;


			g_pNewItemTooltip->RenderItemTooltip(iTargetX, iTargetY, pEquipmentItemSlot, false);

		}
	}
}

void SEASON3B::CNewUIViewCharInfo::ReceiveViewCharacterInfo(BYTE* ReceiveBuffer)
{
	DATA_VIEW_CHARINFO Data = *(DATA_VIEW_CHARINFO*)ReceiveBuffer;
	m_ViewCharInfo = CHARVIEW_INFO();
	strcpy_s(curName, Data.NameChar);

	m_ViewCharInfo.Class = Data.Class;
	m_ViewCharInfo.TotalPoint = Data.TotalPoint;
	m_ViewCharInfo.Level = Data.Level;
	m_ViewCharInfo.MasterLevel = Data.MasterLevel;
	m_ViewCharInfo.Reset = Data.Reset;
	m_ViewCharInfo.MasterReset = Data.MasterReset;
	strcpy_s(m_ViewCharInfo.GuildName, Data.GuildName);
	m_ViewCharInfo.GuildStatus = Data.GuildStatus;

	//ITEM* pItemtemp;
	for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
	{
		m_ViewCharInfo.ItemEquipment[i] = g_pNewItemMng->CreateItem(Data.Inventory[i]);
	}
	/*for (int i : EquipmentInven)
	{
		m_ViewCharInfo.ItemEquipment[i] = g_pNewItemMng->CreateItem(Data.Inventory[i]);

		g_ConsoleDebug->Write(2, "1.ViewEquipment[slot:%d] Type: %d", i, m_ViewCharInfo.ItemEquipment[i]->Type);
	}*/

	m_PhotoView.SetClass(gCharacterManager.ChangeServerClassTypeToClientClassType(m_ViewCharInfo.Class));
	m_PhotoView.SetEquipmentPacket(Data.Equipment);


	// Show UI
	g_pNewUISystem->Toggle(INTERFACE_VIEW_CHARINFO);
}