//
//  VariableSystem.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "VariableSystem.hpp"

namespace ns
{
    namespace NovelComponents
    {
        sf::Text* TemporarySettings::dialogue = nullptr, *TemporarySettings::name = nullptr;
        
        NovelVariable::~NovelVariable() { if (type == String) delete[] value.asString; }
        NovelVariable::NovelVariable() { }
        NovelVariable::NovelVariable(const int& value) { Set(value); }
        NovelVariable::NovelVariable(const std::wstring& value) { Set(value); }
        NovelVariable::NovelVariable(const bool& value) { Set(value); }
        void NovelVariable::Set(const int& value)
        {
            if (type == String) delete[] this->value.asString;
            type = Integer;
            this->value.asInt = value;
        }
        void NovelVariable::Set(const std::wstring& value)
        {
            if (type == String) delete[] this->value.asString;
            type = String;
            this->value.asString = (wchar_t*)malloc(sizeof(wchar_t) * (value.length() + 1));
            for (int i = 0; i <= value.length(); i++)
                this->value.asString[i] = value[i];
        }
        void NovelVariable::Set(const bool& value)
        {
            if (type == String) delete[] this->value.asString;
            type = Boolean;
            this->value.asBoolean = value;
        }
        std::ostream& operator<<(std::ostream& os, const NovelVariable& Var)
        {
            switch (Var.type)
            {
                case NovelVariable::NotSet: os << "NotSet"; break;
                case NovelVariable::Integer: os << Var.value.asInt; break;
                case NovelVariable::Boolean: os << (Var.value.asBoolean ? "true" : "false"); break;
                case NovelVariable::String: os << base::utf8(std::wstring(Var.value.asString)); break;
            }
            return os;
        }
        
        std::unordered_map<std::wstring, NovelVariable*> VariableSystem::localVariables;
        NovelVariable* VariableSystem::FindVariable(const std::wstring& variableName) { return (localVariables.find(variableName) != localVariables.end()) ? localVariables.at(variableName) : nullptr; }
        void VariableSystem::clear()
        {
            for (auto& key : localVariables) if (key.second) delete key.second;
            localVariables.clear();
        }
    }
}
