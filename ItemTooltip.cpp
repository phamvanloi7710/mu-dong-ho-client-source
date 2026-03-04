#include "stdafx.h"
#include "ItemTooltip.h"
#include "Util.h"
#include "CharacterManager.h"
#include "ZzzCharacter.h"
#define ITEM(x, y)					((x * 512) + y)
nCInfo GInfo;


nCInfo::nCInfo()
{
	this->m_CustomInfo.clear();
	this->m_CustomInfoSet.clear();
	this->m_CustomDescripcionInfo.clear();
}

nCInfo::~nCInfo()
{
}

void nCInfo::loadnInformation(nInformation* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].ItemIndexMin == -1 || info[n].ItemIndexMax == -1)
		{
			return;
		}
		//this->m_CustomInfo.insert(std::pair<int, nInformation>(info[n].ItemIndex, info[n]));
		this->m_CustomInfo.push_back(info[n]);
	}
}
void nCInfo::loadnInformationSet(nInformation* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].ItemIndexMin == -1 || info[n].ItemIndexMax == -1)
		{
			return;
		}
		//this->m_CustomInfo.insert(std::pair<int, nInformation>(info[n].ItemIndex, info[n]));
		this->m_CustomInfoSet.push_back(info[n]);
	}
}
void nCInfo::loadnText(nText* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].Index < 0 || info[n].Index > 7680)
		{
			return;
		}
		this->m_CustomDescripcionInfo.insert(std::pair<int, nText>(info[n].Index, info[n]));
	}
}

