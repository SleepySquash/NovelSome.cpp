//
//  NovelsLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 12/02/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "NovelsLibrary.hpp"

namespace ns
{
    list<NovelInfo> Novels::info;
    
    
    NovelInfo::NovelInfo(const std::wstring& path)
    {
        this->path = path;
        details = nullptr;
        name = L"";
        
        std::wifstream wif;
#ifdef _WIN32
        wif.open(path);
#else
        wif.open(base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
        if (wif.is_open())
        {
            std::wstring line;
            nss::CommandSettings command;
            command.lowercaseCommand = false;
            
            int infoRead{ 0 };
            bool read{ true };
            while (!wif.eof() && read)
            {
                std::getline(wif, line);
                command.Command(line);
                
                if (nss::Command(command, L"name ")) { name = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                else if (nss::Command(command, L"preview ")) { preview = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                
                if (infoRead == 2) read = false;
            }
            
            wif.close();
        }
        
        if (!name.length() || name == L"")
        {
            int pos = path.find_last_of(L'/');
            if (pos != std::wstring::npos && pos + 1 != name.length())
                name = path.substr(pos + 1, name.length());
            else name = L"NOT SET";
        }
    }
    
    
    void Novels::clear() { info.clear(); }
    unsigned long Novels::LoadNovels()
    {
        clear();
        
        list<std::wstring> directories;
#if defined(__ANDROID__)
        directories.push_back(base::utf16(externalDataPath()) + L"Built-In Novels/");
        directories.push_back(base::utf16(externalDataPath()) + L"Novels/");
#elif defined(__APPLE__)
        directories.push_back(base::utf16(documentsPath()) + L"Novels/");
        directories.push_back(base::utf16(resourcePath()) + L"Novels/");
#else
        directories.push_back(executablePath() + L"Novels/");
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
                        int pos = fileName.find_last_of('.');
                        if (pos != std::string::npos)
                        {
                            std::wstring extension = fileName.substr(pos, fileName.length());
                            if (extension == L".nsdata") Novels::info.push_back(NovelInfo((*it) + fileName));
                        }
                        else
                            directories.push_back((*it) + fileName + L"/");
                    }
                }
                _closedir(directory);
            }
        if (ent != NULL) free(ent);
        
        return Novels::info.size();
    }
}
