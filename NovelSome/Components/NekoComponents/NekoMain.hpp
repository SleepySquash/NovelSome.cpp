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

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/List.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors.hpp>

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
