//
//  NovelLoader.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 09/07/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "NovelLoader.hpp"

namespace ns
{
    void NovelLoader::ReceiveMessage(MessageHolder& message)
    {
        if (message.info == "StartNovel") entity->AddComponent<NovelComponents::Novel>(message.additional, reinterpret_cast<NovelInfo*>(message.address));
    }
}
