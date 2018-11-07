//
//  List.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef List_hpp
#define List_hpp

namespace ns
{
    template<typename T> struct StackElement
    {
        T data;
        StackElement<T>* next;
        
        StackElement(const T& v, StackElement<T>* n) : data(v), next(n) { }
    };
    template<typename T> struct Stack
    {
        StackElement<T>* head{ nullptr };
        unsigned int size{ 0 };
        
        ~Stack()
        {
            while (head != nullptr)
            {
                StackElement<T>* next = head->next;
                delete head;
                head = next;
            }
        }
        void Push(const T& value)
        {
            ++size;
            head = new StackElement<T>(value, head);
        }
        T Pop()
        {
            if (head != nullptr)
            {
                --size;
                T value = head->data;
                StackElement<T>* next = head->next;
                
                delete head;
                head = next;
                
                return value;
            }
            return T();
        }
        void Clear()
        {
            while (head != nullptr)
            {
                StackElement<T>* next = head->next;
                delete head;
                head = next;
            }
            head = nullptr;
            size = 0;
        }
        bool IsEmpty()
        {
            return (size == 0 || head == nullptr);
        }
        T operator[](int i)
        {
            if (head != nullptr)
            {
                if (i > 0)
                {
                    StackElement<T>* ret = head;
                    for (int k = 0; k < i && ret->next != nullptr; ++k)
                        ret = ret->next;
                    
                    return ret->data;
                }
                else if (i == 0)
                    return head->data;
            }
            
            return T();
        }
    };
    
    
    
    
    template<typename T> struct DirectedListElement
    {
        T data;
        DirectedListElement<T>* next;
        
        DirectedListElement(const T& v, DirectedListElement<T>* n) : data(v), next(n) { }
    };
    template<typename T> struct UndirectedListElement
    {
        T data;
        UndirectedListElement<T>* next;
        UndirectedListElement<T>* prev;
        
        UndirectedListElement(const T& v, UndirectedListElement<T>* n = nullptr, UndirectedListElement<T>* p = nullptr) : data(v), next(n), prev(p) { }
    };
    template<typename T> struct UndirectedList
    {
        UndirectedListElement<T>* head{ nullptr };
        UndirectedListElement<T>* last{ nullptr };
        unsigned int size{ 0 };
        
        ~UndirectedList()
        {
            while (head != nullptr)
            {
                UndirectedListElement<T>* next = head->next;
                delete head;
                head = next;
            }
        }
        void Push(const T& value)
        {
            ++size;
            head = new UndirectedListElement<T>(value, head);
            if (head->next == nullptr)
                last = head;
            if (head->next != nullptr)
                head->next->prev = head;
        }
        void PushBack(const T& value)
        {
            ++size;
            
            if (head != nullptr)
            {
                last->next = new UndirectedListElement<T>(value);
                last->next->prev = last;
                last = last->next;
            }
            else
            {
                head = new UndirectedListElement<T>(value, head);
                last = head;
                if (head->next != nullptr)
                    head->next->prev = head;
            }
        }
        void Emplace(const T& value)
        {
            PushBack(value);
        }
        void Insert(const T& value)
        {
            Push(value);
        }
        void InsertAt(const T& value, int i)
        {
            Push(value);
        }
        T Pop()
        {
            if (head != nullptr)
            {
                --size;
                T value = head->data;
                UndirectedListElement<T>* next = head->next;
                
                delete head;
                head = next;
                if (head == nullptr)
                    last = nullptr;
                else if (head->next == nullptr)
                    last = head;
                
                return value;
            }
            return T();
        }
        T PopAt(int i)
        {
            if (head != nullptr)
            {
                if (i > 0)
                {
                    UndirectedListElement<T>* ret = head;
                    for (int k = 0; k < i && ret->next != nullptr; ++k)
                        ret = ret->next;
                    if (ret == last)
                        last = ret->prev;
                    
                    T value = ret->data;
                    if (ret->prev != nullptr)
                        ret->prev->next = ret->next;
                    if (ret->next != nullptr)
                        ret->next->prev = ret->prev;
                    if (ret == head)
                        head = ret->next;
                    
                    delete ret;
                    return value;
                }
                else if (i == 0)
                    return Pop();
            }
            return T();
        }
        void Clear()
        {
            while (head != nullptr)
            {
                UndirectedListElement<T>* next = head->next;
                delete head;
                head = next;
            }
            head = nullptr;
            size = 0;
        }
        bool IsEmpty()
        {
            return (size == 0 || head == nullptr);
        }
        T operator[](int i)
        {
            if (head != nullptr)
            {
                if (i > 0)
                {
                    UndirectedListElement<T>* ret = head;
                    for (int k = 0; k < i && ret->next != nullptr; ++k)
                        ret = ret->next;
                    
                    return ret->data;
                }
                else if (i == 0)
                    return head->data;
            }
            
            return T();
        }
    };
    
    
    
    
    
    template<typename T> struct List
    {
        T* data = nullptr;
        List<T>* next = nullptr;
        List<T>* prev = nullptr;
        
        List() { }
        List(T* t, List<T>* list) : data(t), next(list)
        {
            list->prev = this;
        }
    };
    
    namespace list
    {
        template<typename T> List<T>* Insert(List<T>* head)
        {
            List<T>* element = new List<T>();
            element->data = nullptr;
            element->next = nullptr;
            element->prev = nullptr;
            if (head != nullptr)
            {
                element->next = head;
                head->prev = element;
            }
            
            return element;
        }
        template<typename T> List<T>* Remove(List<T>* element)
        {
            if (element->prev == nullptr)
            {
                List<T>* newHead{ element->next };
                if (element->next != nullptr)
                    element->next->prev = nullptr;
                delete element;
                
                return newHead;
            }
            else
            {
                if (element->next != nullptr)
                    element->next->prev = element->prev;
                element->prev->next = element->next;
                
                delete element;
            }
        }
        template<typename T> unsigned int Size(List<T>* head)
        {
            unsigned int size{ 0 };
            while (head != nullptr)
            {
                head = head->next;
                size++;
            }
            
            return size;
        }
        
        //Append/Emplace - last position
    }
}

#endif /* List_hpp */
