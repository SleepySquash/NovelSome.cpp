//
//  NovelMenu.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 04/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "NovelMenu.hpp"

namespace ns
{
    namespace NSMenuComponents
    {
        void NovelMenu::Init() { }
        void NovelMenu::PollEvent(sf::Event& event)
        {
            if (!active) return;
            guiSystem.PollEvent(event);
        }
        void NovelMenu::Resize(const unsigned int& width, const unsigned int& height)
        {
            if (!active) return;
            guiSystem.Resize(width, height);
        }
        void NovelMenu::Draw(sf::RenderWindow* window)
        {
            if (!active) return;
            guiSystem.Draw(window);
        }
        void NovelMenu::ReceiveMessage(MessageHolder& message)
        {
            if (message.info == "Novel :: Destroying")
            {
                active = true;
                /*if (novelMusic.getStatus() == sf::Music::Status::Paused) {
                    novelMusic.play(); novelMusic.setPlayingOffset(sf::seconds(novelMusic_from)); }*/
                gs::requestWindowRefresh = true;
                Resize(gs::width, gs::height);
                message = MessageHolder();
            }
            else if (active) guiSystem.NovelObject::ReceiveMessage(message);
        }
    }
}
