//
//  Language.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/05/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "Language.hpp"

namespace ns
{
    std::unordered_map<std::string, std::wstring> Language::dictionary;
    std::string Language::currentLanguage{ "en" };
    std::wstring Language::menufont, Language::font;
        
    std::wstring Language::get(const std::string& str)
    {
        auto it = dictionary.find(str);
        if (it != dictionary.end()) return (*it).second;
        return base::utf16(str);
    }
    void Language::Default()
    {
        currentLanguage = "en";
        menufont = L"Pacifica.ttf";
        font = L"Arial.ttf";
        dictionary["Novel selection"] = L"Novel selection";
        dictionary["Editor"] = L"Editor";
        dictionary["Settings"] = L"Settings";
        dictionary["Exit"] = L"Exit";
        dictionary["Welcome to the"] = L"Welcome to the";
        dictionary["BACK"] = L"BACK";
        dictionary["Read"] = L"Read";
        dictionary["Account"] = L"Account";
        dictionary["Password"] = L"Password";
        dictionary["Login"] = L"Login";
        dictionary["Registration"] = L"Registration";
        dictionary["Description"] = L"Description";
        dictionary["Continue"] = L"Continue";
    }
    void Language::Load(const std::wstring& path)
    {
        menufont = L"Pacifica.ttf";
        font = L"Arial.ttf";
        std::wifstream wif;
#ifdef _WIN32
        wif.open(path);
#else
        wif.open(base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
        
        if (wif.is_open())
        {
            int version{ 1 };
            std::wstring line; nss::CommandSettings command;
            while (!wif.eof())
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"%version ")) version = nss::ParseAsInt(command);
                else if (nss::Command(command, L"%name ")) currentLanguage = utf8(nss::ParseAsString(command));
                else if (nss::Command(command, L"%font "))
                {
                    font = nss::ParseAsMaybeQuoteString(command);
                    if (!base::FileExists(L"Data/Fonts/" + font) && !base::FileExists(utf16(resourcePath()) + L"Data/Fonts/" + font))
                        font = L"Arial.ttf";
                }
                else if (nss::Command(command, L"%menu ") || nss::Command(command, L"%menufont "))
                {
                    menufont = nss::ParseAsMaybeQuoteString(command);
                    if (!base::FileExists(L"Data/Fonts/" + menufont) && !base::FileExists(utf16(resourcePath()) + L"Data/Fonts/" + menufont))
                        menufont = L"Pacifica.ttf";
                }
                else if (version == 1)
                {
                    std::wstring stated = nss::ParseUntil(command, L'='); ++command.lastPos;
                    std::wstring translation = nss::ParseUntil(command, L'\n');
                    if (stated.length() && translation.length())
                    {
                        nss::Trim(stated); nss::Trim(translation);
                        dictionary[utf8(stated)] = translation;
                    }
                }
            }
            wif.close(); /// for (auto& p : Language::dictionary) cout << p.first << " : " << ns::base::utf8(p.second) << endl;
        }
    }
    void Language::SaveData()
    {
        std::wstring path = utf16(documentsPath()) + L"Language.nsoptions";
        if (!base::FileExists(path)) base::CreateDirectory(base::utf16(documentsPath()));
        
        std::wofstream wof;
#ifdef _WIN32
        wof.open(path);
#else
        wof.open(base::utf8(path));
#endif
        wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        if (wof.is_open()) wof << utf16(currentLanguage) << endl;
    }
    void Language::LoadData()
    {
        std::wstring path = utf16(documentsPath()) + L"Language.nsoptions";
        std::wifstream wif;
#ifdef _WIN32
        wif.open(path);
#else
        wif.open(base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
        
        if (wif.is_open())
        {
            std::wstring line; std::getline(wif, line); wif.close();
            currentLanguage = utf8(line);
        }
    }
}
