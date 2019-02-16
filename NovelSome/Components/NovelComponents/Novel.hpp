//
//  NovelComponents.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef NovelComponents_hpp
#define NovelComponents_hpp

#include <iostream>
#include <list>
using std::list;
#include <vector>
using std::vector;

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
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

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
            GUI::TextButton menuBackButton;
            
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
        public:
            Novel* novel{ nullptr };
            enum modeEnum{ appearing, waiting, appeared, disappearing };
            modeEnum mode{ waiting };
            
            int maxAlpha{ 255 };
            float appearTime{ 0.15f };
            float disappearTime{ 0.15f };
            
            bool countdownLastTouchedMoment{ false };
            float lastTouchedMoment{ 0.f };
            
            GamePause();
            ~GamePause();
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow *window) override;
            void Resize(unsigned int width, unsigned int height) override;
        };
        
        
        
        
        
        
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
            sf::Sprite sprite;
            Novel* novel{ nullptr };
            list<Background*>::iterator groupPointer;
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
            
            void LoadImage(const sf::String&);
            void Resize(unsigned int width, unsigned int height) override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void SetNovel(Novel* novel);
            void SetGroup(const list<Background*>::iterator& element);
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
            std::wstring printingString{ L"" };
            std::wstring currentString{ L"" };
            
            sf::Text charText;
            sf::String charString{ "" };
        private:
            GUISystem* guiSystem{ nullptr };
            Skins::Dialogue* skin{ nullptr };
            CharacterData* character{ nullptr };
            Novel* novel{ nullptr };
            list<Dialogue*>::iterator groupPointer;
            
            bool drawCharacterName{ false };
            bool fontLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
            bool visible{ true };
            
        public:
            enum modeEnum {appearing, waiting, waitingForTime, waitingForInput, waitingForChoose, disappearing, deprecated};
            modeEnum mode{ appearing };
            
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            modeEnum afterAppearSwitchTo{ waitingForInput };
            
            unsigned int textAppearPos{ 0 };
            unsigned int textAppearMax{ 0 };
            unsigned int textAppearI{ 0 };
            float characterInSecond{ 0.04f };
            float elapsedCharacterSum{ 0 };
            enum class textAppearModeEnum {fading, printing};
            textAppearModeEnum textAppearMode{ textAppearModeEnum::printing };
            
            unsigned int workingArea_l{ 0 };
            unsigned int workingArea_r{ 0 };
            unsigned int workingArea_t{ 0 };
            unsigned int workingArea_b{ 0 };
            
            int maxAlpha = 255;
            bool forcePressInsideDialogue{ true };
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 30 };
            float outlineThickness{ 0.f };
            
            wchar_t leftSpeechAddition{ 0 };
            wchar_t rightSpeechAddition{ 0 };
            int afterRedLineShift{ 0 };
            
            float waitingTime{ 2.f };
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            Dialogue(Novel* novel);
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetGroup(const list<Dialogue*>::iterator& element);
            void SetCharacter(CharacterData* character);
            void SetCharacterName(const sf::String& characterName);
            void SetDialogue(const sf::String& dialogue);
            void SetStateMode(modeEnum newMode);
            void SetGUISystem(GUISystem* system);
        };
        
        
        
        
        
        class Choose : public NovelObject
        {
        public:
            sf::Text text;
            sf::String textString{ "" };
            
        private:
            GUISystem* guiSystem{ nullptr };
            Skins::Dialogue* skin{ nullptr };
            Novel* novel{ nullptr };
            list<Choose*>::iterator groupPointer;
            
            GUI::TextButton button;
            int startingYY{ 0 };
            
            vector<std::wstring> actions;
            vector<std::wstring> choices;
            vector<int> choiceStart;
            
            bool fontLoaded{ false };
            sf::Int8 alpha{ 0 };
            float currentTime{ 0.f };
            
            bool visible{ true };
            
        public:
            enum modeEnum {appearing, waitingForInput, disappearing, deprecated};
        private:
            modeEnum mode{ appearing };
            
        public:
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ atDisappearing };
            
            int maxAlpha = 255;
            std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
            unsigned int characterSize{ 42 };
            
            float appearTime{ 0.6f };
            float disappearTime{ 0.6f };
            
            Choose(Novel* novel);
            void Update(const sf::Time& elapsedTime) override;
            void PollEvent(sf::Event& event) override;
            void Draw(sf::RenderWindow* window) override;
            void Destroy() override;
            void Resize(unsigned int width, unsigned int height) override;
            void SetGroup(const list<Choose*>::iterator& element);
            void SetStateMode(modeEnum newMode);
            void SetGUISystem(GUISystem* system);
            void AddChoice(const std::wstring& line);
            void AddAction(const std::wstring& line);
            void InitChoose();
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
            sf::Sprite sprite;
            sf::String imagePath{ L"" };
            
            Novel* novel{ nullptr };
            list<Character*>::iterator groupPointer;
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
            void SetGroup(const list<Character*>::iterator& element);
            void SetStateMode(modeEnum newMode);
            void SetCharacterData(CharacterData* characterData);
            CharacterData* GetCharacterData();
            void SetPosition(positionEnum position, float x, float y);
        };
        
        
        
        
        
        
        class SoundPlayer : public NovelObject
        {
        private:
            std::unique_ptr<char[]> fileInMemory;
            
            sf::Sound sound;
            Novel* novel;
            list<SoundPlayer*>::iterator groupPointer;
            
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
            void SetGroup(const list<SoundPlayer*>::iterator& element);
            void LoadFromFile(const sf::String& fileName);
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
            list<MusicPlayer*>::iterator groupPointer;
            
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
            void SetGroup(const list<MusicPlayer*>::iterator& element);
            void LoadFromFile(const sf::String& fileName);
            void SetStateMode(modeEnum newMode);
            void SetLoop(bool setLoop);
            void SetPlayingOffset(sf::Time timeOffset);
        };
        
        
        
        
        
        
        struct Skin;
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
            list<GUIObject*> guiObjects;
            
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
                guiObjects.push_back(component);
                
                component->SetGUISystem(this);
                component->Init();
                component->Resize(gs::width, gs::height);
                
                return component;
            }
            void VariableChange(const std::wstring& varName);
            void SetAlpha(sf::Int8 alpha);
            void SetAlphaIfBigger(sf::Int8 alpha);
            void Clear();
            void SetNovel(Novel* novel);
            void SetParent(GUIObject* guiSystem);
            bool LoadFromFile(const std::wstring& fileName, Skin* skin = nullptr, std::wstring guiScope = L"");
            void PrintIerarchy();
            void ResetResize();
        };
        
        
        
        struct GUIConstrainsResult
        {
            int result;
            bool dependsOnVariable;
            bool needsToBeScaled;
            bool constant;
            
            GUIConstrainsResult() { }
            GUIConstrainsResult(int res, bool depends = false, bool consta = false, bool needs = false) : result(res), dependsOnVariable(depends), constant(consta), needsToBeScaled(needs) { }
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
            
            
            int sleft{ 0 };
            int sright{ 0 };
            int stop{ 0 };
            int sbottom{ 0 };
            
            int swidth{ 0 };
            int sheight{ 0 };
            
            int sposX{ 0 };
            int sposY{ 0 };
            
            
            std::wstring leftS{ L"" };
            std::wstring rightS{ L"" };
            std::wstring topS{ L"" };
            std::wstring bottomS{ L"" };
            
            std::wstring widthS{ L"" };
            std::wstring heightS{ L"" };
            
            std::wstring posXS{ L"" };
            std::wstring posYS{ L"" };
            
            
            //std::wstring displayWhenVariableIsTrue{ L"" };
            bool notSet[8] = { false, false, false, false,  false, false,  false, false };
            bool constant[8] = { false, false, false, false,  false, false,  false, false };
            bool onlyNeedsToBeScaled[8] = { false, false, false, false,  false, false,  false, false };
            bool isDependsOnVariable[8] = { false, false, false, false,  false, false,  false, false };
            bool isDependsOnConstrains[8] = { false, false, false, false,  false, false,  false, false };
            bool isDependsOnParent[8] = { false, false, false, false,  false, false,  false, false };
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
            virtual void PreCalculate(unsigned int width, unsigned int height) { }
            virtual void Resize(unsigned int width, unsigned int height) { }
            virtual void Destroy() { }
            virtual void SetAlpha(sf::Int8 alpha) { }
            virtual void SetColor(const sf::Color& fillColour) { }
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
                void SetColor(const sf::Color& fillColour) override;
            };
            
            struct Text : GUIObject
            {
                sf::Text text;
                unsigned int characterSize{ 16 };
                float thickness{ 0.f };
                std::wstring textString{ L"Text" };
                
                std::wstring fontName{ L"" };
                bool fontLoaded{ false };
                
                void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void PreCalculate(unsigned int width, unsigned int height) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(sf::Int8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
                void SetString(const std::wstring& wstr);
                void SetFont(const std::wstring& font);
                void SetOutlineThickness(const float& thickness);
                void SetCharacterSize(const unsigned int& characterSize);
            };
            
            struct Image : GUIObject
            {
                sf::Texture texture;
                sf::Sprite sprite;
                
                bool spriteLoaded{ false };
                std::wstring imagePath{ L"" };
                
                void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(sf::Int8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
                void LoadImage(const std::wstring& path);
            };
            
            struct Button : GUIObject
            {
                sf::Text text;
                std::wstring title{ L"" };
                unsigned int characterSize{ 16 };
                float thickness{ 0.f };
                bool fontLoaded{ false };
                
                sf::Texture textures[3];
                sf::Sprite sprites[3];
                bool spriteLoaded[3] = { };
                std::wstring imagePath[3] = { L"", L"", L"" };
                
                /*void Init() override;
                void Update(const sf::Time& elapsedTime) override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(unsigned int width, unsigned int height) override;
                void SetAlpha(sf::Int8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
                void LoadImage(const std::wstring& path, int num);*/
            };
            
            struct DialogueConstrains : GUIObject
            {
                
            };
        }
        
        
        
        
        
        namespace Skins
        {
            struct Dialogue
            {
                GUISystem gui;
                
                GUIObject* dialogueRect{ nullptr };
                GUIObject* textConstrains{ nullptr };
                
                GUIObject* nameRect{ nullptr };
                GUIObject* nameConstrains{ nullptr };
                
                unsigned int characterSize{ 30 };
                int maxAlpha = 255;
                bool forcePressInsideDialogue{ true };
                float characterInSecond{ 0.04f };
                unsigned int textAppearMode{ 1 };
                std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
                
                wchar_t leftSpeechAddition{ 0 };
                wchar_t rightSpeechAddition{ 0 };
                int afterRedLineShift{ 0 };
                
                sf::Color fillColor{ sf::Color::White };
                sf::Color outlineColor{ sf::Color::Black };
                float outlineThickness{ 0.f };
                
                float appearTime{ 0.6f };
                float disappearTime{ 0.6f };
            };
            struct Choose
            {
                GUISystem guiChoose;
                GUIObject* chooseRect{ nullptr };
                GUIObject* chooseConstrains{ nullptr };
                
                unsigned int characterSize{ 36 };
                int maxAlpha = 255;
                std::wstring fontName{ L"NotoSansCJK-Regular.ttc" };
                
                float appearTime{ 0.6f };
                float disappearTime{ 0.6f };
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
            std::wstring defaultFontName{ L"NotoSansCJK-Regular.ttc" };
            
            GUISystem gamePauseGUI;
            Skins::Dialogue dialogue;
            Skins::Choose choose;
            Skins::Background background;
            Skins::Character character;
            Skins::Music music;
            Skins::Ambient ambient;
            Skins::Sound sound;
            
            void RestoreToDefaults();
            void LoadFromFile(const std::wstring& fileName);
        };
        
        
        
        
        
        
        
        struct NovelLibrary
        {
            Novel* novel{ nullptr };
            list<CharacterData*> charData;
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
            
            std::wstring nsdataPath{ L"" };
            std::wstring folderPath{ L"" };
            nss::CommandSettings command;
            std::unordered_map<std::wstring, NovelVariable*> localVariables;
            
            bool eof{ false };
            bool fileOpened{ false };
            std::wstring line;
            
            list<NovelObject*> onHold;
            std::wifstream wif;
            
        public:
            GamePause gamePause;
            NovelLibrary library;
            Skin skin;
            
            list<std::wstring> lines;
            int preloadLinesAmount{ 12 };
            
            list<Background*> backgroundGroup;
            list<Character*> characterGroup;
            list<Dialogue*> dialogueGroup;
            list<Choose*> chooseGroup;
            list<SoundPlayer*> soundGroup;
            list<MusicPlayer*> musicGroup;
            list<GUISystem*> GUIGroup;
            
            Novel(const std::wstring& path);
            ~Novel();
            void Update(const sf::Time& elapsedTime) override;
            void ResourcesPreloading(list<std::wstring>& lines, std::wstring& line);
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void OnHold(NovelObject* component);
            void UnHold(NovelObject* component);
            std::wstring GetFolderPath();
            NovelVariable* FindVariable(const std::wstring& variableName);
            void VariableChange(const std::wstring& name);
            void LocalVariables_Set(const std::wstring& name, std::wstring value);
            void LocalVariables_Set(const std::wstring& name, bool value);
            void LocalVariables_Set(const std::wstring& name, int value);
            void RemoveFromGroup(const list<Background*>::iterator& groupPointer);
            void RemoveFromGroup(const list<Dialogue*>::iterator& groupPointer);
            void RemoveFromGroup(const list<Choose*>::iterator& groupPointer);
            void RemoveFromGroup(const list<Character*>::iterator& groupPointer);
            void RemoveFromGroup(const list<SoundPlayer*>::iterator& groupPointer);
            void RemoveFromGroup(const list<MusicPlayer*>::iterator& groupPointer);
            void RemoveFromGroup(const list<GUISystem*>::iterator& groupPointer);
            template<typename T> void FreeGroup(list<T*> elements) { elements.clear(); }
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
