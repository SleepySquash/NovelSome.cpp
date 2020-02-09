//
//  Background.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Background_hpp
#define Background_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors/Image.hpp"
#include "../../Engine/GUI/Button/SomeButtons.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "Abstract/Skin.hpp"
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
        struct Background : NovelObject, Savable
        {
            sf::Sprite sprite;
            std::wstring imagePath{ L"" }, imageName{ L"" }, folderPath{ L"" };
            
            bool visible{ true }, loaded{ false };
            sf::Uint8 alpha{ 0 }, maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };
            
            float scaleX{ 1.f }, scaleY{ 1.f };
            float defaultPositionX{ 0.f }, defaultPositionY{ 0.f };
            enum fitModeEnum { noFit, defaultFit, fillCentre, stretch };
            fitModeEnum fitMode { fillCentre };
            
            Mode mode{ Mode::Appear }, switchTo{ Mode::Exist };
            MessageBack messageBack{ MessageBack::AtAppearance };
            
            Background* hideAfter{ nullptr };
            
            bool doParallax{ gs::isParallaxEnabled };
            float parallaxPower { gs::defaultParallaxBackground };
            
            Background(const std::wstring& folderPath = L"");
            void LoadImage(const std::wstring&);
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void ReceiveMessage(MessageHolder& message) override;
            void CalculateScale(unsigned int width, unsigned int height);
            void CalculateParallax(int mouseX, int mouseY);
            void SetStateMode(const Mode& newMode);
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Backgroud_h */
