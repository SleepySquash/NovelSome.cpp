//
//  Misc.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Misc_hpp
#define Misc_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <minEH/Engine/NovelSystem.hpp>

#include "Abstract/Savable.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct Waiting : NovelObject, Savable
        {
            float currentTime{ 0.f }, waitingTime{ 1.f };
            Waiting();
            void Update(const sf::Time& elapsedTime) override;
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Misc_h */
