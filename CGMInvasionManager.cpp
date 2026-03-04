#include "stdafx.h"
#include "jpexs.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "CGMInvasionManager.h"
#include "NewUISystem.h"

extern int FontHeight;
extern char* getMonsterName(int type);

void SEASON3B::CGMInvasionManager::LoadImages()
{
	LoadBitmap("Interface\\HUD\\bg250.tga", IMAGE_INVASION_FRAME_BG, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\button_close_navi.tga", IMAGE_INVASION_CLOSE_BTN, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\timerserver-bar.tga", IMAGE_INVASION_TITLE_BAR, GL_LINEAR);
}

void SEASON3B::CGMInvasionManager::UnloadImages()
{
	DeleteBitmap(IMAGE_INVASION_FRAME_BG);
	DeleteBitmap(IMAGE_INVASION_CLOSE_BTN);
	DeleteBitmap(IMAGE_INVASION_TITLE_BAR);
}

SEASON3B::CGMInvasionManager::CGMInvasionManager()
{
	m_CountActive = 0;
	is_Opentable = false;
	currentInvasion = -1;
	last_time = std::chrono::steady_clock::now();

	m_Pos.x = 0;
	m_Pos.y = 0;
	m_RenderFrameX = 0;
	m_RenderFrameY = 0;

	InvasionInfo.clear();
}

SEASON3B::CGMInvasionManager::~CGMInvasionManager()
{
	for (type_map_invasion::iterator it = InvasionInfo.begin(); it != InvasionInfo.end(); ++it)
	{
		it->second.total_monster.clear();
	}

	InvasionInfo.clear();
}

void SEASON3B::CGMInvasionManager::SetInvasion(int Index, DWORD TimeRemaing, char* Name)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		currentInvasion = Index;
		INVASION_GLOBAL_INFO& invasion = InvasionInfo[Index];

		// Asignar la nueva información
		invasion.index = Index;
		invasion.Name = Name; // Si 'Name' es un char*, usa std::string para asignar directamente
		invasion.TimeRemaing = TimeRemaing;
		// Limpiar los monstruos previos si los hubiera
		invasion.total_monster.clear();
	}
}

