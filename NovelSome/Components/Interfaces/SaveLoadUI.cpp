//
//  SaveLoadUI.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 02/06/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "SaveLoadUI.hpp"

namespace ns
{
    void SaveLoadUI::Init()
    {
        captionText.setFont(*fc::GetFont(L"Pacifica.ttf"));
        captionText.setOutlineColor(sf::Color::Black);
        backButton.setString(L"< " + lang::get("BACK"));
        backButton.setFont(L"Pacifica.ttf");
        backButton.setCharacterSize(78);
        backButton.valign = Valign::Bottom;
        backButton.halign = Halign::Left;
        blackScreenShape.setFillColor(sf::Color(0,0,0,170));
        saveloadButton.setFont(L"Pacifica.ttf");
        saveloadButton.setCharacterSize(70);
    }
    void SaveLoadUI::Destroy() { if (active) CleanUp(); }
    void SaveLoadUI::Update(const sf::Time& elapsedTime)
    {
        if (!active) return;
        switch (mode)
        {
            case appearing:      //currentTime = appearTime;
                if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                if (currentTime >= appearTime) { alpha = 255; currentTime = 0.f; mode = existing; }
                else alpha = (sf::Uint8)(255 * (currentTime / appearTime));
                UpdateAlpha(); gs::requestWindowRefresh = true;
                break;
                
            case disappearing:     //currentTime = disappearTime;
                if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                if (currentTime >= disappearTime) { active = false; CleanUp(); alpha = 0; currentTime = 0.f; mode = existing; }
                else alpha = (sf::Uint8)(255 - (255 * (currentTime / disappearTime)));
                UpdateAlpha(); gs::requestWindowRefresh = true;
                break;
            default: break;
        }
    }
    void SaveLoadUI::CleanUp() { /*ic::DeleteImage(L"Data/Neko/Person/Vanilla.png");*/ }
    void SaveLoadUI::PollEvent(sf::Event& event)
    {
        if (!active || !gs::isActiveInterface(this)) return;
        
        if (backButton.PollEvent(event) || (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)) entity->SendMessage({"SaveLoadUI :: Close"});
        else if (saveloadButton.PollEvent(event))
        {
            if (!available) return;
            try {
                if (loading) NovelComponents::LoadGameState(absolutePath, entity, novel);
                else         NovelComponents::SaveGameState(absolutePath, *system);
            } catch (std::exception& e) { cout << "Error :: SaveLoadUI :: " << e.what() << "." << endl; }
            entity->SendMessage({"SaveLoadUI :: Close"});
        }
    }
    void SaveLoadUI::Resize(const unsigned int& width, const unsigned int& height)
    {
        if (!active) return;
        
        blackScreenShape.setSize({(float)gs::width, (float)gs::height});
        backButton.Resize(width, height);
        backButton.setPosition(35*gs::scale, 3.8f*gs::height/4);
        captionText.setCharacterSize(120 * gs::scale);
        captionText.setOutlineThickness(gs::scale);
        captionText.setOrigin(captionText.getLocalBounds().width/2, captionText.getLocalBounds().height/1.3);
        captionText.setPosition(gs::width/2, gs::height/6);
        
        saveloadButton.Resize(width, height);
        saveloadButton.setPosition(gs::width/2, gs::height/2);
    }
    void SaveLoadUI::Draw(sf::RenderWindow* window)
    {
        if (!active || gs::ignoreDraw) return;
        
        window->draw(blackScreenShape);
        window->draw(captionText);
        backButton.draw(window);
        saveloadButton.draw(window);
    }
    void SaveLoadUI::ReceiveMessage(MessageHolder& message)
    {
        if (message.info == "SaveUI") { loading = false; captionText.setString(lang::get("Saving")); saveloadButton.setString(lang::get("Save")); Switch(true); }
        else if (message.info == "LoadUI") { loading = true; captionText.setString(lang::get("Loading")); saveloadButton.setString(lang::get("Load")); Switch(true); }
        else if (message.info == "SaveLoadUI :: Close") Switch(false);
        else if (message.info == "SaveUI :: Novel") novel = reinterpret_cast<NovelInfo*>(message.address);
        else if (message.info == "SaveUI :: System") system = reinterpret_cast<NovelSystem*>(message.address);
    }
    void SaveLoadUI::Switch(const bool& on)
    {
        if (on && !active)
        {
            std::wifstream wif;
#ifdef _WIN32
            wif.open(novel->path);
#else
            wif.open(base::utf8(novel->path));
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            
            std::wstring defaultname, id; bool idfound{ false };
            if (wif.is_open())
            {
                std::wstring line; nss::CommandSettings command;
                command.lowercaseCommand = false; bool read{ true }, foundAny{ false }; int linesRead{ 0 }, infoRead{ 0 };
                while (!wif.eof() && read)
                {
                    std::getline(wif, line); command.Command(line);
                    if (nss::Command(command, L"name ")) { defaultname = nss::ParseAsMaybeQuoteString(command); ++infoRead; }
                    else if (nss::Command(command, L"id ")) { id = nss::ParseAsMaybeQuoteString(command); idfound = true; ++infoRead; }
                    else if (nss::Command(command, L"name:")) foundAny = true; else if (foundAny) ++linesRead;
                    if (linesRead > 30) { defaultname = novel->name; read = false; }
                    if (infoRead >= 2) read = false;
                }
                wif.close();
            }
            if ((available = (defaultname.length() && defaultname != L"")))
            {
                std::wstring name = base::GetFilenameWOProhibited(defaultname);
                if (idfound) absolutePath = utf16(documentsPath()) + L"Saves/id/" + id/* + L" " + name*/ + L"/save.nsave";
                else absolutePath = utf16(documentsPath()) + L"Saves/local/" + name + L"/save.nsave";
                //cout << "ABSOLUTE PATH: " << utf8(absolutePath) << endl;
            }
            gs::PushInterface(this); active = true; mode = appearing; entity->SortAbove(this); Resize(gs::width, gs::height);
        } else if (!on && active) { gs::RemoveInterface(this); mode = disappearing; }
    }
    void SaveLoadUI::UpdateAlpha()
    {
        captionText.setFillColor(sf::Color(captionText.getFillColor().r, captionText.getFillColor().g, captionText.getFillColor().b, alpha));
        captionText.setOutlineColor(sf::Color(captionText.getOutlineColor().r, captionText.getOutlineColor().g, captionText.getOutlineColor().b, alpha));
        blackScreenShape.setFillColor(sf::Color(blackScreenShape.getFillColor().r, blackScreenShape.getFillColor().g, blackScreenShape.getFillColor().b, 170.f * ((float)alpha/255)));
        blackScreenShape.setOutlineColor(sf::Color(blackScreenShape.getOutlineColor().r, blackScreenShape.getOutlineColor().g, blackScreenShape.getOutlineColor().b, 170.f * ((float)alpha/255.f)));
        backButton.setAlpha(alpha); saveloadButton.setAlpha(alpha);
    }
    unsigned long SaveLoadUI::LoadSlots()
    {
        list<std::wstring> directories;
        directories.push_back(base::GetFolderPath(absolutePath));
        
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
                            if (extension == L".nsave") saves.push_back((*it) + fileName);
                        }
                        else directories.push_back((*it) + fileName + L"/");
                    }
                }
                _closedir(directory);
            }
        if (ent != NULL) free(ent);
        return saves.size();
    }
    void SaveLoadUI::ClearSlots()
    {
        saves.clear();
    }
}
