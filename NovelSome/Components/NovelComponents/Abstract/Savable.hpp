//
//  Savable.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 10/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#ifndef Savable_hpp
#define Savable_hpp

#include <iostream>
#include <fstream>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/Settings.hpp>

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct Savable
        {
            std::wstring tag;
            virtual void Save(std::wofstream& wof) = 0;
            virtual std::pair<std::wstring, bool> Load(std::wifstream& wif);
            
            Savable(const std::wstring& tag = L"");
        };
    }
}

#endif /* Savable_hpp */
