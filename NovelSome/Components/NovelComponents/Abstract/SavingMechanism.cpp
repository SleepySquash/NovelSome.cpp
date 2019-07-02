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
            std::wstring folderPath = base::GetFolderPath(fileName);
            try { if (!base::FileExists(folderPath)) base::CreateDirectory(folderPath); }
            catch (...) { throw std::runtime_error("Couldn't create the save file (perhaps directory does not exist or permission is denied?)"); return; }
            
            std::wofstream wof;
#ifdef _WIN32
            wof.open(fileName);
#else
            try { wof.open(utf8(fileName)); }
            catch (...) { throw std::runtime_error("Couldn't create the save file (perhaps directory does not exist or permission is denied?)"); return; }
#endif
            wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            if (wof.is_open())
            {
                for (auto& n : system)
                    if (n->priority == -31000)
                    {
                        Savable* savable;
                        if ((savable = dynamic_cast<Savable*>(n)))
                        {
                            wof << L"TAG(" << n->priority << ")=" << savable->tag << endl;
                            savable->Save(wof);
                            wof << endl;
                        }
                        break;
                    }
                for (auto it = system.begin(); it != system.end(); ++it)
                    if ((*it)->priority != -31000)
                    {
                        Savable* savable;
                        if ((savable = dynamic_cast<Savable*>(*it)))
                        {
                            wof << L"TAG(" << (*it)->priority << ")=" << savable->tag << endl;
                            savable->Save(wof);
                            wof << endl;
                        }
                    }
                wof.close();
            } else throw std::runtime_error("Couldn't create the save file (perhaps directory does not exist or permission is denied?)");
        }
    }
}
