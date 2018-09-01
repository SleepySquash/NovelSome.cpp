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

#include "../Engine/List.hpp"
#include "../Engine/EntitySystem.hpp"
#include "../Engine/StaticMethods.hpp"
#include "../Engine/NovelSomeScript.hpp"

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
            
            bool spriteLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum {appearing, existing, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atAppearance };
            modeEnum afterAppearSwitchTo{ existing };
            
            int maxAlpha{ 255 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            bool visible{ true };
            
            void LoadImage(sf::String path);
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
            
            bool fontLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum {appearing, waiting, waitingForTime, waitingForInput, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
            
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            modeEnum afterAppearSwitchTo{ waitingForInput };
            
            int maxAlpha = 255;
            bool forcePressInsideDialogue{ true };
            std::string fontName{ "Arial Unicode.ttf" };
            unsigned int characterSize{ 40 };
            
            float waitingTime{ 2.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetNovel(NovelComponent* novel);
            void SetDialogue(sf::String dialogue);
        };
        
        
        
        class NovelComponent : public Component
        {
        private:
            EntitySystem system;
            Entity* entity;
            sf::String nsdataPath{ "" };
            sf::String folderPath{ "" };
            nss::CommandSettings command;
            
            bool eof{ false };
            bool onHold{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            std::wifstream wif;
            
        public:
            NovelComponent(sf::String path) : nsdataPath(path)
            {
                folderPath = nss::GetFolderPath(path);
                entity = system.AddEntity();
                
#ifdef _WIN32
                wif.open(sf::String(resourcePath() + path).toWideString());
#else
                wif.open(resourcePath() + path);
#endif
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
                        command.Command(line);
                        
                        if (nss::Command(command, L"\""))
                        {
                            std::wstring dialogueLine = nss::ParseUntil(command, '"');
                            onHold = true;
                            
                            auto* component = entity->AddComponent<ns::NovelComponents::DialogueComponent>();
                            component->SetNovel(this);
                            component->fontName = "NotoSansCJK-Regular.ttc";
                            component->characterSize = 40;
                            component->forcePressInsideDialogue = true;
                            component->afterAppearSwitchTo = component->waitingForInput;
                            component->sendMessageBack = component->atDisappearing;
                            
                            component->SetDialogue(dialogueLine);
                        }
                        else if (nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                        {
                            nss::ParseUntil(command, '"');
                            std::wstring filePath = nss::ParseUntil(command, '"');
                            
                            onHold = true;
                            
                            auto* component = entity->AddComponent<ns::NovelComponents::BackgroundComponent>();
                            component->SetNovel(this);
                            
                            component->LoadImage(filePath);
                        }
                        
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
            sf::String GetFolderPath()
            {
                return folderPath;
            }
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
