//
//  GUIInterface.hpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 22/11/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef GUIInterface_hpp
#define GUIInterface_hpp

#include <SFML/Graphics.hpp>

#include "../Essentials/Base.hpp"
#include "Settings.hpp"
#include "Collectors.hpp"
#include "MessageHolder.hpp"

namespace ns
{
    enum class Halign { Left, Center, Right };
    enum class Valign { Top, Center, Bottom };
    namespace GUI
    {
        struct Button
        {
            sf::Vector2i dot;
            sf::FloatRect bounds{ 0, (float)gs::relativeWidth, 0, (float)gs::relativeHeight };
            int leftBound{ 0 }; bool regulateBounds{ false };
            int maxAlpha{ 255 };
            bool visible{ true }, active{ true };
            
            Halign halign{ Halign::Center };
            Valign valign{ Valign::Center };
            
            virtual void Draw(sf::RenderTarget* window);
            virtual void Resize(unsigned int width, unsigned int height);
            virtual bool PollEvent(sf::Event& event);
            virtual void ReceiveMessage(MessageHolder& message);
            virtual void resetScale();
            virtual void setAlpha(const sf::Uint8& alpha);
            virtual sf::Uint8 getAlpha();
            virtual void setPosition(float x, float y);
            virtual void setVisible(bool vis);
            virtual void setColor(const sf::Color& fillColour);
        };
        
        struct TextButton : Button
        {
            bool fontLoaded{ false };
            bool onPress{ false }, wasPressed{ false }, ignoreWasPressed{ false }, characterScale{ false };
            
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            TextButton();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void CorrectBoundaries();
            bool PollEvent(sf::Event& event) override;
            void resetScale() override;
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setColor(const sf::Color& fillColour) override;
        };
        
        struct SpriteButton : Button
        {
            bool spriteLoaded{ false };
            bool onPress{ false }, wasPressed{ false }, ignoreWasPressed{ false }, characterScale{ false };
            
            sf::Sprite sprite;
            std::wstring textureName{ L"" };
            float scale{ 1.f };
            
            ~SpriteButton();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void ReceiveMessage(MessageHolder& message) override;
            void resetScale() override;
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setTexture(const std::wstring& imagePath, MessageSender* sender = nullptr);
            void setTexture(sf::Texture* texture);
            void setScale(const float& scl);
            void setColor(const sf::Color& fillColour) override;
        };
        
        struct SpriteButtons : Button
        {
            bool spriteLoaded{ false }, anyButtonPressed{ false };
            unsigned long index{ 0 }, pressedIndex{ 0 };
            bool onPress{ false }, ignoreWasPressed{ false }, characterScale{ false };
            
            sf::Sprite sprite;
            std::wstring textureName{ L"" };
            float scale{ 1.f };
            
            ~SpriteButtons();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void ReceiveMessage(MessageHolder& message) override;
            void resetScale() override;
            void eventPolled(sf::Event& event);
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setTexture(const std::wstring& imagePath, MessageSender* sender = nullptr);
            void setTexture(sf::Texture* texture);
            void setScale(const float& scl);
            void setColor(const sf::Color& fillColour) override;
        };
        
        struct RectangleButton : Button
        {
            bool fontLoaded{ false };
            bool onPress{ false }, wasPressed{ false }, ignoreWasPressed{ false }, characterScale{ false };
            
            sf::RectangleShape shape;
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            RectangleButton();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void resetScale() override;
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setColor(const sf::Color& fillColour) override;
        };
        
        struct RectangleButtons : Button
        {
            bool fontLoaded{ false };
            
            bool anyButtonPressed{ false };
            unsigned long index{ 0 }, pressedIndex{ 0 };
            bool onPress{ false };
            bool ignoreWasPressed{ false };
            bool characterScale{ false };
            
            sf::RectangleShape shape;
            sf::Color shapeFillColor{ sf::Color(0,0,0, 170) };
            
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            RectangleButtons();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void resetScale() override;
            void eventPolled(sf::Event& event);
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setColor(const sf::Color& fillColour) override;
        };
    }
}

#endif /* GUIInterface_hpp */
