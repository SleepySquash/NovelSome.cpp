//
//  Client.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 29/05/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#ifndef ClientComponent_hpp
#define ClientComponent_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors.hpp"
#include "../../Engine/Client.hpp"

using namespace ns;
using std::endl;
using std::cout;
using std::list;
using std::vector;
using ns::base::utf8;
using ns::base::utf16;

namespace HeavensGate
{
    struct ClientComponent : Component
    {
        void Init() override;
        void Update(const sf::Time& elapsedTime) override;
        void PollEvent(sf::Event& event) override;
        void Resize(const unsigned int& width, const unsigned int& height) override;
    };
}

#endif /* ClientComponent_hpp */
