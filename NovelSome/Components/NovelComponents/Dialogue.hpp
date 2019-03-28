//
//  Dialogue.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Dialogue_hpp
#define Dialogue_hpp

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
#include "../../Engine/Collectors/Font.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "Skin.hpp"
#include "CharacterLibrary.hpp"
#include "GUISystem.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct Dialogue : NovelObject
        {
            sf::Text text;
            std::wstring textString{ L"" };
            std::wstring printingString{ L"" }, currentString{ L"" };
            
            sf::Text charText;
            std::wstring charString{ L"" };
            
            GUISystem* guiSystem{ nullptr };
            CharacterData* character{ nullptr };
            
            bool visible{ true };
            bool drawCharacterName{ false }, fontLoaded{ false };
            sf::Int8 alpha{ 0 }; int maxAlpha{ 255 };
            float currentTime{ 0.f }, waitingTime{ 2.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            
            enum modeEnum {appearing, waiting, waitingForTime, waitingForInput, waitingForChoose, disappearing, deprecated};
            modeEnum mode{ appearing };
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            modeEnum afterAppearSwitchTo{ waitingForInput };
            
            unsigned int textAppearPos{ 0 }, textAppearMax{ 0 }, textAppearI{ 0 };
            float characterInSecond{ 0.04f }, elapsedCharacterSum{ 0 };
            enum class textAppearModeEnum {fading, printing};
            textAppearModeEnum textAppearMode{ textAppearModeEnum::printing };
            
            sf::IntRect workingArea;
            bool forcePressInsideDialogue{ true };
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 30 };
            float outlineThickness{ 0.f };
            
            wchar_t leftSpeechAddition{ 0 }, rightSpeechAddition{ 0 };
            int afterRedLineShift{ 0 };
            
            Dialogue(GUISystem* guiSystem = nullptr);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetCharacter(CharacterData* character);
            void SetCharacterName(const sf::String& characterName);
            void SetDialogue(const sf::String& dialogue);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        
        
        struct Choose : NovelObject
        {
            sf::Text text;
            sf::String textString{ "" };
            
            GUISystem* guiSystem{ nullptr };
            Skins::Dialogue* skin{ nullptr };
            list<Choose*>::iterator groupPointer;
            
            GUI::TextButton button;
            int startingYY{ 0 };
            
            vector<std::wstring> actions, choices;
            vector<int> choiceStart;
            
            bool fontLoaded{ false };
            sf::Int8 alpha{ 0 }; int maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            
            bool visible{ true };
            
            enum modeEnum {appearing, waitingForInput, disappearing, deprecated};
            modeEnum mode{ appearing };
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 42 };
            
            Choose(GUISystem* guiSystem = nullptr);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetStateMode(modeEnum newMode);
            void AddChoice(const std::wstring& line);
            void AddAction(const std::wstring& line);
            void InitChoose();
        };
    }
}

#endif /* Dialogue_h */
