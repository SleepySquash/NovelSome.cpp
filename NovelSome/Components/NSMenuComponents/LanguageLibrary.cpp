//
//  LanguageLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 03/06/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "LanguageLibrary.hpp"

namespace ns
{
    std::list<LanguageInfo> Languages::list;
    unsigned long Languages::languagesLoaded{ 0 };
    
    
    LanguageInfo::LanguageInfo() { }
    LanguageInfo::LanguageInfo(const std::wstring& path)
    {
        this->path = path; name = L""; initials = "";
        
        std::wifstream wif;
#ifdef _WIN32
        wif.open(path);
#else
        wif.open(base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
        
        if (wif.is_open())
        {
            std::wstring line; nss::CommandSettings command;
            command.lowercaseCommand = false;
            int infoRead{ 0 }; bool read{ true };
            while (!wif.eof() && read)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"%name ")) { initials = utf8(nss::ParseAsMaybeQuoteString(command)); ++infoRead; }
                else if (nss::Command(command, L"%display ")) { name = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                if (infoRead >= 2) read = false;
            }
            
            wif.close();
        }
        
        if (!initials.length() || initials == "")
        {
            int pos = path.find_last_of(L'/');
            if (pos != std::wstring::npos && pos + 1 != initials.length())
                initials = utf8(path.substr(pos + 1, initials.length()));
            else initials = "";
        }
        if (!name.length() || name == L"") name = utf16(initials);
    }
    
    
    void Languages::clear() { list.clear(); }
    unsigned long Languages::LoadLanguages()
    {
        clear();
        list.push_back(LanguageInfo());
        list.back().initials = "en";
        list.back().name = L"English";
        
        std::list<std::wstring> directories;
#if defined(__ANDROID__)
        directories.push_back(base::utf16(externalDataPath()) + L"Data/Language/");
#elif defined(__APPLE__)
        directories.push_back(base::utf16(documentsPath()) + L"Data/Language/");
        directories.push_back(base::utf16(resourcePath()) + L"Data/Language/");
#else
        directories.push_back(executablePath() + L"Data/Language/");
#endif
        
#ifdef _WIN32
        _WDIR *directory;
        _wdirent *ent{ NULL };
#define _opendir _wopendir
#define _readdir _wreaddir
#define _closedir _wclosedir
#else
        DIR *directory;
        dirent *ent{ NULL };
#define _opendir opendir
#define _readdir readdir
#define _closedir closedir
#endif
        
        for (auto it = directories.begin(); it != directories.end(); ++it)
#ifdef _WIN32
            if ((directory = _opendir((*it).c_str())) != NULL)
#else
            if ((directory = _opendir(base::utf8(*it).c_str())) != NULL)
#endif
            {
                while ((ent = _readdir(directory)) != NULL)
                {
#ifdef _WIN32
                    std::wstring fileName = ent->d_name;
#else
                    std::wstring fileName = base::utf16(ent->d_name);
#endif
                    if (fileName != L"." && fileName != L"..")
                    {
                        if (ent->d_type != DT_DIR) Languages::list.push_back(LanguageInfo((*it) + fileName));
                        else directories.push_back((*it) + fileName + L"/");
                    }
                }
                _closedir(directory);
            }
        if (ent != NULL) free(ent);
        
        languagesLoaded = Languages::list.size();
        return languagesLoaded;
    }
    void Languages::LoadData()
    {
        Language::LoadData(); LoadLanguages();
        if (lang::currentLanguage != "en")
            for (auto& l : list)
                if (l.initials == lang::currentLanguage) {
                    lang::Load(l.path); break; }
    }
}
