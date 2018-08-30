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

#include "../ResourcePath.hpp"

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
    };
}

#endif /* StaticMethods_hpp */
