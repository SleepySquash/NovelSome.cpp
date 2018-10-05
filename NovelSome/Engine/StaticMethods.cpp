//
//  StaticMethods.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "StaticMethods.hpp"

namespace ns
{
    sf::Font* FontCollector::GetFont(const std::wstring& fontName)
    {
        if (fonts.find(fontName) != fonts.end())
            return fonts.at(fontName);
        else
        {
            sf::Font* font = new sf::Font();
            
            sf::String fullPath = sf::String(resourcePath()) + L"Data/Fonts/" + fontName;
            bool fontLoaded{ false };
#ifdef _WIN32
            //TODO: Fix memory leak
            std::ifstream ifStream(fullPath.toWideString(), std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory, filesize);
                ifStream.close();
                
                fontLoaded = font->loadFromMemory(fileInMemory, filesize);
            }
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(fullPath.toWideString());
            if (!(fontLoaded = font->loadFromFile(u8str)))
                std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
#endif
            
            if (fontLoaded)
            {
                fonts.emplace(fontName, font);
                return font;
            }
            else
                return nullptr;
        }
    }
    void FontCollector::FreeFonts()
    {
        for (const auto& key : fonts)
            if (key.second != nullptr)
                delete key.second;
        fonts.clear();
    }
    std::unordered_map<std::wstring, sf::Font*> FontCollector::fonts = { {L"", nullptr} };
    
    
    
    
    
    
    
    sf::Image* ImageCollector::LoadImage(const std::wstring& imageName, unsigned int mode)
    {
        if (images.find(imageName) != images.end())
        {
            usage[imageName]++;
            return images.at(imageName);
        }
        else
        {
            sf::Image* image = new sf::Image();
            
            std::wstring fullPath = sf::String(resourcePath()) + imageName;
            
            if (mode != 0 && !base::FileExists(fullPath))
            {
                std::wstring getExtention = base::GetExtentionFromString(fullPath);
                std::wstring woExtention = base::GetStringWithNoExtention(fullPath, getExtention);
                if (!base::FileExists(sf::String(woExtention + L"@0x" + getExtention)) &&
                    !base::FileExists(sf::String(woExtention + L"@1x" + getExtention)) &&
                    !base::FileExists(sf::String(woExtention + L"@2x" + getExtention)) &&
                    !base::FileExists(sf::String(woExtention + L"@3x" + getExtention)))
                {
                    bool found{ false };
                    std::wstring onlyFolder = base::GetFolderPath(fullPath);
                    
                    std::wstring onlyFileName = L"";
                    for (int i = onlyFolder.length(); i < fullPath.length(); i++)
                        onlyFileName += fullPath[i];
                    
                    switch (mode)
                    {
                        case 1:
                            for (int i = 0; i <= 1 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0:
                                        currentPath = onlyFolder + L"backgrounds/" + onlyFileName;
                                        break;
                                    case 1:
                                        currentPath = onlyFolder + L"Backgrounds/" + onlyFileName;
                                        break;
                                    default:
                                        break;
                                }
                                
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath))))
                                    fullPath = currentPath;
                            }
                            break;
                            
                        case 2:
                            for (int i = 0; i <= 1 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0:
                                        currentPath = onlyFolder + L"images/" + onlyFileName;
                                        break;
                                    case 1:
                                        currentPath = onlyFolder + L"sprites/" + onlyFileName;
                                        break;
                                    case 2:
                                        currentPath = onlyFolder + L"Images/" + onlyFileName;
                                        break;
                                    case 3:
                                        currentPath = onlyFolder + L"Sprites/" + onlyFileName;
                                        break;
                                    default:
                                        break;
                                }
                                
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath))))
                                    fullPath = currentPath;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            
            if (GlobalSettings::isResolutionClassEnabled)
            {
                std::wstring getExtention = base::GetExtentionFromString(fullPath);
                std::wstring woExtention = base::GetStringWithNoExtention(fullPath, getExtention);
                
                switch (ns::gs::resolutionClass)
                {
                    case 0:
                        if (base::FileExists(woExtention + L"@0x" + getExtention))
                            fullPath = woExtention + L"@0x" + getExtention;
                        else if (base::FileExists(woExtention + L"@1x" + getExtention))
                            fullPath = woExtention + L"@1x" + getExtention;
                        else if (base::FileExists(woExtention + L"@2x" + getExtention))
                            fullPath = woExtention + L"@2x" + getExtention;
                        else if (base::FileExists(woExtention + L"@3x" + getExtention))
                            fullPath = woExtention + L"@3x" + getExtention;
                        break;
                        
                    case 1:
                        if (base::FileExists(woExtention + L"@1x" + getExtention))
                            fullPath = woExtention + L"@1x" + getExtention;
                        else if (base::FileExists(woExtention + L"@2x" + getExtention))
                            fullPath = woExtention + L"@2x" + getExtention;
                        else if (base::FileExists(woExtention + L"@0x" + getExtention))
                            fullPath = woExtention + L"@0x" + getExtention;
                        else if (base::FileExists(woExtention + L"@3x" + getExtention))
                            fullPath = woExtention + L"@3x" + getExtention;
                        break;
                        
                    case 2:
                        if (base::FileExists(woExtention + L"@2x" + getExtention))
                            fullPath = woExtention + L"@2x" + getExtention;
                        else if (base::FileExists(woExtention + L"@1x" + getExtention))
                            fullPath = woExtention + L"@1x" + getExtention;
                        else if (base::FileExists(woExtention + L"@3x" + getExtention))
                            fullPath = woExtention + L"@3x" + getExtention;
                        else if (base::FileExists(woExtention + L"@0x" + getExtention))
                            fullPath = woExtention + L"@0x" + getExtention;
                        break;
                        
                    case 3:
                        if (base::FileExists(woExtention + L"@3x" + getExtention))
                            fullPath = woExtention + L"@3x" + getExtention;
                        else if (base::FileExists(woExtention + L"@2x" + getExtention))
                            fullPath = woExtention + L"@2x" + getExtention;
                        else if (base::FileExists(woExtention + L"@1x" + getExtention))
                            fullPath = woExtention + L"@1x" + getExtention;
                        else if (base::FileExists(woExtention + L"@0x" + getExtention))
                            fullPath = woExtention + L"@0x" + getExtention;
                        break;
                        
                    default:
                        break;
                }
            }
            else
            {
                if (!base::FileExists(fullPath))
                {
                    std::wstring getExtention = base::GetExtentionFromString(fullPath);
                    std::wstring woExtention = base::GetStringWithNoExtention(fullPath, getExtention);
                    
                    if (base::FileExists(sf::String(woExtention + L"@3x" + getExtention)))
                        fullPath = woExtention + L"@3x" + getExtention;
                    else if (base::FileExists(sf::String(woExtention + L"@2x" + getExtention)))
                        fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(sf::String(woExtention + L"@1x" + getExtention)))
                        fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(sf::String(woExtention + L"@0x" + getExtention)))
                        fullPath = woExtention + L"@0x" + getExtention;
                }
            }
            
            bool imageLoaded{ false };
