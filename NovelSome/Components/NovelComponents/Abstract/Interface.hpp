//
//  Interface.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Interface_hpp
#define Interface_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "Skin.hpp"
#include "../GUISystem.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct Interface
        {
            GUISystem guiPause;
            GUISystem guiDialogue;
            GUISystem guiChoose;
            
            std::wstring defaultDialogue, defaultPause, defaultChoose;
            bool ignoreDialogue{ false }, ignorePause{ false }, ignoreChoose{ false };
            
            ~Interface();
            enum class RestoreInterface{ All, Dialogue, Choose, Pause };
            void RestoreToDefaults(const RestoreInterface& restore = RestoreInterface::All);
            void LoadFromFile(const std::wstring& fileName, const std::wstring& folderPath = L"");
        };
    }
}

#endif /* Interface_hpp */
