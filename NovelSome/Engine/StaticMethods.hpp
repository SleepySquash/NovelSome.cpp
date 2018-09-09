//
//  StaticMethods.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef StaticMethods_hpp
#define StaticMethods_hpp

#include <iostream>
#include <unordered_map>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/ResourcePath.hpp"
#ifdef _WIN32
    #include "../Essentials/versionhelpers.h"
#endif

namespace ns
{
    class FontCollector
    {
    private:
        FontCollector() { }
        
    public:
        static std::unordered_map<std::string, sf::Font*> fonts;
        static sf::Font& GetFont(std::string fontName);
        static void FreeFonts();
    };
    
    
    
    class GlobalSettings
    {
    public:
        static unsigned int width;
        static unsigned int height;
        static int windowPositionOffset;
        static bool isPauseEnabled;
        static bool isPause;
    };
}

#endif /* StaticMethods_hpp */
