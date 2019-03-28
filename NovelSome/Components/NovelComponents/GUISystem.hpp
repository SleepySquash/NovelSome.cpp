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

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

#include "VariableSystem.hpp"
#include "Skin.hpp"

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
        struct GUISystem : NovelObject
        {
        private:
            int lastWidth{ 0 }, lastHeight{ 0 };
            
        public:
            GUIObject* parent{ nullptr };
            unsigned int users{ 0 };
            list<GUIObject*> guiObjects;
            bool isNovel{ false };
            
            sf::Int8 lastAlpha{ 0 }, alpha{ 0 };
            
            int maxAlpha{ 255 };
            float currentTime{ 0.f }, appearTime{ 0.6f }, disappearTime{ 0.6f };;
            enum modeEnum { appearing, existing, disappearing, deprecated };
            modeEnum mode{ appearing };
            enum sendMessageBackEnum { noMessage, atAppearance, atDisappearing, atDeprecated };
            sendMessageBackEnum sendMessageBack{ atAppearance };
            
            ~GUISystem();
            GUISystem();
            void Destroy() override;
            void Update(const sf::Time& elapsedTime) override;
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void VariableResize(unsigned int width, unsigned int height);
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
            void SetAlpha(sf::Int8 alpha);
            void SetAlphaIfBigger(sf::Int8 alpha);
            void clear();
            bool LoadFromFile(const std::wstring& fileName, std::wstring guiScope = L"");
            void PrintIerarchy();
            void ResetResize();
        };
        
        
        
        struct GUIConstrainsResult
        {
            int result;
            bool dependsOnVariable, needsToBeScaled, constant;
            
            GUIConstrainsResult();
            GUIConstrainsResult(int res, bool depends = false, bool consta = false, bool needs = false);
        };
        struct GUIConstrains
        {
            //Если строка содержит какие-то переменные, не меняющиеся с Resize, то они будут расчитаны и записаны сюда, а строки будут обнулены, чтобы при других Resize не происходило перерасчёта величин.
            int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };
            int width{ 0 }, height{ 0 }, posX{ 0 }, posY{ 0 };
            
            int sleft{ 0 }, sright{ 0 }, stop{ 0 }, sbottom{ 0 };
            int swidth{ 0 }, sheight{ 0 }, sposX{ 0 }, sposY{ 0 };
            
            std::wstring leftS{ L"" }, rightS{ L"" }, topS{ L"" }, bottomS{ L"" };
            std::wstring widthS{ L"" }, heightS{ L"" }, posXS{ L"" }, posYS{ L"" };
            
            //std::wstring displayWhenVariableIsTrue{ L"" };
            bool notSet[8] = { false, false, false, false,  false, false,  false, false },
            constant[8] = { false, false, false, false,  false, false,  false, false },
            onlyNeedsToBeScaled[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnVariable[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnConstrains[8] = { false, false, false, false,  false, false,  false, false },
            isDependsOnParent[8] = { false, false, false, false,  false, false,  false, false };
            std::unordered_map<std::wstring, bool> dependOnVariables;
            
            GUIConstrainsResult Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height, std::wstring& line);
            void Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height);
        };
        struct GUIObject
        {
            GUISystem* guiSystem{ nullptr }, *child{ nullptr };
            enum fadingsModeEnum { appearing, disappearing, offline };
            fadingsModeEnum fadingsMode{ offline };
            
            unsigned char maxAlpha = 255;
            bool visible{ true }, ignoreVariableChange{ false }, regulateFadings{ false };
            float currentTime{ 0.f }, appearTime{ 0.f }, disappearTime{ 0.f };
            
            GUIConstrains constrains;
            
            virtual void Init();
            virtual void Update(const sf::Time& elapsedTime);
            virtual void Draw(sf::RenderWindow* window);
            virtual void PreCalculate(unsigned int width, unsigned int height);
            virtual void Resize(unsigned int width, unsigned int height);
            virtual void Destroy();
            virtual void SetAlpha(sf::Int8 alpha);
            virtual void SetColor(const sf::Color& fillColour);
            void FadingUpdate(const sf::Time& elapsedTime);
            void SetFadings(fadingsModeEnum newMode, float forTime = 0.f);
            void VariableChange(const std::wstring& varName);
            GUISystem* GetChildSystem();
        };
        
        
        
        namespace GUIObjects
        {
            struct Rectangle : GUIObject
            {
                sf::RectangleShape shape;
                
                void Init() override;
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
            
            struct DialogueConstrains : GUIObject { };
        }
    }
}

#endif /* GUISystem_h */
