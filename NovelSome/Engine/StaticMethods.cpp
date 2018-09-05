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
    sf::Font& FontCollector::GetFont(std::string fontName)
    {
        if (fonts.find(fontName) != fonts.end())
            return *fonts.at(fontName);
        else
        {
            sf::Font* font = new sf::Font();
            if (font->loadFromFile(resourcePath() + "Data/Fonts/" + fontName))
            {
                fonts.emplace(fontName, font);
                return *font;
            }
        }
    }
    void FontCollector::FreeFonts()
    {
        for (const auto& key : fonts)
        {
            if (key.second != nullptr)
                delete key.second;
        }
    }

    std::unordered_map<std::string, sf::Font*> FontCollector::fonts = { {"", nullptr} };
    
    unsigned int GlobalSettings::width = 0;
    unsigned int GlobalSettings::height = 0;
    bool GlobalSettings::isPauseEnabled = true;
    bool GlobalSettings::isPause = false;
}
