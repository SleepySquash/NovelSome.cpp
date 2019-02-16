//
//  NovelsLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 12/02/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
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
        
        DIR *directory;
        dirent *ent{ NULL };
        for (auto it = directories.begin(); it != directories.end(); ++it)
            if ((directory = opendir(base::utf8(*it).c_str())) != NULL)
            {
                while ((ent = readdir(directory)) != NULL)
                {
                    std::string fileName = ent->d_name;
                    if (fileName != "." && fileName != "..")
                    {
                        int pos = fileName.find_last_of('.');
                        if (pos != std::string::npos)
                        {
                            std::string extension = fileName.substr(pos, fileName.length());
                            if (extension == ".nsdata")
                                Novels::info.push_back(NovelInfo((*it) + base::utf16(fileName)));
                        }
                        else
                            directories.push_back((*it) + base::utf16(fileName) + L"/");
                    }
                }
                closedir(directory);
            }
        if (ent != NULL) free(ent);
        
        return Novels::info.size();
    }
}
