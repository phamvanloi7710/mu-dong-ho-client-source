#pragma once
#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUI3DRenderMng.h"
#include "UIWindows.h"
#include "UIControls.h"

namespace SEASON3B
{
	class CNewUIViewCharInfo :public CNewUIObj, public INewUI3DRenderObj
	{
		struct CHARVIEW_INFO
		{
			ITEM* ItemEquipment[MAX_NEW_EQUIPMENT];
			BYTE Class;
			int TotalPoint;
			int Level;
			int MasterLevel;
			int Reset;
			int MasterReset;
			char GuildName[11];
			int GuildStatus;
			DWORD Equipment[13];

			CHARVIEW_INFO()
			{
				for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
				{
					ItemEquipment[i] = nullptr;
				}

				Class = -1;
				TotalPoint = 0;
				Level = 0;
				MasterLevel = 0;
				Reset = 0;
				MasterReset = 0;
				memset(GuildName, 0, sizeof(GuildName));
				GuildStatus = -1;
				memset(Equipment, 0, sizeof(Equipment));
			}

			CHARVIEW_INFO(ITEM* gItemEquipment[MAX_NEW_EQUIPMENT], BYTE gClass, int gTotalPoint, int gLevel, int gMasterLevel, int gReset, int gMasterReset, const char* gGuildName, int gGuildStatus,
				DWORD gEquipment[13])
			{
				// Copy item equipment
				for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
				{
					ItemEquipment[i] = gItemEquipment[i];
				}

				Class = gClass;
				TotalPoint = gTotalPoint;
				Level = gLevel;
				MasterLevel = gMasterLevel;
				Reset = gReset;
				MasterReset = gMasterReset;

				memset(GuildName, 0, sizeof(GuildName));
				if (gGuildName)
					strncpy(GuildName, gGuildName, sizeof(GuildName) - 1);

				GuildStatus = gGuildStatus;
				memcpy(Equipment, gEquipment, sizeof(Equipment));
			}
		};
	public:
		enum WindowsSize
		{
			eMainWidth = 340,
			eMainHeight = 360,
		};

	public:
		CNewUIViewCharInfo();
		virtual ~CNewUIViewCharInfo();


		bool Create(CNewUIManager* pNewUIMng, CNewUI3DRenderMng* pNewUI3DRenderMng, float x, float y);
		void Release();
		void SetInfo();
		void SetPos(float x, float y);
		void LoadImages();
		void UnloadImages();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth();

		void Init();

		void OpenningProcess();
		void ClosingProcess();

		void RenderFrame();
		void RenderText();

		void Render3D();
		bool IsVisible() const;
		void EventOrderWindows(double WindowsX, double WindowsY);

		void RenderItemEquipment(EQUIPMENT_ITEM* equipment, ITEM* pEquippedItem);

		void ReceiveViewCharacterInfo(BYTE* ReceiveBuffer);

		void SetEquipementRender(int index, bool bRender = true);
		void SetEquipementPosition(int index, int x, int y);
		bool CheckEquipementIndex(int index);

		static void UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB);

		char curName[20];
		CHARVIEW_INFO m_ViewCharInfo;
		CUIPhotoViewer m_PhotoView;
	private:
		CNewUIManager* m_pNewUIMng;
		CNewUI3DRenderMng* m_pNewUI3DRenderMng;

		POINT m_Pos;

		bool m_bDragging;
		int m_DragOffsetX;
		int m_DragOffsetY;
		int m_iPointedSlot;
		int m_iBackPointed;

		EQUIPMENT_ITEM m_Equipment[MAX_NEW_EQUIPMENT];

		void RenderEquippedItem();
		void RenderItemToolTip(int iSlotIndex);

		std::vector<int> EquipmentInven;

	protected:
		void SetEquipmentSlotInfo();
		void SetEquipmentInfo(EQUIPMENT_ITEM* Equipment, int index, int imageindex, int x, int y, int width, int height);
	};


}