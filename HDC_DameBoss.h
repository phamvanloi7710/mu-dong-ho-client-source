#pragma once
#include "Protocol.h"

struct BOSS_DAMAGE_INFO
{
	char Name[11];
	int Damage;
};


// Giữ nguyên định nghĩa struct này cho khớp dữ liệu GS gửi
struct HDC_DAMEBOSS_STR
{
	WORD monsterid;
	int Level;
	float MinLife;
	float MaxLife;
	char szName[11];
	QWORD Kill;
	BYTE Rate;
	DWORD LastHitTime;
	DWORD ClearTime;
};

class HDC_DAMEBOSS_CLASS
{
public:
	bool CheckDrawBxhDame; // bật/tắt hiển thị
	DWORD LastUpdateTime;
	BOSS_DAMAGE_INFO Rank[10];
	int RankCount;
	char BossName[64];
	int BossHPPercent;

	HDC_DAMEBOSS_CLASS();
	void Initz();
	void DAMEGETINFO(LPBYTE lpMsg);
	void DAMEDRAW();
	void DAMECLEAR();
	//void DAMEUPDATE();
	WORD HDC_DAMEBOSS_ID;    // ID của Boss
	float MaxLife;           // Máu tối đa
	float MinLife;           // Máu hiện tại
	int Level;               // Level boss
	DWORD ClearTime;         // Thời gian clear BXH
	DWORD ClearTimeConfig;         // Thời gian clear BXH

	//SEASON3B::CNewUIScrollBar m_ScrollBar;
	//int m_ScrollPos; // vị trí cuộn (dòng đầu tiên hiển thị)

};

extern HDC_DAMEBOSS_CLASS gDmgBoss;