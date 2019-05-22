//
//  SavingMechanism.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 10/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#ifndef SavingMechanism_hpp
#define SavingMechanism_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../../Essentials/Base.hpp"
#include "../../../Engine/NovelSystem.hpp"
#include "../../../Engine/Settings.hpp"
#include "../../../Engine/NovelSomeScript.hpp"

#include "Savable.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

#undef CreateDirectory
namespace ns
{
    namespace NovelComponents
    {
        struct SaveLoadManager
        {
            std::unordered_map<std::wstring, std::function<void>> loadable;
            SaveLoadManager();
        };
        
        void SaveGameState(const std::wstring& fileName, NovelSystem& system);
        void LoadGameState(std::wstring fileName, NovelSystem& system);
    }
}

#endif /* SavingMechanism_hpp */
