//
//  GUISystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef GUISystem_hpp
#define GUISystem_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/List.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/NovelSystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "Abstract/Savable.hpp"
#include "Abstract/VariableSystem.hpp"
#include "Abstract/Skin.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;
using std::vector;

namespace ns
{
    namespace NovelComponents
    {
        struct GUIObject;
        struct GUISystem : NovelObject, Savable
        {
        private:
            int lastWidth{ 0 }, lastHeight{ 0 };
            
        public:
            GUIObject* parent{ nullptr };
            unsigned int users{ 0 };
            list<GUIObject*> guiObjects;
            bool isNovel{ false };
            
            std::wstring scope{ L"" }, fileName{ L"" }, trueFileName{ L"" }, folderPath;
            sf::RectangleShape constrainsShape; bool drawConstrains{ gs::drawGUIBoundaries };
            
            sf::Uint8 lastAlpha{ 0 }, alpha{ 0 };
            
            sf::Uint8 maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };;
            enum modeEnum { appearing, existing, disappearing, deprecated };
            modeEnum mode{ appearing };
            enum sendMessageBackEnum { noMessage, atAppearance, atDisappearing, atDeprecated };
            sendMessageBackEnum sendMessageBack{ atAppearance };
            
            ~GUISystem();
            GUISystem();
            void Destroy() override;
            void PollEvent(sf::Event& event) override;
            void Update(const sf::Time& elapsedTime) override;
            void Resize(const unsigned int& width, const unsigned int& height) override;
            void Draw(sf::RenderWindow* window) override;
            void ReceiveMessage(MessageHolder& message) override;
            void VariableResize(const unsigned int& width, const unsigned int& height);
            template<typename T, typename ...Args> T* AddComponent(Args... args)
            {
                T* component = new T(args...);
                guiObjects.push_back(component);
                
                component->guiSystem = this;
                component->Init();
                component->Resize(gs::width, gs::height);
                
                return component;
            }
            void VariableChange(const std::wstring& varName);
            void SetAlpha(sf::Uint8 alpha);
            void SetAlphaIfBigger(sf::Uint8 alpha);
            void clear();
            bool LoadFromFile(const std::wstring& fileName, std::wstring guiScope = L"");
            void PrintIerarchy();
            void ResetResize();
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
        
        
        
        struct GUIConstrainsResult
        {
            int result;
            bool dependsOnVariable, needsToBeScaled, constant, scaledByGlobalFactor;
            
            GUIConstrainsResult();
            GUIConstrainsResult(int res, bool depends = false, bool consta = false, bool needs = false, bool globfac = false);
        };
        //struct ScreenBoundsRect { float right{ 0 }; float bottom{ 0 }; };
        struct GUIConstrains
        {
            //Если строка содержит какие-то переменные, не меняющиеся с Resize, то они будут расчитаны и записаны сюда, а строки будут обнулены, чтобы при других Resize не происходило перерасчёта величин.
            sf::FloatRect globalBounds, localBounds;
            
            int sleft{ 0 }, sright{ 0 }, stop{ 0 }, sbottom{ 0 };
            int swidth{ 0 }, sheight{ 0 }, sposX{ 0 }, sposY{ 0 };
            
            std::wstring leftS{ L"" }, rightS{ L"" }, topS{ L"" }, bottomS{ L"" };
            std::wstring widthS{ L"" }, heightS{ L"" }, posXS{ L"" }, posYS{ L"" };
            
