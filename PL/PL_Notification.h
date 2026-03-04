#pragma once
#include "stdafx.h"
#include "Protocol.h"

namespace SEASON3B
{
#pragma pack(push, 1)
#pragma pack(pop)

	typedef struct
	{
		PSBMSG_HEAD header;
		char Content[255];
	} PMSG_GC_NOTIFICATION_DATA_RECV, * LPMSG_GC_NOTIFICATION_DATA_RECV;

	class PLCNotification : public CNewUIObj
	{
	public:
		PLCNotification();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		bool Render();
		void Release();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Update();
		float GetLayerDepth();
		//void SetPos(float x, float y);
		void OpenningProcess();
		void ClosingProcess();

		void OpenNotificationWindow(BYTE* ReceiveBuffer);
		//bool CheckNotificationWindow() { return gInterface.Data[eNotificationMain].OnShow; };
		//void DrawNotificationPopup();
		//void RenderCloseNotificationMainButton(int IMGcode, float x, float y, float w, float h, float uw, float uh, float a8, float a9);
		void ShowNotification(char Content[255]);
		char Content[255];

		CNewUIButton m_ButtonCloseMain;
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;

		int BUTTON_WIDTH;
		int BUTTON_HEIGHT;
	};
}