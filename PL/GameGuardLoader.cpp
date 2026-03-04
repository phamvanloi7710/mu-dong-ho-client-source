// GameGuardWrapper.cpp
#include "StdAfx.h"
#include "GameGuardLoader.h"
#include <iostream>
GameGuardWrapper gameGuard;
GameGuardWrapper::GameGuardWrapper()
    : hDLL(nullptr), pRenderQRToBase64(nullptr), pLoadLogoWithWIC(nullptr),
    pLoadTextureFromMemory(nullptr), pGetDataFromAPI(nullptr),
    pBuildQRFromJsonData(nullptr) {
}

GameGuardWrapper::~GameGuardWrapper() {
    UnloadDLL();
}

bool GameGuardWrapper::LoadDLL(const std::string& dllPath) {
    // Unload nếu đã load trước đó
    UnloadDLL();

    // Load DLL
    hDLL = LoadLibraryA(dllPath.c_str());
    if (!hDLL) {
        return false;
    }

    // Get function addresses
    pRenderQRToBase64 = (RenderQRToBase64Func)GetProcAddress(hDLL, "RenderQRToBase64");
    pLoadLogoWithWIC = (LoadLogoWithWICFunc)GetProcAddress(hDLL, "LoadLogoWithWIC");
    pLoadTextureFromMemory = (LoadTextureFromMemoryFunc)GetProcAddress(hDLL, "LoadTextureFromMemory");
    pGetDataFromAPI = (GetDataFromAPIFunc)GetProcAddress(hDLL, "GetDataFromAPI");
    pBuildQRFromJsonData = (BuildQRFromJsonDataFunc)GetProcAddress(hDLL, "BuildQRFromJsonData");

    // Kiểm tra tất cả hàm
    if (!pRenderQRToBase64 || !pLoadLogoWithWIC || !pLoadTextureFromMemory || !pGetDataFromAPI || !pBuildQRFromJsonData) {
        UnloadDLL();
        return false;
    }

    return true;
}

void GameGuardWrapper::UnloadDLL() {
    if (hDLL) {
        FreeLibrary(hDLL);
        hDLL = nullptr;
        pRenderQRToBase64 = nullptr;
        pLoadLogoWithWIC = nullptr;
        pLoadTextureFromMemory = nullptr;
        pGetDataFromAPI = nullptr;
        pBuildQRFromJsonData = nullptr;
    }
}

bool GameGuardWrapper::IsLoaded() const {
    return hDLL != nullptr;
}

const char* GameGuardWrapper::RenderQRToBase64(const char* text, const LogoImage* logo) {
    if (!pRenderQRToBase64) return nullptr;
    return pRenderQRToBase64(text, logo);
}

LogoImage GameGuardWrapper::LoadLogoWithWIC(const char* filePath) {
    if (!pLoadLogoWithWIC) return LogoImage{ 0, 0, {} };
    return pLoadLogoWithWIC(filePath);
}

unsigned int GameGuardWrapper::LoadTextureFromMemory(const char* qrDataURL) {
    if (!pLoadTextureFromMemory) return 0;
    TEXTURE_QRCODE = pLoadTextureFromMemory(qrDataURL);
    LoadBitmap("PL\\RechargeMoney\\current_payment.jpg", TEXTURE_QRCODE, GL_LINEAR);
    return TEXTURE_QRCODE;
}

const char* GameGuardWrapper::GetDataFromAPI(const char* url, const char* customHeaders,
    const char* data, const char* method) {
    if (!pGetDataFromAPI) return "{\"error\":\"dll_not_loaded\"}";
    return pGetDataFromAPI(url, customHeaders, data, method);
}

const char* GameGuardWrapper::BuildQRFromJsonData(const char* jsonText) {
    if (!pBuildQRFromJsonData) return nullptr;
    return pBuildQRFromJsonData(jsonText);
}