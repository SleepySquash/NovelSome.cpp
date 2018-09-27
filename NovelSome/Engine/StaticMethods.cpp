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
            std::ifstream ifStream(fullPath.toWideString(), std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                fileInMemory.reset(new char[static_cast<unsigned int>(filesize)]);
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory.get(), filesize);
                ifStream.close();
                
                fontLoaded = font->loadFromMemory(fileInMemory.get(), filesize);
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
    
    sf::RenderWindow* GlobalSettings::window = nullptr;
    
    unsigned int GlobalSettings::width = 0;
    unsigned int GlobalSettings::height = 0;
    
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
