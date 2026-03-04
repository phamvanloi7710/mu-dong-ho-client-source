// GameGuardWrapper.h
#pragma once

#include <windows.h>
#include <string>
#include <vector>

struct LogoImage
{
    int w;
    int h;
    unsigned char* data;
    int dataSize;
};

class GameGuardWrapper {
private:
    HMODULE hDLL;

    // Con trỏ hàm
    typedef const char* (__cdecl* RenderQRToBase64Func)(const char*, const LogoImage*);
    typedef LogoImage(__cdecl* LoadLogoWithWICFunc)(const char*);
    typedef unsigned int(__cdecl* LoadTextureFromMemoryFunc)(const char*);
    typedef const char* (__cdecl* GetDataFromAPIFunc)(const char*, const char*, const char*, const char*);
    typedef const char* (__cdecl* BuildQRFromJsonDataFunc)(const char*);

    RenderQRToBase64Func pRenderQRToBase64;
    LoadLogoWithWICFunc pLoadLogoWithWIC;
    LoadTextureFromMemoryFunc pLoadTextureFromMemory;
    GetDataFromAPIFunc pGetDataFromAPI;
    BuildQRFromJsonDataFunc pBuildQRFromJsonData;

public:
    GameGuardWrapper();
    ~GameGuardWrapper();

    bool LoadDLL(const std::string& dllPath);
    void UnloadDLL();
    bool IsLoaded() const;
    GLuint TEXTURE_QRCODE;
    // Wrapper functions
    const char* RenderQRToBase64(const char* text, const LogoImage* logo);
    LogoImage LoadLogoWithWIC(const char* filePath);
    unsigned int LoadTextureFromMemory(const char* qrDataURL);
    const char* GetDataFromAPI(const char* url, const char* customHeaders,
    const char* data, const char* method);
    const char* BuildQRFromJsonData(const char* jsonText);
};

extern GameGuardWrapper gameGuard;