//
//  GUI.cpp
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
        GUISystem::~GUISystem()
        {
            Clear();
        }
        void GUISystem::Update(const sf::Time& elapsedTime)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    list->data->Update(elapsedTime);
                }
        }
        void GUISystem::Draw(sf::RenderWindow* window)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    list->data->Draw(window);
                }
        }
        void GUISystem::Resize(unsigned int width, unsigned int height)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    list->data->Resize(width, height);
                }
        }
        void GUISystem::SetAlpha(int alpha)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    list->data->SetAlpha(alpha);
                }
        }
        void GUISystem::Clear()
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    
                    list->data->Destroy();
                    delete list->data;
                    delete list;
                }
            guiObjects = nullptr;
        }
        
        
        
        void GUIObject::SetGUISystem(GUISystem *system)
        {
            this->system = system;
        }
        
        
        
        namespace GUIObjects
        {
            void Rectangle::Init()
            {
                shape.setPosition(100, 100);
                shape.setFillColor(sf::Color::Black);
            }
            void Rectangle::Update(const sf::Time& elapsedTime)
            {
                
            }
            void Rectangle::Draw(sf::RenderWindow* window)
            {
                window->draw(shape);
            }
            void Rectangle::Resize(unsigned int width, unsigned int height)
            {
                shape.setSize({(float)width/10, (float)height/10});
            }
            void Rectangle::SetAlpha(int alpha)
            {
                
            }
        }
    }
}
