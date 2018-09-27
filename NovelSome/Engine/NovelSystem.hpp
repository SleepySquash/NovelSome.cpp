//
//  NovelSystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef NovelSystem_hpp
#define NovelSystem_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "List.hpp"
#include "StaticMethods.hpp"

namespace ns
{
    class NovelObject;
    class NovelSystem;
    
    class NovelObject
    {
    private:
        NovelSystem* novelSystem{ nullptr };
        
    public:
        int priority{ 0 };
        
        virtual ~NovelObject();
        virtual void Init();
        virtual void Update(const sf::Time&);
        virtual void Draw(sf::RenderWindow*);
        virtual void Resize(unsigned int width, unsigned int height);
        virtual void PollEvent(sf::Event& event);
        virtual void Destroy();
        void SetPriority(int priority);
        void ChangePriority(int priority);
        void SetNovelSystem(NovelSystem* novelSystem);
        NovelSystem* GetNovelSystem();
    };
    
    class NovelSystem
    {
    private:
        List<NovelObject>* objects = nullptr;
        List<NovelObject>* lastObject = nullptr;
        
    public:
        
        NovelSystem();
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        void PopComponent(NovelObject* component);
        void Destroy();
        void ChangePriorityOf(NovelObject* component, int priority);
        
        template<typename T, typename ...Args> T* PrioritizeComponent(int priority, Args... args)
        {
            T* component = new T(args...);
            List<NovelObject>* element = new List<NovelObject>;
            element->data = component;
            element->data->priority = priority;
            element->next = nullptr;
            element->prev = nullptr;
            
            List<NovelObject>* temp = objects;
            bool Done{ false };
            while (!Done && temp != nullptr)
            {
                if (temp->data->priority > priority)
                {
                    if (temp->prev != nullptr)
                    {
                        temp->prev->next = element;
                        element->prev = temp->prev;
                        element->next = temp;
                        temp->prev = element;
                    }
                    else
                    {
                        objects->prev = element;
                        element->next = objects;
                        objects = element;
                    }
                    Done = true;
                }
                temp = temp->next;
            }
            if (!Done && temp == nullptr)
            {
                element->prev = lastObject;
                if (lastObject == nullptr)
                    objects = element;
                else
                    lastObject->next = element;
                lastObject = element;
            }
            
            component->SetNovelSystem(this);
            component->Init();
            component->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
            
            return component;
        }
    };
}

#endif /* NovelSystem_hpp */
