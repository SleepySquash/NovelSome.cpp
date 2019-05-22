//
//  MainMenu.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/02/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "MainMenu.hpp"

namespace ns
{
    namespace NSMenuComponents
    {
        MainMenu::MainMenu() { }
        MainMenu::~MainMenu() { ic::DeleteImage(L"Data/Images/background.jpg"); }
        void MainMenu::Init()
        {
            sf::Texture* texture = ic::RequestHigherTexture(L"Data/Images/background.jpg", entity);//ic::LoadTexture(L"Data/Images/background.jpg");
            if ((spriteLoaded = texture))
            {
                background.setTexture(*texture);
                background.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
            }
            
            
            tempButton.setString(L"Привет! Я кнопка справа.");
            tempButton.setCharacterSize(60); tempButton.thickness = 2;
            tempButton.setFont(L"Pacifica.ttf"); tempButton.halign = Halign::Right;
            
            novelSelected = Novels::info.end();
            if ((fontLoaded = (fc::GetFont(L"Pacifica.ttf")))) welcomeToNovelSome.setFont(*fc::GetFont(L"Pacifica.ttf"));
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
            languageButton.setString(L"Language");
            languageButton.setFont(L"Pacifica.ttf");
            languageButton.setCharacterSize(57);
            languageButton.valign = Valign::Bottom;
            languageButton.halign = Halign::Left;
            
            novelsButton.valign = editorButton.valign = settingsButton.valign = exitButton.valign = Valign::Top;
            novelsButton.characterScale = editorButton.characterScale = settingsButton.characterScale = exitButton.characterScale = accountButton.characterScale = backButton.characterScale = languageButton.characterScale = true;
            novelsButton.thickness = editorButton.thickness = settingsButton.thickness = exitButton.thickness = accountButton.thickness = languageButton.thickness = backButton.thickness = 2;
            
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
            
            //guiSystem.LoadFromFile(L"Novels/Bundle/skin.nskin", L"test2");
            //guiSystem.SetAlpha(255);
        }
        void MainMenu::ChangePageTo(const Page& to)
        {
            /*switch (to)
            {
                    case
            }*/
        }
        void MainMenu::PollEvent(sf::Event& event)
        {
            if (!active) return;
            
            if (doParallax && event.type == sf::Event::MouseMoved)
            {
                if (novelShowBackground && page == Page::Novels) CalculateParallax(novelBackground, event.mouseMove.x, event.mouseMove.y);
                else CalculateParallax(background, event.mouseMove.x, event.mouseMove.y);
            }
            
            float yy;
            switch (page)
            {
                case Page::Main:
                    if (novelsButton.PollEvent(event))
                    {
                        if (doParallax && novelShowBackground) CalculateParallax(novelBackground, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                        page = Page::Novels; if (novelMusic.getStatus() == sf::Music::Status::Paused) novelMusic.play();
                        if (!novelsLoaded) {
                            novelsLoaded = Novels::LoadNovels(); CalculateScrollBounds();
                            if (yyNovels_to == yyNovels_from) yyNovels = yyNovels_to;
                            else
                            {
                                int yyHeight = (novelButtons.shape.getLocalBounds().height/gs::scaley + 10);
                                float yyOnScreen = (float)gs::relativeHeight/yyHeight;
                                yyNovels = yyNovels_to - yyOnScreen/3 * yyHeight;
                            }
                        }
                    }
                    else if (editorButton.PollEvent(event)) page = Page::Editor;
                    else if (settingsButton.PollEvent(event)) page = Page::Settings;
#if !defined(SFML_SYSTEM_IOS) && !defined(SFML_SYSTEM_ANDROID)
                    else if (exitButton.PollEvent(event)) exit(0);
#endif
                    else if (accountButton.PollEvent(event)) { /* ... */ }
                    else if (languageButton.PollEvent(event)) page = Page::Language;
                    break;
                case Page::Novels:
                    if (backButton.PollEvent(event))
                    {
                        novelButtons.anyButtonPressed = false;
                        if (isNovelSelected && gs::verticalOrientation) UnselectNovel();
                        else { page = Page::Main; novelMusic.pause(); if (doParallax) CalculateParallax(background, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y); }
                    }
                    else
                    {
                        if (event.type == sf::Event::MouseWheelScrolled)
                        {
                            if (event.mouseWheelScroll.delta != 0 && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                            {
                                yyNovels += scrollSensitivity * event.mouseWheelScroll.delta;
                                if (yyNovels < yyNovels_from) yyNovels = yyNovels_from;
                                if (yyNovels > yyNovels_to) yyNovels = yyNovels_to;
                            }
                        }
                        else if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right))
                        {
                            if (isNovelSelected && gs::verticalOrientation) UnselectNovel();
                            else { page = Page::Main; novelMusic.pause(); if (doParallax) CalculateParallax(background, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y); }
                        }
                        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Down && isNovelSelected) {
                            ++novelSelected; if (novelSelected == Novels::info.end()) novelSelected = Novels::info.begin();
                            SelectNovel(novelSelected, true, true); }
                        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Up && isNovelSelected) {
                            if (novelSelected == Novels::info.begin()) novelSelected = Novels::info.end(); --novelSelected;
                            SelectNovel(novelSelected, true, true); }
                        else
                        {
                            if ((event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) || event.type == sf::Event::TouchMoved)
                            {
                                if (event.type == sf::Event::MouseMoved) dot = { event.mouseMove.x, event.mouseMove.y }; else dot = { event.touch.x, event.touch.y };
                                yyNovels += (dot.y - gs::lastMousePos.second)/gs::scaley;
                                if (yyNovels < yyNovels_from) yyNovels = yyNovels_from;
                                if (yyNovels > yyNovels_to) yyNovels = yyNovels_to;
                                scrollThershold += abs(dot.y - gs::lastMousePos.second)/gs::scaley;
                            }
                            
                            bool unselectNovelSelected{ false };
                            if (((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) || event.type == sf::Event::TouchEnded) && scrollThershold < 4) unselectNovelSelected = true;
                            if (scrollThershold > 4) novelButtons.anyButtonPressed = false;
                            if ((!gs::verticalOrientation || !isNovelSelected) && scrollThershold < 4)
                            {
                                yy = yyNovels*gs::scaley; long i = 0;
                                for (auto it = Novels::info.begin(); it != Novels::info.end() && active; ++it)
                                {
                                    novelButtons.index = i;
                                    
                                    novelButtons.setString((*it).name);
                                    novelButtons.setPosition(novelButtons.shape.getPosition().x, yy);
                                    if (novelButtons.PollEvent(event))
                                    {
                                        unselectNovelSelected = false;
                                        SelectNovel(it);
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
                                wif.open((*novelSelected).path);
    #else
                                wif.open(base::utf8((*novelSelected).path));
    #endif
                                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
                                
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
                                if (!active) {
                                    novelMusic.pause();
                                    entity->AddComponent<NovelComponents::Novel>(base::GetFolderPath((*novelSelected).path) + scenario, &(*novelSelected)); }
                                else cout << "MainMenu :: PollEvent :: Novel start failed with no scenario found." << endl;
                            }
                            
                            if (unselectNovelSelected)
                            {
                                if (doParallax) CalculateParallax(background, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                                sf::Vector2i dot; if (event.type == sf::Event::MouseButtonReleased) dot = { event.mouseButton.x, event.mouseButton.y }; else dot = { event.touch.x, event.touch.y };
                                unselectNovelSelected = !novelBackShape.getGlobalBounds().contains(dot.x, dot.y);
                                if (unselectNovelSelected)
                                {
                                    if (isNovelSelected && novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
                                    isNovelSelected = false;
                                    novelMusic.stop();
                                }
                            }
                            if ((event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) || event.type == sf::Event::TouchEnded) scrollThershold = 0;
                        }
                    }
                    break;
                case Page::Editor: if (backButton.PollEvent(event)) page = Page::Main;
                    else if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)) page = Page::Main;
                    break;
                case Page::Settings: if (backButton.PollEvent(event)) page = Page::Main;
                    else if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)) page = Page::Main;
                    else if (tempButton.PollEvent(event)) { gs::drawGUIBoundaries = !gs::drawGUIBoundaries;
                        if (gs::drawGUIBoundaries) tempButton.setString(L"Да, рисуем границы!!!");
                        else tempButton.setString(L"О нет! Границы не рисуем!"); }
                    break;
                case Page::Language: if (backButton.PollEvent(event)) page = Page::Main;
                    else if ((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)) page = Page::Main;
                    break;
                default: break;
            }
        }
        void MainMenu::Resize(const unsigned int& width, const unsigned int& height)
        {
            //guiSystem.Resize(width, height);
            if (!active) return;
            if (spriteLoaded)
            {
                float factorX = (float)width / background.getTexture()->getSize().x * (doParallax? 1 + parallaxPower : 1);
                float factorY = (float)height / background.getTexture()->getSize().y * (doParallax? 1 + parallaxPower : 1);
                float scaleFactor = factorX > factorY ? factorX : factorY;
                background.setScale(scaleFactor, scaleFactor);
                background.setPosition(width/2, height/2);
            }
            
            tempButton.Resize(width, height);
            tempButton.setPosition(width/3, 3*height/4);
            
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
            
            languageButton.Resize(width, height);
            languageButton.setPosition(10*gs::scale, gs::height - 20*gs::scale);
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
            if (gs::verticalOrientation)
            {
                novelBackShape.setSize({gs::width * 4.65f/5, gs::height * 4.f/5});
                novelBackShape.setPosition(0.35*gs::width/10, gs::height/15);
                novelButtons.setSize({gs::width * 4.7f/5, 120*gs::scScale});
                novelButtons.setPosition(0.3*gs::width/10, 0);
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
            
            CalculateScrollBounds();
            if (novelShowBackground)
            {
                float factorX = (float)width / novelBackground.getTexture()->getSize().x * (doParallax? 1 + parallaxPower : 1);
                float factorY = (float)height / novelBackground.getTexture()->getSize().y * (doParallax? 1 + parallaxPower : 1);
                float scaleFactor = factorX > factorY ? factorX : factorY;
                novelBackground.setScale(scaleFactor, scaleFactor);
                novelBackground.setPosition(width/2, height/2);
            }
            if (isNovelSelected)
            {
                novelText.setCharacterSize(35 * gs::scScale);
                novelText.setPosition(novelBackShape.getPosition().x + 5*gs::scalex, 0);
                novelTextDescription = nss::GetStringWithLineBreaks(novelText, novelRawDescription, novelBackShape.getSize().x - 10*gs::scalex, 0, 10*gs::scale);
            }
            
            if (doParallax)
            {
                if (novelShowBackground) CalculateParallax(novelBackground, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                else CalculateParallax(background, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
            }
        }
        void MainMenu::CalculateScrollBounds()
        {
            if (Novels::info.size())
            {
                int yyHeight = (novelButtons.shape.getLocalBounds().height/gs::scaley + 10);
                long yyNovelsHeight = yyHeight * Novels::info.size();
                if (yyNovelsHeight < gs::relativeHeight - gs::relativeHeight/4) {
                    yyNovels_from = gs::relativeHeight/2 - yyNovelsHeight/2;
                    yyNovels_to = yyNovels_from;
                } else {
                    float yyOnScreen = (float)gs::relativeHeight/yyHeight;
                    yyNovels_from = -yyNovelsHeight + yyHeight*yyOnScreen/2;//(yyOnScreen - 0.5)*yyHeight;
                    yyNovels_to = yyOnScreen/2 * yyHeight;
                }
                if (yyNovels < yyNovels_from) yyNovels = yyNovels_from;
                if (yyNovels > yyNovels_to) yyNovels = yyNovels_to;
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
                    languageButton.Draw(window);
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
                    if (!isNovelSelected || !gs::verticalOrientation)
                    {
                        yy = yyNovels*gs::scaley;
                        for (auto it = Novels::info.begin(); it != Novels::info.end(); ++it)
                        {   //TODO: Make this draw only the ones in the scope.
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
                case Page::Settings: backButton.Draw(window); tempButton.Draw(window); break;
                case Page::Language: backButton.Draw(window); break;
                default: break;
            }
            
            //guiSystem.Draw(window);
        }
        void MainMenu::ReceiveMessage(MessageHolder& message)
        {
            if (message.info == "NovelComponents :: Novel :: Destroying")
            {
                active = true;
                if (novelMusic.getStatus() == sf::Music::Status::Paused) {
                    novelMusic.play(); novelMusic.setPlayingOffset(sf::seconds(novelMusic_from)); }
                gs::requestWindowRefresh = true;
                Resize(gs::width, gs::height);
            }
            else if (nss::Command(message.info, "Request"))
            {
                if (message.additional == L"Data/Images/background.jpg")
                {
                    spriteLoaded = false;
                    sf::Texture* texture = ic::LoadTexture(L"Data/Images/background.jpg");
                    if (texture)
                    {
                        background.setTexture(*texture, true);
                        background.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
                        
                        float factorX = (float)gs::width / texture->getSize().x * (doParallax? 1 + parallaxPower : 1);
                        float factorY = (float)gs::height / texture->getSize().y * (doParallax? 1 + parallaxPower : 1);
                        float scaleFactor = factorX > factorY ? factorX : factorY;
                        background.setScale(scaleFactor, scaleFactor);
                        background.setPosition(gs::width/2, gs::height/2);
                        
                        if (doParallax) CalculateParallax(background, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                        
                        spriteLoaded = true;
                    }
                }
                else if (message.additional == novelBackTexture && novelShowBackground)
                {
                    sf::Texture* texture = ic::LoadTexture(novelBackTexture);
                    if (texture)
                    {
                        novelBackground.setTexture(*texture, true);
                        novelBackground.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
                        float factorX = (float)gs::width / texture->getSize().x * (doParallax? 1 + parallaxPower : 1);
                        float factorY = (float)gs::height / texture->getSize().y * (doParallax? 1 + parallaxPower : 1);
                        float scaleFactor = factorX > factorY ? factorX : factorY;
                        novelBackground.setScale(scaleFactor, scaleFactor);
                        novelBackground.setPosition(gs::width/2, gs::height/2);
                        if (doParallax) CalculateParallax(novelBackground, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                    }
                }
            }
        }
        void MainMenu::CalculateParallax(sf::Sprite& sprite, int dotX, int dotY)
        {
            if (dotX >= 0 && dotY >= 0 && dotX <= gs::width && dotY <= gs::height)
            {
                float posX = gs::width/2 + (int)(dotX - gs::width/2) * parallaxPower;
                float posY = gs::height/2 + (int)(dotY - gs::height/2) * parallaxPower;
                sprite.setPosition(posX, posY);
            }
        }
        void MainMenu::SelectNovel(list<NovelInfo>::iterator& it, bool focus, bool force)
        {
            if (!isNovelSelected || novelSelected != it || force)
            {
                novelMusic.stop();
                if (novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
                isNovelSelected = true; novelSelected = it;
                novelRawDescription = L""; novelAuthor = L"";
                std::wstring back{ L"" }, music{ L"" };
                float musicFrom{ 0 }; bool done{ false };
                
                std::wifstream wif;
#ifdef _WIN32
                wif.open((*it).path);
#else
                wif.open(base::utf8((*novelSelected).path));
#endif
                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
                
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
                        else if (nss::Command(command, L"music "))
                        {
                            music = nss::ParseAsMaybeQuoteString(command); ++infoRead;
                            while (command.lastPos < line.length())
                            {
                                nss::SkipSpaces(command);
                                std::wstring fromstr = nss::ParseUntil(command, L' ');
                                if (fromstr != L"from") musicFrom = base::atof(fromstr);
                            }
                        }
                        
                        if (infoRead >= 4) done = true;
                    }
                    
                    wif.close();
                }
                if (back != L"" && back.length())
                {
                    std::wstring path = base::GetFolderPath((*it).path) + back;
                    sf::Texture* texture = ic::RequestHigherTexture(path, entity, 1);//ic::LoadTexture(path, 1);
                    if (texture)
                    {
                        novelBackTexture = path;
                        novelBackground.setTexture(*texture, true);
                        novelBackground.setOrigin(texture->getSize().x/2, texture->getSize().y/2);
                        float factorX = (float)gs::width / texture->getSize().x * (doParallax? 1 + parallaxPower : 1);
                        float factorY = (float)gs::height / texture->getSize().y * (doParallax? 1 + parallaxPower : 1);
                        float scaleFactor = factorX > factorY ? factorX : factorY;
                        novelBackground.setScale(scaleFactor, scaleFactor);
                        novelBackground.setPosition(gs::width/2, gs::height/2);
                        if (doParallax) CalculateParallax(novelBackground, sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                        novelShowBackground = true;
                    }
                }
                if (novelRawDescription == L"") { novelRawDescription = novelTextDescription = L"Описание: ..."; }
                else
                {
                    novelRawDescription = L"Описание: " + novelRawDescription;
                    novelText.setCharacterSize(35 * gs::scScale);
                    novelText.setPosition(novelBackShape.getPosition().x + 5*gs::scalex, 0);
                    novelTextDescription = nss::GetStringWithLineBreaks(novelText, novelRawDescription, novelBackShape.getSize().x - 10*gs::scalex, 0, 10*gs::scale);
                }
                if (novelAuthor == L"") novelAuthor = L"???";
                if (music != L"")
                {
                    fileInMemory.release();
                    if (sc::LoadMusic(novelMusic, base::GetFolderPath((*it).path) + music, fileInMemory))
                    {
                        novelMusic.setLoop(true);
                        novelMusic.setVolume(100 * gs::maxVolumeMusic * gs::maxVolumeGlobal);
                        if (musicFrom >= 0) {novelMusic.setPlayingOffset(sf::seconds(musicFrom)); novelMusic_from = musicFrom; } else novelMusic_from = 0;
                        novelMusic.play();
                    }
                }
            }
            if (focus && isNovelSelected)
            {
                // TODO
            }
        }
        void MainMenu::UnselectNovel()
        {
            if (novelShowBackground) { novelShowBackground = false; ic::DeleteImage(novelBackTexture); }
            isNovelSelected = false; novelMusic.stop();
        }
    }
}
