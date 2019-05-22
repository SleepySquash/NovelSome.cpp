//
//  VariableSystem.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef VariableSystem_hpp
#define VariableSystem_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../../Essentials/Base.hpp"
#include "../../../Engine/List.hpp"
#include "../../../Engine/EntitySystem.hpp"
#include "../../../Engine/NovelSystem.hpp"
#include "../../../Engine/Settings.hpp"
#include "../../../Engine/GUIInterface.hpp"
#include "../../../Engine/NovelSomeScript.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct TemporarySettings
        {
            static sf::Text* dialogue, *name;
        };
        
        struct NovelVariable
        {
            enum typeEnum { NotSet, Integer, String, Boolean };
            typeEnum type = NotSet;
            union
            {
                bool asBoolean;
                int asInt;
                wchar_t* asString;
            } value;
            
            ~NovelVariable();
            NovelVariable();
            NovelVariable(const int& integer);
            NovelVariable(const std::wstring& integer);
            NovelVariable(const bool& integer);
            void Set(const int& value);
            void Set(const std::wstring& value);
            void Set(const bool& value);
            
            friend std::ostream& operator<<(std::ostream& os, const NovelVariable& Var);
        };
        
        struct VariableSystem
        {
            static std::unordered_map<std::wstring, NovelVariable*> localVariables;
            
            static NovelVariable* FindVariable(const std::wstring& variableName);
            static void clear();
        };
        typedef VariableSystem var;
    }
}

#endif /* VariableSystem_hpp */