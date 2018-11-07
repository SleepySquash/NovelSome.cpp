//
//  NovelSystem.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "NovelSystem.hpp"

namespace ns
{
    NovelObject::~NovelObject() { }
    void NovelObject::Init() { }
    void NovelObject::Update(const sf::Time&) { }
    void NovelObject::Draw(sf::RenderWindow*) { }
    void NovelObject::Resize(unsigned int width, unsigned int height) { }
    void NovelObject::PollEvent(sf::Event& event) { }
    void NovelObject::Destroy() { }
    void NovelObject::SetPriority(int priority)
    {
        this->priority = priority;
        //TODO: Sorting
    }
    void NovelObject::ChangePriority(int priority)
    {
        if (novelSystem != nullptr)
            novelSystem->ChangePriorityOf(this, priority);
    }
    void NovelObject::SetNovelSystem(NovelSystem* novelSystem)
    {
        this->novelSystem = novelSystem;
    }
    NovelSystem* NovelObject::GetNovelSystem()
    {
        return novelSystem;
    }
    
    
    
    NovelSystem::NovelSystem()
    {
        objects = nullptr;
        lastObject = nullptr;
    }
    void NovelSystem::Update(const sf::Time& elapsedTime)
    {
        List<NovelObject>* next = nullptr;
        if (objects != nullptr)
            for (auto* list = objects; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Update(elapsedTime);
            }
    }
    void NovelSystem::Draw(sf::RenderWindow* window)
    {
        List<NovelObject>* next = nullptr;
        if (objects != nullptr)
            for (auto* list = objects; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Draw(window);
            }
    }
    void NovelSystem::Resize(unsigned int width, unsigned int height)
    {
        List<NovelObject>* next = nullptr;
        if (objects != nullptr)
            for (auto* list = objects; list != nullptr; list = next)
            {
                next = list->next;
                list->data->Resize(width, height);
            }
    }
    void NovelSystem::PollEvent(sf::Event& event)
    {
        List<NovelObject>* next = nullptr;
        if (lastObject != nullptr)
            for (auto* list = lastObject; list != nullptr; list = next)
            {
                next = list->prev;
                list->data->PollEvent(event);
            }
    }
    void NovelSystem::PopComponent(NovelObject* component)
    {
        List<NovelObject>* current = objects;
        List<NovelObject>* before = objects;
        
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
                objects = current->next;
                if (objects != nullptr)
                    objects->prev = nullptr;
                if (current == lastObject)
                    lastObject = nullptr;
            }
            else
            {
                before->next = current->next;
                if (current->next != nullptr)
                    current->next->prev = before;
                if (current == lastObject)
                    lastObject = before;
            }
            
            delete current->data;
            delete current;
        }
    }
    void NovelSystem::Destroy()
    {
        List<NovelObject>* next = nullptr;
        if (objects != nullptr)
            for (auto* list = objects; list != nullptr; list = next)
            {
                next = list->next;
                
                list->data->Destroy();
                delete list->data;
                delete list;
            }
    }
    void NovelSystem::ChangePriorityOf(NovelObject* component, int priority)
    {
        bool found{ false };
        List<NovelObject>* obj{ nullptr };
        for (auto* list = objects; list != nullptr && !found; list = list->next)
        {
            if ((found = (list->data == component)))
            {
                if (list->prev != nullptr)
                    list->prev->next = list->next;
                if (list->next != nullptr)
                    list->next->prev = list->prev;
                obj = list;
            }
        }
        
        if (found && obj != nullptr)
        {
            for (auto* list = objects; list != nullptr; list = list->next)
            {
                if (list->data->priority > priority)
                {
                    if (list->prev != nullptr)
                        list->prev->next = obj;
                    obj->next = list;
                    obj->prev = list->prev;
                    list->prev = obj;
                    obj->data->priority = priority;
                }
            }
        }
    }
}
