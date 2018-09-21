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
    sf::Font* FontCollector::GetFont(const std::string& fontName)
    {
        if (fonts.find(fontName) != fonts.end())
            return fonts.at(fontName);
        else
        {
            sf::Font* font = new sf::Font();
            if (font->loadFromFile(resourcePath() + "Data/Fonts/" + fontName))
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

    std::unordered_map<std::string, sf::Font*> FontCollector::fonts = { {"", nullptr} };
    
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
    bool GlobalSettings::isPauseEnabled = true;
    bool GlobalSettings::isPause = false;
}
