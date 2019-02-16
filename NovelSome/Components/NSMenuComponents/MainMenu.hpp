//
//  MainMenu.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/02/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
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
#include <dirent.h>

#include <dirent.h>
#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/StaticMethods.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "../NovelComponents/Novel.hpp"
#include "NovelsLibrary.hpp"

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
            bool active{ true };
            sf::Sprite background;
            bool spriteLoaded{ false };
            enum class Page { Main, Novels, Editor, Settings };
            Page page{ Page::Main };
            
            sf::Text welcomeToNovelSome;
            bool fontLoaded{ false };
            GUI::TextButton novelsButton;
            GUI::TextButton editorButton;
            GUI::TextButton settingsButton;
            GUI::TextButton exitButton;
            GUI::TextButton accountButton;
            GUI::TextButton backButton;
            
            bool verticalOrientation{ false };
            bool isNovelSelected{ false };
            list<NovelInfo>::iterator novelSelected;
            unsigned long novelsLoaded{ 0 };
            GUI::RectangleButtons novelButtons;
            sf::RectangleShape novelBackShape;
            GUI::TextButton novelStartButton;
            sf::Text novelText; float yyNovelText{ 0 };
            std::wstring novelTextDescription{ L"" }, novelRawDescription{ L"" }, novelAuthor{ L"" };
            bool novelShowBackground{ false }; sf::Sprite novelBackground; std::wstring novelBackTexture{ L"" };
            
            MainMenu();
            ~MainMenu();
            void Init() override;
            void PollEvent(sf::Event& event) override;
            void Resize(unsigned int width, unsigned int height) override;
            void Draw(sf::RenderWindow* window) override;
            void RecieveMessage(MessageHolder& message) override;
        };
    }
}

#endif /* MainMenu_hpp */