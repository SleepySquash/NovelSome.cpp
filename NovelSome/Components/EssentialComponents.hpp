//
//  EssentialComponents.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef EssentialComponents_hpp
#define EssentialComponents_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Engine/EntitySystem.hpp"
#include "../Engine/StaticMethods.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace EssentialComponents
    {
        class DebugComponent : public Component
        {
        private:
            sf::Text performanceText;
            sf::Text projectText;
            sf::Text buildText;
            bool fontLoaded{ false };
            
            sf::Time deltaTime{ sf::Time::Zero };
            int frames{ 0 };
            
        public:
            DebugComponent(sf::String build);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            
        };
        
        
        
        class GamePause : public Component
        {
        private:
            sf::RectangleShape shape;
            
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum{ appearing, waiting, disappearing };
            modeEnum mode{ waiting };
            
            int maxAlpha{ 170 };
            float appearTime{ 0.15f };
            float disappearTime{ 0.15f };
            
            GamePause();
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow *window) override;
            void Resize(unsigned int width, unsigned int height) override;
        };
    }
}

#endif /* EssentialComponents_hpp */
