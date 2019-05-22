//
//  SavingMechanism.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 10/04/2019.
//  Copyright © 2019 Melanholy Hill. All rights reserved.
//

#include "SavingMechanism.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void SaveGameState(const std::wstring& fileName, NovelSystem& system)
        {
            //if (!base::FileExists(fileName)) base::CreateDirectory(base::utf16(documentsPath()));
            std::wstring folderPath = base::GetFolderPath(fileName);
            if (!base::FileExists(folderPath)) base::CreateDirectory(folderPath);
            
            std::wofstream wof;
#ifdef _WIN32
            wof.open(fileName);
#else
            wof.open(base::utf8(fileName));
#endif
            wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            if (wof.is_open())
            {
                for (auto n : system)
                {
                    Savable* savable;
                    if ((savable = dynamic_cast<Savable*>(n)))
                    {
                        wof << L"TAG(" << n->priority << ")=" << savable->tag << endl;
                        savable->Save(wof);
                        wof << endl;
                    }
                }
                wof.close();
            }
        }
    }
}
