//
//  SettingsUI.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 30/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef SettingsUI_hpp
#define SettingsUI_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/ResourcePath.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors/Font.hpp"
#include "../../Engine/Collectors/Image.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/Language.hpp"

using std::cin;
using std::cout;
using std::endl;
using ns::base::utf8;
using ns::base::utf16;

namespace ns
{
    struct SettingsUI : Component
    {
        bool active{ false };
        
        sf::RectangleShape blackScreenShape;
        enum class Page{ customization, graphics, sound, network }; Page page{ Page::customization };
        GUI::TextButton customizationButton, graphicsButton, soundButton, networkButton;
        GUI::TextButton backButton, tempButton;
        
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
    };
}

#endif /* SettingsUI_hpp */
