#include "stdafx.h"
#include "HealthBar.h"
#include "ZzzCharacter.h"
#include "ZzzObject.h"
#include "Util.h"

CHealthBar gHealthBar;

CHealthBar::CHealthBar()
{
    ClearNewHealthBar();
}

CHealthBar::~CHealthBar() {}

void GCNewHealthBarRecv(BYTE* ReceiveBuffer)
{
    PMSG_NEW_HEALTH_BAR_RECV* lpMsg = (PMSG_NEW_HEALTH_BAR_RECV*)ReceiveBuffer; // 🔹 THÊM DÒNG NÀY

    gHealthBar.ClearNewHealthBar();

    for (int n = 0; n < lpMsg->count; n++)
    {
        PMSG_NEW_HEALTH_RECV* lpInfo =
            (PMSG_NEW_HEALTH_RECV*)(((BYTE*)lpMsg) +
                sizeof(PMSG_NEW_HEALTH_BAR_RECV) + (sizeof(PMSG_NEW_HEALTH_RECV) * n));

        gHealthBar.InsertNewHealthBar(lpInfo->index, lpInfo->type, lpInfo->rate, lpInfo->ViewHP);
    }
}

void CHealthBar::ClearNewHealthBar()
{
    for (int n = 0; n < MAX_MAIN_VIEWPORT; n++)
    {
        gNewHealthBar[n].index = 0xFFFF;
        gNewHealthBar[n].type = 0;
        gNewHealthBar[n].rate = 0;
        gNewHealthBar[n].ViewHP[0] = 0;
        gNewHealthBar[n].ViewHP[1] = 0;
    }
}

void CHealthBar::InsertNewHealthBar(WORD index, BYTE type, BYTE rate, DWORD* ViewHP)
{
    for (int n = 0; n < MAX_MAIN_VIEWPORT; n++)
    {
        if (gNewHealthBar[n].index == 0xFFFF)
        {
            gNewHealthBar[n].index = index;
            gNewHealthBar[n].type = type;
            gNewHealthBar[n].rate = rate;
            gNewHealthBar[n].ViewHP[0] = ViewHP[0];
            gNewHealthBar[n].ViewHP[1] = ViewHP[1];
            return;
        }
    }
}

NEW_HEALTH_BAR* CHealthBar::GetNewHealthBar(WORD index, BYTE type)
{
    for (int n = 0; n < MAX_MAIN_VIEWPORT; n++)
    {
        if (gNewHealthBar[n].index == index && gNewHealthBar[n].type == type)
            return &gNewHealthBar[n];
    }
    return 0;
}


void CHealthBar::DrawNewHealthBar()
{
	float Width = 75.f;
	char Text[100], Text2[100], Text3[100];
	vec3_t Angle;
	int ScreenX, ScreenY;

	static float LastHP[MAX_MAIN_VIEWPORT] = { 0 };
	static float DamageTimer[MAX_MAIN_VIEWPORT] = { 0 };

	for (int j = 0; j < MAX_CHARACTERS_CLIENT; ++j)
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(j);
		if (!pCharacter || !pCharacter->Object.Live)
			continue;

		OBJECT* o = &pCharacter->Object;
		NEW_HEALTH_BAR* lpNewHealthBar = GetNewHealthBar(pCharacter->Key, o->Kind);
		if (!lpNewHealthBar)
			continue;

		Vector(
			pCharacter->Object.Position[0],
			pCharacter->Object.Position[1],
			pCharacter->Object.Position[2] + pCharacter->Object.BoundingBoxMax[2] + 100.f,

			Angle);

        if (pCharacter->Object.Kind == 2)
        {
            BeginOpengl(0, 0, 640, 480);
            Projection(Angle, &ScreenX, &ScreenY);
            EndOpengl();

            ScreenX -= (int)(Width / 2);

            // ✅ Hiển thị máu/tên khi chuột trong vùng quanh monster
            if (CheckMouseIn(ScreenX - 60, ScreenY - 100, 120, 160, 0))
            {
                auto FormatNumber = [](int num) -> std::string {
                    char buffer[16];
                    if (num >= 1000000000)
                        sprintf(buffer, "%.1fKKK", num / 1000000000.0);
                    else if (num >= 1000000)
                        sprintf(buffer, "%.1fKK", num / 1000000.0);
                    else if (num >= 1000)
                        sprintf(buffer, "%.1fK", num / 1000.0);
                    else
                        sprintf(buffer, "%d", num);
                    return std::string(buffer);
                };

                std::string hpCurrent = FormatNumber(lpNewHealthBar->ViewHP[0]);
                std::string hpMax = FormatNumber(lpNewHealthBar->ViewHP[1]);

                sprintf(Text, "%s / %s (%d%%)", hpCurrent.c_str(), hpMax.c_str(), lpNewHealthBar->rate);

                g_pRenderText->SetFont(g_hFont);
                g_pRenderText->SetTextColor(255, 255, 255, 255);
                g_pRenderText->SetBgColor(0, 0, 0, 128);
                g_pRenderText->RenderText(ScreenX + (Width / 2), ScreenY - 8, Text, Width, 0, 8);

                sprintf(Text2, "%s", pCharacter->ID);
                g_pRenderText->RenderText(ScreenX + (Width / 2), ScreenY - 18, Text2, Width, 0, 8);

                EnableAlphaTest();
                glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
                RenderColor((float)(ScreenX), (float)(ScreenY + 2), Width, 6.f);

                float HP_Percentage = (float)lpNewHealthBar->ViewHP[0] / (float)lpNewHealthBar->ViewHP[1];
                float HP_Width = Width * HP_Percentage;

                float Red = 1.0f - HP_Percentage;
                float Green = HP_Percentage;
                glColor4f(Red, Green, 0.f, 1.f);
                RenderColor((float)(ScreenX + 1), (float)(ScreenY + 3), HP_Width - 2, 4.f);

                DisableAlphaBlend();
               
}
		}
		else if (pCharacter->Object.Kind == 1)
		{

			BeginOpengl(0, 0, 640, 480); 
			Projection(Angle, &ScreenX, &ScreenY);
			EndOpengl();
			if (CheckMouseIn(ScreenX - 20, ScreenY, 40, 120))
			{
				if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {

					EnableAlphaTest();

					glColor4f(0, 0.f, 0.f, 1.f);
					RenderColor((float)((GetWindowsX / 2) - 51), (float)(GetWindowsY - 80 - 1), 102, 8.f);

					glColor4f(1.f, 0.f, 0.f, 1.f);
					RenderColor((float)((GetWindowsX / 2) - 50), (float)(GetWindowsY - 80), lpNewHealthBar->rate, 6.f);

					DisableAlphaBlend();
					glColor3f(1.f, 1.f, 1.f);

					EnableAlphaBlend();

					g_pRenderText->SetFont(g_hFont);
					g_pRenderText->SetTextColor(255, 255, 255, 255);
					g_pRenderText->SetBgColor(0, 0, 0, 0);

					sprintf(Text3, "%d%%", lpNewHealthBar->rate);

					g_pRenderText->RenderText((float)((GetWindowsX / 2)), (float)(GetWindowsY - 81), Text3, 100.f, 0, 8);
				}
			}
		}

	}
	DisableAlphaBlend();
	glColor3f(1.f, 1.f, 1.f);
}

