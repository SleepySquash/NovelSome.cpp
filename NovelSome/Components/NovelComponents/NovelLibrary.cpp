//
//  NovelLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        NovelLibrary::NovelLibrary() { }
        NovelLibrary::~NovelLibrary()
        {
            FreeData();
        }
        void NovelLibrary::FreeData()
        {
            for (const auto& key : characterLibrary)
            {
                if (key.second != nullptr)
                    delete key.second;
            }
        }
        void NovelLibrary::ScanForCharacters()
        {
            if (novel != nullptr)
            {
#ifdef _WIN32
                _WDIR *dir;
                struct _wdirent *ent;
#define _opendir _wopendir
#define _readdir _wreaddir
#define _closedir _wclosedir
#else
                DIR *dir;
                struct dirent *ent;
#define _opendir opendir
#define _readdir readdir
#define _closedir closedir
#endif
                
                sf::String novelPath{ executablePath() + novel->GetFolderPath() };
                
                List<sf::String>* folders{ nullptr };
                List<sf::String>* lastFolders{ nullptr };
                folders = new List<sf::String>();
                folders->data = new sf::String("");
                folders->prev = nullptr;
                folders->next = nullptr;
                lastFolders = folders;
                
                List<sf::String>* currentFolder = folders;
                while (currentFolder != nullptr)
                {
                    sf::String fullpath{ novelPath + *currentFolder->data};
#ifdef _WIN32
                    if ((dir = _opendir(fullpath.toWideString().c_str())) != NULL)
#else
                        if ((dir = _opendir(fullpath.toAnsiString().c_str())) != NULL)
#endif
                        {
                            while ((ent = _readdir(dir)) != NULL)
                            {
                                std::wstring entryName{ sf::String(ent->d_name) };
                                if (entryName != L"." && entryName != L"..")
                                {
                                    std::wstring extention = ns::base::GetExtentionFromString(entryName);
                                    if (extention == L"")
                                    {
                                        List<sf::String>* newFolder = new List<sf::String>();
                                        newFolder->data = new sf::String(*currentFolder->data + entryName + L"/");
                                        newFolder->prev = lastFolders;
                                        newFolder->next = nullptr;
                                        lastFolders->next = newFolder;
                                        lastFolders = newFolder;
                                    }
                                    else if (extention == L".nschar")
                                    {
                                        std::wifstream wif;
                                        sf::String filePath = (fullpath + L"/" + entryName);
#ifdef _WIN32
                                        wif.open(filePath.toWideString());
#else
                                        std::wstring _wpath = filePath;
                                        std::string _path(_wpath.begin(), _wpath.end());
                                        wif.open(_path);
#endif
                                        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                                        
                                        if (!wif.is_open())
                                            cout << "Warning :: NovelComponent :: File couldn't be opened, path: " << filePath.toAnsiString() << endl;
                                        else
                                        {
                                            bool eof{ false };
                                            std::wstring line;
                                            nss::CommandSettings command;
                                            CharacterData* charData = new CharacterData();
                                            
                                            while (!eof)
                                            {
                                                if (!wif.eof())
                                                {
                                                    std::getline(wif, line);
                                                    command.Command(line);
                                                    
                                                    if (nss::Command(command, L"name "))
                                                    {
                                                        std::wstring nameParsed;
                                                        if (command.line[command.lastPos] == L'"')
                                                            nameParsed = nss::ParseAsQuoteString(command);
                                                        else
                                                            nameParsed = nss::ParseUntil(command, ' ');
                                                        
                                                        charData->name = nameParsed;
                                                    }
                                                    else if (nss::Command(command, L"dname ") || nss::Command(command, L"display "))
                                                    {
                                                        std::wstring nameParsed;
                                                        if (command.line[command.lastPos] == L'"')
                                                            nameParsed = nss::ParseAsQuoteString(command);
                                                        else
                                                            nameParsed = nss::ParseUntil(command, ' ');
                                                        charData->displayName = nameParsed;
                                                    }
                                                    else if (nss::Command(command, L"state "))
                                                    {
                                                        nss::ParseUntil(command, '"');
                                                        std::wstring stateName = nss::ParseUntil(command, '"');
                                                        //TODO: State parsing
                                                    }
                                                }
                                                else
                                                    eof = true;
                                            }
                                            
                                            
                                            if (charData->name == L"")
                                            {
#ifdef _WIN32
                                                charData->name = ns::base::GetStringWithNoExtention(ent->d_name);
#else
                                                std::string entDNameString(ent->d_name);
                                                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
                                                charData->name = ns::base::GetStringWithNoExtention(converter.from_bytes(entDNameString));
#endif
                                            }
                                            if (charData->displayName == L"")
                                                charData->displayName = charData->name;
                                            
                                            charData->filePath = *currentFolder->data + L"/" + entryName;
                                            
                                            if (novel->library.characterLibrary.find(charData->name) != novel->library.characterLibrary.end())
                                            {
                                                std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Character with following name is already placed: '" << charData->name.toAnsiString() << '\'' << std::endl;
                                                
                                                //TODO: replaceCharacterIfExists as static ns::GlobalSettings
                                                bool replaceCharacterIfExists{ true }; //if not, then to the name should be added "name2" etc
                                                if (replaceCharacterIfExists)
                                                {
                                                    delete novel->library.characterLibrary.at(charData->name.toWideString());
                                                    novel->library.characterLibrary.emplace(charData->name.toWideString(), charData);
                                                }
                                            }
                                            else
                                                novel->library.characterLibrary.emplace(charData->name.toWideString(), charData);
                                        }
                                        wif.close();
                                    }
                                }
                            }
                            if (ent != NULL)
                                free(ent);
                            ent = NULL;
                            _closedir(dir);
                        }
                        else
                            std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Could not open directory: '" << fullpath.toAnsiString() << '\'' << std::endl;
                    currentFolder = currentFolder->next;
                }
                
                List<sf::String>* next{ nullptr };
                for (; folders != nullptr; folders = next)
                {
                    next = folders->next;
                    
                    delete folders->data;
                    delete folders;
                }
            }
            else
                std::cout << "Error :: NovelLibrary :: ScanForCharacters :: Novel or library not set: '" << '\'' << std::endl;
        }
        void NovelLibrary::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
    }
}
