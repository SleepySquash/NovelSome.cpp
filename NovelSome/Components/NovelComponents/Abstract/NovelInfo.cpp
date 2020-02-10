//
//  NovelInfo.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 10.02.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "NovelInfo.hpp"

namespace ns
{
    NovelInfo::NovelInfo(const std::wstring& path)
    {
        this->path = path;
        //details = nullptr;
        name = L"";
        
        std::wifstream wif;
#ifdef _WIN32
        wif.open(path);
#else
        wif.open(base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
        
        std::wstring defaultname;
        if (wif.is_open())
        {
            std::wstring line; nss::CommandSettings command;
            command.lowercaseCommand = false;
            
            int infoRead{ 0 }, linesRead{ 0 }; bool read{ true };
            while (!wif.eof() && read)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"name ")) { defaultname = nss::ParseAsMaybeQuoteString(command); ++infoRead; linesRead = 0; }
                if (nss::Command(command, L"name:" + utf16(lang::currentLanguage) + L" ")) { name = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                else ++linesRead;
                //else if (nss::Command(command, L"preview ")) { preview = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                if (infoRead >= 1 && linesRead >= 5) read = false;
            }
            wif.close();
        }
        
        if (!name.length() || name == L"") name = defaultname;
        if (!name.length() || name == L"")
        {
            int pos = path.find_last_of(L'/');
            if (pos != std::wstring::npos && pos + 1 != name.length())
                name = path.substr(pos + 1, name.length());
            else name = L"NOT SET";
        }
        //if (!display.length() || display == L"") display = name;
    }
}
