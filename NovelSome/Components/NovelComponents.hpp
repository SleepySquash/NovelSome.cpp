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
        
        class Background : public Component
        {
        private:
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            NovelComponent* novel{ nullptr };
            List<Background>* groupPointer{ nullptr };
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
            
            enum fitModeEnum { noFit, defaultFit, fillCentre, stretch };
            fitModeEnum fitMode { fillCentre };
            
            int maxAlpha{ 255 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            bool visible{ true };
            
            void LoadImage(sf::String path);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void SetNovel(NovelComponent* novel);
            void SetGroup(List<Background>* element);
            void CalculateScale(unsigned int width, unsigned int height);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        class Dialogue : public ns::Component
        {
        private:
            sf::RectangleShape shape;
            sf::Text text;
            NovelComponent* novel{ nullptr };
            List<Dialogue>* groupPointer{ nullptr };
            
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
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetNovel(NovelComponent* novel);
            void SetGroup(List<Dialogue>* element);
            void SetDialogue(sf::String dialogue);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        class Character : Component
        {
            
        };
        
        
        
        class AudioPlayer : Component
        {
            
        };
        
        
        
        class GUISystem : Component
        {
            
        };
        
        
        
        class NovelComponent : public Component
        {
        private:
            EntitySystem system;
            Entity* layers;
            
            sf::String nsdataPath{ "" };
            sf::String folderPath{ "" };
            nss::CommandSettings command;
            
            bool eof{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            List<Component>* onHold{ nullptr };
            unsigned int onHoldSize{ 0 };
            
            std::wifstream wif;
            
        public:
            List<ns::NovelComponents::Background>* backgroundGroup{ nullptr };
            List<ns::NovelComponents::Character>* characterGroup{ nullptr };
            List<ns::NovelComponents::Dialogue>* dialogueGroup{ nullptr };
            List<ns::NovelComponents::AudioPlayer>* audioGroup{ nullptr };
            List<ns::NovelComponents::GUISystem>* GUIGroup{ nullptr };
            
            NovelComponent(sf::String path) : nsdataPath(path)
            {
                folderPath = nss::GetFolderPath(path);
                layers = system.AddEntity();
                
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
                while (fileOpened && !eof && (onHold == nullptr || onHoldSize == 0))
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
                            
                            auto* component = layers->PrioritizeComponent<ns::NovelComponents::Dialogue>(10000);
                            OnHold(component);
                            
                            component->SetNovel(this);
                            component->fontName = "NotoSansCJK-Regular.ttc";
                            component->characterSize = 40;
                            component->forcePressInsideDialogue = true;
                            component->afterAppearSwitchTo = component->waitingForInput;
                            component->sendMessageBack = component->atDisappearing;
                            
                            dialogueGroup = ns::list::Insert<ns::NovelComponents::Dialogue>(dialogueGroup);
                            dialogueGroup->data = component;
                            component->SetGroup(dialogueGroup);
                            
                            component->SetDialogue(dialogueLine);
                        }
                        else if (nss::Command(command, L"background hide"))
                        {
                            if (backgroundGroup != nullptr)
                            {
                                List<Background>* temp = backgroundGroup;
                                while (temp != nullptr)
                                {
                                    if (temp->data != nullptr)
                                    {
                                        OnHold(temp->data);
                                        temp->data->sendMessageBack = temp->data->atDeprecated;
                                        temp->data->SetStateMode(temp->data->disappearing);
                                    }
                                    temp = temp->next;
                                    //TODO: Hold must be an array of queue, so every object removes it's own OnHold
                                }
                            }
                        }
                        else if (nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                        {
                            nss::ParseUntil(command, '"');
                            std::wstring filePath = nss::ParseUntil(command, '"');
                            
                            auto* component = layers->PrioritizeComponent<ns::NovelComponents::Background>(0);
                            component->SetPriority(0);
                            OnHold(component);
                            
                            component->SetNovel(this);
                            component->fitMode = component->fillCentre;
                            
                            backgroundGroup = ns::list::Insert<ns::NovelComponents::Background>(backgroundGroup);
                            backgroundGroup->data = component;
                            component->SetGroup(backgroundGroup);
                            
                            component->LoadImage(filePath);
                        }
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
            void OnHold(ns::Component* component)
            {
                onHold = ns::list::Insert<ns::Component>(onHold);
                onHold->data = component;
                
                onHoldSize++;
            }
            void UnHold(ns::Component* component)
            {
                List<Component>* temp = onHold;
                while (temp != nullptr)
                {
                    List<Component>* next = temp->next;
                    if (temp->data == component)
                    {
                        if (temp == onHold)
                            onHold = temp->next;
                        if (temp->next != nullptr)
                            temp->next->prev = temp->prev;
                        if (temp->prev != nullptr)
                            temp->prev->next = temp->next;
                        delete temp;
                        
                        onHoldSize--;
                        next = nullptr;
                    }
                    
                    temp = next;
                }
            }
            sf::String GetFolderPath()
            {
                return folderPath;
            }
            template<typename T> void RefreshGroup(List<T>* group)
            {
                while (group != nullptr)
                {
                    List<T>* next = group->next;
                    if (group->data == nullptr)
                        ns::list::Remove<T>(group);
                    group = next;
                }
            }
            void RemoveFromGroup(List<Background>* groupPointer);
            void RemoveFromGroup(List<Dialogue>* groupPointer);
            void RemoveFromGroup(List<Character>* groupPointer);
            void RemoveFromGroup(List<AudioPlayer>* groupPointer);
            void RemoveFromGroup(List<GUISystem>* groupPointer);
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
