//
//  EntitySystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef EntitySystem_hpp
#define EntitySystem_hpp

#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "List.hpp"
#include "StaticMethods.hpp"

namespace ns
{
    class Component;
    class Entity;
    class EntitySystem;
    
    class Component
    {
    private:
        Entity* entity{ nullptr };
        
    public:
        int priority{ 0 };
        
        virtual ~Component();
        virtual void Init();
        virtual void Update(const sf::Time&);
        virtual void Draw(sf::RenderWindow*);
        virtual void Resize(unsigned int width, unsigned int height);
        virtual void PollEvent(sf::Event& event);
        virtual void Destroy();
        void SetPriority(int priority);
        void SetEntity(Entity* entity);
        Entity* GetEntity();
    };
    
    class Entity
    {
    private:
        EntitySystem* system = nullptr;
        List<Component>* components = nullptr;
        List<Component>* lastComponent = nullptr;
        
    public:
        
        Entity();
        Entity(EntitySystem* system);
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        void PopComponent(Component* component);
        void Destroy();
        void SetEntitySystem(EntitySystem* system);
        
        //for some reason putting this code in .cpp crashes the linker, so it has to remain in definition's .hpp file
        template<typename T, typename ...Args> T* AddComponent(Args... args)
        {
            T* component = new T(args...);
            List<Component>* element = (List<Component>*)malloc(sizeof(List<Component>));
            element->data = component;
            element->next = nullptr;
            element->prev = lastComponent;
            
            if (lastComponent == nullptr)
                components = element;
            else
                lastComponent->next = element;
            lastComponent = element;
            
            component->SetEntity(this);
            component->Init();
            component->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
            
            return component;
        }
        template<typename T, typename ...Args> T* PrioritizeComponent(int priority, Args... args)
        {
            T* component = new T(args...);
            List<Component>* element = (List<Component>*)malloc(sizeof(List<Component>));
            element->data = component;
            element->data->priority = priority;
            element->next = nullptr;
            element->prev = nullptr;
            
            List<Component>* temp = components;
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
                        components->prev = element;
                        element->next = components;
                        components = element;
                    }
                    Done = true;
                }
                temp = temp->next;
            }
            if (!Done && temp == nullptr)
            {
                element->prev = lastComponent;
                if (lastComponent == nullptr)
                    components = element;
                else
                    lastComponent->next = element;
                lastComponent = element;
            }
            
            component->SetEntity(this);
            component->Init();
            component->Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
            
            return component;
        }
        
        int GetComponentsCount();
        List<Component>* GetComponentsListHead();
    };
    
    class EntitySystem
    {
    private:
        List<Entity>* entities = nullptr;
        List<Entity>* lastEntity = nullptr;
        
    public:
        
        EntitySystem();
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        Entity* AddEntity();
        void PopEntity(Entity* entity);
        void Destroy();
        int GetEntityCount();
        List<Entity>* GetEntitiesListHead();
        
    };
}

#endif /* EntitySystem_hpp */
