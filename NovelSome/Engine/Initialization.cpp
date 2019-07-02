//
//  Initialization.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 01/07/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "Initialization.hpp"

void DrawInParallel(sf::RenderWindow* window, EntitySystem* system)
{
    window->setActive();
    
    window->clear();
    system->Draw(window);
    window->display();
}



void CalculateScaleRatios(unsigned int width, unsigned int height)
{
    float factorX = (float)width / gs::relativeWidth; gs::scalex = factorX;
    float factorY = (float)height / gs::relativeHeight; gs::scaley = factorY;
    
    float ratioFactorX = (float)width/(float)height;
    float ratioFactorY = (float)height/(float)width;
    gs::verticalOrientation = ratioFactorX < 1.23;
    
    gs::scale = factorX > factorY ? factorX : factorY;
    gs::scScale = gs::scale;
    
    if (ratioFactorX < 1.3 && ratioFactorY < 1.3) gs::scale *= 0.9;
    if (ratioFactorX < 1.3 && ratioFactorY < 1.3) gs::scScale *= 0.9;
    if (ratioFactorY > 1)
    {
        float m = gs::scale;
        if (ratioFactorY < 1.2)
            gs::scale = gs::scale - m*(ratioFactorY - 1)*1.6;
        else if (ratioFactorY < 2)
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(ratioFactorY - 1.2)*0.17;
        else if (ratioFactorY < 2.46)
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(2 - 1.2)*0.17 - m*(ratioFactorY - 2)*0.12;
        else if (ratioFactorY < 3)
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(ratioFactorY - 2.46)*0.07;
        else if (ratioFactorY < 4.8)
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(ratioFactorY - 3)*0.04;
        else if (ratioFactorY < 8)
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(ratioFactorY - 4.8)*0.02;
        else
            gs::scale = gs::scale - m*(1.2 - 1)*1.6 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(8 - 4.8)*0.02;
        
        m = gs::scScale;
        if (ratioFactorY < 1.2)
            gs::scScale = gs::scScale - m*(ratioFactorY - 1)*0.72;
        else if (ratioFactorY < 2)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(ratioFactorY - 1.2)*0.17;
        else if (ratioFactorY < 2.46)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(ratioFactorY - 2)*0.12;
        else if (ratioFactorY < 3)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(ratioFactorY - 2.46)*0.07;
        else if (ratioFactorY < 4.8)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(ratioFactorY - 3)*0.04;
        else if (ratioFactorY < 8)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(ratioFactorY - 4.8)*0.02;
        else
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(8 - 4.8)*0.02;
    }
    else if (ratioFactorX > 1)
    {
        float m = gs::scale;
        if (ratioFactorX < 1.9)
            gs::scale = gs::scale - m*(ratioFactorX - 1)*0.175;
        else if (ratioFactorX < 3)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(ratioFactorX - 1.9)*0.27;
        else if (ratioFactorX < 4.24)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(ratioFactorX - 3)*0.15;
        else if (ratioFactorX < 7)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(ratioFactorX - 4.24)*0.05;
        else if (ratioFactorX < 11)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(ratioFactorX - 7)*0.02;
        else
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(11 - 7)*0.02;
        gs::scScale = gs::scale;
    }
    if (gs::scale < 0.18) gs::scale = 0.18;
    if (gs::scScale < 0.18) gs::scScale = 0.18;
}



void SetResolutionClass()
{
    if (gs::resolutionClassSetting == -1)
    {
        unsigned int width;
        unsigned int height;
        
        if (sf::VideoMode::getDesktopMode().width >= sf::VideoMode::getDesktopMode().height)
        {
            width = sf::VideoMode::getDesktopMode().width;
            height = sf::VideoMode::getDesktopMode().height;
        }
        else
        {
            width = sf::VideoMode::getDesktopMode().height;
            height = sf::VideoMode::getDesktopMode().width;
        }
        
        if (width <= 640 && height <= 480) gs::resolutionClass = 0;
        else if (width <= 1024 && height <= 768) gs::resolutionClass = 1;
        else if (width <= 1920 && height <= 1080) gs::resolutionClass = 2;
        else gs::resolutionClass = 3;
        
#ifdef SFML_SYSTEM_IOS
        std::string device = iOSDeviceName();
        if (nss::Command(device, "iPhone"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 6));
            if (version <= 4) gs::resolutionClass = 0;
            else if (version <= 7) gs::resolutionClass = 1;
            else if (version == 8)
            {
                int underVersion = base::ConvertToInt(nss::ParseUntil(device, '\0', 8));
                if (underVersion == 4) gs::resolutionClass = 1;
                else gs::resolutionClass = 2;
            }
        }
        else if (nss::Command(device, "iPad"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 4));
            if (version <= 1) gs::resolutionClass = 0;
            else if (version <= 2) gs::resolutionClass = 1;
        }
        else if (nss::Command(device, "iPod"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 4));
            if (version <= 3) gs::resolutionClass = 0;
            else if (version <= 4) gs::resolutionClass = 1;
            else if (version <= 6) gs::resolutionClass = 1;
        }
