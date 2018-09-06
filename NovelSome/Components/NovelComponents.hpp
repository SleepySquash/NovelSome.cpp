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

//CharacterData in NovelLibrary
#include <unordered_map>

//Folder path
#include <dirent.h>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/Base.hpp"
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
        class Novel;
        
        
        class GamePause : public Component
        {
        private:
            sf::RectangleShape shape;
            Novel* novel{ nullptr };
            
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum{ appearing, waiting, disappearing };
            modeEnum mode{ waiting };
            
            int maxAlpha{ 170 };
            float appearTime{ 0.3f };
            float disappearTime{ 0.3f };
            
            GamePause(Novel* novel)
            {
                this->novel = novel;
                shape.setFillColor(sf::Color(0,0,0,0));
            }
            void Update(const sf::Time& elapsedTime) override
            {
                switch (mode)
                {
                    case appearing:
                        if (currentTime < appearTime)
                            currentTime += elapsedTime.asSeconds();
                        
                        if (currentTime >= appearTime)
                        {
                            alpha = maxAlpha;
                            currentTime = 0.f;
                            mode = waiting;
                        }
                        else
                            alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                        shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                        break;
                        
                    case disappearing:
                        if (currentTime < disappearTime)
                            currentTime += elapsedTime.asSeconds();
                        
                        if (currentTime >= disappearTime)
                        {
                            alpha = 0;
                            currentTime = 0.f;
                            mode = waiting;
                        }
                        else
                            alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                        shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                        break;
                        
                    default:
                        break;
                }
            }
            void PollEvent(sf::Event& event) override
            {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                {
                    ns::GlobalSettings::isPause = !ns::GlobalSettings::isPause;
                    mode = ns::GlobalSettings::isPause? appearing : disappearing;
                    currentTime = 0.f;
                }
            }
            void Draw(sf::RenderWindow *window) override
            {
                if (alpha != 0)
                    window->draw(shape);
            }
            void Resize(unsigned int width, unsigned int height) override
            {
                shape.setSize({(float)width, (float)height});
            }
        };
        
        
        
        class Waiting : public Component
        {
            float currentTime{ 0.f };
            Novel* novel{ nullptr };
            
        public:
            float waitingTime{ 1.f };
            
            void Update(const sf::Time& elapsedTime) override;
            void SetNovel(Novel* novel);
        };
        
        
        
        class Background : public Component
        {
        private:
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            Novel* novel{ nullptr };
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
            void SetNovel(Novel* novel);
            void SetGroup(List<Background>* element);
            void CalculateScale(unsigned int width, unsigned int height);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        class Dialogue : public ns::Component
        {
        private:
            sf::RectangleShape shape;
            sf::Text text;
            Novel* novel{ nullptr };
            List<Dialogue>* groupPointer{ nullptr };
            
            sf::RectangleShape charShape;
            sf::Text charText;
            sf::String charString{ "" };
            
            bool drawCharacterName{ false };
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
            void SetNovel(Novel* novel);
            void SetGroup(List<Dialogue>* element);
            void SetCharacter(sf::String character);
            void SetDialogue(sf::String dialogue);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        struct CharacterData : public Component
        {
            sf::String name{ "" };
            sf::String displayName{ "" };
        };
        
        
        
        class Character : public Component
        {
            
        };
        
        
        
        class AudioPlayer : public Component
        {
            
        };
        
        
        
        class GUISystem : public Component
        {
            
        };
        
        
        
        class NovelLibrary : public Component
        {
        private:
            Novel* novel{ nullptr };
            
        public:
            List<CharacterData>* charData{ nullptr };
            std::unordered_map<std::wstring, CharacterData*> characterLibrary;
            
            NovelLibrary(Novel* novel);
            ~NovelLibrary();
            void FreeData();
            void ScanForCharacters();
        };
        
        
        
        class Novel : public Component
        {
        private:
            EntitySystem system;
            Entity* layers;
            Entity* essentials;
            
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
            ns::NovelComponents::NovelLibrary* library{ nullptr };
            
            Novel(sf::String path);
            ~Novel();
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void OnHold(ns::Component* component);
            void UnHold(ns::Component* component);
            sf::String GetFolderPath();
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
            template<typename T> void FreeGroup(List<T>* list)
            {
                ns::List<T>* next{ nullptr };
                for (; list != nullptr; list = next)
                {
                    next = list->next;
                    if (list != nullptr)
                        delete list;
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
