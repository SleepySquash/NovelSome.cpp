//
//  GUIInterface.hpp
//  AlternativeTransport-MLaToA2018
//
//  Created by Никита Исаенко on 22/11/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef GUISomeButtons_hpp
#define GUISomeButtons_hpp

#include "Button.hpp"

namespace ns
{
    namespace GUI
    {
        struct TextButton : Button
        {
            bool wasPressed{ false }, correctBoundaries{ false };
                       
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
        
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            
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
            void updateColor() override;
        };
    
        struct TextButtons : Button
        {
            unsigned long index{ 0 }, pressedIndex{ 0 };
            bool anyButtonPressed{ false }, anyButtonHovered{ false };
            
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            
            TextButtons();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void eventPolled(sf::Event& event);
            void setAlpha(const sf::Uint8& alpha) override;
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setColor(const sf::Color& fillColour) override;
            void updateColor() override;
        };
        
        struct SpriteButton : Button
        {
            bool wasPressed{ false }, invertXAxes{ false };
            
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
            unsigned long index{ 0 }, pressedIndex{ 0 };
            bool anyButtonPressed{ false };
            
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
            bool wasPressed{ false };
            
            sf::RectangleShape shape;
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            sf::Uint8 ralpha{ 255 };
            
            RectangleButton();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void resetScale() override;
            void setAlpha(const sf::Uint8& alpha) override;
            void setAlphaEx(const sf::Uint8 &alpha, const sf::Uint8 &ralpha);
            sf::Uint8 getAlpha() override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void setColor(const sf::Color& fillColour) override;
            void updateColor() override;
        };
    
        struct RectangleButtons : Button
        {
            bool anyButtonPressed{ false }, drawShape{ true };
            unsigned long index{ 0 }, pressedIndex{ 0 };
            
            sf::RectangleShape shape;
            bool drawOutline{ true };
            sf::Color onormalColor{ sf::Color::Black },
                      ohoverColor{ 140, 140, 140 },
                      opressColor{ sf::Color::Black };
            sf::Color snormalColor{ sf::Color::Black },
                      shoverColor{ 225, 225, 225 },
                      spressColor{ 200, 200, 200 };
            bool drawOutlineShape{ true };
            sf::Color sonormalColor{ sf::Color::White },
                      sohoverColor{ 40, 40, 40 },
                      sopressColor{ sf::Color::White };
            sf::Uint8 ralpha{ 255 };
            
            sf::Text text;
            sf::String string;
            unsigned int characterSize{ 20 };
            float thickness{ 1.f };
            
            RectangleButtons();
            void Draw(sf::RenderTarget* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            bool PollEvent(sf::Event& event) override;
            void eventPolled(sf::Event& event);
            void setAlpha(const sf::Uint8& alpha) override;
            void setPosition(float x, float y) override;
            void setSize(const sf::Vector2f& vec);
            void setFont(const std::wstring& fontname);
            void setString(const std::wstring& string);
            void setCharacterSize(const unsigned int size);
            void updateColor() override;
        };
    }
}

#endif /* GUISomeButtons_hpp */
