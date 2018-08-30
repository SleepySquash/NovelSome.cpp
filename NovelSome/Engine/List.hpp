//
//  List.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef List_hpp
#define List_hpp

template<typename T> struct List
{
    T* data = nullptr;
    List<T>* next = nullptr;
    List<T>* prev = nullptr;
};

#endif /* List_hpp */