//void CHealthBar::DrawNewHealthBar()
//{
//
//    float Width = 65.f;
//    char Text[100];
//    vec3_t Angle;
//    int ScreenX, ScreenY;
//
//    static float LastHP[MAX_MAIN_VIEWPORT] = { 0 };
//    static float DamageTimer[MAX_MAIN_VIEWPORT] = { 0 };
//
//    for (int j = 0; j < MAX_CHARACTERS_CLIENT; ++j)
//    {
//        CHARACTER* pCharacter = gmCharacters->GetCharacter(j);
//        if (!pCharacter || !pCharacter->Object.Live)
//            continue;
//
//        OBJECT* o = &pCharacter->Object;
//        NEW_HEALTH_BAR* lpNewHealthBar = GetNewHealthBar(pCharacter->Key, o->Kind);
//        if (!lpNewHealthBar)
//            continue;
//
//        Vector(o->Position[0], o->Position[1],
//            o->Position[2] + o->BoundingBoxMax[2] + 100.f, Angle);
//
//        BeginOpengl(0, 0, GetWindowsX, GetWindowsY);
//        Projection(Angle, &ScreenX, &ScreenY);
//        EndOpengl();
//
//        if (o->Kind == KIND_MONSTER)
//        {
//            ScreenX -= (int)(Width / 2);
//
//            EnableAlphaTest();
//            glColor4f(0.2f, 0.2f, 0.2f, 0.8f);
//            RenderColor((float)(ScreenX), (float)(ScreenY + 2), Width, 6.f);
//
//            float HP_Percentage = (float)lpNewHealthBar->ViewHP[0] / (float)lpNewHealthBar->ViewHP[1];
//            float HP_Width = Width * HP_Percentage;
//
//            if (lpNewHealthBar->ViewHP[0] < LastHP[j])
//                DamageTimer[j] = 2.0f;
//
//            if (LastHP[j] > lpNewHealthBar->ViewHP[0])
//                LastHP[j] -= ((LastHP[j] - lpNewHealthBar->ViewHP[0]) * 0.05f);
//            else
//                LastHP[j] = lpNewHealthBar->ViewHP[0];
//
//            float LastHP_Percentage = (float)LastHP[j] / (float)lpNewHealthBar->ViewHP[1];
//            float LastHP_Width = min(Width, Width * LastHP_Percentage);
//
//            if (DamageTimer[j] > 0)
//            {
//                glColor4f(0.8f, 0.0f, 0.0f, DamageTimer[j] * 0.66f);
//                RenderColor((float)(ScreenX + HP_Width), (float)(ScreenY + 3),
//                    LastHP_Width - HP_Width, 4.f);
//                DamageTimer[j] -= 0.008f;
//            }
//
//            float Red = 1.0f - HP_Percentage;
//            float Green = HP_Percentage;
//            glColor4f(Red, Green, 0.f, 1.f);
//            RenderColor((float)(ScreenX + 1), (float)(ScreenY + 3), HP_Width - 2, 4.f);
//
//            DisableAlphaBlend();
//            glColor3f(1.f, 1.f, 1.f);
//
//            g_pRenderText->SetFont(g_hFont);
//            g_pRenderText->SetTextColor(255, 255, 255, 255);
//            sprintf(Text, "%d%%", lpNewHealthBar->rate);
//            g_pRenderText->RenderText(ScreenX + (Width / 2), ScreenY - 12, Text, Width, 0, 8);
//        }
//    }
//}