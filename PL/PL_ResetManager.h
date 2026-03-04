#pragma once

#if USING_RESET_WITH_GUI
#include <Button.h>
#define MAX_ITEM_JEWEL_KOF		10

#define	RESETVIP_CHECKBOX			0
#define	RESETNORMAL_CHECKBOX		1
#define	RESETADDPOINTSTR_CHECKBOX	2
#define	RESETADDPOINTAGI_CHECKBOX	3
#define	RESETADDPOINTVIT_CHECKBOX	4
#define	RESETADDPOINTENE_CHECKBOX	5

struct AUTO_RESET_REQ
{
	PBMSG_HEAD2 h;
	bool	AutoReset;
	bool	resetNormal;
	bool	resetVip;
	bool	resetAddPointStrType;
	bool	resetAddPointAgiType;
	bool	resetAddPointVitType;
	bool	resetAddPointEneType;
	bool	resetNoAddPointType;
};

namespace SEASON3B
{
	class CPLResetSystem : public CNewUIObj
	{
	public:
		CPLResetSystem();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		bool Render();
		void Release();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Update();
		float GetLayerDepth();
		void OpenningProcess();
		void ClosingProcess();
		void RenderButtons();

		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
	private:
		// New 12/02/2026
		float MainWidth;
		float MainHeight;
		float StartX;
		float StartY;
		bool	AutoResetStatus;
		bool	resetVip;
		bool	resetNormal;
		bool	resetAddPointStrType;
		bool	resetAddPointAgiType;
		bool	resetAddPointVitType;
		bool	resetAddPointEneType;
		bool	resetNoAddPointType;

		CButton m_Btn[7];
	};
}
#endif // USING_RESET_WITH_GUI