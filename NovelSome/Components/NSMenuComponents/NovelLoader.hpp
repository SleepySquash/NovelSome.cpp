//
//  NovelLoader.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 09/07/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef NovelLoader_hpp
#define NovelLoader_hpp

#include <iostream>

#include <minEH/Engine/EntitySystem.hpp>

#include "../NovelComponents/Novel.hpp"

namespace ns
{
    struct NovelLoader : Component
    {
        void ReceiveMessage(MessageHolder& message) override;
    };
}

#endif /* NovelLoader_hpp */
