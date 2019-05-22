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

#include "../../../Essentials/Base.hpp"
#include "../../../Engine/Settings.hpp"
#include "../../../Engine/GUIInterface.hpp"
#include "../../../Engine/NovelSomeScript.hpp"

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
            
            ~Interface();
            void RestoreToDefaults();
            void LoadFromFile(const std::wstring& fileName);
        };
    }
}

#endif /* Interface_hpp */
