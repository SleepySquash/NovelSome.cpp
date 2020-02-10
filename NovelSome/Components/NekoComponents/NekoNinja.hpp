//
//  NekoNinjaComponents.hpp
//  NekoPlace
//
//  Created by Никита Исаенко on 25/01/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef NekoNinjaComponents_hpp
#define NekoNinjaComponents_hpp

#include <iostream>
#include <vector>
#include <list>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors.hpp>

using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::list;

using namespace ns;

namespace NekoNinja
{
    struct SceneBackground : Component
    {
        sf::Sprite sprite;
        bool spriteLoaded{ false };
        
        void Destroy() override { ic::DeleteImage(L"Data/Images/park.jpg"); }
        void Init() override
        {
            sf::Texture* texture = ic::LoadTexture(L"Data/Images/park.jpg");
            if (texture != nullptr) { sprite.setTexture(*texture); spriteLoaded = true; }
        }
        void Resize(const unsigned int& width, const unsigned int& height) override
        {
            if (spriteLoaded)
            {
                float factorX = (float)width / sprite.getTexture()->getSize().x;
                float factorY = (float)height / sprite.getTexture()->getSize().y;
                
                float scaleFactor = factorX > factorY ? factorX : factorY;
                sprite.setScale(scaleFactor, scaleFactor);
            }
        }
        void Draw(sf::RenderWindow* window) override
        {
            if (spriteLoaded) window->draw(sprite);
        }
    };
    
    
    
    struct NekoInfo
    {
        std::wstring name{ L"" };
        unsigned int chance{ 0 };
        
        NekoInfo() { }
        NekoInfo(const std::wstring& name, const unsigned int& chance) : name(name), chance(chance) { }
    };
    struct NekoLibrary
    {
        static vector<NekoInfo> neko;
    };
    typedef NekoLibrary nl;
    
    
    
    
    struct NekoObject
    {
        static float gravity;
        
        float xVelocity{ 0.f }, yVelocity{ 0.f };
        float x{ 0 }, y{ 0 };
        bool tamed{ false }, offline{ false };
        
        sf::Sprite sprite;
        std::wstring nekoName{ L"Shigure" };
        
        void Init()
        {
            //int nekoChance = rand() % 10000;
            int nekoChance = rand() % nl::neko.size();
            nekoName = nl::neko[nekoChance].name;
            
            x = rand() % gs::relativeWidth;
            y = gs::relativeHeight;
            
            if (x < gs::relativeWidth/13) x = gs::relativeWidth/13;
            if (x > gs::relativeWidth - gs::relativeWidth/13) x = gs::relativeWidth - gs::relativeWidth/13;
            
            if (x < gs::relativeWidth/2) xVelocity = rand() % 6; else xVelocity = -rand() % 6;
            yVelocity = -18.f + rand() % 4;
            
            sf::Texture* texture = ic::LoadTexture(L"Data/Neko/" + nekoName + L".png");
            if (texture != nullptr) sprite.setTexture(*texture);
            sprite.setOrigin(190, 0);
        }
        void Destroy() { offline = true; ic::DeleteImage(L"Data/Neko/" + nekoName + L".png"); }
        void Update(const sf::Time& elapsedTime)
        {
            yVelocity += gravity;
            if (y > (gs::relativeHeight + 100)) Destroy();
            
            x += xVelocity * 1;
            y += yVelocity * 1;
            
            sprite.setPosition(x * gs::scalex, y * gs::scaley);
        }
        void Resize()
        {
            sprite.setPosition(x * gs::scalex, y * gs::scaley);
            sprite.setScale(gs::scale, gs::scale);
        }
        void Draw(sf::RenderWindow* window)
        {
            window->draw(sprite);
        }
    };
    
    struct Controller : Component
    {
        list<NekoObject> nekos;
        std::wstring backgroundImage{ L"Data/Images/park.jpg" };
        
        float elapsedSeconds{ 0.f };
        unsigned int score{ 0 };
        
        void Destroy() override
        {
            for (int i = 0; i < nl::neko.size(); ++i)
                ic::SetDestroyable(L"Data/Neko/" + nl::neko[i].name + L".png", true);
        }
        void Init() override
        {
            for (int i = 0; i < nl::neko.size(); ++i)
                ic::PreloadImage(L"Data/Neko/" + nl::neko[i].name + L".png", 0, false);
            for (int i = 0; i < 10; ++i)
            {
                nekos.push_back(NekoObject());
                nekos.back().Init(); nekos.back().Resize();
            }
        }
        void Update(const sf::Time& elapsedTime) override
        {
            elapsedSeconds += elapsedTime.asSeconds();
            if (elapsedSeconds > 2.f)
            {
                elapsedSeconds = 0.f;
                int count = 1 + rand() % 5;
                for (int i = 0; i < count; ++i)
                {
                    nekos.push_back(NekoObject());
                    nekos.back().Init(); nekos.back().Resize();
                }
            }
            
            for (auto it = nekos.begin(); it != nekos.end(); ++it)
            {
                if ((*it).offline) { it = nekos.erase(it); score += 100; }
                else (*it).Update(elapsedTime);
            }
        }
        void Resize(const unsigned int& width, const unsigned int& height) override
        {
            for (auto& n : nekos) n.Resize();
        }
        void Draw(sf::RenderWindow* window) override
        {
            for (auto& n : nekos) n.Draw(window);
        }
        
        // Свайп реализуется следующим образом: в некий вектор добавляются точки (например, в sf::Lines), которые говорят о текущем положении нажатого пальца. При отпускании этого пальца, точки стираются. Также точки стираются, начиная с конца, если с момента их появления прошло больше, скажем, 0.2 секунд. Коллизия проверяется относительно этих точек. И добавление этих точек (как и уничтожение) должно происходить через определённые промежутки времени для оптимизации работы свайпа.
    };
    
    
    
    struct GUIOverlay : Component
    {
        Controller* control{ nullptr };
        bool fontLoaded{ false };
        
        sf::Text scoreText;
        sf::Sprite pauseSprite;
        
        GUIOverlay(Controller* control) : control(control) { }
        void Destroy() override { ic::DeleteImage(L"Data/Images/pause_button.png"); }
        void Init() override
        {
            scoreText.setFillColor(sf::Color::White);
            scoreText.setOutlineColor(sf::Color::Black);
            scoreText.setOutlineThickness(1.f);
            scoreText.setCharacterSize(15);
            scoreText.setFont(*fc::GetFont(L"Pacifica.ttf"));
            fontLoaded = (scoreText.getFont() != nullptr);
            
            sf::Texture* texture = ic::LoadTexture(L"Data/Images/pause_button.png");
            if (texture != nullptr) pauseSprite.setTexture(*texture);
        }
        void Update(const sf::Time& elapsedTime) override
        {
            if (control != nullptr) scoreText.setString(L"Score: " + std::to_wstring(control->score));
        }
        void Draw(sf::RenderWindow* window) override
        {
            if (fontLoaded) window->draw(scoreText);
            window->draw(pauseSprite);
        }
        void Resize(const unsigned int& width, const unsigned int& height) override
        {
            scoreText.setOutlineThickness(gs::scale);
            scoreText.setCharacterSize((unsigned int)(96 * gs::scale));
            scoreText.setPosition(8*gs::scale, gs::scale);
            
            pauseSprite.setScale(gs::scale, gs::scale);
            pauseSprite.setPosition(width - pauseSprite.getLocalBounds().width*gs::scale - 10*gs::scale, 10*gs::scale);
        }
    };
}

#endif /* NekoNinjaComponents_hpp */
