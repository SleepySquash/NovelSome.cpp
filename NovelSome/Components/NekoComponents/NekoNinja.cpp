//
//  NekoNinjaComponents.cpp
//  NekoPlace
//
//  Created by Никита Исаенко on 25/01/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "NekoNinja.hpp"

namespace NekoNinja
{
    float NekoObject::gravity = 0.20f;
    vector<NekoInfo> NekoLibrary::neko = {
        NekoInfo(L"Shigure", 10000),
        NekoInfo(L"Vanilla", 10000),
        NekoInfo(L"Chocola", 10000),
        NekoInfo(L"Maple", 10000),
        NekoInfo(L"Azuki", 10000),
        NekoInfo(L"Cinnamon", 10000),
        NekoInfo(L"Coconut", 10000),
    };
}
