//
//  MainMenu.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/02/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "MainMenu.hpp"

namespace ns
{
    namespace NSMenuComponents
    {
        MainMenu::MainMenu()
        {
            sf::Texture* texture = ic::LoadTexture(L"Data/Images/background.jpg");
            if ((spriteLoaded = (texture != nullptr)))
            {
                background.setTexture(*texture);
                background.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
            }
        }
        MainMenu::~MainMenu() { ic::DeleteImage(L"Data/Images/background.jpg"); }
        void MainMenu::Init()
        {
            novelSelected = Novels::info.end();
            if ((fontLoaded = (fc::GetFont(L"Pacifica.ttf") != nullptr))) welcomeToNovelSome.setFont(*fc::GetFont(L"Pacifica.ttf"));
            welcomeToNovelSome.setFillColor(sf::Color::White);
            welcomeToNovelSome.setOutlineColor(sf::Color::Black);
            welcomeToNovelSome.setString("Welcome to the NovelSome");
            
            novelsButton.setString(L"Novel selection >");
            novelsButton.setFont(L"Pacifica.ttf");
            novelsButton.setCharacterSize(67);
            
            editorButton.setString(L"Editor >");
            editorButton.setFont(L"Pacifica.ttf");
            editorButton.setCharacterSize(67);
            
            settingsButton.setString(L"Settings >");
            settingsButton.setFont(L"Pacifica.ttf");
            settingsButton.setCharacterSize(67);
            
#if !defined(SFML_SYSTEM_IOS) && !defined(SFML_SYSTEM_ANDROID)
            exitButton.setString(L"Exit");
            exitButton.setFont(L"Pacifica.ttf");
            exitButton.setCharacterSize(67);
#endif
            
            accountButton.setString(L"You are SleepySquash >");
            accountButton.setFont(L"Pacifica.ttf");
            accountButton.setCharacterSize(52);
            
            novelsButton.valign = editorButton.valign = settingsButton.valign = exitButton.valign = Valign::Top;
            novelsButton.characterScale = editorButton.characterScale = settingsButton.characterScale = exitButton.characterScale = accountButton.characterScale = backButton.characterScale = true;
            novelsButton.thickness = editorButton.thickness = settingsButton.thickness = exitButton.thickness = accountButton.thickness = backButton.thickness = 2;
            
            backButton.setString(L"< BACK");
            backButton.setFont(L"Pacifica.ttf");
            backButton.setCharacterSize(81);
            backButton.halign = Halign::Left;
            backButton.valign = Valign::Bottom;
            
            novelButtons.setFont(L"Pacifica.ttf");
            novelButtons.setCharacterSize(52);
            novelButtons.thickness = 2.f;
            
            novelBackShape.setFillColor(sf::Color(0,0,0, 170));
            novelBackShape.setOutlineColor(sf::Color(255,255,255,190));
            novelStartButton.setString(L"Read");
            novelStartButton.setFont(L"Pacifica.ttf");
            novelStartButton.setCharacterSize(72);
            novelStartButton.thickness = 2;
            novelStartButton.valign = Valign::Bottom;
            
            if ((fontLoaded = (fc::GetFont(L"Pacifica.ttf") != nullptr))) novelText.setFont(*fc::GetFont(L"Pacifica.ttf"));
            novelText.setFillColor(sf::Color::White);
            novelText.setOutlineColor(sf::Color::Black);
        }
        void MainMenu::PollEvent(sf::Event& event)
        {
            if (!active) return;
            
            float yy;
            switch (page)
            {
                case Page::Main:
                    if (novelsButton.PollEvent(event))
                    {
                        page = Page::Novels;
                        if (!novelsLoaded) novelsLoaded = Novels::LoadNovels();
                    }
                    else if (editorButton.PollEvent(event)) page = Page::Editor;
                    else if (settingsButton.PollEvent(event)) page = Page::Settings;
#if !defined(SFML_SYSTEM_IOS) && !defined(SFML_SYSTEM_ANDROID)
                    else if (exitButton.PollEvent(event)) exit(0);
#endif
                    else if (accountButton.PollEvent(event)) { /* ... */ }
                    break;
                case Page::Novels:
                    if (backButton.PollEvent(event)) {
                        if (isNovelSelected && verticalOrientation)
                        {
                            if (novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
                            isNovelSelected = false;
                        } else page = Page::Main; }
                    else
                    {
                        bool unselectNovelSelected{ true };
                        if (event.type != sf::Event::MouseButtonReleased && event.type != sf::Event::TouchEnded)
                            unselectNovelSelected = false;
                        
                        if (!verticalOrientation || !isNovelSelected)
                        {
                            yy = gs::height/3; long i = 0;
                            for (auto it = Novels::info.begin(); it != Novels::info.end() && active; ++it)
                            {
                                novelButtons.index = i;
                                
                                novelButtons.setString((*it).name);
                                novelButtons.setPosition(novelButtons.shape.getPosition().x, yy);
                                if (novelButtons.PollEvent(event))
                                {
                                    unselectNovelSelected = false;
                                    if (!isNovelSelected || novelSelected != it)
                                    {
                                        if (novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
                                        isNovelSelected = true; novelSelected = it;
                                        novelRawDescription = L""; novelAuthor = L"";
                                        std::wstring back = L"";
                                        std::wifstream wif; bool done{ false };
        #ifdef _WIN32
                                        wif.open((*it).path);
        #else
                                        wif.open(base::utf8((*novelSelected).path));
        #endif
                                        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                                        
                                        int infoRead{ 0 };
                                        if (wif.is_open())
                                        {
                                            std::wstring line;
                                            nss::CommandSettings command;
                                            while (!wif.eof() && !done)
                                            {
                                                std::getline(wif, line);
                                                command.Command(line);
                                                
                                                if (nss::Command(command, L"background ")) {
                                                    back = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                                                else if (nss::Command(command, L"description ")) {
                                                    novelRawDescription = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                                                else if (nss::Command(command, L"author ")) {
                                                    novelAuthor = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                                                
                                                if (infoRead >= 3) done = true;
                                            }
                                            
                                            wif.close();
                                        }
                                        if (back != L"" && back.length())
                                        {
                                            std::wstring path = base::GetFolderPath((*it).path) + back;
                                            sf::Texture* texture = ic::LoadTexture(path, 1);
                                            if (texture)
                                            {
                                                novelBackTexture = path;
                                                novelBackground.setTexture(*texture, true);
                                                novelBackground.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
                                                float factorX = (float)gs::width / texture->getSize().x;
                                                float factorY = (float)gs::height / texture->getSize().y;
                                                float scaleFactor = factorX > factorY ? factorX : factorY;
                                                novelBackground.setScale(scaleFactor, scaleFactor);
                                                novelBackground.setPosition(gs::width/2, gs::height/2);
                                                novelShowBackground = true;
                                            }
                                        }
                                        if (novelRawDescription == L"") { novelRawDescription = novelTextDescription = L"Описание: ..."; }
                                        else
                                        {
                                            novelRawDescription = L"Описание: " + novelRawDescription;
                                            novelText.setCharacterSize(35 * gs::scScale);
                                            novelText.setPosition(novelBackShape.getPosition().x + 5*gs::scalex, 0);
                                            novelTextDescription = nss::GetStringWithLineBreaks(novelText, novelRawDescription, novelBackShape.getSize().x - 10*gs::scalex, 10*gs::scale);
                                        }
                                        if (novelAuthor == L"") novelAuthor = L"???";
                                    }
                                }
                                yy += novelButtons.shape.getGlobalBounds().height + 10*gs::scaley; ++i;
                            }
                            novelButtons.eventPolled(event);
                        }
                        
                        if (isNovelSelected && novelStartButton.PollEvent(event))
                        {
                            unselectNovelSelected = false;
                            std::wstring scenario = L"";
                            std::wifstream wif; bool done{ false };
#ifdef _WIN32
                            wif.open((*it).path);
#else
                            wif.open(base::utf8((*novelSelected).path));
#endif
                            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                            
                            if (wif.is_open())
                            {
                                std::wstring line;
                                nss::CommandSettings command;
                                while (!wif.eof() && !done)
                                {
                                    std::getline(wif, line);
                                    command.Command(line);
                                    
                                    if (nss::Command(command, L"scenario ")) { scenario = nss::ParseAsMaybeQuoteString(command); done = true; }
                                }
                                
                                wif.close();
                            }
                            
                            active = !(scenario != L"");
                            if (!active) entity->AddComponent<NovelComponents::Novel>(base::GetFolderPath((*novelSelected).path) + scenario);
                            else cout << "MainMenu :: PollEvent :: Novel start failed with no scenario found." << endl;
                        }
                        
                        if (unselectNovelSelected)
                        {
                            sf::Vector2i dot; if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y }; else dot = { event.touch.x, event.touch.y };
                            unselectNovelSelected = !novelBackShape.getGlobalBounds().contains(dot.x, dot.y);
                            if (unselectNovelSelected)
                            {
                                if (isNovelSelected && novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
                                isNovelSelected = false;
                            }
                        }
                    }
                    break;
                case Page::Editor: if (backButton.PollEvent(event)) page = Page::Main; break;
                case Page::Settings: if (backButton.PollEvent(event)) page = Page::Main; break;
                default: break;
            }
        }
        void MainMenu::Resize(unsigned int width, unsigned int height)
        {
            if (!active) return;
            if (spriteLoaded)
            {
                float factorX = (float)width / background.getTexture()->getSize().x;
                float factorY = (float)height / background.getTexture()->getSize().y;
                float scaleFactor = factorX > factorY ? factorX : factorY;
                background.setScale(scaleFactor, scaleFactor);
                background.setPosition(width/2, height/2);
            }
            
            welcomeToNovelSome.setCharacterSize(108 * gs::scScale);
            welcomeToNovelSome.setOutlineThickness(2 * gs::scScale);
            welcomeToNovelSome.setString(L"Welcome to the NovelSome");
            welcomeToNovelSome.setScale(1, 1);
            welcomeToNovelSome.setOrigin(welcomeToNovelSome.getGlobalBounds().width/2, 0);
            if (gs::width < welcomeToNovelSome.getGlobalBounds().width)
            {
                welcomeToNovelSome.setString(L"    Welcome to\nthe NovelSome");
                welcomeToNovelSome.setOrigin(welcomeToNovelSome.getGlobalBounds().width/2, 0);
                if (gs::width < welcomeToNovelSome.getGlobalBounds().width)
                    welcomeToNovelSome.setScale((float)gs::width/welcomeToNovelSome.getGlobalBounds().width, 1);
            }
            
            novelsButton.Resize(width, height);
            editorButton.Resize(width, height);
            settingsButton.Resize(width, height);
            exitButton.Resize(width, height);
            
            accountButton.Resize(width, height);
            accountButton.setPosition(gs::width/2, 30*gs::scale);
            
            backButton.Resize(width, height);
            backButton.setPosition(40*gs::scalex, gs::height - 50*gs::scaley);
            
            float yy = gs::height/2.4 - welcomeToNovelSome.getGlobalBounds().height/2;
            welcomeToNovelSome.setPosition(gs::width/2, yy); yy += welcomeToNovelSome.getGlobalBounds().height + 25*gs::scale;
            novelsButton.setPosition(gs::width/2, yy); yy += novelsButton.text.getGlobalBounds().height + 6*gs::scale;
            editorButton.setPosition(gs::width/2, yy); yy += editorButton.text.getGlobalBounds().height + 6*gs::scale;
            settingsButton.setPosition(gs::width/2, yy); yy += settingsButton.text.getGlobalBounds().height + 6*gs::scale;
#if !defined(SFML_SYSTEM_IOS) && !defined(SFML_SYSTEM_ANDROID)
            exitButton.setPosition(gs::width/2, yy); yy += exitButton.text.getGlobalBounds().height + 6*gs::scale;
#endif
            
            novelBackShape.setOutlineThickness(2.f * gs::scale);
            verticalOrientation = ((float)width/height < 1);
            if (verticalOrientation)
            {
                novelBackShape.setSize({gs::width * 4.4f/5, gs::height * 4.f/5});
                novelBackShape.setPosition(0.6*gs::width/10, gs::height/15);
                novelButtons.setSize({gs::width * 4.f/5, 120*gs::scScale});
                novelButtons.setPosition(gs::width/10, 0);
            } else {
                novelBackShape.setSize({(1280 - 630 - 30)*gs::scalex, gs::height * 4.f/5});
                novelBackShape.setPosition(15*gs::scalex, gs::height/15);
                novelButtons.setSize({630*gs::scalex, 120*gs::scScale});
                novelButtons.setPosition(gs::width - novelButtons.shape.getSize().x, 0);
            }
            novelButtons.Resize(width, height);
            novelStartButton.Resize(width, height);
            novelStartButton.setPosition(novelBackShape.getPosition().x + novelBackShape.getSize().x/2, novelBackShape.getPosition().y + novelBackShape.getSize().y - 20*gs::scaley);
            novelText.setOutlineThickness(2 * gs::scale);
            yyNovelText = novelBackShape.getPosition().y;
            
            if (novelShowBackground)
            {
                float factorX = (float)width / novelBackground.getTexture()->getSize().x;
                float factorY = (float)height / novelBackground.getTexture()->getSize().y;
                float scaleFactor = factorX > factorY ? factorX : factorY;
                novelBackground.setScale(scaleFactor, scaleFactor);
                novelBackground.setPosition(width/2, height/2);
            }
            if (isNovelSelected)
            {
                novelText.setCharacterSize(35 * gs::scScale);
                novelText.setPosition(novelBackShape.getPosition().x + 5*gs::scalex, 0);
                novelTextDescription = nss::GetStringWithLineBreaks(novelText, novelRawDescription, novelBackShape.getSize().x - 10*gs::scalex, 10*gs::scale);
            }
        }
        void MainMenu::Draw(sf::RenderWindow* window)
        {
            if (!active) return;
            if (novelShowBackground && page == Page::Novels) window->draw(novelBackground);
            else window->draw(background);
            
            float yy; int i = 0; bool sizeChanged{ false };
            switch (page)
            {
                case Page::Main:
                    window->draw(welcomeToNovelSome);
                    novelsButton.Draw(window);
                    editorButton.Draw(window);
                    settingsButton.Draw(window);
#if !defined(SFML_SYSTEM_IOS) && !defined(SFML_SYSTEM_ANDROID)
                    exitButton.Draw(window);
#endif
                    accountButton.Draw(window);
                    break;
                case Page::Novels:
                    if (isNovelSelected)
                    {
                        window->draw(novelBackShape);
                        novelStartButton.Draw(window);
                        
                        yy = yyNovelText;
                        novelText.setCharacterSize(70 * gs::scScale);
                        novelText.setString(novelSelected->name);
                        if ((sizeChanged = (novelText.getGlobalBounds().width > novelBackShape.getSize().x)))
                            novelText.setScale(novelBackShape.getSize().x/novelText.getGlobalBounds().width, novelBackShape.getSize().x/novelText.getGlobalBounds().width);
                        novelText.setPosition(novelBackShape.getPosition().x + novelBackShape.getSize().x/2 - novelText.getGlobalBounds().width/2, yy);
                        window->draw(novelText); yy += novelText.getGlobalBounds().height + 20*gs::scaley;
                        if (sizeChanged) novelText.setScale(1, 1);
                        
                        novelText.setCharacterSize(35 * gs::scScale);
                        novelText.setString(L"by " + novelAuthor);
                        novelText.setPosition(novelBackShape.getPosition().x + novelBackShape.getSize().x/2 - novelText.getGlobalBounds().width/2, yy);
                        window->draw(novelText); yy += novelText.getGlobalBounds().height + 15*gs::scaley;
                        
                        novelText.setString(novelTextDescription);
                        novelText.setPosition(novelBackShape.getPosition().x + 5*gs::scalex, yy);
                        window->draw(novelText); yy += novelText.getGlobalBounds().height + 15*gs::scaley;
                    }
                    if (!isNovelSelected || !verticalOrientation)
                    {
                        yy = gs::height/3;
                        for (auto it = Novels::info.begin(); it != Novels::info.end(); ++it)
                        {
                            novelButtons.index = i;
                            novelButtons.setString((*it).name);
                            novelButtons.setPosition(novelButtons.shape.getPosition().x, yy);
                            novelButtons.Draw(window);
                            yy += novelButtons.shape.getGlobalBounds().height + 10*gs::scaley; ++i;
                        }
                    }
                    backButton.Draw(window);
                    break;
                case Page::Editor: backButton.Draw(window); break;
                case Page::Settings: backButton.Draw(window); break;
                default: break;
            }
        }
        void MainMenu::RecieveMessage(MessageHolder& message)
        {
            if (message.info == "NovelComponents :: Novel :: Returning to the menu")
            {
                active = true;
                gs::requestWindowRefresh = true;
                Resize(gs::width, gs::height);
                message = MessageHolder();
            }
        }
    }
}
