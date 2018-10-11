//
//  GUIInterface.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 08/10/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef GUIInterface_hpp
#define GUIInterface_hpp

#include <iostream>

#include <SFML/Graphics.hpp>

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    namespace GUI
    {
        struct TextButton
        {
            sf::Text text;
        };
    }
}

#endif /* GUIInterface_hpp */
