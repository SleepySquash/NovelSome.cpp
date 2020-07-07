//
//  Choice.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 13.02.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef Choice_hpp
#define Choice_hpp

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
#include <minEH/Engine/Collectors/Font.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "Abstract/Skin.hpp"
#include "Abstract/CharacterLibrary.hpp"
#include "GUISystem.hpp"
#include "Abstract/Savable.hpp"
#include "Abstract/Modes.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct Choose : NovelObject, Savable
        {
            sf::Text text;
            
            GUISystem* guiSystem{ nullptr };
            Skins::Dialogue* skin{ nullptr };
            list<Choose*>::iterator groupPointer;
            
            GUI::TextButton button; int startingYY{ 0 };
            
            vector<std::wstring> actions, choices;
            vector<int> choiceStart;
            
            bool fontLoaded{ false };
            sf::Uint8 alpha{ 0 }; int maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            bool visible{ true };
            
            Mode mode{ Mode::Appear };
            MessageBack messageBack{ MessageBack::AtDisappearance };
            
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 42 };
            
            Choose(GUISystem* guiSystem = nullptr);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void SetStateMode(const Mode& newMode);
            void AddChoice(const std::wstring& line);
            void AddAction(const std::wstring& line);
            void InitChoose();
            void ReceiveMessage(MessageHolder &message) override;
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Choice_hpp */
