//
//  TestComponents.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef TestComponents_hpp
#define TestComponents_hpp

#include <iostream>

//NovelComponent
#include <locale>
#include <fstream>
#include <codecvt>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Engine/List.hpp"
#include "../Engine/EntitySystem.hpp"
#include "../Engine/StaticMethods.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace TestComponents
    {
        class TestComponent : public ns::Component
        {
        private:
            sf::Text text;
            
        public:
            TestComponent();
            TestComponent(std::string str);
            void Init() override;
            void Draw(sf::RenderWindow* window) override;
            void SetString(std::string str);
            void SetFont(const sf::Font& font);
            void SetSize(unsigned int size);
            void SetColor(const sf::Color& color);
            void SetPosition(float x, float y);
            
        };
        
        
        
        class EntityListComponent : public ns::Component
        {
        private:
            sf::Text text;
            ns::EntitySystem system;
            bool fontLoaded{ false };
            
        public:
            EntityListComponent(ns::EntitySystem system);
            void Init() override;
            void Draw(sf::RenderWindow* window) override;
            
        };
        
        
        
        class DebugComponent : public ns::Component
        {
        private:
            sf::Text performanceText;
            sf::Text projectText;
            sf::Text buildText;
            bool fontLoaded{ false };
            
        public:
            DebugComponent(sf::String build);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            
        };
        
        
        
        class BackgroundComponent : public Component
        {
        private:
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            std::string imagePath;
            
            bool spriteLoaded = false;
            
            sf::Int8 alpha{ 0 };
            int maxAlpha = 255;
            
            enum modeEnum {appearing, existing, disappearing, deprecated};
            modeEnum mode{ appearing };
            
            float currentTime{ 0.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
        public:
            bool visible = true;
            
            BackgroundComponent(std::string path);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            
            void CalculateScale(unsigned int width, unsigned int height);
        };
        
        
        
        class DialogueComponent : public ns::Component
        {
        private:
            sf::RectangleShape shape;
            sf::Text text;
            
            sf::Int8 alpha{ 0 };
            int maxAlpha = 255;
            
            enum modeEnum {appearing, waiting, waitingForTime, waitingForInput, disappearing, deprecated};
            modeEnum mode{ appearing };
            
            bool mustPressInsideDialogue{ true };
            std::string fontName{ "Arial Unicode.ttf" };
            unsigned int characterSize{ 40 };
            
            float waitingTime{ 2.f };
            float currentTime{ 0.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
        public:
            DialogueComponent(sf::String dialogue);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
        };
        
        
        /*NovelComponent(sf::String path) : nsdataPath(path)
        {
            entity = system.AddEntity();
            
            wif.open(resourcePath() + path);
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            if (wif.is_open())
            {
                ns::Entity* entity = system.AddEntity();
                fileOpened = true;

                while (wif.good())
                {
                    std::getline(wif, line);
                    if (!wif.eof())
                    {
                        if (line[0] == L'"')
                        entity->AddComponent<ns::TestComponents::DialogueComponent>(line);
                        std::cout << sf::String(line).toAnsiString() << std::endl;
                    }
                }
            }
            else
                cout << "Debug :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
            
            if (!(fileOpened = wif.is_open()))
                cout << "Debug :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
        }*/
    }
}

#endif /* TestComponents_hpp */
