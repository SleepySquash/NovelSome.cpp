//
//  CharacterLibrary.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "CharacterLibrary.hpp"

namespace ns
{
    namespace NovelComponents
    {
        std::unordered_map<std::wstring, CharacterData*> CharacterLibrary::library;
        
        CharacterLibrary::CharacterLibrary() { }
        CharacterLibrary::~CharacterLibrary() { clear(); }
        void CharacterLibrary::clear()
        {
            for (const auto& key : library)
                if (key.second) delete key.second;
            library.clear();
        }
        std::unordered_map<std::wstring, CharacterData*>::const_iterator CharacterLibrary::find(const std::wstring& key) { return library.find(key); }
        bool CharacterLibrary::exists(const std::wstring& key) { return (library.find(key) != library.end()); }
        CharacterData* CharacterLibrary::at(const std::wstring& key) { return (exists(key)) ? library.at(key) : nullptr; }
        void CharacterLibrary::ScanForCharacters(const std::wstring& folderPath)
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
            
        std::wstring novelPath{ folderPath };
        if (!base::FileExists(novelPath)) novelPath = base::utf16(resourcePath()) + novelPath;
        
        list<std::wstring> folders;
        folders.push_back(L"");
        
        for (list<std::wstring>::iterator currentFolder = folders.begin(); currentFolder != folders.end(); ++currentFolder)
        {
            std::wstring fullpath{ novelPath + *currentFolder };
#ifdef _WIN32
            if ((dir = _opendir(fullpath.c_str())) != NULL)
#else
                if ((dir = _opendir(base::utf8(fullpath).c_str())) != NULL)
#endif
                {
                    while ((ent = _readdir(dir)) != NULL)
                    {
#ifdef _WIN32
                        std::wstring entryName{ (ent->d_name) };
#else
                        std::wstring entryName{ base::utf16(ent->d_name) };
#endif
                        if (entryName != L"." && entryName != L"..")
                        {
                            std::wstring extention = base::GetExtentionFromString(entryName);
                            if (ent->d_type == DT_DIR) folders.push_back(*currentFolder + entryName + L"/");
                            else if (extention == L".nschar")
                            {
                                std::wifstream wif;
                                std::wstring filePath = (fullpath + entryName);
#ifdef _WIN32
                                wif.open(filePath);
#else
                                wif.open(base::utf8(filePath));
#endif
                                wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                                
                                if (!wif.is_open())
                                    cout << "Warning :: NovelComponent :: File couldn't be opened, path: " << base::utf8(filePath) << endl;
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
                                                charData->name = nss::ParseAsMaybeQuoteString(command);
                                            else if (nss::Command(command, L"dname ") || nss::Command(command, L"display "))
                                                charData->display = nss::ParseAsMaybeQuoteString(command);
                                            else if (nss::Command(command, L"state "))
                                            {
                                                nss::ParseUntil(command, '"');
                                                std::wstring stateName = nss::ParseUntil(command, '"');
                                                //TODO: State parsing
                                            }
                                            else if (nss::Command(command, L"fill ") || nss::Command(command, L"fillcolor ") || nss::Command(command, L"color ") || nss::Command(command, L"colour "))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255)
                                                    charData->fillColor = possibleColor;
                                            }
                                            else if (nss::Command(command, L"outline ") || nss::Command(command, L"outlinecolor ") || nss::Command(command, L"ocolor ") || nss::Command(command, L"ocolour "))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255)
                                                    charData->outlineColor = possibleColor;
                                            }
                                            else if (nss::Command(command, L"thickness "))
                                            {
                                                std::wstring possibleThickness = nss::ParseUntil(command, '\0');
                                                if (possibleThickness.length() != 0) {
                                                    charData->outlineThickness = ns::base::atof(possibleThickness);
                                                    charData->thicknessSet = true; }
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
                                        charData->name = base::GetStringWithNoExtention(converter.from_bytes(entDNameString));
#endif
                                    }
                                    if (charData->display == L"") charData->display = charData->name;
                                    
                                    charData->filePath = std::wstring(*currentFolder) + entryName;
                                    if (library.find(charData->name) != library.end())
                                    {
                                        std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Character with following name is already placed: '" << utf8(charData->name) << '\'' << std::endl;
                                        
                                        //TODO: replaceCharacterIfExists as static ns::GlobalSettings
                                        bool replaceCharacterIfExists{ true }; //if not, then to the name should be added "name2" etc
                                        if (replaceCharacterIfExists)
                                        {
                                            delete library.at(charData->name);
                                            library.emplace(charData->name, charData);
                                        }
                                    }
                                    else library.emplace(charData->name, charData);
                                }
                                wif.close();
                            }
                        }
                    }
                    if (ent != NULL) free(ent);
                    ent = NULL;
                    _closedir(dir);
                }
                else std::cout << "Warning :: NovelLibrary :: ScanForCharacters :: Could not open directory: '" << base::utf8(fullpath) << '\'' << std::endl;
            }
        }
    }
}
