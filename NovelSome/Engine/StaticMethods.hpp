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

#include <thread>
#include <codecvt>
#include <fstream>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/ResourcePath.hpp"
#ifdef _WIN32
    #include "../Essentials/versionhelpers.h"
#endif
#include "../Essentials/Base.hpp"
#include "NovelSomeScript.hpp"

using base::utf8;
using base::utf16;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;

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
    
    
    
    extern bool Collector_UseFullPath;
    std::wstring PathWithFolderDetermination(const std::wstring& fileName, unsigned int mode);
    std::wstring PathWithResolutionDetermination(const std::wstring& fileName, unsigned int mode);
    struct icThreadsJoiner;
    struct ImageCollectorObject
    {
        sf::Image* image{ nullptr };
        sf::Texture* texture{ nullptr };
        int usage{ 0 };
        bool destroyable{ true };
        
        ImageCollectorObject(sf::Image* image = nullptr, sf::Texture* texture = nullptr, int usage = 0, bool destroyable = true) : image(image), texture(texture), usage(usage), destroyable(destroyable) { }
    };
    class ImageCollector
    {
    private:
        ImageCollector() { }
        
    public:
        static std::unordered_map<std::wstring, ImageCollectorObject> images;
        static std::unordered_map<std::wstring, std::thread> threads;
        
        static icThreadsJoiner threadsJoiner;
        static sf::Image* LoadImage(const std::wstring& imageName, unsigned int mode = 2);
        static void ThreadImage(std::wstring imageName, unsigned int mode, bool destroyable = true, bool loadTexture = false);
        static void PreloadImage(const std::wstring& imageName, unsigned int mode = 2, bool destroyable = true);
        static sf::Texture* LoadTexture(const std::wstring& imageName, unsigned int mode = 2);
        static void PreloadTexture(const std::wstring& imageName, unsigned int mode = 2, bool destroyable = true);
        static sf::Texture* RequestHigherTexture(const std::wstring& imageName, unsigned int mode = 2);
        static sf::Image* FindImage(const std::wstring& imageName);
        static sf::Texture* FindTexture(const std::wstring& imageName);
        static void SetDestroyable(std::wstring imageName, bool destroyable);
        static void DeleteImage(const std::wstring& imageName);
        static void EraseImage(const std::wstring& imageName);
        static void FreeImages();
    };
    struct icThreadsJoiner { ~icThreadsJoiner() { for (auto& t : ImageCollector::threads) t.second.join(); ImageCollector::threads.clear(); } };
    
    
    
    struct scThreadsJoiner;
    struct SoundCollectorObject
    {
        sf::SoundBuffer* sound{ nullptr };
        int usage{ 0 };
        bool destroyable{ true };
        
        SoundCollectorObject(sf::SoundBuffer* sound = nullptr, int usage = 0, bool destroyable = true) : sound(sound), usage(usage), destroyable(destroyable) { }
    };
    class SoundCollector
    {
    private:
        SoundCollector() { }
        
    public:
        static std::unordered_map<std::wstring, SoundCollectorObject> sounds;
        static std::unordered_map<std::wstring, std::thread> threads;
        
        static scThreadsJoiner threadsJoiner;
        static sf::SoundBuffer* LoadSound(const std::wstring& soundName, unsigned int mode = 3);
        static void ThreadSound(std::wstring soundName, unsigned int mode, bool destroyable = true);
        static void PreloadSound(const std::wstring& soundName, unsigned int mode = 3, bool destroyable = true);
        static void SetDestroyable(std::wstring soundName, bool destroyable);
        static void DeleteSound(const std::wstring& soundName);
        static void EraseSound(const std::wstring& soundName);
        static void FreeSounds();
    };
    struct scThreadsJoiner { ~scThreadsJoiner() { for (auto& t : SoundCollector::threads) t.second.join(); SoundCollector::threads.clear(); } };
    
    
    
    struct GlobalSettings
    {
        static void Load(const std::wstring& path);
        static void Save(const std::wstring& path);
        
        static sf::RenderWindow* window;
        
        static unsigned int width;
        static unsigned int height;
        
        static unsigned int resizeToWidth;
        static unsigned int resizeToHeight;
        
        static float scale, scScale;
        static float scalex, scaley;
        static unsigned int relativeWidth;
        static unsigned int relativeHeight;
        
        static int windowPositionOffset;
        
        static bool isVerticalSyncEnabled;
        static int framerateLimit;
        static int framerateNoFocus;
        
        static bool isResolutionClassEnabled;
        static int resolutionClassSetting;
        static int resolutionClass;
        
        static bool isPauseEnabled;
        static bool isPause;
        static bool inGame;
        static bool requestWindowRefresh;
        
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
    typedef SoundCollector sc;
}

#endif /* StaticMethods_hpp */
