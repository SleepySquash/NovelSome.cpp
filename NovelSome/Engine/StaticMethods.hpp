//
//  StaticMethods.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef StaticMethods_hpp
#define StaticMethods_hpp

#include <iostream>
#include <unordered_map>

#include <codecvt>

#ifdef _WIN32
    //std::ifstream in GetFont
    #include <fstream>
#endif

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/ResourcePath.hpp"
#ifdef _WIN32
#include "../Essentials/versionhelpers.h"
#endif
#include "../Essentials/Base.hpp"

namespace ns
{
    class FontCollector
    {
    private:
        FontCollector() { }
        
    public:
        static std::unordered_map<std::wstring, sf::Font*> fonts;
        static sf::Font* GetFont(const std::wstring& fontName);
        static void FreeFonts();
    };
    
    
    
    class ImageCollector
    {
    private:
        ImageCollector() { }
        
    public:
        static std::unordered_map<std::wstring, sf::Image*> images;
        static std::unordered_map<std::wstring, int> usage;
        
        static sf::Image* LoadImage(const std::wstring& imageName, unsigned int mode = 0);
        static void DeleteImage(const std::wstring& imageName);
        static void FreeImages();
    };
    
    
    
    class GlobalSettings
    {
    public:
        static sf::RenderWindow* window;
        
        static unsigned int width;
        static unsigned int height;
        
        static unsigned int resizeToWidth;
        static unsigned int resizeToHeight;
        
        static unsigned int relativeWidth;
        static unsigned int relativeHeight;
        static float scale;
        
        static int windowPositionOffset;
        
        static bool isVerticalSyncEnabled;
        static int framerateLimit;
        static int framerateNoFocus;
        
        static bool isResolutionClassEnabled;
        static int resolutionClassSetting;
        static int resolutionClass;
        
        static bool isPauseEnabled;
        static bool isPause;
        
        static bool isParallaxEnabled;
        static float defaultParallaxBackground;
        static float defaultParallaxFar;
        static float defaultParallaxNormal;
        static float defaultParallaxClose;
        static float defaultParallaxFrontground;
        
        static bool forcePressInsideDialogue;
        
        static float maxVolumeGlobal;
        static float maxVolumeMusic;
        static float maxVolumeAmbeint;
        static float maxVolumeSound;
    };
    
    typedef GlobalSettings gs;
    typedef FontCollector fc;
    typedef ImageCollector ic;
}

#endif /* StaticMethods_hpp */
