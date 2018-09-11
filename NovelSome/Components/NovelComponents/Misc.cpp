//
//  Misc.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Waiting::Update(const sf::Time& elapsedTime)
        {
            if (novel != nullptr)
            {
                if (currentTime < waitingTime)
                    currentTime += elapsedTime.asSeconds();
                else
                {
                    novel->UnHold(this);
                    GetNovelSystem()->PopComponent(this);
                }
            }
        }
        void Waiting::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
    }
}
