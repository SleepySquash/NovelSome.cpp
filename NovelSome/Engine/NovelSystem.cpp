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
    void NovelObject::Resize(const unsigned int& width, const unsigned int& height) { }
    void NovelObject::PollEvent(sf::Event& event) { }
    void NovelObject::Destroy() { }
    void NovelObject::ReceiveMessage(MessageHolder&) { }
    void NovelObject::SendMessage(MessageHolder message) { ReceiveMessage(message); }
    void NovelObject::SetPriority(int priority) { this->priority = priority; /* TODO: Sorting */ }
    void NovelObject::ChangePriority(int priority) { if (novelSystem) novelSystem->ChangePriorityOf(this, priority); }
    
    
    
    NovelSystem::NovelSystem() { }
    void NovelSystem::Update(const sf::Time& elapsedTime)
    {
        list<NovelObject*>::iterator it = objects.begin();
        while (it != objects.end())
        {
            if ((*it)->offline) { delete (*it); it = objects.erase(it); }
            else { (*it)->Update(elapsedTime); ++it; }
        }
    }
    void NovelSystem::Draw(sf::RenderWindow* window)
    {
        if (objects.size())
            for (auto e : objects)
                if (!e->offline) e->Draw(window);
    }
    void NovelSystem::Resize(const unsigned int& width, const unsigned int& height)
    {
        if (objects.size())
            for (auto e : objects)
                if (!e->offline) e->Resize(width, height);
    }
    void NovelSystem::PollEvent(sf::Event& event)
    {
        if (objects.size())
            for (auto it = objects.rbegin(); it != objects.rend(); ++it)
                if (!(*it)->offline) (*it)->PollEvent(event);
    }
    void NovelSystem::PopComponent(NovelObject* component)
    {
        component->offline = true;
        component->Destroy();
    }
    void NovelSystem::SendMessage(MessageHolder message) { ReceiveMessage(message); }
    void NovelSystem::ReceiveMessage(MessageHolder& message) {
        if (objects.size())
            for (auto e : objects)
                if (!e->offline) e->ReceiveMessage(message);
    }
    void NovelSystem::clear()
    {
        list<NovelObject*>::iterator it = objects.begin();
        while (it != objects.end()) { if (!(*it)->offline) (*it)->Destroy(); delete (*it); it = objects.erase(it); }
        objects.clear();
    }
    bool NovelSystem::empty() { return objects.empty(); }
    void NovelSystem::ChangePriorityOf(NovelObject* component, int priority)
    {
        std::list<NovelObject*>::iterator it = std::find(objects.begin(), objects.end(), component);
        if (it != objects.end())
        {
            objects.erase(it);
            
            bool done{ false };
            for (list<NovelObject*>::iterator it = objects.begin(); it != objects.end() && !done; ++it)
                if ((*it)->priority > priority)
                {
                    objects.insert(it, component);
                    done = true;
                }
            if (!done)
                objects.push_back(component);
        }
    }
    list<NovelObject*>::iterator NovelSystem::begin() { return objects.begin(); }
    list<NovelObject*>::iterator NovelSystem::end() { return objects.end(); }
    list<NovelObject*>::const_iterator NovelSystem::cbegin() const { return objects.cbegin(); }
    list<NovelObject*>::const_iterator NovelSystem::cend() const { return objects.cend(); }
    list<NovelObject*>::const_reverse_iterator NovelSystem::crbegin() const { return objects.crbegin(); }
    list<NovelObject*>::const_reverse_iterator NovelSystem::crend() const { return objects.crend(); }
    list<NovelObject*>::reverse_iterator NovelSystem::rbegin() { return objects.rbegin(); }
    list<NovelObject*>::reverse_iterator NovelSystem::rend() { return objects.rend(); }
}
