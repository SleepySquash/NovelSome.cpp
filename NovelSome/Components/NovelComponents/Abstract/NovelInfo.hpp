//
//  NovelInfo.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 10.02.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#ifndef NovelInfo_hpp
#define NovelInfo_hpp

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
        std::wstring name, path;            // TODO: NovelHolder, содержащий нужную инфу о новелле для сохранения/загрузки.
        NovelInfo(const std::wstring& path);
    };
}

#endif /* NovelInfo_hpp */