bool nGetTextNum(int TextIndex)
{
	std::map<int, nText>::iterator it = GInfo.m_CustomDescripcionInfo.find(TextIndex);

	if (it != GInfo.m_CustomDescripcionInfo.end())
	{
		return 1;
	}

	return 0;
}
char* nGetTextLine(int TextIndex)
{
	std::map<int, nText>::iterator it = GInfo.m_CustomDescripcionInfo.find(TextIndex);

	if (it != GInfo.m_CustomDescripcionInfo.end())
	{
		return it->second.Text;
	}

	return "no found";
}
extern	char    TextList[90][100];
extern	int     TextListColor[90];
extern	int     TextBold[90];
bool CheckItemSlotOpt(int IndexItem, int Level, int Exl)
{
	int CountSlot = 0;
	BYTE byFirstClass = gCharacterManager.GetBaseClass(Hero->Class);

	int GetIndexSlot[6] =
	{
		Hero->BodyPart[BODYPART_HELM].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_ARMOR].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_PANTS].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_GLOVES].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_BOOTS].Type - MODEL_ITEM,
	};
	int GetLevelSlot[6] =
	{
	   Hero->BodyPart[BODYPART_HELM].Level,
		Hero->BodyPart[BODYPART_ARMOR].Level,
		Hero->BodyPart[BODYPART_PANTS].Level,
		Hero->BodyPart[BODYPART_GLOVES].Level,
		Hero->BodyPart[BODYPART_BOOTS].Level,
	};
	int GetExlSlot[6] =
	{
	   Hero->BodyPart[BODYPART_HELM].Option1,
		Hero->BodyPart[BODYPART_ARMOR].Option1,
		Hero->BodyPart[BODYPART_PANTS].Option1,
		Hero->BodyPart[BODYPART_GLOVES].Option1,
		Hero->BodyPart[BODYPART_BOOTS].Option1,
	};
	for (int Slot = 0; Slot < 6; Slot++)
	{
		if ((int)GetIndexSlot[Slot] == (int)ITEM((7 + Slot), IndexItem)
			&& Level <= GetLevelSlot[Slot] && Exl <= GetExlSlot[Slot])
		{
			CountSlot++;
		}
	}
	if (CountSlot >= 5 || (CountSlot >= 4 && (byFirstClass == 3 || byFirstClass == 6)))
	{
		return 1;
	}
	return 0;
}
#include "CBInterface.h"
//int nCInfo::nInformationOP(int TextNum, ITEM* ItemSearch)
//{
//	int ItemLevel = (ItemSearch->Level >> 3) & 15; // Level thực
//	int GetIndexItemSet = ItemSearch->Type % 512;
//	bool SetColorGray = false;
//
//	// ----------------------------
//	// SECTION 1: SET ITEM
//	// ----------------------------
//	for (int i = 0; i < this->m_CustomInfoSet.size(); i++)
//	{
//		auto& infoSet = this->m_CustomInfoSet[i];
//		if (infoSet.ItemIndexMin == -1 || infoSet.ItemIndexMax == -1) continue;
//		if (ItemSearch->Type < (MODEL_HELM - 1171) || ItemSearch->Type >= (MODEL_WING - 1171)) continue;
//		if (GetIndexItemSet < infoSet.ItemIndexMin || GetIndexItemSet > infoSet.ItemIndexMax) continue;
//
//		// Hiển thị OptionName
//		wsprintf(TextList[TextNum], infoSet.OptionName);
//		TextListColor[TextNum] = infoSet.OptionColor;
//		TextBold[TextNum++] = 1;
//
//		// Hiển thị Text từng dòng, sáng dần theo Level
//		for (int ib = 0; ib < 15; ib++)
//		{
//			if (!nGetTextNum(infoSet.TextIndex[ib])) continue;
//
//			int LineLevel = infoSet.LevelItem[ib];
//
//			if (LineLevel == -1) continue;
//
//			if (!CheckItemSlotOpt(GetIndexItemSet, LineLevel, infoSet.Exc))
//				SetColorGray = true;
//			else
//			{
//				// Logic sáng dần:
//				// Level < ItemLevel -> gray (10)
//				// Level == ItemLevel -> màu gốc
//				// Level > ItemLevel -> gray (10)
//				SetColorGray = (LineLevel != ItemLevel);
//			}
//
//			wsprintf(TextList[TextNum], "%s", nGetTextLine(infoSet.TextIndex[ib]));
//			TextListColor[TextNum] = SetColorGray ? 10 : infoSet.TextColor[ib];
//			TextBold[TextNum++] = 0;
//		}
//	}
//
//	// ----------------------------
//	// SECTION 0: NORMAL ITEM
//	// ----------------------------
//	for (int i = 0; i < this->m_CustomInfo.size(); i++)
//	{
//		auto& info = this->m_CustomInfo[i];
//		if (info.ItemIndexMin == -1) continue;
//		if (ItemSearch->Type < info.ItemIndexMin || ItemSearch->Type > info.ItemIndexMax) continue;
//
//		if ((info.LevelMin != -1 && ItemLevel < info.LevelMin) ||
//			(info.LevelMax != -1 && ItemLevel > info.LevelMax))
//			continue;
//
//		if (info.Anc != -1 && (ItemSearch->ExtOption % 0x04) == 0) continue;
//
//		// Hiển thị OptionName
//		wsprintf(TextList[TextNum], info.OptionName);
//		TextListColor[TextNum] = info.OptionColor;
//		TextBold[TextNum++] = 1;
//
//		// Hiển thị Text từng dòng, sáng dần theo Level
//		for (int ib = 0; ib < 15; ib++)
//		{
//			if (!nGetTextNum(info.TextIndex[ib])) continue;
//
//			int LineLevel = info.LevelItem[ib];
//			if (LineLevel == -1) continue;
//
//			if ((LineLevel > ItemLevel && LineLevel != -1) || ((int)ItemSearch->Option1 < info.Exc && info.Exc != -1))
//				SetColorGray = true;
//			else
//				SetColorGray = (LineLevel != ItemLevel); // Sáng dần theo Level
//
//			wsprintf(TextList[TextNum], "%s", nGetTextLine(info.TextIndex[ib]));
//			TextListColor[TextNum] = SetColorGray ? 10 : info.TextColor[ib];
//			TextBold[TextNum++] = 0;
//		}
//	}
//
//	return TextNum;
//}
int nCInfo::nInformationOP(int TextNum, ITEM* ItemSearch)
{
	int ItemLevel = (ItemSearch->Level >> 3) & 15; // Level thực
	int GetIndexItemSet = ItemSearch->Type % 512;

	// ----------------------------
	// SECTION 1: CUSTOM ITEM SET
	// ----------------------------
	for (int i = 0; i < static_cast<int>(this->m_CustomInfoSet.size()); i++)
	{
		auto& infoSet = this->m_CustomInfoSet[i];

		if (infoSet.ItemIndexMin == -1 || infoSet.ItemIndexMax == -1)
			continue;
		if (ItemSearch->Type < (MODEL_SHIELD - 1171) || ItemSearch->Type >= (MODEL_BOOTS - 1171))
			continue;
		if (GetIndexItemSet < infoSet.ItemIndexMin || GetIndexItemSet > infoSet.ItemIndexMax)
			continue;

		// Kiểm tra xem có dòng nào phù hợp với level hiện tại
		bool HasLineAtLevel = false;
		for (int ib = 0; ib < 15; ib++)
		{
			if (!nGetTextNum(infoSet.TextIndex[ib])) continue;
			if (infoSet.LevelItem[ib] == ItemLevel)
			{
				HasLineAtLevel = true;
				break;
			}
		}
		if (!HasLineAtLevel) continue; // bỏ qua nếu không có dòng nào phù hợp level

		// Kiểm tra số món set đang mặc
		bool HasFullSet = CheckItemSlotOpt(GetIndexItemSet, ItemLevel, infoSet.Exc);

		// Hiển thị OptionName
		wsprintf(TextList[TextNum], infoSet.OptionName);
		TextListColor[TextNum] = infoSet.OptionColor;
		TextBold[TextNum++] = 1;

		// Hiển thị dòng chi tiết theo level
		for (int ib = 0; ib < 15; ib++)
		{
			if (!nGetTextNum(infoSet.TextIndex[ib])) continue;
			if (infoSet.LevelItem[ib] != ItemLevel) continue;

			bool SetColorGray = !HasFullSet; // chỉ sáng nếu mặc đủ set
			wsprintf(TextList[TextNum], "%s", nGetTextLine(infoSet.TextIndex[ib]));
			TextListColor[TextNum] = SetColorGray ? 10 : infoSet.TextColor[ib];
			TextBold[TextNum++] = 0;
		}

		break; // dừng sau 1 infoSet phù hợp
	}

	// ----------------------------
	// SECTION 0: NORMAL ITEM
	// ----------------------------
	for (int i = 0; i < static_cast<int>(this->m_CustomInfo.size()); i++)
	{
		auto& info = this->m_CustomInfo[i];

		if (info.ItemIndexMin == -1) continue;
		if (ItemSearch->Type < info.ItemIndexMin || ItemSearch->Type > info.ItemIndexMax) continue;
		if ((info.LevelMin != -1 && ItemLevel < info.LevelMin) ||
			(info.LevelMax != -1 && ItemLevel > info.LevelMax)) continue;
		if (info.Anc != -1 && (ItemSearch->ExtOption % 0x04) == 0) continue;

		// Kiểm tra xem có dòng nào phù hợp với level hiện tại
		bool HasLineAtLevel = false;
		for (int ib = 0; ib < 15; ib++)
		{
			if (!nGetTextNum(info.TextIndex[ib])) continue;
			if (info.LevelItem[ib] == ItemLevel)
			{
				HasLineAtLevel = true;
				break;
			}
		}
		if (!HasLineAtLevel) continue;

		// Hiển thị OptionName
		wsprintf(TextList[TextNum], info.OptionName);
		TextListColor[TextNum] = info.OptionColor;
		TextBold[TextNum++] = 1;

		// Hiển thị dòng chi tiết theo level
		for (int ib = 0; ib < 15; ib++)
		{
			if (!nGetTextNum(info.TextIndex[ib])) continue;
			if (info.LevelItem[ib] != ItemLevel) continue;

			wsprintf(TextList[TextNum], "%s", nGetTextLine(info.TextIndex[ib]));
			TextListColor[TextNum] = info.TextColor[ib];
			TextBold[TextNum++] = 0;
		}

		break; // dừng sau 1 info phù hợp
	}

	return TextNum;
}


