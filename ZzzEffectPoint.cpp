///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAi.h"
#include "ZzzEffect.h"
#include "DSPlaySound.h"
#include "WSClient.h"


PARTICLE  Points[MAX_POINTS];

int g_iLatestPoint = -1;

void CreatePoint(vec3_t Position, int Value, vec3_t Color, float scale, bool bMove, bool bRepeatedly, int SubType)
{
	for (int i = 0; i < MAX_POINTS; i++)
	{
		PARTICLE* o = &Points[i];
		if (!o->Live)
		{
			o->Live = true;
			o->Type = Value;
			o->SubType = SubType;
			VectorCopy(Position, o->Position);
			o->Position[2] += 140.f;
			VectorCopy(Color, o->Angle);
			o->bRepeatedly = bRepeatedly;
			o->fRepeatedlyHeight = RequestTerrainHeight(o->Position[0], o->Position[1]) + 140.0f;
			o->Gravity = 10.f;
			o->Scale = scale;
			o->LifeTime = 0;
			o->bEnableMove = bMove;
			return;
		}
	}
}
//--Phong Render Point
void RenderNumberNew(vec3_t Position, int Num, vec3_t Color, float Alpha, float Scale)
{
	vec3_t p;
	VectorCopy(Position, p);
	vec3_t Light[4];
	VectorCopy(Color, Light[0]);
	VectorCopy(Color, Light[1]);
	VectorCopy(Color, Light[2]);
	VectorCopy(Color, Light[3]);

	if (Num == -1)
	{
		float UV[4][2];
		TEXCOORD(UV[0], 0.f, 32.f / 32.f);
		TEXCOORD(UV[1], 32.f / 256.f, 32.f / 32.f);
		TEXCOORD(UV[2], 32.f / 256.f, 17.f / 32.f);
		TEXCOORD(UV[3], 0.f, 17.f / 32.f);
		RenderSpriteUV(BITMAP_FONT + 1, p, 45, 20, UV, Light, Alpha);
	}
	else if (Num == -2)
	{
		RenderSprite(BITMAP_FONT_HIT, p, 32 * Scale, 20 * Scale, Light[0], 0.f, 0.f, 0.f, 27.f / 32.f, 15.f / 16.f);
	}
	else
	{
		// ==== Novo código para adicionar separador de milhar ====
		char buffer[32];
		char formatted[32];
		itoa(Num, buffer, 10);

		SetConsoleTitleA(buffer);


		int len = strlen(buffer);
		int j = 0;
		int count = 0;

		// Percorre de trás para frente inserindo pontos
		for (int i = len - 1; i >= 0; i--)
		{
			if (count && count % 3 == 0)
			{
				formatted[j++] = '.';  // separador de milhar
			}
			formatted[j++] = buffer[i];
			count++;
		}
		formatted[j] = '\0';

		// Inverte a string formatada para ficar na ordem correta
		for (int i = 0; i < j / 2; i++)
		{
			char temp = formatted[i];
			formatted[i] = formatted[j - 1 - i];
			formatted[j - 1 - i] = temp;
		}

		// ==== Renderização ====
		p[0] -= strlen(formatted) * 5.f;
		unsigned int Length = strlen(formatted);
		p[0] -= Length * Scale * 0.125f;
		p[1] -= Length * Scale * 0.125f;

		for (unsigned int i = 0; i < Length; i++)
		{
			float UV[4][2];
			float u = 0;

			if (formatted[i] == '.')
			{
				// Posição do caractere '.' no sprite font (suponha que esteja na posição 10, ajuste conforme seu bitmap)
				u = (float)(10) * 16.f / 256.f;
			}
			else
			{
				u = (float)(formatted[i] - '0') * 16.f / 256.f;
			}

			TEXCOORD(UV[0], u, 16.f / 32.f);
			TEXCOORD(UV[1], u + 16.f / 256.f, 16.f / 32.f);
			TEXCOORD(UV[2], u + 16.f / 256.f, 0.f);
			TEXCOORD(UV[3], u, 0.f);

			RenderSpriteUV(BITMAP_FONT + 1, p, Scale, Scale, UV, Light, Alpha);
			p[0] += Scale * 0.5f;
			p[1] += Scale * 0.5f;
		}
	}
}

void RenderPoints(BYTE byRenderOneMore)
{
	EnableAlphaTest();
	DisableDepthTest();
	for (int i = 0; i < MAX_POINTS; i++)
	{
		PARTICLE* o = &Points[i];
		if (o->Live)
		{
			if (byRenderOneMore == 1)
			{
				if (o->Position[2] > 350.f)
					continue;
			}
			else if (byRenderOneMore == 2)
			{
				if (o->Position[2] <= 300.f)
					continue;
			}
			else if (o->bRepeatedly)
			{
				if (o->Position[2] <= o->fRepeatedlyHeight)
					continue;
			}

			if (o->SubType == 0)
			{
				//--RenderNumber(o->Position, o->Type, o->Angle, o->Gravity * (0.4f), o->Scale); cái này là gốc
				RenderNumberNew(o->Position, o->Type, o->Angle, o->Gravity * 0.4f, o->Scale + 2.f);
			}
		}
	}
}

void MovePoints()
{
	for (int i = 0; i < MAX_POINTS; i++)
	{
		PARTICLE* o = &Points[i];

		if (o->Live)
		{
			o->LifeTime -= AdvanceByDelta(1.f);

			if (o->LifeTime < 0)
			{
				if (o->bRepeatedly && o->Position[2] > o->fRepeatedlyHeight)
				{
					o->Gravity = 10.0f;
					o->bRepeatedly = false;
				}

				if (o->bEnableMove)
				{
					o->Position[2] += AdvanceByDelta(o->Gravity);
				}

				o->Gravity -= AdvanceByDelta(0.3f);

				if (o->Gravity <= 0.f)
					o->Live = false;

				if (o->Type != -2)
				{
					o->Scale -= AdvanceByDelta(5.f);
					if (o->Scale < 15.f)
						o->Scale = 15.f;
				}
			}
		}
	}
}
