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

//std::unique_ptr<> in Music and Background
#include <memory>

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

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace NovelComponents
    {
        class Novel;
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
            sf::Texture texture;
            sf::Sprite sprite;
            Novel* novel{ nullptr };
            List<Background>* groupPointer{ nullptr };
            sf::String imagePath{ L"" };
            
            bool spriteLoaded{ false };
            bool backgroundLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
            float defaultPositionX{ 0.f }, defaultPositionY{ 0.f };
            
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
            
            float scaleX{ 1.f }, scaleY{ 1.f };
            
            bool doParallax{ ns::GlobalSettings::isParallaxEnabled };
            float parallaxPower { ns::GlobalSettings::defaultParallaxBackground };
            
            int maxAlpha{ 255 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            bool visible{ true };
            
            void LoadImage(sf::String path);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void SetNovel(Novel* novel);
            void SetGroup(List<Background>* element);
            void CalculateScale(unsigned int width, unsigned int height);
            void CalculateParallax(int mouseX, int mouseY);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        
        
        
        class GUISystem;
        struct CharacterData;
        namespace Skins
        {
            struct Dialogue;
        }
        class Dialogue : public NovelObject
        {
        public:
            sf::Text text;
            sf::String textString{ "" };
            
            sf::Text charText;
            sf::String charString{ "" };
        private:
            GUISystem* guiSystem{ nullptr };
            Skins::Dialogue* skin{ nullptr };
            CharacterData* character{ nullptr };
            Novel* novel{ nullptr };
            List<Dialogue>* groupPointer{ nullptr };
            
            bool drawCharacterName{ false };
            bool fontLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
            bool visible{ true };
            
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
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 30 };
            
            float waitingTime{ 2.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            Dialogue(Novel* novel);
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetGroup(List<Dialogue>* element);
            void SetCharacter(CharacterData* character);
            void SetCharacterName(const sf::String& characterName);
            void SetDialogue(const sf::String& dialogue);
            void SetStateMode(modeEnum newMode);
            void SetGUISystem(GUISystem* system);
        };
        
        
        
        
        
        
        struct CharacterData
        {
            sf::String name{ "" };
            sf::String displayName{ "" };
            sf::String filePath{ "" };
            
            sf::Color fillColor{ sf::Color::White };
            sf::Color outlineColor{ sf::Color::Black };
            float outlineThickness{ 0.f };
        };
        
        
        
        class Character : public NovelObject
        {
        private:
            sf::Texture texture;
            sf::Sprite sprite;
            sf::String imagePath{ L"" };
            
            Novel* novel{ nullptr };
            List<Character>* groupPointer{ nullptr };
            CharacterData* characterData{ nullptr };
            sf::String state{ "" };
            
            bool spriteLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
            float defaultPositionX{ 0.f }, defaultPositionY{ 0.f };
            
        public:
            enum modeEnum {appearing, existing, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atAppearance };
            modeEnum afterAppearSwitchTo{ existing };
            
            float scaleX{ 1.f }, scaleY{ 1.f };
            
            float customX{ 0 }, customY{ 0 };
            enum positionEnum { custom, left, cleft, center, cright, right };
            positionEnum position { center };
            
            bool doParallax{ ns::GlobalSettings::isParallaxEnabled };
            float parallaxPower { ns::GlobalSettings::defaultParallaxNormal };
            
            int maxAlpha{ 255 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            bool visible{ true };
            
            void LoadState(sf::String state);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void CalculateParallax(int mouseX, int mouseY);
            void SetNovel(Novel* novel);
            void SetGroup(List<Character>* element);
            void SetStateMode(modeEnum newMode);
            void SetCharacterData(CharacterData* characterData);
            CharacterData* GetCharacterData();
            void SetPosition(positionEnum position, float x, float y);
        };
        
        
        
        
        
        
        class SoundPlayer : public NovelObject
        {
        private:
            std::unique_ptr<char[]> fileInMemory;
            
            sf::SoundBuffer soundBuffer;
            sf::Sound sound;
            Novel* novel;
            List<SoundPlayer>* groupPointer{ nullptr };
            
            bool audioLoaded{ false };
            bool loop{ false };
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
            
            float appearTime{ 0.f };
            float disappearTime{ 0.f };
            
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void SetNovel(Novel* novel);
            void SetGroup(List<SoundPlayer>* element);
            void LoadFromFile(sf::String fileName);
            void SetStateMode(modeEnum newMode);
            void SetLoop(bool setLoop);
            void SetPlayingOffset(sf::Time timeOffset);
        };
        
        
        
        class MusicPlayer : public NovelObject
        {
        private:
            std::unique_ptr<char[]> fileInMemory;
            
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
        private:
            int lastWidth{ 0 };
            int lastHeight{ 0 };
            
        public:
            Novel* novel{ nullptr };
            GUIObject* parent{ nullptr };
            unsigned int users{ 0 };
            List<GUIObject>* guiObjects{ nullptr };
            List<GUIObject>* lastGuiObjects{ nullptr };
            
            sf::Int8 lastAlpha{ 0 };
            
            ~GUISystem();
            GUISystem(Novel* novel = nullptr);
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void VariableResize(unsigned int width, unsigned int height);
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
            void VariableChange(const std::wstring& varName);
            void SetAlpha(sf::Int8 alpha);
            void SetAlphaIfBigger(sf::Int8 alpha);
            void Clear();
            void SetNovel(Novel* novel);
            void SetParent(GUIObject* guiSystem);
        };
        
        
        
        struct GUIConstrainsResult
        {
            int result;
            bool dependsOnVariable;
            bool constant;
            
            GUIConstrainsResult() { }
            GUIConstrainsResult(int res, bool depends, bool consta) : result(res), dependsOnVariable(depends), constant(consta) { }
        };
        struct GUIConstrains
        {
            //Если строка содержит какие-то переменные, не меняющиеся с Resize, то они будут расчитаны и записаны сюда, а строки будут обнулены, чтобы при других Resize не происходило перерасчёта величин.
            int left{ 0 };
            int right{ 0 };
            int top{ 0 };
            int bottom{ 0 };
            
            int width{ 0 };
            int height{ 0 };
            
            int posX{ 0 };
            int posY{ 0 };
            
            
            bool leftC{ false };
            bool rightC{ false };
            bool topC{ false };
            bool bottomC{ false };
            
            bool widthC{ false };
            bool heightC{ false };
            
            bool posXC{ false };
            bool posYC{ false };
            
            
            std::wstring leftS{ L"" };
            std::wstring rightS{ L"" };
            std::wstring topS{ L"" };
            std::wstring bottomS{ L"" };
            
            std::wstring widthS{ L"" };
            std::wstring heightS{ L"" };
            
            std::wstring posXS{ L"" };
            std::wstring posYS{ L"" };
            
            
            //std::wstring displayWhenVariableIsTrue{ L"" };
            bool isDependsOnVariable[8] = { false, false, false, false,  false, false,  false, false };
            std::unordered_map<std::wstring, bool> dependOnVariables;
            
            GUIConstrainsResult Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height, std::wstring& line);
            void Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height);
        };
        struct GUIObject
        {
            bool isHappy{ true };
            
            unsigned char maxAlpha = 255;
            bool visible{ true };
            
            GUISystem* guiSystem{ nullptr };
            GUISystem* child{ nullptr };
            
            enum fadingsModeEnum { appearing, disappearing, offline };
            fadingsModeEnum fadingsMode{ offline };
            
            bool ignoreVariableChange{ false };
            bool regulateFadings{ false };
            float currentTime{ 0.f };
            float appearTime{ 0.f };
            float disappearTime{ 0.f };
            
            GUIConstrains constrains;
            
            virtual void Init() { }
            virtual void Update(const sf::Time& elapsedTime) { }
            virtual void Draw(sf::RenderWindow* window) { }
            virtual void Resize(unsigned int width, unsigned int height) { }
            virtual void Destroy() { }
            virtual void SetAlpha(sf::Int8 alpha) { }
            void FadingUpdate(const sf::Time& elapsedTime);
            void SetFadings(fadingsModeEnum newMode, float forTime = 0.f);
            void VariableChange(const std::wstring& varName);
            void SetGUISystem(GUISystem *system);
            GUISystem* GetChildSystem();
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
                void SetAlpha(sf::Int8 alpha) override;
            };
            
            struct DialogueRectangle : GUIObject
            {
                sf::RectangleShape shape;
                
                void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(sf::Int8 alpha) override;
            };
            
            struct NameRectangle : GUIObject
            {
                sf::RectangleShape shape;
                
                void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(sf::Int8 alpha) override;
            };
        }
        
        
        
        
        
        namespace Skins
        {
            struct Dialogue
            {
                GUISystem gui;
                GUIObjects::DialogueRectangle* dialogueRect{ nullptr };
                GUIObjects::NameRectangle* nameRect{ nullptr };
                
                unsigned int characterSize{ 30 };
                int maxAlpha = 255;
                bool forcePressInsideDialogue{ true };
                std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
                
                float appearTime{ 0.6f };
                float disappearTime{ 0.6f };
                
                int textXOffset = 40;
                int textYOffset = 10;
                int textWidth = 80;
                
                int nameBoxXOffset = 40;
                int nameBoxYOffset = 15;
                int nameTextXOffset = 45;
                int nameTextYOffset = 20;
                int nameTextWidth = 90;
            };
            struct Background
            {
                int maxAlpha = 255;
                float appearTime{ 0.6f };
                float disappearTime{ 0.6f };
                
                bool doParallax{ ns::gs::isParallaxEnabled };
                float parallaxPower{ ns::gs::defaultParallaxBackground };
            };
            struct Character
            {
                int maxAlpha = 255;
                float appearTime{ 0.6f };
                float disappearTime{ 0.6f };
                
                bool doParallax{ ns::gs::isParallaxEnabled };
                float parallaxClose{ ns::gs::defaultParallaxClose };
                float parallaxNormal{ ns::gs::defaultParallaxNormal };
                float parallaxFar{ ns::gs::defaultParallaxFar };
            };
            struct Music
            {
                float maxVolume = 100;
                float appearTime{ 1.f };
                float disappearTime{ 1.f };
            };
            struct Ambient
            {
                float maxVolume = 100;
                float appearTime{ 1.f };
                float disappearTime{ 1.f };
            };
            struct Sound
            {
                float maxVolume = 100;
                float appearTime{ 0.f };
                float disappearTime{ 0.f };
            };
        }
        struct Skin
        {
            Skins::Dialogue dialogue;
            Skins::Background background;
            Skins::Character character;
            Skins::Music music;
            Skins::Ambient ambient;
            Skins::Sound sound;
            
            void restoreToDefaults()
            {
                //TODO: Defaults
            }
        };
        
        
        
        
        
        
        
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
        
        struct NovelVariable
        {
            enum typeEnum { NotSet, Integer, String, Boolean };
            typeEnum type = NotSet;
            union
            {
                bool asBoolean;
                int asInt;
                wchar_t* asString;
            } value;
            
            ~NovelVariable();
            NovelVariable();
            NovelVariable(const int& integer);
            NovelVariable(const std::wstring& integer);
            NovelVariable(const bool& integer);
            void Set(const int& value);
            void Set(const std::wstring& value);
            void Set(const bool& value);
            
            friend std::ostream& operator<<(std::ostream& os, const NovelVariable& Var);
        };
        
        
        
        
        
        
        
        class Novel : public Component
        {
        private:
            NovelSystem layers;
            
            sf::String nsdataPath{ "" };
            sf::String folderPath{ "" };
            nss::CommandSettings command;
            std::unordered_map<std::wstring, NovelVariable*> localVariables;
            
            bool eof{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            List<NovelObject>* onHold{ nullptr };
            unsigned int onHoldSize{ 0 };
            
            std::wifstream wif;
            
        public:
            NovelLibrary library;
            Skin skin;
            
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
            NovelVariable* FindVariable(const std::wstring& variableName);
            void VariableChange(const std::wstring& name);
            void LocalVariables_Set(const std::wstring& name, std::wstring value);
            void LocalVariables_Set(const std::wstring& name, bool value);
            void LocalVariables_Set(const std::wstring& name, int value);
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
