#include "stdafx.h"
#include "SPK_PKSystem.h"
#include "ZzzInterface.h"
#include "ZzzCharacter.h"
#include "ZzzObject.h"
#include "ZzzOpenGLUtil.h"
#include "ZzzTexture.h"
#include "UIControls.h"
#include "NewUISystem.h"
#include "CGMProtect.h"
#include "CBInterface.h"
SPKPKSystem gPKSys;

DWORD GetColorByIndex(int index)
{
    switch (index)
    {
    case 0: return Color4f(255, 255, 0, 0);    // Đỏ
    case 1: return Color4f(255, 255, 255, 0);  // Vàng
    case 2: return Color4f(255, 0, 255, 0);    // Xanh lá
    case 3: return Color4f(255, 0, 255, 255);  // Cyan
    case 4: return Color4f(255, 128, 0, 128);  // Tím
    case 5: return Color4f(255, 255, 165, 0);  // Cam
    default: return Color4f(255, 255, 255, 255);
    }
}

// ============================================================================
// Hiệu ứng chuyển màu động (gradient PK rực rỡ)
// ============================================================================
DWORD GetColorByIndex_Animated(int index, DWORD startTime, DWORD endTime)
{
    DWORD now = GetTickCount();
    float t = 0.0f;

    if (endTime > startTime)
    {
        DWORD total = endTime - startTime;
        DWORD elapsed = (now - startTime);
        if (elapsed > total) elapsed = total;
        t = (float)elapsed / (float)total;
    }

    BYTE r1, g1, b1, r2, g2, b2;
    switch (index)
    {
    case 0: // Đỏ → Vàng
        r1 = 255; g1 = 0;   b1 = 0;
        r2 = 255; g2 = 255; b2 = 0;
        break;
    case 1: // Vàng → Trắng
        r1 = 255; g1 = 255; b1 = 0;
        r2 = 255; g2 = 255; b2 = 255;
        break;
    case 2: // Xanh lá → Xanh biển
        r1 = 0;   g1 = 255; b1 = 0;
        r2 = 0;   g2 = 128; b2 = 255;
        break;
    case 3: // Tím → Hồng
        r1 = 200; g1 = 0;   b1 = 200;
        r2 = 255; g2 = 120; b2 = 255;
        break;
    case 4: // Cam → Vàng nhạt
        r1 = 255; g1 = 140; b1 = 0;
        r2 = 255; g2 = 255; b2 = 150;
        break;
    case 5: // Cyan → Trắng
        r1 = 0;   g1 = 255; b1 = 255;
        r2 = 255; g2 = 255; b2 = 255;
        break;
    default:
        r1 = g1 = b1 = r2 = g2 = b2 = 255;
        break;
    }

    BYTE r = (BYTE)(r1 + (r2 - r1) * t);
    BYTE g = (BYTE)(g1 + (g2 - g1) * t);
    BYTE b = (BYTE)(b1 + (b2 - b1) * t);

    return Color4f(255, r, g, b);
}


void SPKPKSystem::AddPKNotice(const char* killer, const char* victim, int colorIndex, bool isLocalKiller)
{
    PKNoticeData data = { 0 };
    data.StartTime = GetTickCount();
    data.Time = data.StartTime + 5000; // 5 giây
    data.BaseColor = colorIndex;
    data.IsLocalPlayerKiller = isLocalKiller;

    char msg[256];
    sprintf(msg, "%s SÁT HẠI %s", killer, victim);
    _strupr_s(msg);

    strncpy(data.Mess, msg, sizeof(data.Mess) - 1);
    data.Mess[sizeof(data.Mess) - 1] = '\0';

    this->m_DataNoticePK.push_back(data);
}

void SPKPKSystem::DrawNoticePKSystem()
{
    if (m_DataNoticePK.empty())
        return;

    DWORD now = GetTickCount();
    float centerX = (float)(MAX_WIN_WIDTH / 2);
    float baseY = (float)(MAX_WIN_HEIGHT / 2) - 200.0f;

    // --- Nếu party đang hiển thị thì đẩy xuống ---
    if (g_pPartyListWindow && g_pPartyListWindow->IsVisible()) {
        POINT ptPartyBottom{};
        g_pPartyListWindow->GetRightBottomPos(&ptPartyBottom);

        // Khoảng cách từ giữa màn hình đến party có thể khác nhau tuỳ UI
        // Nếu party nằm ở góc trên trái thì có thể dùng trực tiếp ptPartyBottom.y
        baseY = max(baseY, (float)ptPartyBottom.y + 20.0f);
    }



    for (size_t i = 0; i < m_DataNoticePK.size();)
    {
        PKNoticeData& n = m_DataNoticePK[i];

        if (now >= n.Time)
        {
            m_DataNoticePK.erase(m_DataNoticePK.begin() + i);
            continue;
        }

        DWORD life = n.Time - now;
        DWORD elapsed = now - n.StartTime;

        // --- Hiệu ứng mờ ---
        float alpha = 1.0f;
        if (elapsed < 300)
            alpha = (elapsed / 300.0f);
        else if (life < 400)
            alpha = (life / 400.0f);
        if (alpha < 0.05f)
            alpha = 0.05f;

        // --- Vị trí text ---
        float posY = baseY + i * 14.0f;
        float posX = centerX;

        // --- Nền mờ (căn giữa theo text) ---
        float bgWidth = 120.0f;
        float bgHeight = 12.0f;
        float bgStartX = posX - (bgWidth / 2.0f) + 260.0f;
        float bgStartY = posY - (bgHeight / 2.0f) - 5.0f ; // canh giữa, hạ 1px cho khớp text

        if (g_pCBInterface)
        {
            g_pCBInterface->DrawBarForm(
                bgStartX,
                bgStartY,
                bgWidth,
                bgHeight,
                0.0f, 0.0f, 0.0f, 0.50f * alpha
            );
        }

        // --- Màu chữ ---
        DWORD colorAnimated = GetColorByIndex_Animated(i % 6, n.StartTime, n.Time);
        BYTE r = GetRValue(colorAnimated);
        BYTE g = GetGValue(colorAnimated);
        BYTE b = GetBValue(colorAnimated);

        // --- Text ---
        g_pRenderText->SetFont(g_hFontBold);
        g_pRenderText->SetBgColor(0, 0, 0, 0);
        g_pRenderText->SetTextColor(r, g, b, (BYTE)(alpha * 255));
        g_pRenderText->RenderText((int)posX +60 , (int)posY - 5, n.Mess, 400, 1, RT3_SORT_CENTER); // -5 cho vừa dòng
        ++i;
    }
}


