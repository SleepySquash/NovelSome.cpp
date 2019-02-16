//
//  NovelSystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef NovelSystem_hpp
#define NovelSystem_hpp

#include <iostream>
#include <list>
using std::list;

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "StaticMethods.hpp"

using std::cout;
using std::cin;
using std::endl;

namespace ns
{
    class NovelObject;
    class NovelSystem;
    
    class NovelObject
    {
    public:
        NovelSystem* novelSystem{ nullptr };
        bool offline{ false };
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
    public:
        list<NovelObject*> objects;
        
        NovelSystem();
        void Update(const sf::Time& elapsedTime);
        void Draw(sf::RenderWindow* window);
        void Resize(unsigned int width, unsigned int height);
        void PollEvent(sf::Event& event);
        void PopComponent(NovelObject* component);
        void clear();
        bool empty();
        void ChangePriorityOf(NovelObject* component, int priority);
        
        template<typename T, typename ...Args> T* AddComponent(Args... args)
        {
            T* component = new T(args...);
            objects.push_back(component);
            
            component->SetNovelSystem(this);
            component->Init();
            component->Resize(gs::width, gs::height);
            
            return component;
        }
        template<typename T, typename ...Args> T* PrioritizeComponent(int priority, Args... args)
        {
            T* component = new T(args...);
            component->priority = priority;
            
            bool done{ false };
            for (list<NovelObject*>::iterator it = objects.begin(); it != objects.end() && !done; ++it)
                if ((*it)->priority > priority)
                {
                    objects.insert(it, component);
                    done = true;
                }
            if (!done)
                objects.push_back(component);
            
            component->SetNovelSystem(this);
            component->Init();
            component->Resize(gs::width, gs::height);
            
            return component;
        }
    };
}

#endif /* NovelSystem_hpp */
