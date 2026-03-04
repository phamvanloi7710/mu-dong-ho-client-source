#pragma once
#include "stdafx.h"
#include "WSClient.h"
#include "NewUiButton.h"
class PLCInterface
{
public:
	PLCInterface();
	static PLCInterface* GetInstance();
	void RenderNormalMain(float PositionX, float PositionY, float MainWidth, float MainHeight, const char* zText);
};

#define g_PLInterface PLCInterface::GetInstance()