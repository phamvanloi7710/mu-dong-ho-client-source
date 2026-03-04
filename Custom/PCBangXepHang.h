#pragma once
#include "stdafx.h"
#include "NewUIButton.h"
#include "NewUI3DRenderMng.h"
#include "NewUIInventoryCtrl.h"
#include "NewUITextBox.h"
#include "UIControls.h"
#include "NewUIManager.h"

namespace SEASON3B
{
	class CNewUIPCDraw : public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_BCUSTOM_WINDOW = 31630,		// newui_position02.tga			(70, 25)	
			IMAGE_BCUSTOM_WINDOW_31322,
			IMAGE_BCUSTOM_WINDOW_31353,
			IMAGE_BCUSTOM_WINDOW_31357,
			IMAGE_BCUSTOM_WINDOW_31355,
			IMAGE_BCUSTOM_WINDOW_31356,
			IMAGE_BCUSTOM_WINDOW_32453,
			IMAGE_BCUSTOM_WINDOW_31326,

			//info box
			IMAGE_INFO_TOP_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_PIXEL,
			IMAGE_INFO_BOTTOM_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_PIXEL,
			IMAGE_INFO_LEFT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_LEFT_PIXEL,
			IMAGE_INFO_RIGHT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_RIGHT_PIXEL,

			IMAGE_INFO_TOP_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_LEFT,	//. newui_item_table01(L).tga (14,14)
			IMAGE_INFO_TOP_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_RIGHT,	//. newui_item_table01(R).tga (14,14)
			IMAGE_INFO_BOTTOM_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_LEFT,	//. newui_item_table02(L).tga (14,14)
			IMAGE_INFO_BOTTOM_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_RIGHT,	//. newui_item_table02(R).tga (14,14)		

		};
	public:
		CNewUIPCDraw();
		~CNewUIPCDraw();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void SetPos(int x, int y);
		void Release();

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		float GetLayerDepth();
		bool Update();
		bool Render();

		bool DrawCustomWindows(float* StartX, float* StartY, float CuaSoW, float CuaSoH, int eNumWindow, LPCSTR Text, ...);
		void DrawInfoBox(float PosX, float PosY, float Width, float Height, DWORD bkcolor = 0x0, int Arg6 = 0, int Arg7 = 0);
		bool RenderCheckBox(float PosX, float PosY, DWORD TextColor, bool Checkis, LPCSTR Text, ...);
		bool RenderInputBox(float PosX, float PosY, float W, float H, char* TextSet, CUITextInputBox*& Input, UIOPTIONS UiOption, int MaxChar = 50, bool isPass = 0);
		void DrawBarForm(float PosX, float PosY, float Width, float Height, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

		void DrawCircleUI(float x, float y, float radius, float r, float g, float b, float a, int screenWidth, int screenHeight);
	private:

		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;

		void LoadImages();
		void UnloadImages();
	};
}


