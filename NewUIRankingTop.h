#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"

#define MAX_RANKING_VIEW 10

namespace SEASON3B
{
	struct CUSTOM_RANKING_DATA
	{
		char szName[20];
		char value2[25];
		char value3[25];
		bool CheckOnline;

		CUSTOM_RANKING_DATA(char* Name, char* col2, char* col3, bool online)
		{
			memset(this->szName, 0, sizeof(this->szName));
			memset(this->value2, 0, sizeof(this->value2));
			memset(this->value3, 0, sizeof(this->value3));
			strcpy_s(this->szName, Name);
			strcpy_s(this->value2, col2);
			strcpy_s(this->value3, col3);
			this->CheckOnline = online;
		}

	};

	class CNewUIRankingTop : public CNewUIObj
	{
	public:
		enum WindowsSize
		{
			eMainWidth = 490,
			eMainHeight = 300,
		};
		enum RankTab
		{
			eRankTotal,
			eRankReset,
			eRankVip,
			eRankCustom,
			eRankCoin,
			eRankBoss,
			eEndRankTab,

		};
		enum IMAGE_LIST
		{

			IMAGE_TOP_BACK1 = BITMAP_IMAGE_FRAME_EMU + 1,
			IMAGE_TOP_BACK2 = BITMAP_IMAGE_FRAME_EMU + 2,
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
			IMAGE_TOP_LEVEL1 = BITMAP_IMAGE_FRAME_EMU + 4,
			IMAGE_TOP_LEVEL2 = BITMAP_IMAGE_FRAME_EMU + 5,
			IMAGE_TOP_LEVEL3 = BITMAP_IMAGE_FRAME_EMU + 6,

			IMAGE_TAB_CLASS = IMAGE_TAB_SIZE_72x22,

			IMAGE_ONLINE = CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_MINIMAPICON_HERO,
			IMAGE_OFFLINE = CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_MINIMAPICON_HOLYITEM_PC,

			IMAGE_VIEW = BITMAP_INFO,
		};
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;

		CNewUIScrollBar* m_ScrollBar;
		//CNewUIButton	m_BtnViewInfo;

		bool is_request;
		size_t selectIndex;
		char m_RankName[50];
		char m_Colum1[50];
		char m_Colum2[50];
		char m_Colum3[50];

		std::vector<CUSTOM_RANKING_DATA> gCustomRanking;

		BYTE CountTab;
		int m_iCurTab;
		int m_ClassTab;
		int listInPage;

		bool m_bDragging;
		int m_DragOffsetX;
		int m_DragOffsetY;
	public:
		CNewUIRankingTop();
		virtual ~CNewUIRankingTop();

		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();
		void SetInfo();
		void SetPos(float x, float y);
		void LoadImages();
		void UnloadImages();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth(); //. 10.5f

		void OpenningProcess();
		void ClosingProcess();

		void RenderFrame();
		void RenderTexte();

		void ReceiveRankingListInfo(BYTE* ReceiveBuffer);
	private:
		void RequestServerRankingInfo(int Index, int byClass);
	};
}