void SEASON3B::CGMInvasionManager::SetMonsterKill(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		INVASION_GLOBAL_INFO* invasion = FindInvasion(Index);

		if (invasion != NULL)
		{
			size_t length = invasion->total_monster.size();

			for (size_t i = 0; i < length; i++)
			{
				INVASION_MONSTER_INFO* info = &invasion->total_monster[i];

				if (info->MonsterIndex == MonsterIndex)
				{
					info->Monster_Kill = Monster_Kill;

					info->MonsterCount = MonsterCount;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::SetMonsterCount(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		INVASION_GLOBAL_INFO* invasion = FindInvasion(Index);

		if (invasion != NULL)
		{
			invasion->total_monster.push_back({ MonsterIndex, Monster_Kill, MonsterCount });
		}
	}
}

void SEASON3B::CGMInvasionManager::Update()
{
	auto current_time = std::chrono::steady_clock::now();
	double difTime = std::chrono::duration<double>(current_time - last_time).count();

	if (difTime >= 1.0)
	{
		m_CountActive = 0;
		last_time = current_time;

		for (type_map_invasion::iterator it = InvasionInfo.begin(); it != InvasionInfo.end(); ++it)
		{
			INVASION_GLOBAL_INFO* invasion = &it->second;

			if (invasion->TimeRemaing > 0)
			{
				m_CountActive++;
				invasion->TimeRemaing -= 1;

				if (currentInvasion == -1)
				{
					currentInvasion = it->first;
				}
			}
			else
			{
				if (currentInvasion == it->first)
				{
					currentInvasion = -1;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::SetPos(float RenderFrameX, float RenderFrameY)
{
	float RenderSizeX = 160.0;

	m_RenderFrameX = RenderFrameX;

	m_RenderFrameY = RenderFrameY;

	m_RenderFrameX -= (RenderSizeX / 2.f);
}

bool SEASON3B::CGMInvasionManager::UpdateMouseEvent()
{
	float RenderSizeX = 160.0;
	float RenderSizeY = 160.0;

	RenderSizeY = ((FontHeight + 2) * 2) / g_fScreenRate_y;

	if ((m_RenderFrameX + RenderSizeX) > GetScreenWidth()
		|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
		|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
		|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
		)
	{
		return false;
	}

	if (SEASON3B::CheckMouseIn(m_RenderFrameX, m_RenderFrameY, RenderSizeX, RenderSizeY))
	{
		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (m_CountActive > 0 && SEASON3B::CheckMouseIn(m_RenderFrameX + 3.0, m_RenderFrameY + (RenderSizeY - 18.f) / 2.f, 17.f, 18.f))
			{
				this->PreviousInvasion();
			}
			else if (m_CountActive > 0 && SEASON3B::CheckMouseIn(m_RenderFrameX + RenderSizeX - 20.0, m_RenderFrameY + (RenderSizeY - 18.f) / 2.f, 17.f, 18.f))
			{
				this->NextInvasion();
			}
			else
			{
				is_Opentable = !is_Opentable;
			}
		}
		return true;
	}

	return false;
}


void SEASON3B::CGMInvasionManager::RenderFrame(float RenderFrameX, float RenderFrameY)
{
	float RenderSizeX = 160.0f;
	float RenderSizeY = 160.0f;
	float _ScreenX = 0.f, _ScreenY = 0.f;

	if ((m_RenderFrameX + RenderSizeX) > GetScreenWidth()
		|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
		|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
		|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL))
	{
		return;
	}

	if (m_CountActive > 0)
	{
		INVASION_GLOBAL_INFO* Invasion = this->FindInvasion(currentInvasion);

		_ScreenX = (m_RenderFrameX * g_fScreenRate_x);
		_ScreenY = (m_RenderFrameY * g_fScreenRate_y);

		RenderSizeY = ((FontHeight + 2) * 2);

		// ✅ Giữ lại thanh tiêu đề, bỏ nền đen
		SEASON3B::RenderImageF(IMAGE_TITTLE_SERVER_INTERFACE, m_RenderFrameX, m_RenderFrameY,
			RenderSizeX, 22.0f, 0.0f, 0.0f, 223.0f, 32.0f);



		//------------------------------------------------------------
// ✨ Hiệu ứng sáng - tối cho chữ event (chuẩn MU 5.2, có alpha)
//------------------------------------------------------------
		static float animTime = 0.0f;
		animTime += 0.05f; // tốc độ sáng-tối (mềm mượt hơn)
		if (animTime > 6.28f) animTime = 0.0f;

		float brightness = (sinf(animTime) * 0.5f) + 0.5f; // 0.0 → 1.0

		// Tăng độ tương phản vàng
		int r = static_cast<int>(210 + 45 * brightness);
		int g = static_cast<int>(150 + 105 * brightness);
		int b = static_cast<int>(10 + 60 * brightness);

		// ⚠️ MU main 5.2 dùng màu kiểu ARGB (Alpha ở 8 bit cao)
		DWORD textColor = (0xFF << 24) | (b << 16) | (g << 8) | r;

		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(0); // Không để màu nền phủ
		g_pRenderText->SetTextColor(textColor); // ✅ Dùng màu động có alpha

		// 🟡 Vẽ tên event
		const char* eventName = (Invasion && Invasion->Name[0] != '\0') ? Invasion->Name.c_str() : "Invasion";
		g_pRenderText->RenderTextClipped(
			_ScreenX,
			_ScreenY,
			eventName,
			(RenderSizeX * g_fScreenRate_x),
			RenderSizeY,
			RT3_SORT_CENTER);

		// 🔹 Hiển thị mũi tên sát mép trong của khung UI
		if (is_Opentable)
		{
			float btnW = 10.0f;
			float btnH = 10.0f;

			// Mũi tên trái — sát mép trong bên trái
			RenderlookFetch(IMAGE_ND_BTN_L,
				m_RenderFrameX + 2.0f,                   // căn 4px bên trong
				m_RenderFrameY + (18.0f - btnH) / 2.f,
				true);

			// Mũi tên phải — lùi vào một chút để không vượt khung
			RenderlookFetch(IMAGE_ND_BTN_R,
				m_RenderFrameX + RenderSizeX - btnW - 10.0f, // lùi vào trong thêm 6px
				m_RenderFrameY + (18.0f - btnH) / 2.f,
				true);
		}

		// 🔹 Hiển thị danh sách boss khi mở bảng
		if (is_Opentable)
		{
			if (Invasion && Invasion->TimeRemaing > 0)
			{
				char pszText[100];
				size_t length = Invasion->total_monster.size();

				int totalseconds = Invasion->TimeRemaing;
				int hours = totalseconds / 3600;
				int minutes = (totalseconds / 60) % 60;
				int seconds = totalseconds % 60;

				sprintf_s(pszText, "Time: %02d:%02d:%02d", hours, minutes, seconds);

				float listStartY = m_RenderFrameY + 22.0f + 6.0f;      // bắt đầu ngay dưới title
				float lineHeight = FontHeight + 2.0f;                  // chiều cao mỗi dòng
				float totalHeight = (2.0f * FontHeight) + (length * lineHeight) + 8.0f; // Time + danh sách boss + padding

				// 🔹 Vẽ nền đen mờ phía sau (không block click)
				float bgX = m_RenderFrameX;
				float bgY = listStartY - 4.0f;
				float bgW = RenderSizeX;
				float bgH = totalHeight;

				// Lưu trạng thái OpenGL hiện tại
				glPushAttrib(GL_ALL_ATTRIB_BITS);

				// Chỉ bật blending (không bật alpha test)
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glColor4f(0.0f, 0.0f, 0.0f, 0.6f);

				// Vẽ hình chữ nhật nền
				RenderColor(bgX, bgY, bgW, bgH);

				// Khôi phục lại trạng thái render cũ
				glPopAttrib();

				// ✅ Sau đó vẽ text như bình thường
				g_pRenderText->SetTextColor(CLRDW_GOLD);
				g_pRenderText->RenderTextClipped(_ScreenX, listStartY, pszText,
					(RenderSizeX * g_fScreenRate_x), FontHeight * 2, RT3_SORT_CENTER);

				_ScreenY = listStartY + FontHeight * 2;
				g_pRenderText->SetFont(g_hFont);
				g_pRenderText->SetTextColor(CLRDW_WHITE3);

				for (size_t i = 0; i < length; i++)
				{
					INVASION_MONSTER_INFO* Data = &Invasion->total_monster[i];
					sprintf_s(pszText, "- %s (%d / %d)",
						getMonsterName(Data->MonsterIndex),
						Data->Monster_Kill,
						Data->MonsterCount);

					g_pRenderText->RenderTextClipped(
						_ScreenX + (6 * g_fScreenRate_x),
						_ScreenY,
						pszText, 0, FontHeight + 2,
						RT3_SORT_LEFT);

					_ScreenY += FontHeight + 2;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::NextInvasion()
{
	// Buscar el iterador que apunta a la clave actual
	auto it = InvasionInfo.find(currentInvasion);

LABEL_REPEAT:
	// Si no existe o ya está al final, no hay siguiente
	if (it == InvasionInfo.end() || std::next(it) == InvasionInfo.end())
	{
		return; // Retornar si no hay siguiente clave
	}

	// Avanzar al siguiente iterador
	++it;

	// Devolver la siguiente clave
	if (it->second.TimeRemaing == 0)
	{
		goto LABEL_REPEAT;
	}

	currentInvasion = it->first;
}

void SEASON3B::CGMInvasionManager::PreviousInvasion()
{
	// Buscar el iterador que apunta a la clave actual
	auto it = InvasionInfo.find(currentInvasion);

LABEL_REPEAT:
	// Si no existe o ya es el primero, no hay clave anterior
	if (it == InvasionInfo.end() || it == InvasionInfo.begin())
	{
		return; // Retornar si no hay clave anterior
	}

	// Retroceder al iterador anterior
	--it;

	if (it->second.TimeRemaing == 0)
	{
		goto LABEL_REPEAT;
	}

	currentInvasion = it->first;
}

INVASION_GLOBAL_INFO* SEASON3B::CGMInvasionManager::FindInvasion(int Index)
{
	// Buscar en el map usando find
	auto it = InvasionInfo.find(Index);

	// Verificar si se encontró el elemento
	if (it != InvasionInfo.end())
	{
		// Devolver un puntero al valor asociado (INVASION_GLOBAL_INFO)
		return &(it->second);
	}

	return NULL;
}