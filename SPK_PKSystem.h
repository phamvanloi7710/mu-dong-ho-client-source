#pragma once
#include <vector>
#include <Windows.h>

struct PKNoticeData
{
    char Mess[256];
    DWORD Time;
    DWORD StartTime;
    DWORD BaseColor;
    bool IsLocalPlayerKiller;
};

class SPKPKSystem
{
public:
    void DrawNoticePKSystem();
    void AddPKNotice(const char* killer, const char* victim, int colorIndex, bool isLocalKiller);
    std::vector<PKNoticeData> m_DataNoticePK;
};

extern SPKPKSystem gPKSys;