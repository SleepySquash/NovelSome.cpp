//
//  NovelsLibrary.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 12/02/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef NovelsLibrary_hpp
#define NovelsLibrary_hpp

#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

#include <locale>
#include <fstream>
#include <codecvt>
#include <dirent.h>

#include <minEH/Essentials/ResourcePath.hpp>
#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>
#include <minEH/Engine/Language.hpp>

#include "../NovelComponents/Abstract/NovelInfo.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::vector;
using std::unordered_map;

namespace ns
{
    struct Novels
    {
        static list<NovelInfo> info;
        
        static void clear();
        static unsigned long LoadNovels();
    };
}

#endif /* NovelsLibrary_hpp */
