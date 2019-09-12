//
//  LanguageLibrary.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 03/06/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef LanguageLibrary_hpp
#define LanguageLibrary_hpp

#include <iostream>
#include <list>

#include <locale>
#include <fstream>
#include <codecvt>
#include <dirent.h>

#include "../../Essentials/ResourcePath.hpp"
#include "../../Essentials/Base.hpp"
#include "../../Engine/NovelSomeScript.hpp"
#include "../../Engine/Language.hpp"

using std::cin;
using std::cout;
using std::endl;

namespace ns
{
    struct LanguageInfo
    {
        std::wstring name, path;
        std::string initials;
        LanguageInfo();
        LanguageInfo(const std::wstring& path);
    };
    
    struct Languages
    {
        static std::list<LanguageInfo> list;
        static unsigned long languagesLoaded;
        static void clear();
        static unsigned long LoadLanguages();
        static void LoadData();
    };
}

#endif /* LanguageLibrary_hpp */
