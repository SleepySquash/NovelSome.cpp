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

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors/Image.hpp"
#include "../../Engine/GUI/Button/SomeButtons.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "Abstract/CharacterLibrary.hpp"
#include "Abstract/Skin.hpp"
#include "Abstract/Savable.hpp"

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
            std::wstring imagePath{ L"" };
            
            CharacterData* characterData{ nullptr };
            std::wstring state{ L"" }, folderPath{ L"" };
            
            bool spriteLoaded{ false };
            sf::Uint8 alpha{ 0 }; int maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            
            float defaultPositionX{ 0.f }, defaultPositionY{ 0.f };
            
            enum modeEnum {appearing, existing, disappearing, deprecated};
            modeEnum mode{ appearing };
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atAppearance };
            modeEnum afterAppearSwitchTo{ existing };
            
            float scaleX{ 1.f }, scaleY{ 1.f }, scaleRawX{ 1.f }, scaleRawY{ 1.f };
            
            float customX{ 0 }, customY{ 0 };
            enum positionEnum { custom, left, cleft, center, cright, right };
            positionEnum position { center };
            
            bool doParallax{ gs::isParallaxEnabled };
            float parallaxPower { gs::defaultParallaxNormal };
            bool visible{ true };
            
            Character();
            void LoadState(const std::wstring& stateName);
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void ReceiveMessage(MessageHolder &message) override;
            void CalculateParallax(int mouseX, int mouseY);
            void SetStateMode(modeEnum newMode);
            void SetCharacterData(CharacterData* characterData);
            void SetPosition(positionEnum pos, float x, float y);
            
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Character_h */
