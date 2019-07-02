//
//  Language.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Language_hpp
#define Language_hpp

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "../Essentials/Base.hpp"
#include "NovelSomeScript.hpp"

namespace ns
{
    class Language
    {
        static std::unordered_map<std::string, std::wstring> dictionary;
        
    public:
        static std::string currentLanguage;
        
        static std::wstring get(const std::string&);
        static void Default();
        static void Load(const std::wstring&);
        
        static void SaveData();
        static void LoadData();
    };
    
    typedef Language lang;
}

#endif /* Language_hpp */
