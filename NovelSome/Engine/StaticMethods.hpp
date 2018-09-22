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
        static sf::Font* GetFont(const std::string& fontName);
        static void FreeFonts();
    };
    
    
    
    class GlobalSettings
    {
    public:
        static sf::RenderWindow* window;
        
        static unsigned int width;
        static unsigned int height;
        
        static unsigned int relativeWidth;
        static unsigned int relativeHeight;
        static float scale;
        
        static int windowPositionOffset;
        
        static bool isVerticalSyncEnabled;
        static int framerateLimit;
        static int framerateNoFocus;
        
        static bool isPauseEnabled;
        static bool isPause;
        
        static bool isParallaxEnabled;
        static float defaultParallaxBackground;
        static float defaultParallaxFar;
        static float defaultParallaxNormal;
        static float defaultParallaxClose;
        static float defaultParallaxFrontground;
    };
    
    typedef GlobalSettings gs;
    typedef FontCollector fc;
}

#endif /* StaticMethods_hpp */
