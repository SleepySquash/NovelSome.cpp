//
//  List.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef List_hpp
#define List_hpp

namespace ns
{
    template<typename T> struct List
    {
        T* data = nullptr;
        List<T>* next = nullptr;
        List<T>* prev = nullptr;
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