#ifdef _WIN32
            std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << base::ConvertToUTF8(fullPath) << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory, filesize);
                ifStream.close();
                
                imageLoaded = image->loadFromMemory(fileInMemory, filesize);
                delete[] fileInMemory;
            }
#else
            if (!(imageLoaded = image->loadFromFile(base::ConvertToUTF8(fullPath))))
                std::cerr << "Unable to open file: " << base::ConvertToUTF8(fullPath) << std::endl;
#endif
            
            if (imageLoaded)
            {
                usage.emplace(imageName, 1);
                images.emplace(imageName, image);
                return image;
            }
            else
                return nullptr;
        }
    }
    void ImageCollector::DeleteImage(const std::wstring& imageName)
    {
        if (images.find(imageName) != images.end())
        {
            usage[imageName]--;
            if (usage.at(imageName) <= 0)
            {
                sf::Image* image = images.at(imageName);
                if (image != nullptr)
                {
                    usage.erase(imageName);
                    images.erase(imageName);
                    
                    delete image;
                }
            }
        }
    }
    void ImageCollector::FreeImages()
    {
        for (const auto& key : images)
            if (key.second != nullptr)
                delete key.second;
        images.clear();
        usage.clear();
    }
    std::unordered_map<std::wstring, int> ImageCollector::usage = { {L"", 0} };
    std::unordered_map<std::wstring, sf::Image*> ImageCollector::images = { {L"", nullptr} };
    
    
    
    
    
    
    sf::RenderWindow* GlobalSettings::window = nullptr;
    
    unsigned int GlobalSettings::width = 0;
    unsigned int GlobalSettings::height = 0;
    
    unsigned int GlobalSettings::resizeToWidth = 0;
    unsigned int GlobalSettings::resizeToHeight = 0;
    
    unsigned int GlobalSettings::relativeWidth = 0;
    unsigned int GlobalSettings::relativeHeight = 0;
    float GlobalSettings::scale = 1.f;
    
#ifdef _WIN32
    int GlobalSettings::windowPositionOffset = IsWindows8OrGreater() ? GetSystemMetrics(SM_CXSIZEFRAME) : 0;
#else
    int GlobalSettings::windowPositionOffset = 0;
#endif
    
    bool GlobalSettings::isVerticalSyncEnabled = true;
    int GlobalSettings::framerateLimit = 120;
    int GlobalSettings::framerateNoFocus = 30;
    
    bool GlobalSettings::isResolutionClassEnabled = true;
    int GlobalSettings::resolutionClassSetting = -1;
    int GlobalSettings::resolutionClass = 1;
    
    bool GlobalSettings::isPauseEnabled = true;
    bool GlobalSettings::isPause = false;
    
    bool GlobalSettings::isParallaxEnabled = true;
    float GlobalSettings::defaultParallaxBackground = 0.018;
    float GlobalSettings::defaultParallaxFar = 0.032;
    float GlobalSettings::defaultParallaxNormal = 0.062;
    float GlobalSettings::defaultParallaxClose = 0.105;
    float GlobalSettings::defaultParallaxFrontground = 0.13;
    
    bool GlobalSettings::forcePressInsideDialogue = true;
    
    float GlobalSettings::maxVolumeGlobal = 1.f;
    float GlobalSettings::maxVolumeMusic = 1.f;
    float GlobalSettings::maxVolumeAmbeint = 1.f;
    float GlobalSettings::maxVolumeSound = 1.f;
}
