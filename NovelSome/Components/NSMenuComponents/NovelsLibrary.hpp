//
//  NovelsLibrary.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 12/02/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
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

#include "../../Essentials/ResourcePath.hpp"
#include "../../Essentials/Base.hpp"
#include "../../Engine/NovelSomeScript.hpp"

using std::cin;
using std::cout;
using std::endl;
using std::list;
using std::vector;
using std::unordered_map;

namespace ns
{
    struct NovelDetailedInfo
    {
        std::wstring description{ L"" }, author{ L"" };
        std::wstring backgroundPath{ L"" };
    };

    struct NovelInfo
    {
        std::wstring name, path, preview;
        NovelDetailedInfo* details;
        
        NovelInfo(const std::wstring& path);
        ~NovelInfo() { delete details; }
    };

    struct Novels
    {
        static list<NovelInfo> info;
        
        static void clear();
        static unsigned long LoadNovels();
    };
}

#endif /* NovelsLibrary_hpp */
