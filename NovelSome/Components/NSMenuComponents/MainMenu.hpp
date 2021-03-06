//
//  MainMenu.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/02/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef MainMenu_hpp
#define MainMenu_hpp

#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

#include <locale>
#include <fstream>
#include <codecvt>
#include <memory>
#include <dirent.h>

#include <dirent.h>
#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/GUI/TextField.hpp>
#include <minEH/Engine/GUI/Button/RoundedRectangleButton.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>
#include <minEH/Engine/Client.hpp>
#include <minEH/Engine/Language.hpp>
#include <minEH/Components/Helpers/LanguageLibrary.hpp>

#include "NovelsLibrary.hpp"
#include "NovelMenu.hpp"
#include "../Interfaces/Popup.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::vector;

namespace ns
{
    namespace NSMenuComponents
    {
        struct MainMenu : Component
        {
            //NovelComponents::GUISystem guiSystem;
            
            bool active{ true }, ignoreNovelsEnding{ false };
            sf::Sprite background;
            bool spriteLoaded{ false };
            enum class Page { Main, Novels, Editor, Settings, Language, Account };
            Page page{ Page::Main };
            
            sf::Sprite blur; sf::Uint8 blurAlpha{ 0 };
            bool drawBlur{ false }, updateBlur{ false }; float elapsedBlur{ 0 };
            enum class Blur { appearing, disappearing, existing } blurState{ Blur::existing };
            
            bool doParallax{ gs::isParallaxEnabled };
            float parallaxPower{ gs::defaultParallaxBackground };
            
            sf::Text welcomeToNovelSome;
            bool fontLoaded{ false };
            GUI::TextButton novelsButton, editorButton, settingsButton, exitButton, accountButton, languageButton, backButton;
            
            long yyNovels{ 0 }, yyNovels_from{ 0 }, yyNovels_to{ 0 };
            bool isNovelSelected{ false };
            list<NovelInfo>::iterator novelSelected;
            unsigned long novelsLoaded{ 0 };
            GUI::RoundedRectangleButton novelButtons;
            sf::RectangleShape novelBackShape;
            GUI::TextButton novelStartButton, novelLoadButton;
            sf::Text novelText; float yyNovelText{ 0 };
            std::wstring novelTextDescription{ L"" }, novelRawDescription{ L"" }, novelAuthor{ L"" };
            bool novelShowBackground{ false }; sf::Sprite novelBackground; std::wstring novelBackTexture{ L"" };
            std::unique_ptr<char[]> fileInMemory;
            sf::Music novelMusic; float novelMusic_from{ 0 };
            std::wstring menuPath{ L"" }, scenario{ L"" };
            bool hasMenu{ false }; GUI::TextButton novelMenuButton;
#ifdef _WIN32
            int scrollSensitivity{ 28 };
#else
            int scrollSensitivity{ 14 };
#endif
            int scrollThershold{ 30 }, scrolldx{ 0 };
            sf::Vector2i scrollDot{ 0, 0 }, dot{ 0, 0 };
            
            sf::Text fieldTextUsername, fieldTextPassword;
            GUI::TextField usernameField, passwordField;
            GUI::TextButton loginButton, registerButton;
            
            GUI::RectangleButtons languageButtons;
            
            MainMenu();
            ~MainMenu();
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Draw(sf::RenderWindow* window) override;
            void ReceiveMessage(MessageHolder& message) override;
            void CalculateParallax(sf::Sprite& sprite, int dotX, int dotY);
            void CalculateScrollBounds();
            void SelectNovel(list<NovelInfo>::iterator& it, bool focus = true, bool force = false);
            void UnselectNovel();
            void ChangePageTo(const Page& to);
            void BlurAppear();
            void BlurDisappear();
        };
    }
}

#endif /* MainMenu_hpp */
