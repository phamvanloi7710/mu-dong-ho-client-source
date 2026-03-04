#pragma once
#include "WSclient.h"
#define SET_NUMBERHB(x) ((BYTE)((DWORD)(x)>>(DWORD)8))
#define SET_NUMBERLB(x) ((BYTE)((DWORD)(x)&0xFF))
#define SET_NUMBERHW(x) ((WORD)((DWORD)(x)>>(DWORD)16))
#define SET_NUMBERLW(x) ((WORD)((DWORD)(x)&0xFFFF))
#define SET_NUMBERHDW(x) ((DWORD)((QWORD)(x)>>(QWORD)32))
#define SET_NUMBERLDW(x) ((DWORD)((QWORD)(x)&0xFFFFFFFF))

#define MAKE_NUMBERW(x,y) ((WORD)(((BYTE)((y)&0xFF))|((BYTE)((x)&0xFF)<<8)))
#define MAKE_NUMBERDW(x,y) ((DWORD)(((WORD)((y)&0xFFFF))|((WORD)((x)&0xFFFF)<<16)))
#define MAKE_NUMBERQW(x,y) ((QWORD)(((DWORD)((y)&0xFFFFFFFF))|((DWORD)((x)&0xFFFFFFFF)<<32)))
//#define C2_F3E2 0xE2



struct PMSG_NEW_HEALTH_BAR_RECV
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
	DWORD ClearTimeConfig;
};

struct PMSG_NEW_HEALTH_RECV
{
	WORD index;
	BYTE type;
	BYTE rate;
	DWORD ViewHP[2];
};

struct XULY_CGPACKET_SOLAN
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD ThaoTac;
	DWORD SoLan;
};


struct XULY_CGPACKET_VONGQUAY
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD StartRoll;
	DWORD IndexWin;
};


struct PMSG_VONGQUAY_SEND
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
};

struct ListVongQuaySend
{
	int IndexVongQuay;
	char Name[90];
};

struct PMSG_YCVONGQUAY_SEND
{
	PSWMSG_HEAD header; // C2:F3:E2
	BYTE count;
	int IndexYC;
	int CountItem;
	int WCYC;
	int WPYC;
	int GPYC;
};

struct LISTITEMVONGQUAY_SENDINFO
{
	float SizeBMD;
	short Index;
	BYTE Dur;
	BYTE Item[12];
	int  PeriodTime;
};

struct INFOITEM_DOIITEM_CLIENT
{
	PSWMSG_HEAD header;
	BYTE ActiveMix;
	BYTE ItemChinh[16];
	BYTE ItemPhu[3][16];
	BYTE ItemKetQua[6][16];
	int Rate;
};

//==Struct Client
struct INFO_VONGQUAY_LOCAL_ITEM
{
	float SizeBMD;
	short Index;
	BYTE Item[12];
};


struct VONGQUAY_CLIENT
{
	int IndexYC;
	int CountItem;
	int WCYC;
	int WPYC;
	int GPYC;
	std::vector<INFO_VONGQUAY_LOCAL_ITEM> ListItemVongQuay;

	void Clear()
	{
		IndexYC = 0;
		CountItem = 0;
		WCYC = 0;
		WPYC = 0;
		GPYC = 0;
		ListItemVongQuay.clear();
	}
};

struct VONGQUAYINFO_CLIENT
{
	int IndexVongQuay;
	char NameVongQuay[90];
	std::vector<ListVongQuaySend> DanhSachVongQuay;
	void Clear()
	{
		IndexVongQuay = 0;
		memset(NameVongQuay, 0, sizeof(NameVongQuay));
		DanhSachVongQuay.clear();
	}
};

extern VONGQUAYINFO_CLIENT mDataVongQuayClient;
extern VONGQUAY_CLIENT mDataListItemVongQuayClient;
//===Move Item

struct PMSG_ITEM_MOVE_RECV
{
	PSBMSG_HEAD h;
	BYTE sFlag;
	BYTE tFlag;
	BYTE Source;
	BYTE Target;
};
struct XULY_CGPACKET
{
	PSBMSG_HEAD header; // C3:F3:03
	DWORD ThaoTac;
};
struct SEND_COUNTLIST
{
	PSWMSG_HEAD header;
	int Count;
	BYTE Type;
};


#pragma pack(push, 1)
struct PMSG_AUTO_HP_SEND
{
	PSBMSG_HEAD header; // header subcode D3:10
	BYTE ThaoTac;       // 1 = bật, 0 = tắt
};
#pragma pack(pop)

struct RANK_INFO_SEND
{
	char NameRank[128];
};

struct PMSG_NOTICE_SEND
{
	PBMSG_HEAD header; // C1:0D
	BYTE type;
	BYTE count;
	BYTE opacity;
	WORD delay;
	DWORD color;
	BYTE speed;
	char message[256];
};

struct PMSG_CONNECT_CLIENT_RECV
{
	PSBMSG_HEAD header; // C1:F1:00
	BYTE result;
	BYTE index[2];
	BYTE ClientVersion[5];
};
struct PMSG_PK_NOTICE_SEND 
{
	PBMSG_HEAD header;
	char Mess[64];
	DWORD Time;
	DWORD BaseColor;
};
struct PMSG_HARDWAREID_INFO_SEND 
{
	PSBMSG_HEAD header;
	char HardwareId[36];
};
extern std::vector <std::string> m_DataSelectNameTop;
BOOL ProtocolCoreEx(BYTE head, BYTE* lpMsg, int size, int key);
void GCConnectClientRecv2(PMSG_CONNECT_CLIENT_RECV* lpMsg);