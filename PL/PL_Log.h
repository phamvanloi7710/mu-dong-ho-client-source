// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include <Windows.h>

#define MAX_LOG 99

enum eLogType
{
	LOG_GENERAL = 0,
	LOG_CHAT = 1,
	LOG_COMMAND = 2,
	LOG_TRADE = 3,
	LOG_CONNECT = 4,
	LOG_HACK = 5,
	LOG_CASH_SHOP = 6,
	LOG_CHAOS_MIX = 7,
#if BAG_EVENT_DROP
	LOG_ITEMBAGDROP = 8,
#endif
	LOG_VQMM,
	LOG_DOIITEM,

};

struct LOG_INFO
{
	BOOL Active;
	char Directory[256];
	int Day;
	int Month;
	int Year;
	char Filename[256];
	HANDLE File;
};

class CLog
{
public:
	CLog();
	virtual ~CLog();
	void AddLog(BOOL active, const char* directory);
	void Output(eLogType type, const char* text, ...);
private:
	LOG_INFO m_LogInfo[MAX_LOG];
	int m_count;
};

extern CLog gLog;
