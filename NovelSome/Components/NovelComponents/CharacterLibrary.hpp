//
//  CharacterLibrary.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef CharacterLibrary_hpp
#define CharacterLibrary_hpp

#include <iostream>
#include <list>
#include <vector>

#include <locale>
#include <fstream>
#include <codecvt>
#include <unordered_map>
#include <dirent.h>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;
using std::list;

namespace ns
{
    namespace NovelComponents
    {
        struct CharacterData
        {
            std::wstring name{ L"" }, display{ L"" }, filePath{ L"" };
            sf::Color fillColor{ sf::Color::White };
            sf::Color outlineColor{ sf::Color::Black };
            float outlineThickness{ 0.f };
        };
        
        struct CharacterLibrary
        {
            static std::unordered_map<std::wstring, CharacterData*> library;
            
            static void clear();
            static std::unordered_map<std::wstring, CharacterData*>::const_iterator find(const std::wstring& key);
            static bool exists(const std::wstring& key);
            static CharacterData* at(const std::wstring& key);
            static void ScanForCharacters(const std::wstring& folderPath);
            
        private:
            CharacterLibrary();
            ~CharacterLibrary();
        };
        typedef CharacterLibrary cl;
    }
}


#endif /* CharacterLibrary_hpp */
