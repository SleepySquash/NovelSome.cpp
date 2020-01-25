//
//  Button.hpp
//  NekoUI
//
//  Created by Никита Исаенко on 18.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef GUIButton_hpp
#define GUIButton_hpp

#include <SFML/Graphics.hpp>

#include "../../../Essentials/Base.hpp"
#include "../../Settings.hpp"
#include "../../Collectors/Font.hpp"
#include "../../Collectors/Image.hpp"
#include "../enums.hpp"

namespace ns
{
    namespace GUI
    {
        struct Button
        {
            int leftBound{ 0 };
            sf::FloatRect bounds{ 0, (float)gs::relativeWidth, 0, (float)gs::relativeHeight };
            
            sf::Vector2i dot;
            bool regulateBounds{ false }, wasHovered{ false };
            sf::Uint8 alpha{ 255 }, maxAlpha{ 255 };
            bool visible{ true }, active{ true }, loaded{ false };
            bool onPress{ false }, ignoreWasPressed{ false }, characterScale{ false };
            
            bool noColorChanging{ false };
            sf::Color normalColor{ sf::Color::White },
                      hoverColor{ 255, 255, 210 },
                      pressColor{ sf::Color::Yellow };
            
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
            virtual void updateColor();
        };
    }
}


#endif /* GUIButton_hpp */
