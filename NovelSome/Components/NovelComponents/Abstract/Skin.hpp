//
//  Skin.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Skin_hpp
#define Skin_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>
#include <minEH/Engine/Language.hpp>

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct GUIObject;
        namespace Skins
        {
            struct Dialogue
            {
                GUIObject* dialogueRectH{ nullptr }, *textConstrainsH{ nullptr };
                GUIObject* nameRectH{ nullptr }, *nameConstrainsH{ nullptr };
                GUIObject* dialogueRectV{ nullptr }, *textConstrainsV{ nullptr };
                GUIObject* nameRectV{ nullptr }, *nameConstrainsV{ nullptr };
                
                unsigned int characterSize{ 30 };
                int maxAlpha = 255;
                bool forcePressInsideDialogue{ true };
                float characterInSecond{ 0.04f };
                unsigned int textAppearMode{ 1 };
                std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
                
                wchar_t leftSpeechAddition{ 0 }, rightSpeechAddition{ 0 };
                int afterRedLineShift{ 0 };
                
                sf::Color fillColor{ sf::Color::White }, outlineColor{ sf::Color::Black };
                sf::Color nameFillColor{ sf::Color::White }, nameOutlineColor{ sf::Color::Black };
                float outlineThickness{ 0.f }, nameThickness{ 0.f };
                
                float appearTime{ 0.6f }, disappearTime{ 0.6f };
            };
            struct Choose
            {
                GUIObject* chooseRect{ nullptr }, *chooseConstrains{ nullptr };
                
                unsigned int characterSize{ 36 };
                int maxAlpha = 255;
                std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
                float appearTime{ 0.6f }, disappearTime{ 0.6f };
            };
            struct Background
            {
                int maxAlpha = 255;
                float appearTime{ 0.6f }, disappearTime{ 0.6f };
                bool doParallax{ gs::isParallaxEnabled };
                float parallaxPower{ gs::defaultParallaxBackground };
            };
            struct Character
            {
                int maxAlpha = 255;
                float appearTime{ 0.6f }, disappearTime{ 0.6f };
                bool doParallax{ gs::isParallaxEnabled };
                float parallaxClose{ gs::defaultParallaxClose }, parallaxNormal{ gs::defaultParallaxNormal }, parallaxFar{ gs::defaultParallaxFar };
            };
            struct Music { float maxVolume{ 100 }, appearTime{ 1.f }, disappearTime{ 1.f }; };
            struct Ambient { float maxVolume{ 100 }, appearTime{ 1.f }, disappearTime{ 1.f }; };
            struct Sound { float maxVolume{ 100 }, appearTime{ 1.f }, disappearTime{ 1.f }; };
        }
        struct Skin
        {
            static Skin* self;
            
            //std::wstring defaultFontName{ L"NotoSansCJK-Regular.ttc" };
            std::wstring defaultFontName{ L"Arial.ttf" }, folderScope{ L"" };
            Skins::Dialogue dialogue;
            Skins::Choose choose;
            Skins::Background background;
            Skins::Character character;
            Skins::Music music;
            Skins::Ambient ambient;
            Skins::Sound sound;
            
            bool changedDialogue{ false }, changedChoose{ false }, changedBackground{ false }, changedCharacter{ false }, changedMusic{ false }, changedAmbient{ false }, changedSound{ false };
            std::wstring defaultDialogue, defaultChoose, defaultBackground, defaultCharacter, defaultMusic, defaultAmbient, defaultSound;
            Skin();
            void RestoreToDefaults(const std::wstring& scopeName = L"");
            void LoadFromFile(const std::wstring& fileName, const std::wstring& scopeName = L"");
        };
    }
}

#endif /* Skin_hpp */
