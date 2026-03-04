#pragma once
#include "Protocol.h"
#define MAX_MAIN_VIEWPORT 400

struct NEW_HEALTH_BAR
{
    WORD index;
    BYTE type;
    BYTE rate;
    DWORD ViewHP[2];
};

class CHealthBar
{
public:
    CHealthBar();
    ~CHealthBar();

    void ClearNewHealthBar();
    NEW_HEALTH_BAR* GetNewHealthBar(WORD index, BYTE type);
    void DrawNewHealthBar();
    void InsertNewHealthBar(WORD index, BYTE type, BYTE rate, DWORD* ViewHP);

private:
    NEW_HEALTH_BAR gNewHealthBar[MAX_MAIN_VIEWPORT];
};

extern CHealthBar gHealthBar;


void GCNewHealthBarRecv(BYTE* ReceiveBuffer);