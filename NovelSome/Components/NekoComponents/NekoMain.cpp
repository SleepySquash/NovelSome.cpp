//
//  NekoMain.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 09/10/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "NekoMain.hpp"

namespace nekoui
{
    void Game::Update(const sf::Time &elapsedTime)
    {
        system.Update(elapsedTime);
    }
}
