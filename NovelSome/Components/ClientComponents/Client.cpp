//
//  Client.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 29/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "Client.hpp"

namespace HeavensGate
{
    void ClientComponent::Init()
    {
        Client::Connect();
    }
    void ClientComponent::Update(const sf::Time& elapsedTime)
    {
        
    }
    void ClientComponent::PollEvent(sf::Event& event)
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space) Client::Send();
    }
    void ClientComponent::Resize(const unsigned int& width, const unsigned int& height)
    {
        
    }
}
