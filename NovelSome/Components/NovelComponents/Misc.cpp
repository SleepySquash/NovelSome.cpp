//
//  Misc.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Misc.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Waiting::Update(const sf::Time& elapsedTime)
        {
            if (currentTime < waitingTime) currentTime += elapsedTime.asSeconds();
            else { novelSystem->SendMessage({"UnHold", this}); novelSystem->PopComponent(this); }
        }
    }
}
