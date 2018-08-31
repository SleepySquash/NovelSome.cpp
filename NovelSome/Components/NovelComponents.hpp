//
//  NovelComponents.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef NovelComponents_hpp
#define NovelComponents_hpp

#include <iostream>

//NovelComponent
#include <locale>
#include <fstream>
#include <codecvt>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "List.hpp"
#include "../Engine/EntitySystem.hpp"
#include "../Engine/StaticMethods.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace NovelComponents
    {
        class NovelComponent;
        
        class BackgroundComponent : public Component
        {
        private:
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            NovelComponent* novel;
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
            void SetNovel(NovelComponent* novel);
            void CalculateScale(unsigned int width, unsigned int height);
        };
        
        class DialogueComponent : public ns::Component
        {
        private:
            sf::RectangleShape shape;
            sf::Text text;
            NovelComponent* novel;
            
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum {appearing, waiting, waitingForTime, waitingForInput, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
            
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            
            modeEnum afterAppearSwitchTo{ waitingForInput };
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            
            int maxAlpha = 255;
            bool forcePressInsideDialogue{ true };
            std::string fontName{ "Arial Unicode.ttf" };
            unsigned int characterSize{ 40 };
            
            float waitingTime{ 2.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            DialogueComponent(sf::String dialogue);
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetNovel(NovelComponent* novel);
        };
        
        
        
        class NovelComponent : public Component
        {
        private:
            EntitySystem system;
            Entity* entity;
            sf::String nsdataPath = "";
            
            bool eof{ false };
            bool onHold{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            std::wifstream wif;
            
        public:
            NovelComponent(sf::String path) : nsdataPath(path)
            {
                entity = system.AddEntity();
                
                wif.open(resourcePath() + path);
                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                
                if (!(fileOpened = wif.is_open()))
                    cout << "Error :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
            }
            ~NovelComponent()
            {
                wif.close();
                system.Destroy();
            }
            void Update(const sf::Time& elapsedTime) override
            {
                while (fileOpened && !eof && !onHold)
                {
                    if (wif.fail() || wif.bad())
                        cout << "Warning :: NovelComponent :: Stream.fail() or Stream.bad() caught: " << nsdataPath.toAnsiString() << endl;
                    
                    if (!wif.eof())
                    {
                        std::getline(wif, line);
                        
                        if (line[0] == L'"')
                        {
                            onHold = true;
                            
                            auto* component = entity->AddComponent<ns::NovelComponents::DialogueComponent>(line);
                            component->SetNovel(this);
                            component->fontName = "Arial Unicode.ttf";
                            component->characterSize = 40;
                            component->forcePressInsideDialogue = true;
                            component->afterAppearSwitchTo = component->waitingForInput;
                            component->sendMessageBack = component->atDisappearing;
                        }
                        /*else if (line[0] == L"background ")
                        {
                            
                        }*/
                        
                        std::cout << "LINE: " << sf::String(line).toAnsiString() << std::endl;
                    }
                    else
                        eof = true;
                }
                
                if (fileOpened)
                    system.Update(elapsedTime);
            }
            void Draw(sf::RenderWindow* window) override
            {
                if (fileOpened)
                    system.Draw(window);
            }
            void Resize(unsigned int width, unsigned int height) override
            {
                if (fileOpened)
                    system.Resize(width, height);
            }
            void PollEvent(sf::Event& event) override
            {
                if (fileOpened)
                    system.PollEvent(event);
            }
            void UnHold()
            {
                onHold = false;
            }
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
