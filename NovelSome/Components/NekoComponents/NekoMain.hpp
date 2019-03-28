//
//  NekoMain.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 09/10/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef NekoMain_hpp
#define NekoMain_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors.hpp"

namespace nekoui
{
    struct Game : public ns::Component
    {
        ns::EntitySystem system;
        
        void Update(const sf::Time& elapsedTime) override;
    };
    
    struct Neko : public ns::Component
    {
        sf::Texture texture;
        sf::Sprite sprite;
    };
}

#endif /* NekoMain_hpp */
