//
//  SaveLoadUI.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 02/06/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef SaveLoadUI_hpp
#define SaveLoadUI_hpp

#include <iostream>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/ResourcePath.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors/Font.hpp>
#include <minEH/Engine/Collectors/Image.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/NovelSystem.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/Language.hpp>

#include "../NovelComponents/Abstract/SavingMechanism.hpp"
#include "../NSMenuComponents/NovelsLibrary.hpp"

using std::cin;
using std::cout;
using std::endl;
using ns::base::utf8;
using ns::base::utf16;

namespace ns
{
    struct SaveLoadUI : Component
    {
        bool active{ false }, loading{ false }, available{ false };
        NovelInfo* novel{ nullptr };
        NovelSystem* system{ nullptr };
        std::wstring absolutePath{ L"" };
        
        sf::RectangleShape blackScreenShape;
        sf::Text captionText;
        GUI::TextButton backButton, saveloadButton;
        
        std::vector<std::wstring> saves;
        sf::RectangleShape slotShape;
        float slotXX, slotYY;
        
        enum modeEnum { appearing, existing, disappearing };
        modeEnum mode{ appearing }; sf::Uint8 alpha{ 0 };
        float currentTime{ 0.f }, appearTime{ 0.2f }, disappearTime{ 0.1f };
        
        void Init() override;
        void Destroy() override;
        void Update(const sf::Time& elapsedTime) override;
        void CleanUp();
        void PollEvent(sf::Event& event) override;
        void Resize(const unsigned int& width, const unsigned int& height) override;
        void Draw(sf::RenderWindow* window) override;
        void ReceiveMessage(MessageHolder& message) override;
        void Switch(const bool& on);
        void UpdateAlpha();
        unsigned long LoadSlots();
        void ClearSlots();
    };
}

#endif /* SaveLoadUI_hpp */
