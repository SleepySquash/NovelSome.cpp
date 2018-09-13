//
//  Background.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Background::LoadImage(sf::String path)
        {
            spriteLoaded = false;
            backgroundLoaded = false;
            if (novel != nullptr)
            {
                sf::String fullPath = sf::String(resourcePath()) + novel->GetFolderPath() + path;
                
#ifdef _WIN32
                std::ifstream ifStream(fullPath.toWideString(), std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    fileInMemory.reset(new char[static_cast<unsigned int>(filesize)]);
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory.get(), filesize);
                    ifStream.close();
                    
                    backgroundLoaded = image.loadFromMemory(fileInMemory.get(), filesize);
                }
#else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath.toWideString());
                if (!(backgroundLoaded = image.loadFromFile(u8str)))
                    std::cerr << "Unable to open file: " << fullPath.toAnsiString() << std::endl;
#endif
                
                if (backgroundLoaded)
                {
                    bool textureLoaded{ false };
                    imagePath = sf::String(path);
                    if (image.getSize().x > sf::Texture::getMaximumSize() || image.getSize().y > sf::Texture::getMaximumSize())
                        textureLoaded = texture.loadFromImage(image, sf::IntRect(0, 0, image.getSize().x > sf::Texture::getMaximumSize() ? sf::Texture::getMaximumSize() : image.getSize().x, image.getSize().y > sf::Texture::getMaximumSize() ? sf::Texture::getMaximumSize() : image.getSize().y));
                    else
                        textureLoaded = texture.loadFromImage(image);
                    
                    if (textureLoaded)
                    {
                        spriteLoaded = true;
                        texture.setSmooth(true);
                        sprite.setTexture(texture);
                        
                        Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                    }
                }
                
                if (!spriteLoaded)
                {
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
                    this->GetNovelSystem()->PopComponent(this);
                }
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Background::Resize(unsigned int width, unsigned int height)
        {
            CalculateScale(width, height);
        }
        void Background::Update(const sf::Time& elapsedTime)
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
                        mode = afterAppearSwitchTo;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atAppearance)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case disappearing:
                    if (currentTime < disappearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = deprecated;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDeprecated)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
                    break;
                    
                default:
                    break;
            }
        }
        void Background::Draw(sf::RenderWindow* window)
        {
            if (spriteLoaded && visible)
                window->draw(sprite);
        }
        void Background::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void Background::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void Background::SetGroup(List<Background>* element)
        {
            this->groupPointer = element;
        }
        void Background::CalculateScale(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                float scaleFactorX, scaleFactorY, scaleFactor;
                scaleFactorX = (float)width / texture.getSize().x;
                scaleFactorY = (float)height / texture.getSize().y;
                switch (fitMode)
                {
                    case defaultFit:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        break;
                        
                    case fillCentre:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        sprite.setPosition((float)width/2 - sprite.getLocalBounds().width/2*scaleFactor - sprite.getOrigin().x*scaleFactor,
                                           (float)height/2 - sprite.getLocalBounds().height/2*scaleFactor - sprite.getOrigin().y*scaleFactor);
                        break;
                        
                    case stretch:
                        sprite.setScale(scaleFactorX, scaleFactorY);
                        break;
                        
                    default:
                        break;
                }
            }
        }
        void Background::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode && mode != deprecated)
            {
                currentTime = 0.f;
                mode = newMode;
                if (novel != nullptr)
                    if (newMode == disappearing && sendMessageBack == atDisappearing)
                        novel->UnHold(this);
            }
        }
    }
}
