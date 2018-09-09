//
//  EntitySystem.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "EntitySystem.hpp"

namespace ns
{
    Component::~Component() { }
    void Component::Init() { }
    void Component::Update(const sf::Time&) { }
    void Component::Draw(sf::RenderWindow*) { }
    void Component::Resize(unsigned int, unsigned int) { }
    void Component::PollEvent(sf::Event&) { }
    void Component::Destroy() { }
    void Component::SetEntity(Entity* entity)
    {
        this->entity = entity;
    }
    Entity* Component::GetEntity()
    {
        return entity;
    }
    void Component::SetPriority(int priority)
    {
        this->priority = priority;
    }
    
    
    Entity::Entity()
    {
        system = nullptr;
        components = nullptr;
        lastComponent = nullptr;
    }

    Entity::Entity(EntitySystem* system)
    {
        this->system = system;
        components = nullptr;
        lastComponent = nullptr;
    }

    void Entity::Update(const sf::Time& elapsedTime)
    {
        List<Component>* next = nullptr;
        if (components != nullptr)
            for (auto* list = components; list != nullptr; list = next)
            {
                next = list->next; //cuz current element could be destroyed it that exact Update() method
                list->data->Update(elapsedTime);
            }
    }

    void Entity::Draw(sf::RenderWindow* window)
    {
        List<Component>* next = nullptr;
        if (components != nullptr)
            for (auto* list = components; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Draw(window);
            }
    }
    
    void Entity::Resize(unsigned int width, unsigned int height)
    {
        List<Component>* next = nullptr;
        if (components != nullptr)
            for (auto* list = components; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Resize(width, height);
            }
    }
    
    void Entity::PollEvent(sf::Event& event)
    {
        List<Component>* next = nullptr;
        if (lastComponent != nullptr)
            for (auto* list = lastComponent; list != nullptr; list = next)
            {
                next = list->prev;
                list->data->PollEvent(event);
            }
    }
    
    void Entity::PopComponent(Component* component)
    {
        List<Component>* current = components;
        List<Component>* before = components;
        
        while (current != nullptr)
        {
            if (current->data != component)
            {
                before = current;
                current = current->next;
            }
            else
                break;
        }
        
        if (current != nullptr)
        {
            current->data->Destroy();
            
            if (before == current)
            {
                components = current->next;
                if (components != nullptr)
                    components->prev = nullptr;
                if (current == lastComponent)
                    lastComponent = nullptr;
            }
            else
            {
                before->next = current->next;
                if (current->next != nullptr)
                    current->next->prev = before;
                if (current == lastComponent)
                    lastComponent = before;
            }
            
            delete current->data;
            delete current;
        }
    }
    
    void Entity::Destroy()
    {
        List<Component>* next = nullptr;
        if (components != nullptr)
            for (auto* list = components; list != nullptr; list = next)
            {
                next = list->next;
                
                list->data->Destroy();
                delete list->data;
                delete list;
            }
    }

    void Entity::SetEntitySystem(EntitySystem* system)
    {
        this->system = system;
    }

    int Entity::GetComponentsCount()
    {
        List<Component>* list = components;
        int count = 0;
        
        while (list != nullptr)
        {
            list = list->next;
            count++;
        }
        
        return count;
    }

    List<Component>* Entity::GetComponentsListHead()
    {
        return components;
    }

    
    
    EntitySystem::EntitySystem()
    {
        entities = nullptr;
        lastEntity = nullptr;
    }

    void EntitySystem::Update(const sf::Time& elapsedTime)
    {
        List<Entity>* next = nullptr;
        if (entities != nullptr)
            for (auto* list = entities; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Update(elapsedTime);
            }
    }

    void EntitySystem::Draw(sf::RenderWindow* window)
    {
        List<Entity>* next = nullptr;
        if (entities != nullptr)
            for (auto* list = entities; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Draw(window);
            }
    }
    
    void EntitySystem::Resize(unsigned int width, unsigned int height)
    {
        List<Entity>* next = nullptr;
        if (entities != nullptr)
            for (auto* list = entities; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Resize(width, height);
            }
    }
    
    void EntitySystem::PollEvent(sf::Event& event)
    {
        List<Entity>* next = nullptr;
        if (lastEntity != nullptr)
            for (auto* list = lastEntity; list != nullptr; list = next)
            {
                next = list->prev;
                list->data->PollEvent(event);
            }
    }

    Entity* EntitySystem::AddEntity()
    {
        Entity* entity = new Entity(this); //Using "new" instead of malloc just to call the constractor (malloc doesn't do that, but "new" does)
        List<Entity>* element = (List<Entity>*)malloc(sizeof(List<Entity>));
        element->data = entity;
        element->next = nullptr;
        element->prev = lastEntity;
        
        if (lastEntity == nullptr)
            entities = element;
        else
            lastEntity->next = element;
        lastEntity = element;
        
        return entity;
    }

    void EntitySystem::PopEntity(Entity* entity)
    {
        List<Entity>* current = entities;
        List<Entity>* before = entities;
        
        while (current != nullptr)
        {
            if (current->data != entity)
            {
                before = current;
                current = current->next;
            }
            else
                break;
        }
        
        if (current != nullptr)
        {
            current->data->Destroy();
            
            if (before == current)
            {
                entities = current->next;
                if (entities != nullptr)
                    entities->prev = nullptr;
                if (current == lastEntity)
                    lastEntity = nullptr;
            }
            else
            {
                before->next = current->next;
                if (current->next != nullptr)
                    current->next->prev = before;
                if (current == lastEntity)
                    lastEntity = before;
            }
            
            delete current->data;
            delete current;
        }
    }

    void EntitySystem::Destroy()
    {
        List<Entity>* next = nullptr;
        if (entities != nullptr)
            for (auto* list = entities; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Destroy();
            }
    }
    
    int EntitySystem::GetEntityCount()
    {
        List<Entity>* list = entities;
        int count = 0;
        
        while (list != nullptr)
        {
            list = list->next;
            count++;
        }
        
        return count;
    }

    List<Entity>* EntitySystem::GetEntitiesListHead()
    {
        return entities;
    }
}
