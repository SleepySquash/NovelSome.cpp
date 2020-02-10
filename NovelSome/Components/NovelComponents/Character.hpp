//
//  Character.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Character_hpp
#define Character_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/List.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/NovelSystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors/Image.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "Abstract/CharacterLibrary.hpp"
#include "Abstract/Skin.hpp"
#include "Abstract/Savable.hpp"
#include "Abstract/Modes.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;
using std::vector;

namespace ns
{
    namespace NovelComponents
    {
        struct Character : NovelObject, Savable
        {
            sf::Sprite sprite;
            std::wstring image, folder, state;
            
            CharacterData* characterData{ nullptr };
            
            bool loaded{ false }, visible{ true };
            sf::Uint8 alpha{ 0 }, maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            
            Position position { Position::Center };
            float defaultPositionX{ 0.f }, defaultPositionY{ 0.f }, customX{ 0 }, customY{ 0 };
            float scaleX{ 1.f }, scaleY{ 1.f }, scaleRawX{ 1.f }, scaleRawY{ 1.f };
            
            Mode mode{ Mode::Appear }, switchTo{ Mode::Exist };
            MessageBack messageBack{ MessageBack::AtAppearance };
            
            bool doParallax{ gs::isParallaxEnabled };
            float parallaxPower { gs::defaultParallaxNormal };
            
            Character();
            void LoadState(const std::wstring& stateName);
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void ReceiveMessage(MessageHolder &message) override;
            void CalculateParallax(int mouseX, int mouseY);
            void SetStateMode(const Mode& newMode);
            void SetCharacterData(CharacterData* characterData);
            void SetPosition(const Position& pos, float x = 0, float y = 0);
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Character_h */
