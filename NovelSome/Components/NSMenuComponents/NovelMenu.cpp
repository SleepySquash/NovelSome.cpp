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
        NovelMenu::NovelMenu(const std::wstring& menuPath, const std::wstring& scenario, NovelInfo* nvl) : menuPath(menuPath), scenario(scenario), nvl(nvl) { }
        void NovelMenu::Init()
        {
            std::wstring fullPath = menuPath;
            if (!base::FileExists(fullPath)) fullPath = utf16(resourcePath()) + fullPath;
            if (!guiSystem.LoadFromFile(fullPath, L"menu"))
            {
                
            }
            
            backToMenu.setFont(lang::menufont);
            backToMenu.setString(L"< " + lang::get("Return to menu"));
            backToMenu.halign = Halign::Left;
            backToMenu.setCharacterSize(35);
        }
        void NovelMenu::PollEvent(sf::Event& event)
        {
            if (!active || gs::ignoreEvent) return;
            guiSystem.PollEvent(event);
            if (backToMenu.PollEvent(event)) { entity->SendMessage({"NovelMenu :: ReturnToMenu"}); entity->PopComponent(this); }
            // entity->system->SendMessage({"StartNovel", scenario, &(*novelSelected)});
        }
        void NovelMenu::Resize(const unsigned int& width, const unsigned int& height)
        {
            if (!active) return;
            guiSystem.Resize(width, height);
            backToMenu.Resize(width, height);
            backToMenu.setPosition(10*gs::scale, gs::screenOffsetTop*1.4 + 10*gs::scale);
        }
        void NovelMenu::Draw(sf::RenderWindow* window)
        {
            if (!active) return;
            guiSystem.Draw(window);
            backToMenu.Draw(window);
        }
        void NovelMenu::ReceiveMessage(MessageHolder& message)
        {
            if (message.info == "Novel :: Starting") { active = false;
                /*if (novelMusic.getStatus() == sf::Music::Status::Playing) novelMusic.pause();*/ }
            else if (message.info == "Novel :: Destroying")
            {
                active = true;
                /*if (novelMusic.getStatus() == sf::Music::Status::Paused) {
                    novelMusic.play(); novelMusic.setPlayingOffset(sf::seconds(novelMusic_from)); }*/
                gs::requestWindowRefresh = true;
                Resize(gs::width, gs::height);
                message = MessageHolder();
            }
            else if (message.info == "Load")
            {
                entity->SendMessage({"SaveUI :: Novel", nvl});
                entity->SendMessage({"LoadUI"});
                message = MessageHolder();
            }
            else if (message.info == "NM :: Start")
            {
                entity->system->SendMessage({"StartNovel", scenario, nvl});
                message = MessageHolder();
            }
            else if (active) guiSystem.NovelObject::ReceiveMessage(message);
        }
    }
}
