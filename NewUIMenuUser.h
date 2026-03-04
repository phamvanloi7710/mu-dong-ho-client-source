#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"

#define MAX_BUTTON_CREATE		9
namespace SEASON3B
{
	class CNewUIMenuUser : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
		};
		
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
	public:
		CNewUIMenuUser();
		virtual~CNewUIMenuUser();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();

		void LoadImages();
		void UnloadImages();

		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth(); //. 10.5f

	// new
	public:
		void InitButtons();
		CNewUIButton* m_Button;
		bool m_ButtonVisible[MAX_BUTTON_CREATE];
		CNewUIButton			m_Btn[20];
		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;

		int BUTTON_WIDTH;
		int BUTTON_HEIGHT;
	};

}