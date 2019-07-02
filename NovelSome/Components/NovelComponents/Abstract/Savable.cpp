//
//  Savable.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 19/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "Savable.hpp"

namespace ns
{
    namespace NovelComponents
    {
        std::pair<std::wstring, bool> Savable::Load(std::wifstream& wif) { return { L"", false }; }
        Savable::Savable(const std::wstring& tag) : tag(tag) { }
    }
}
