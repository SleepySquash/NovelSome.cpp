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

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/StaticMethods.hpp"
#include "../../Engine/NovelSomeScript.hpp"

namespace ns
{
    namespace NovelComponents
    {
        class Novel;
    }
}

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace NovelComponents
    {
        class Waiting : public NovelObject
        {
            float currentTime{ 0.f };
            Novel* novel{ nullptr };
            
        public:
            float waitingTime{ 1.f };
            
            void Update(const sf::Time& elapsedTime) override;
            void SetNovel(Novel* novel);
        };
        
        
        
        
        
        
        class Background : public NovelObject
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
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
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
        
        
        
        
        
        
        class Dialogue : public NovelObject
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
        
        
        
        
        
        
        struct CharacterData
        {
            sf::String name{ "" };
            sf::String displayName{ "" };
            sf::String filePath{ "" };
        };
        
        
        
        class Character : public NovelObject
        {
        private:
            sf::Image image;
            sf::Texture texture;
            sf::Sprite sprite;
            
            Novel* novel{ nullptr };
            List<Character>* groupPointer{ nullptr };
            CharacterData* characterData{ nullptr };
            sf::String state{ "" };
            
            bool spriteLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            enum modeEnum {appearing, existing, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atAppearance };
            modeEnum afterAppearSwitchTo{ existing };
            
            float customX{ 0 }, customY{ 0 };
            enum positionEnum { custom, left, cleft, center, cright, right };
            positionEnum position { center };
            
            int maxAlpha{ 255 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            bool visible{ true };
            
            void LoadState(sf::String state);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void SetNovel(Novel* novel);
            void SetGroup(List<Character>* element);
            void SetStateMode(modeEnum newMode);
            void SetCharacterData(CharacterData* characterData);
            CharacterData* GetCharacterData();
            void SetPosition(positionEnum position, float x, float y);
        };
        
        
        
        
        
        
        class SoundPlayer : public NovelObject
        {
        /*private:
            sf::SoundBuffer buffer;
            sf::Sound sound;
            Novel* novel;
            List<SoundPlayer>* groupPointer{ nullptr };
            
        public:
            float maxVolume{ 100 };
            
            float appearTime{ 1.f };
            float disappearTime{ 1.f };
            
            void Destroy() override;
            void SetNovel(Novel* novel);
            void SetGroup(List<MusicPlayer>* element);
            void LoadFromFile(sf::String fileName);*/
        };
        
        
        
        class MusicPlayer : public NovelObject
        {
        private:
            sf::Music music;
            Novel* novel;
            List<MusicPlayer>* groupPointer{ nullptr };
            
            bool audioLoaded{ false };
            bool loop{ true };
            sf::String audioPath{ "" };
            
            float volume{ 0.f };
            sf::Time playingOffset;
            float currentTime{ 0.f };
            float timeToStartDisappearing{ 0.f };
            
        public:
            enum modeEnum {appearing, playing, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
            
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ noMessage };
            
        public:
            float maxVolume{ 100 };
            
            float appearTime{ 1.f };
            float disappearTime{ 1.f };
            
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void SetNovel(Novel* novel);
            void SetGroup(List<MusicPlayer>* element);
            void LoadFromFile(sf::String fileName);
            void SetStateMode(modeEnum newMode);
            void SetLoop(bool setLoop);
            void SetPlayingOffset(sf::Time timeOffset);
        };
        
        
        
        
        
        
        struct GUIObject;
        class GUISystem : public NovelObject
        {
        public:
            List<GUIObject>* guiObjects{ nullptr };
            List<GUIObject>* lastGuiObjects{ nullptr };
            
            ~GUISystem();
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            template<typename T, typename ...Args> T* AddComponent(Args... args)
            {
                T* component = new T(args...);
                List<GUIObject>* element = (List<GUIObject>*)malloc(sizeof(List<GUIObject>));
                element->data = component;
                element->next = nullptr;
                element->prev = lastGuiObjects;
                
                if (lastGuiObjects == nullptr)
                    guiObjects = element;
                else
                    lastGuiObjects->next = element;
                lastGuiObjects = element;
                
                component->SetGUISystem(this);
                component->Init();
                component->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                
                return component;
            }
            void SetAlpha(int alpha);
            void Clear();
        };
        
        
        
        struct GUIObject
        {
            int alpha{ 0 };
            GUISystem* system;
            
            virtual void Init() { }
            virtual void Update(const sf::Time& elapsedTime) { }
            virtual void Draw(sf::RenderWindow* window) { }
            virtual void Resize(unsigned int width, unsigned int height) { }
            virtual void Destroy() { }
            virtual void SetAlpha(int alpha) { }
            void SetGUISystem(GUISystem *system);
        };
        
        
        
        namespace GUIObjects
        {
            struct Rectangle : GUIObject
            {
                sf::RectangleShape shape;
                
                void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(int alpha) override;
            };
        }
        
        
        
        
        
        
        struct NovelLibrary
        {
            Novel* novel{ nullptr };
            List<CharacterData>* charData{ nullptr };
            std::unordered_map<std::wstring, CharacterData*> characterLibrary;
            
            NovelLibrary();
            ~NovelLibrary();
            void SetNovel(Novel* novel);
            void FreeData();
            void ScanForCharacters();
        };
        
        
        
        
        
        
        class Novel : public Component
        {
        private:
            NovelSystem layers;
            
            sf::String nsdataPath{ "" };
            sf::String folderPath{ "" };
            nss::CommandSettings command;
            
            bool eof{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            List<NovelObject>* onHold{ nullptr };
            unsigned int onHoldSize{ 0 };
            
            std::wifstream wif;
            
        public:
            NovelLibrary library;
            GUISystem dialogueGUI;
            
            List<Background>* backgroundGroup{ nullptr };
            List<Character>* characterGroup{ nullptr };
            List<Dialogue>* dialogueGroup{ nullptr };
            List<SoundPlayer>* soundGroup{ nullptr };
            List<MusicPlayer>* musicGroup{ nullptr };
            List<GUISystem>* GUIGroup{ nullptr };
            
            Novel(sf::String path);
            ~Novel();
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void OnHold(NovelObject* component);
            void UnHold(NovelObject* component);
            sf::String GetFolderPath();
            void RemoveFromGroup(List<Background>* groupPointer);
            void RemoveFromGroup(List<Dialogue>* groupPointer);
            void RemoveFromGroup(List<Character>* groupPointer);
            void RemoveFromGroup(List<SoundPlayer>* groupPointer);
            void RemoveFromGroup(List<MusicPlayer>* groupPointer);
            void RemoveFromGroup(List<GUISystem>* groupPointer);
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
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