#endif
        
        cout << "main :: GlobalSettings :: Autocomputed ResolutionClass is " << gs::resolutionClass << "@x." << endl;
    }
    else
        gs::resolutionClass = gs::resolutionClassSetting;
}



void ExecuteInitalization()
{
    CalculateScaleRatios(gs::width, gs::height);
    
    AndroidInitalization();
    
    gs::Load(L"UserDefined.nsoptions");
    SetResolutionClass();
}




#if defined(SFML_SYSTEM_ANDROID)
    char* androidFilesPath = new char[255];
    std::string documentsPath() { return std::string(androidFilesPath); }
    char* androidDataPath = new char[255];
    std::string externalDataPath() { return std::string(androidDataPath); }
#endif

void AndroidInitalization()
{
#if defined(SFML_SYSTEM_ANDROID)
    const char* androidFilesPath1 = sf::getNativeActivity()->internalDataPath;
    int i; for (i = 0; androidFilesPath1[i] != '\0'; ++i) androidFilesPath[i] = androidFilesPath1[i];
        androidFilesPath[i++] = '/'; androidFilesPath[i] = '\0';
        LOGE("InternalDataPath: %s", androidFilesPath);
        
        const char* androidDataPath1 = sf::getNativeActivity()->externalDataPath;
        for (i = 0; androidDataPath1[i] != '\0'; ++i) androidDataPath[i] = androidDataPath1[i];
            androidDataPath[i++] = '/'; androidDataPath[i] = '\0';
            LOGE("ExternalDataPath: %s", androidDataPath);
            
            
            if (!base::FileExists(base::utf16(std::string(androidDataPath) + "Built-in Novels/")))
                base::CreateDirectory(base::utf16(std::string(androidDataPath) + "Built-in Novels/"));
    
    ANativeActivity* activity = sf::getNativeActivity();
    JNIEnv* lJNIEnv = activity->env;
    (activity->vm)->AttachCurrentThread(&lJNIEnv, NULL);
    
    // Retrieve the NativeActivity
    jobject ObjectNativeActivity = activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(ObjectNativeActivity);
    
    // Retrieve the ActivityInfo
    jmethodID MethodGetAssetManager = lJNIEnv->GetMethodID(ClassNativeActivity, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject ObjectAssetManager = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetAssetManager);
    
    AAssetManager* mgr = AAssetManager_fromJava(lJNIEnv, ObjectAssetManager);
    
    bool updateTheNovel{ true };
    std::ifstream fileVersion;
    fileVersion.open(std::string(androidDataPath) + "Built-in Novels/Bundle/version.novelversion");
    if (fileVersion.is_open())
    {
        std::string line; std::getline(fileVersion, line);
        int versionOld = std::atoi(line.c_str()); LOGE("Version OLD: %i", versionOld);
        AAsset* assetVersion = AAssetManager_open(mgr, "Novels/Bundle/version.novelversion", AASSET_MODE_BUFFER);
        if (assetVersion != NULL)
        {
            size_t fileLength = AAsset_getLength(assetVersion);
            char* fileContent = new char[fileLength+1];
            AAsset_read(assetVersion, fileContent, fileLength);
            fileContent[fileLength] = '\0';
            
            int versionNew = std::atoi(fileContent); LOGE("Version NEW: %i", versionNew);
            updateTheNovel = (versionOld != versionNew);
            
            AAsset_close(assetVersion);
        }
    }
    
    if (updateTheNovel)
    {
        std::vector<std::string> directories = { "/Bundle", "/Bundle/Backgrounds", "/Bundle/Characters", "/Bundle/Music", "/Bundle/Sounds", "/Example", base::utf8(L"/Чувственное познание") };
        for (auto& dir : directories)
        {
            if (!base::FileExists(base::utf16(std::string(androidDataPath) + "Built-in Novels" + dir)))
                base::CreateDirectory(base::utf16(std::string(androidDataPath) + "Built-in Novels" + dir));
            
            AAssetDir* assetDir = AAssetManager_openDir(mgr, ("Novels" + dir).c_str());
            const char* filename = (const char*)NULL;
            std::string fileto = std::string(std::string(androidDataPath) + "Built-in Novels" + dir + "/");
            
            while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
            {
                AAsset* asset = AAssetManager_open(mgr, ("Novels" + dir + "/" + std::string(filename)).c_str(), AASSET_MODE_BUFFER);
                if (asset != NULL)
                {
                    const char* filenameto = (fileto + std::string(filename)).c_str();
                    LOGE("filenameto: %s", filenameto);
                    
                    size_t fileLength = AAsset_getLength(asset);
                    char* fileContent = new char[fileLength+1];
                    AAsset_read(asset, fileContent, fileLength);
                    fileContent[fileLength] = '\0';
                    
                    FILE* out = fopen(filenameto, "w");
                    fwrite(fileContent, sizeof(char), fileLength+1, out);
                    fclose(out);
                    
                    delete [] fileContent;
                    AAsset_close(asset);
                }
            }
            AAssetDir_close(assetDir);
        }
    }
    
    (activity->vm)->DetachCurrentThread();
#endif
}
