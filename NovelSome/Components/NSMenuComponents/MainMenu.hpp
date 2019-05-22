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

#include "../../Essentials/Base.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "NovelsLibrary.hpp"
#include "../NovelComponents/Novel.hpp"
#include "../NovelComponents/GUISystem.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::vector;

namespace ns
{
    namespace NSMenuComponents
    {
        struct MainMenu : public Component
        {
            NovelComponents::GUISystem guiSystem;
            
            bool active{ true };
            sf::Sprite background;
            bool spriteLoaded{ false };
            enum class Page { Main, Novels, Editor, Settings, Language };
            Page page{ Page::Main };
            
            bool doParallax{ gs::isParallaxEnabled };
            float parallaxPower{ gs::defaultParallaxBackground };
            
            sf::Text welcomeToNovelSome;
            bool fontLoaded{ false };
            GUI::TextButton novelsButton;
            GUI::TextButton editorButton;
            GUI::TextButton settingsButton;
            GUI::TextButton exitButton;
            GUI::TextButton accountButton;
            GUI::TextButton languageButton;
            GUI::TextButton backButton;
            GUI::TextButton tempButton;
            
            long yyNovels{ 0 }, yyNovels_from{ 0 }, yyNovels_to{ 0 };
            bool isNovelSelected{ false };
            list<NovelInfo>::iterator novelSelected;
            unsigned long novelsLoaded{ 0 };
            GUI::RectangleButtons novelButtons;
            sf::RectangleShape novelBackShape;
            GUI::TextButton novelStartButton;
            sf::Text novelText; float yyNovelText{ 0 };
            std::wstring novelTextDescription{ L"" }, novelRawDescription{ L"" }, novelAuthor{ L"" };
            bool novelShowBackground{ false }; sf::Sprite novelBackground; std::wstring novelBackTexture{ L"" };
            std::unique_ptr<char[]> fileInMemory;
            sf::Music novelMusic; float novelMusic_from{ 0 };
#ifdef _WIN32
            int scrollSensitivity{ 28 };
#else
            int scrollSensitivity{ 14 };
#endif
            int scrollThershold{ 30 }, scrolldx{ 0 };
            sf::Vector2i scrollDot{ 0, 0 }, dot{ 0, 0 };
            
            MainMenu();
            ~MainMenu();
            void Init() override;
            void PollEvent(sf::Event& event) override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Draw(sf::RenderWindow* window) override;
            void ReceiveMessage(MessageHolder& message) override;
            void CalculateParallax(sf::Sprite& sprite, int dotX, int dotY);
            void CalculateScrollBounds();
            void SelectNovel(list<NovelInfo>::iterator& it, bool focus = true, bool force = false);
            void UnselectNovel();
            void ChangePageTo(const Page& to);
        };
    }
}

#endif /* MainMenu_hpp */