            //std::wstring displayWhenVariableIsTrue{ L"" };
            bool notSet[8] = { false, false, false, false,  false, false,  false, false },
            constant[8] = { false, false, false, false,  false, false,  false, false },
            onlyNeedsToBeScaled[8] = { false, false, false, false,  false, false,  false, false },
            scaledByGlobalFactor[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnVariable[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnConstrains[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnParent[8] = { false, false, false, false,  false, false,  false, false };
            std::unordered_map<std::wstring, bool> dependOnVariables;
            
            GUIConstrainsResult Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height, std::wstring& line, float scalingFactor = 1.f);
            void Recalculcalation(GUIObject& guiObject, unsigned int width, unsigned int height);
            float RecalculateCompact(GUIObject& guiObject, GUIConstrainsResult& res, const unsigned int& width, const unsigned int& height, const unsigned int& i, std::wstring& analyze, int& sresult);
        };
        struct GUIObject
        {
            GUISystem* guiSystem{ nullptr }, *child{ nullptr };
            enum fadingsModeEnum { appearing, disappearing, offline };
            fadingsModeEnum fadingsMode{ offline };
            
            sf::Uint8 maxAlpha{ 255 }, alpha{ 0 };
            bool visible{ true }, ignoreVariableChange{ false }, regulateFadings{ false };
            bool scaleHorizontal{ true }, scaleVertical{ true };
            float currentTime{ 0.f }, appearTime{ 0.f }, disappearTime{ 0.f };
            
            GUIConstrains constrains;
            
            virtual void Init();
            virtual void PollEvent(sf::Event& event);
            virtual void Update(const sf::Time& elapsedTime);
            virtual void Resize(const unsigned int& width, const unsigned int& height);
            virtual void Draw(sf::RenderWindow* window);
            virtual void PreCalculate(const unsigned int& width, const unsigned int& height);
            virtual void PreCalculatedSize(const unsigned int& width, const unsigned int& height);
            virtual void Destroy();
            virtual void ReceiveMessage(MessageHolder& message);
            virtual void SetAlpha(sf::Uint8 alpha);
            virtual void SetColor(const sf::Color& fillColour);
            void FadingUpdate(const sf::Time& elapsedTime);
            void SetFadings(fadingsModeEnum newMode, float forTime = 0.f);
            void VariableChange(const std::wstring& varName);
            GUISystem* GetChildSystem();
            NovelSystem* FindNovelSystem();
            NovelObject* FindNovelObject();
        };
        
        
        
        namespace GUIObjects
        {
            struct Rectangle : GUIObject
            {
                sf::RectangleShape shape;
                
                void Init() override;
                void Draw(sf::RenderWindow* window) override;
                void Resize(const unsigned int& width, const unsigned int& height) override;
                void SetAlpha(sf::Uint8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
            };
            
            struct Text : GUIObject
            {
                sf::Text text; bool allowWrap{ false };
                unsigned int characterSize{ 16 };
                float thickness{ 0.f };
                std::wstring textString{ L"Text" };
                sf::FloatRect bounds{ 0, (float)gs::relativeWidth, 0, (float)gs::relativeHeight };
                
                Halign halign{ Halign::Left };
                Valign valign{ Valign::Top };
                
                std::wstring fontName{ L"" };
                bool fontLoaded{ false };
                
                void Init() override;
                void Draw(sf::RenderWindow* window) override;
                void PreCalculate(const unsigned int& width, const unsigned int& height) override;
                void PreCalculatedSize(const unsigned int& width, const unsigned int& height) override;
                void Resize(const unsigned int& width, const unsigned int& height) override;
                void SetAlpha(sf::Uint8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
                void SetString(const std::wstring& wstr);
                void SetFont(const std::wstring& font);
                void SetOutlineThickness(const float& thickness);
                void SetCharacterSize(const unsigned int& characterSize);
                void setPosition(const float& x, const float& y);
                void CorrectBoundaries();
            };
            
            struct Image : GUIObject
            {
                sf::Sprite sprite;
                bool spriteLoaded{ false };
                std::wstring imagePath{ L"" };
                
                int defaultPositionX{ 0 }, defaultPositionY{ 0 };
                float scaleX{ 1.f }, scaleY{ 1.f };
                enum { noFit, defaultFit, fillCentre, fill, stretch, keepAspect, keepAspectSc, defaultFill } fitMode = defaultFit;
                bool doParallax{ /*gs::isParallaxEnabled*/ false };
                float parallaxPower { gs::defaultParallaxBackground };
                
                void Destroy() override;
                void PollEvent(sf::Event& event) override;
                void PreCalculate(const unsigned int& width, const unsigned int& height) override;
                void PreCalculatedSize(const unsigned int& width, const unsigned int& height) override;
                void Resize(const unsigned int& width, const unsigned int& height) override;
                void Draw(sf::RenderWindow* window) override;
                void SetAlpha(sf::Uint8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
                void LoadImage(const std::wstring& path, MessageSender* sender = nullptr);
                void ReceiveMessage(MessageHolder& message) override;
                void CalculateScale(unsigned int width, unsigned int height);
                void CalculateParallax(int mouseX, int mouseY);
            };
            
            struct Button : GUIObject
            {
                enum typeEnum { Nil, Text, Sprite, Rect };
                typeEnum type{ Nil };
                GUI::TextButton* text{ nullptr };
                GUI::SpriteButton* sprite{ nullptr };
                GUI::RectangleButton* rect{ nullptr };
                
                GUI::Button* button{ nullptr };
                std::list<std::wstring> actions;
                bool eventPolling{ false };
                
                Button(const typeEnum& type);
                void Destroy() override;
                void PollEvent(sf::Event& event) override;
                void Update(const sf::Time& elapsedTime) override;
                void PreCalculate(const unsigned int& width, const unsigned int& height) override;
                void PreCalculatedSize(const unsigned int& width, const unsigned int& height) override;
                void Resize(const unsigned int& width, const unsigned int& height) override;
                void Draw(sf::RenderWindow* window) override;
                void ReceiveMessage(MessageHolder& message) override;
                void SetAlpha(sf::Uint8 alpha) override;
                void SetColor(const sf::Color& fillColour) override;
            };
            
            struct DialogueConstrains : GUIObject { };
        }
        
        struct GUIScopeStruct
        {
            union { GUIObject* obj; GUIObjects::Rectangle* rect; GUIObjects::Text* text; GUIObjects::DialogueConstrains* diag; GUIObjects::Image* img; GUIObjects::Button* but; } object;
            enum typeEnum { Object, Rectangle, Text, DialogueConstrains, Image, Button } type = Object;
            GUIScopeStruct(GUIObject* obj = nullptr, const typeEnum& type = Object);
            GUIObject& operator*();
            GUIObject* operator->();
        };
    }
}

#endif /* GUISystem_h */
