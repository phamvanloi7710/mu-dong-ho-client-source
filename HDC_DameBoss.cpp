#include "StdAfx.h"
#include "HDC_DameBoss.h"
#include "ZzzInterface.h"
#include "CustomMonster.h"
#include "Util.h"
#include "jpexs.h"
#include "CBInterface.h"
#include "ZzzCharacter.h"
#include "ZzzInventory.h"
#include "ZzzObject.h"
#include "ZzzBmd.h"
#include "ZzzAI.h"
#include "Protocol.h"
#include "NewUIScrollBar.h"

HDC_DAMEBOSS_CLASS gDmgBoss;

HDC_DAMEBOSS_CLASS::HDC_DAMEBOSS_CLASS()
{
    this->Initz();
}

void HDC_DAMEBOSS_CLASS::Initz()
{
    this->CheckDrawBxhDame = false;
    this->LastUpdateTime = 0;
    this->RankCount = 0;
    memset(this->Rank, 0, sizeof(this->Rank));
    strcpy(this->BossName, "Unknown");
    this->BossHPPercent = 100;
    this->ClearTimeConfig = 0; // ✅ thêm biến timeout

    //this->m_ScrollPos = 0;
}

void HDC_DAMEBOSS_CLASS::DAMEGETINFO(LPBYTE lpMsg)
{
    PMSG_NEW_HEALTH_BAR_RECV* lpRecv = (PMSG_NEW_HEALTH_BAR_RECV*)lpMsg;

    this->CheckDrawBxhDame = true;
    this->LastUpdateTime = GetTickCount();

    if (lpRecv->count == 0)
    {
        this->DAMECLEAR();
        return;
    }

    memset(this->Rank, 0, sizeof(this->Rank));
    this->RankCount = lpRecv->count;

    int offset = sizeof(PMSG_NEW_HEALTH_BAR_RECV);

    for (int i = 0; i < this->RankCount; i++)
    {
        HDC_DAMEBOSS_STR* lpInfo = (HDC_DAMEBOSS_STR*)(lpMsg + offset + i * sizeof(HDC_DAMEBOSS_STR));

        memcpy(this->Rank[i].Name, lpInfo->szName, sizeof(lpInfo->szName));
        this->Rank[i].Damage = (int)lpInfo->Kill;

        if (i == 0)
        {
            this->HDC_DAMEBOSS_ID = lpInfo->monsterid;
            this->BossHPPercent = lpInfo->Rate;
            this->MaxLife = lpInfo->MaxLife;
            this->MinLife = lpInfo->MinLife;
            this->Level = lpInfo->Level;
        }
    }

    strcpy(this->BossName, gCustomMonster.GetNameMonter(this->HDC_DAMEBOSS_ID));
    this->ClearTime = GetTickCount();
    this->ClearTimeConfig = lpRecv->ClearTimeConfig; // ✅ lấy từ server gửi qua
    g_pCBInterface->Data[eWindow_DameBoss].OnShow = true;
    this->CheckDrawBxhDame = true;


    int maxPos = max(1, this->RankCount - 10);
    //this->m_ScrollBar.SetMaxPos(maxPos);
    //this->m_ScrollBar.SetCurPos(0);
    //this->m_ScrollPos = 0;
}

void HDC_DAMEBOSS_CLASS::DAMECLEAR()
{
    this->CheckDrawBxhDame = false;
    this->RankCount = 0;
    memset(this->Rank, 0, sizeof(this->Rank));
}

//// ✅ Hàm cập nhật — phải được gọi mỗi frame
//void HDC_DAMEBOSS_CLASS::DAMEUPDATE()
//{
//    if (!this->CheckDrawBxhDame)
//        return;
//
//    // Cập nhật thao tác chuột lên thanh cuộn
//    this->m_ScrollBar.Update();
//
//    // Lấy vị trí hiện tại
//    this->m_ScrollPos = this->m_ScrollBar.GetCurPos();
//
//    // Hỗ trợ lăn chuột
//    if (CheckMouseIn(450.0f, 100.0f, 180.0f, 160.0f))
//    {
//        if (MouseWheel > 0) this->m_ScrollBar.ScrollUp(1);
//        if (MouseWheel < 0) this->m_ScrollBar.ScrollDown(1);
//    }
//}

void HDC_DAMEBOSS_CLASS::DAMEDRAW()
{
    if (!this->CheckDrawBxhDame)
        return;

    // ⚙️ Thêm dòng này để lấy thời gian hiện tại
    DWORD currentTime = GetTickCount();
    if (this->ClearTimeConfig > 0 && (currentTime - this->ClearTime) > this->ClearTimeConfig)
    {
        this->DAMECLEAR();
        return;
    }

    float X = 450.0f;
    float Y = 100.0f;
    float Width = 150.0f;
    float BaseHeight = 85.0f;
    float Step = 8.0f;
    float MaxHeight = 250.0f;

    // 🔧 Chiều cao động dựa theo số người
    float Height = BaseHeight + (this->RankCount * Step);
    if (Height > MaxHeight)
        Height = MaxHeight;

    // ✅ Vẽ khung chính
    if (!g_pCBInterface->gDrawWindowCustom(&X, &Y, Width, Height, eWindow_DameBoss, "BXH DAMAGE BOSS"))
        return;

    // ✅ Vẽ nền bên trong
    g_pCBInterface->DrawBarForm(X + 8, Y + 35, Width - 16, Height - 45, 0.05f, 0.05f, 0.05f, 0.75f);

    // --------------------------------------
    // 🎯 Vẽ thông tin Boss nằm NGAY DƯỚI TIÊU ĐỀ
    // --------------------------------------
    g_pRenderText->SetFont(g_hFontBold);
    g_pRenderText->SetTextColor(180, 220, 255, 255);
    g_pRenderText->SetBgColor(0, 0, 0, 0);

    char bossInfo[128];
    sprintf(bossInfo, "Boss: %s (%d%% HP)", this->BossName, this->BossHPPercent);
    g_pRenderText->RenderText(X + 70, Y + 36, bossInfo, Width - 40, 0, 8);

    // --------------------------------------
    // 🧾 Vẽ danh sách BXH bên DƯỚI thông tin Boss
    // --------------------------------------
    g_pRenderText->SetFont(g_hFontBold);
    g_pRenderText->SetTextColor(255, 255, 200, 255);



    float textY = Y + 50;
    float lineHeight = 10.0f;

    for (int i = 0; i < this->RankCount; i++)
    {
        // 🎨 Màu theo top
        if (i == 0)
            g_pRenderText->SetTextColor(255, 80, 80, 255);     // 🔴 Top 1
        else if (i == 1)
            g_pRenderText->SetTextColor(80, 200, 255, 255);    // 🔵 Top 2
        else if (i == 2)
            g_pRenderText->SetTextColor(255, 165, 0, 255);     // 🟠 Top 3
        else
            g_pRenderText->SetTextColor(255, 255, 255, 255);   // ⚪ Còn lại

        // 🎯 Cột Top
        char rankStr[8];
        sprintf(rankStr, "%d.", i + 1);
        g_pRenderText->RenderText(X + 15, textY, rankStr, 20, 0, 8);

        // 🎯 Cột Name (căn đều bên trái)
        g_pRenderText->RenderText(X + 50, textY, this->Rank[i].Name, 80, 0, 8);

        // 🎯 Cột Damage (căn phải)
        char dmgStr[32];
        sprintf(dmgStr, "%d dmg", this->Rank[i].Damage);
        g_pRenderText->RenderText(X + 110, textY, dmgStr, 60, 0, 8);

        textY += lineHeight;
    }
}


