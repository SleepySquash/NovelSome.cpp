//
//  NovelComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        EventListener::EventListener(Novel* novel) : novel(novel), Savable(L"Novel") { }
        void EventListener::ReceiveMessage(MessageHolder& message)
        {
            if (message.info == "UnHold" || message.info == "Destroy" || message.info == "Push" || message.info == "Execute" || message.info == "ExecuteNow" || message.info == "ExecuteInsert" || message.info == "ChooseUnHold" || message.info == "Save" || message.info == "Load") novel->ForwardMessage(message);
            else if (message.info == "FinishedExecute" || message.info == "GamePause") novel->gamePause->ReceiveMessage(message);
            else if (message.info == "GamePause :: Return to menu") novel->entity->ReceiveMessage(message);
        }
        void EventListener::Save(std::wofstream& wof) { novel->Save(wof); }
        
        Novel::Novel(const std::wstring& path, NovelInfo* nvl) : scenario(path), nvl(nvl) { }
        Novel::Novel(NovelInfo* nvl) : nvl(nvl) { }
        void Novel::Init()
        {
            layers.PrioritizeComponent<EventListener>(-31000, this);
            
            nsdataPath = nvl->path;
            if ((scenario.length() == 0 || scenario == L"") && nvl)
            {
                std::wifstream wifn; bool done{ false };
#ifdef _WIN32
                wifn.open(nvl->path);
#else
                wifn.open(base::utf8(nvl->path));
#endif
                wifn.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
                
                if (wifn.is_open())
                {
                    std::wstring line; nss::CommandSettings command;
                    while (!wifn.eof() && !done)
                    {
                        std::getline(wifn, line); command.Command(line);
                        if (nss::Command(command, L"scenario ")) { scenario = nss::ParseAsMaybeQuoteString(command); done = true; }
                    }
                    wifn.close();
                }
            }
            
            folderPath = base::GetFolderPath(nsdataPath);
            scenarioPath = folderPath + scenario;
            
            VariableSystem::localVariables.insert({L"@dialogue", new NovelVariable(std::wstring(L""))});
            VariableSystem::localVariables.insert({L"@name", new NovelVariable(std::wstring(L""))});
            //VariableSystem::localVariables.insert({L"version", new NovelVariable(std::wstring(L"Update 0 build 20"))});
            CharacterLibrary::ScanForCharacters(folderPath);
            interface.guiChoose.novelSystem = interface.guiPause.novelSystem = interface.guiDialogue.novelSystem = &layers;
            
            gamePause = entity->AddComponent<GamePause>(&interface.guiPause);
            std::wstring skinPath = L"skin.nskin";
            if (Skin::self) delete Skin::self;
            Skin::self = new Skin();
            Skin::self->folderScope = folderPath;
            if (nvl)
            {
                std::wifstream wifn; bool done{ false };
#ifdef _WIN32
                wifn.open(nsdataPath);
#else
                wifn.open(base::utf8(nsdataPath));
#endif
                wifn.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
                
                if (wifn.is_open())
                {
                    std::wstring line; int remainingLines{ 20 }; nss::CommandSettings command;
                    while (!wifn.eof() && remainingLines > 0)
                    {
                        std::getline(wifn, line); command.Command(line);
                        if (nss::Command(command, L"skin "))
                        {
                            skinPath = nss::ParseAsMaybeQuoteString(command);
                            Skin::self->LoadFromFile(folderPath + skinPath);
                            Skin::self->defaultDialogue = Skin::self->defaultChoose = Skin::self->defaultMusic = Skin::self->defaultSound = Skin::self->defaultBackground = Skin::self->defaultCharacter = Skin::self->defaultAmbient = skinPath;
                            done = true;
                        }
                        else
                        {
                            // if (done) --remainingLines; // TODO: it's not really required since .novel files are not long currently.
                            if (nss::Command(command, L"dialogue "))
                            {
                                std::wstring path = nss::ParseAsMaybeQuoteString(command);
                                Skin::self->LoadFromFile(folderPath + path, L"dialogue"); Skin::self->defaultDialogue = path;
                                interface.ignoreDialogue = interface.guiDialogue.LoadFromFile(folderPath + path, L"dialogue");
                                interface.guiDialogue.trueFileName = interface.defaultDialogue = path;
                                remainingLines = 20;
                            }
                            else if (nss::Command(command, L"choose "))
                            {
                                std::wstring path = nss::ParseAsMaybeQuoteString(command);
                                Skin::self->LoadFromFile(folderPath + path, L"choose"); Skin::self->defaultChoose = path;
                                interface.ignoreChoose = interface.guiChoose.LoadFromFile(folderPath + path, L"choose");
                                interface.guiChoose.trueFileName = interface.defaultChoose = path;
                                remainingLines = 20;
                            }
                            else if (nss::Command(command, L"gamepause ") || nss::Command(command, L"pause "))
                            {
                                std::wstring path = nss::ParseAsMaybeQuoteString(command);
                                interface.ignorePause = interface.guiPause.LoadFromFile(folderPath + path, L"gamepause");
                                interface.guiPause.trueFileName = interface.defaultPause = path;
                                remainingLines = 20;
                            }
                        }
                    }
                    wifn.close();
                }
            }
            interface.LoadFromFile(skinPath, folderPath);
            
            
            entity->SendMessage({"SaveUI :: Novel", nvl});
            entity->SendMessage({"SaveUI :: System", &layers});
            
            
            std::wstring filePath = scenarioPath;
            if (!base::FileExists(filePath)) filePath = base::utf16(resourcePath()) + filePath;
            if (!base::FileExists(filePath)) filePath = folderPath + L"scenario/" + scenario;
            if (!base::FileExists(filePath)) filePath = base::utf16(resourcePath()) + filePath;
            if (!base::FileExists(filePath)) filePath = folderPath + L"Scenario/" + scenario;
            if (!base::FileExists(filePath)) filePath = base::utf16(resourcePath()) + filePath;
#ifdef _WIN32
            wif.open(filePath);
#else
            wif.open(base::utf8(filePath));
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            
            if (!(fileOpened = wif.is_open())) { cout << "Error :: NovelComponent :: File couldn't be opened, path: " << base::utf8(scenarioPath) << endl; if (!noDestroyMessage) entity->SendMessage(MessageHolder("Novel :: Destroying")); }
            else entity->SendMessage(MessageHolder("Novel :: Starting"));
        }
        Novel::~Novel()
        {
            if (!noDestroyMessage) entity->SendMessage(MessageHolder("Novel :: Destroying"));
            if (gamePause) entity->PopComponent(gamePause);
            VariableSystem::clear();
            CharacterLibrary::clear();
            
            wif.close(); layers.clear();
            if (Skin::self) { delete Skin::self; Skin::self = nullptr; }
            
            FreeGroup<Background>(backgroundGroup);
            FreeGroup<Character>(characterGroup);
            FreeGroup<Dialogue>(dialogueGroup);
            FreeGroup<SoundPlayer>(soundGroup);
            FreeGroup<MusicPlayer>(musicGroup);
            FreeGroup<GUISystem>(GUIGroup);
            
            FreeGroup<NovelObject>(onHold);
        }
        // TODO: Make it possible to ignore some lines of novel
        void Novel::ResourcesPreloading(list<std::wstring>& lines, std::wstring& line)
        {
            while (lines.size() <= preloadLinesAmount && !wif.eof())
            {
                std::getline(wif, line); ++position; // cout << utf8(scenario) << " : " << utf8(line) << endl;
                if (line.length() && line[line.length() - 1] == 13) line.erase(line.begin() + (line.size() - 1));
                
                nss::CommandSettings settings;
                settings.Command(line);
                
                bool shouldPush{ true };
                if (nss::Command(settings, L"choose")) { lines.push_front(line); break; }
                else if (nss::Command(settings, L"//")) shouldPush = false;
                else if (!nss::ContainsUsefulInformation(line)) shouldPush = false;
                else if (nss::Command(settings, L"background hide")) { }
                else if (nss::Command(settings, L"hide ")) { }
                else if (nss::Command(settings, L"sound stop")) { }
                else if (nss::Command(settings, L"background add ") || nss::Command(settings, L"background "))
                {
                    std::wstring filePath = nss::ParseAsQuoteString(settings);
                    ic::PreloadTexture(folderPath + filePath, 1, true, &layers);
                }
                else if (nss::Command(settings, L"show "))
                {
                    std::wstring possibleName = nss::ParseUntil(settings, ' ');
                    if (possibleName.length() != 0)
                    {
                        if (CharacterLibrary::exists(possibleName))
                        {
                            std::wstring state{ L"" };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(settings, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")) { }
                                else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:")) { }
                                else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:")) { }
                                else if (nss::Command(argument, L"alpha:") || nss::Command(argument, L"maxalpha:")) { }
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:")) { }
                                else
                                {
                                    std::wstring possibleStateOrPos;
                                    if (argument.line[0] == L'"')
                                        possibleStateOrPos = nss::ParseAsQuoteString(argument);
                                    else possibleStateOrPos = argument.line;
                                    
                                    if (state.length() == 0) state = possibleStateOrPos;
                                }
                            }
                            
                            CharacterData* characterData = CharacterLibrary::at(possibleName);
                            sf::String fullPath = sf::String(resourcePath()) + folderPath + characterData->filePath;
                            std::wstring lookForSpritePath = base::GetFolderPath(folderPath + characterData->filePath);
                            std::wstring spritePath{ L"" };
                            
                            std::wifstream wifc;
#ifdef _WIN32
                            wifc.open(fullPath.toWideString());
#else
                            std::wstring _wpath = fullPath;
                            std::string _path(_wpath.begin(), _wpath.end());
                            wifc.open(_path);
#endif
                            wifc.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                            
                            if (wifc.is_open())
                            {
                                bool eofc{ false };
                                std::wstring linec;
                                nss::CommandSettings commandc;
                                bool stateReading{ false };
                                
                                while (!eofc)
                                {
                                    if (!wifc.eof())
                                    {
                                        std::getline(wifc, linec); commandc.Command(linec);
                                        if (!stateReading)
                                        {
                                            if (nss::Command(commandc, L"state ") || nss::Command(commandc, L"sprite "))
                                            {
                                                nss::SkipSpaces(commandc); std::wstring stateName;
                                                if (commandc.line[commandc.lastPos] == L'"')
                                                    stateName = nss::ParseAsQuoteString(commandc);
                                                else stateName = nss::ParseUntil(commandc, ' ');
                                                
                                                stateReading = (stateName == state) ? true : false;
                                            }
                                        } else {
                                            if (nss::Command(commandc, L"state ") || nss::Command(commandc, L"end") || nss::Command(commandc, L"}"))
                                                stateReading = false;
                                            else if (nss::Command(commandc, L"image ") || nss::Command(commandc, L"sprite "))
                                                spritePath = nss::ParseAsQuoteString(commandc);
                                            else if (nss::Command(commandc, L"\""))
                                            {
                                                commandc.lastPos--;
                                                spritePath = nss::ParseAsQuoteString(commandc);
                                            }
                                            
                                            eofc = stateReading ? false : true;
                                        }
                                    } else eofc = true;
                                }
                            }
                            wifc.close();
                            if (spritePath.length() != 0) ic::PreloadTexture(lookForSpritePath + spritePath, 2, true, &layers);
                        }
                    }
                }
                else if (nss::Command(settings, L"sound "))
                {
                    std::wstring filePath = nss::ParseAsQuoteString(settings);
                    // TODO: sc::PreloadSound(folderPath + filePath);
                    // TODO: Fix crashing due to thread.
                }
                
                if (shouldPush) lines.push_front(line);
            }
        }
        void Novel::Draw(sf::RenderWindow* window) { if (fileOpened && !gs::ignoreDraw) layers.Draw(window); }
        void Novel::Resize(const unsigned int& width, const unsigned int& height) { if (fileOpened) layers.Resize(width, height); }
        void Novel::PollEvent(sf::Event& event) { if (fileOpened && !gs::isPause && !gs::ignoreEvent) layers.PollEvent(event); }
            /*if (fileOpened) {
                if (!gs::isPause) {
                    layers.PollEvent(event);
                    interface.guiDialogue.PollEvent(event);
                    if (interface.guiChoose.alpha != sf::Uint8(0)) interface.guiChoose.PollEvent(event);
                } else if (interface.guiPause.alpha != sf::Uint8(0)) interface.guiPause.PollEvent(event);
            }*/
        void Novel::Save(std::wofstream& wof)
        {
            if (fileOpened)
            {
                wof << L"s: " << scenario << endl;
                wof << L"position: " << position << endl;
                if (!lines.empty())
                {
                    wof << L"b1" << endl;
                    for (auto l : lines) wof << l << endl;
                    wof << L"e" << endl;
                }
                if (!execute.empty())
                {
                    wof << L"b2" << endl;
                    for (auto& l : execute) wof << l << endl;
                    wof << L"e" << endl;
                }
            }
            if (interface.guiDialogue.trueFileName != interface.defaultDialogue) wof << L"i_dialogue: " << interface.guiDialogue.trueFileName << endl;
            if (interface.guiPause.trueFileName != interface.defaultPause) wof << L"i_pause: " << interface.guiPause.trueFileName << endl;
            if (interface.guiChoose.trueFileName != interface.defaultChoose) wof << L"i_choose: " << interface.guiChoose.trueFileName << endl;
            if (Skin::self)
            {
                if (Skin::self->changedDialogue) wof << L"s_dialogue: " << Skin::self->defaultDialogue << endl;
                if (Skin::self->changedChoose) wof << L"s_choose: " << Skin::self->defaultChoose << endl;
                if (Skin::self->changedBackground) wof << L"s_background: " << Skin::self->defaultBackground << endl;
                if (Skin::self->changedCharacter) wof << L"s_character: " << Skin::self->defaultCharacter << endl;
                if (Skin::self->changedMusic) wof << L"s_music: " << Skin::self->defaultMusic << endl;
                if (Skin::self->changedSound) wof << L"s_sound: " << Skin::self->defaultSound << endl;
                if (Skin::self->changedAmbient) wof << L"s_ambient: " << Skin::self->defaultAmbient << endl;
            }
            if (!var::localVariables.empty())
            {
                bool atLeastOneUseful{ false };
                for (auto& v : var::localVariables)
                    if (v.first != L"@name" && v.first != L"@dialogue")
                    {
                        if (!atLeastOneUseful) { wof << L"b3" << endl; atLeastOneUseful = true; }
                        wof << v.first << endl << v.second->type << endl;
                        switch (v.second->type)
                        {
                            case NovelVariable::NotSet: break;
                            case NovelVariable::Integer: wof << v.second->value.asInt << endl; break;
                            case NovelVariable::Boolean: wof << (v.second->value.asBoolean ? L"1" : L"0") << endl; break;
                            case NovelVariable::String: wof << std::wstring(v.second->value.asString) << endl; break;
                        }
                    }
                if (atLeastOneUseful) wof << L"e" << endl;
            }
        }
        void Novel::ForwardMessage(MessageHolder& message)
        {
            if (message.info == "UnHold") UnHold(reinterpret_cast<NovelObject*>(message.address));
            else if (message.info == "Destroy")
            {
                std::list<Background*>::iterator it1 = std::find(backgroundGroup.begin(), backgroundGroup.end(), message.address);
                if (it1 != backgroundGroup.end()) backgroundGroup.erase(it1);
                
                std::list<Character*>::iterator it2 = std::find(characterGroup.begin(), characterGroup.end(), message.address);
                if (it2 != characterGroup.end()) characterGroup.erase(it2);
                
                std::list<Dialogue*>::iterator it3 = std::find(dialogueGroup.begin(), dialogueGroup.end(), message.address);
                if (it3 != dialogueGroup.end()) dialogueGroup.erase(it3);
                
                std::list<Choose*>::iterator it4 = std::find(chooseGroup.begin(), chooseGroup.end(), message.address);
                if (it4 != chooseGroup.end()) chooseGroup.erase(it4);
                
                std::list<MusicPlayer*>::iterator it5 = std::find(musicGroup.begin(), musicGroup.end(), message.address);
                if (it5 != musicGroup.end()) musicGroup.erase(it5);
                
                std::list<SoundPlayer*>::iterator it6 = std::find(soundGroup.begin(), soundGroup.end(), message.address);
                if (it6 != soundGroup.end()) soundGroup.erase(it6);
                
                std::list<MusicPlayer*>::iterator it8 = std::find(ambientGroup.begin(), ambientGroup.end(), message.address);
                if (it8 != ambientGroup.end()) ambientGroup.erase(it8);
                
                std::list<GUISystem*>::iterator it7 = std::find(GUIGroup.begin(), GUIGroup.end(), message.address);
                if (it7 != GUIGroup.end()) GUIGroup.erase(it7);
            }
            else if (message.info == "Push") lines.push_back(message.additional);
            else if (message.info == "ExecuteInsert") { if (execute.empty()) executePosInsert = execute.end(); else executePosInsert = execute.begin(); }
            else if (message.info == "Execute") {++executeOnHold; execute.insert(executePosInsert, message.additional); }
            else if (message.info == "ChooseUnHold")
            {
                UnHold(reinterpret_cast<NovelObject*>(message.address));
                if (dialogueGroup.size() != 0)
                    for (auto d : dialogueGroup)
                        if (d->mode == Dialogue::modeEnum::waitingForChoose)
                            d->SetStateMode(Dialogue::modeEnum::disappearing);
            }
            else if (message.info == "Save") entity->SendMessage({"SaveUI"});
            else if (message.info == "Load") entity->SendMessage({"LoadUI"});
        }
        void Novel::ReceiveMessage(MessageHolder &message)
        {
            if (message.info == "GamePause :: Return to menu") entity->PopComponent(this);
            else if (message.info == "GamePause")
            {
                for (auto& s : soundGroup) s->ReceiveMessage(message);
                for (auto& s : musicGroup) s->ReceiveMessage(message);
                for (auto& s : ambientGroup) s->ReceiveMessage(message);
            }
            else if (nss::Command(message.info, "Request"))
            {
                layers.ReceiveMessage(message);
                interface.guiDialogue.ReceiveMessage(message);
                interface.guiChoose.ReceiveMessage(message);
                interface.guiPause.ReceiveMessage(message);
            }
            else if (nss::Command(message.info, "DestroyNovel")) { gs::isPause = false; noDestroyMessage = true; entity->PopComponent(this); }
        }
        void Novel::OnHold(NovelObject* component) { onHold.push_front(component); if (!execute.empty()) onExecute.push_front(component); }
        void Novel::UnHold(NovelObject* component)
        {
            std::list<NovelObject*>::iterator it = std::find(onHold.begin(), onHold.end(), component);
            if (it != onHold.end()) onHold.erase(it);
            if (!onExecute.empty())
            {
                std::list<NovelObject*>::iterator it = std::find(onExecute.begin(), onExecute.end(), component);
                if (it != onExecute.end()) onExecute.erase(it);
                if (execute.empty() && onExecute.empty()) layers.SendMessage({"FinishedExecute"});
            }
        }
        
        
        
        
        
        
        
        
        void LoadGameState(std::wstring fileName, void* entityaddress, void* nvl)
        {
            if (entityaddress && nvl)
            {
                Entity* entity  = reinterpret_cast<Entity*>(entityaddress);
                if (entity)
                {
                    entity->SendMessage({"DestroyNovel"});
                    NovelInfo* nvlinfo  = reinterpret_cast<NovelInfo*>(nvl);
                    if (nvlinfo) entity->AddComponent<NovelLoader>(fileName, nvlinfo);
                    else throw std::runtime_error("Couldn't load from file specified :: nvlinfo is null");
                } else throw std::runtime_error("Couldn't load from file specified :: entityaddress is null");
            }
        }
        NovelLoader::NovelLoader(const std::wstring& fileName, NovelInfo* nvl) : fileName(fileName), nvl(nvl) { }
        void NovelLoader::Update(const sf::Time& elapsedTime)
        {
            if (!base::FileExists(fileName)) fileName = utf16(resourcePath()) + fileName;
            std::wifstream wif;
#ifdef _WIN32
            wif.open(fileName);
#else
            wif.open(base::utf8(fileName));
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            if (wif.is_open())
            {
                Novel* novel = nullptr;
                std::wstring line; nss::CommandSettings command;
                std::getline(wif, line); command.Command(line);
                while (!wif.eof())
                {
                    if (nss::Command(command, L"tag("))
                    {
                        int layer = base::atoi(nss::ParseUntil(command, L')'));
                        if (nss::Command(command, L"=novel"))
                        {
                            bool done{ false };
                            while (!done)
                            {
                                std::getline(wif, line); command.Command(line);
                                if (nss::Command(command, L"tag(")) { done = true; command.lastPos = 0; }
                                else if (nss::Command(command, L"folderpath: ")) novel->folderPath = nss::ParseUntil(command, L'\n');
                                else if (nss::Command(command, L"scenariopath: ")) novel->scenarioPath = nss::ParseUntil(command, L'\n');
                                else if (nss::Command(command, L"s: ")) novel = entity->AddComponent<Novel>(nss::ParseUntil(command, L'\n'), nvl);
                                else if (nss::Command(command, L"position: ")) novel->position = static_cast<unsigned long>(nss::ParseAsLong(command));
                                else if (nss::Command(command, L"i_dialogue: ")) {
                                    std::wstring path = nss::ParseUntil(command, L'\n');
                                    novel->interface.guiDialogue.clear();
                                    novel->interface.guiDialogue.LoadFromFile(novel->folderPath + path, L"dialogue");
                                    novel->interface.guiDialogue.trueFileName = path;
                                    // Skin::self->RestoreToDefaults(L"dialogue");
                                    Skin::self->LoadFromFile(novel->folderPath + path, L"dialogue"); }
                                else if (nss::Command(command, L"i_pause: ")) {
                                    std::wstring path = nss::ParseUntil(command, L'\n');
                                    novel->interface.guiPause.clear();
                                    novel->interface.guiPause.LoadFromFile(novel->folderPath + path, L"gamepause");
                                    novel->interface.guiPause.trueFileName = path; }
                                else if (nss::Command(command, L"i_choose: ")) {
                                    std::wstring path = nss::ParseUntil(command, L'\n');
                                    novel->interface.guiChoose.clear();
                                    novel->interface.guiChoose.LoadFromFile(novel->folderPath + path, L"choose");
                                    novel->interface.guiChoose.trueFileName = path;
                                    // Skin::self->RestoreToDefaults(L"choose");
                                    Skin::self->LoadFromFile(novel->folderPath + path, L"choose"); }
                                else if (nss::Command(command, L"s_dialogue: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultDialogue != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"dialogue");
                                        Skin::self->changedDialogue = true; Skin::self->defaultDialogue = path; } }
                                else if (nss::Command(command, L"s_choose: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultChoose != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"choose");
                                        Skin::self->changedChoose = true; Skin::self->defaultChoose = path; } }
                                else if (nss::Command(command, L"s_background: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultBackground != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"background");
                                        Skin::self->changedBackground = true; Skin::self->defaultBackground = path; } }
                                else if (nss::Command(command, L"s_character: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultCharacter != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"character");
                                        Skin::self->changedCharacter = true; Skin::self->defaultCharacter = path; } }
                                else if (nss::Command(command, L"s_music: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultMusic != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"music");
                                        Skin::self->changedMusic = true; Skin::self->defaultMusic = path; } }
                                else if (nss::Command(command, L"s_ambient: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultAmbient != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"ambient");
                                        Skin::self->changedAmbient = true; Skin::self->defaultAmbient = path; } }
                                else if (nss::Command(command, L"s_sound: ")) { std::wstring path = nss::ParseUntil(command, L'\n');
                                    if (Skin::self->defaultSound != path) {
                                        Skin::self->LoadFromFile(novel->folderPath + path, L"sound");
                                        Skin::self->changedSound = true; Skin::self->defaultSound = path; } }
                                else if (nss::Command(command, L"b1")) { std::getline(wif, line);
                                    while (line != L"e" && !wif.eof()) { novel->lines.push_back(line); std::getline(wif, line); } }
                                else if (nss::Command(command, L"b2")) { std::getline(wif, line);
                                    while (line != L"e" && !wif.eof()) { novel->execute.push_back(line); std::getline(wif, line); } }
                                else if (nss::Command(command, L"b3"))
                                {
                                    std::getline(wif, line); command.Command(line);
                                    while (line != L"e" && !wif.eof())
                                    {
                                        std::wstring name = nss::ParseUntil(command, L'\n'); NovelVariable *nvar;
                                        if (var::localVariables.find(name) == var::localVariables.end()) {
                                            nvar = new NovelVariable(); var::localVariables.insert({name, nvar});
                                        } else nvar = var::localVariables.at(name);
                                        std::getline(wif, line); command.Command(line);
                                        int type = nss::ParseAsInt(command);
                                        std::getline(wif, line); command.Command(line);
                                        if (type == 1 || type == 2 || type == 3)
                                        {
                                            std::wstring svalue = nss::ParseUntil(command, L'\n');
                                            switch (type)
                                            {
                                                case 1: nvar->Set(base::atoi(svalue)); break;
                                                case 2: nvar->Set(svalue); break;
                                                case 3: nvar->Set(base::atob(svalue)); break;
                                                default: break;
                                            }
                                            std::getline(wif, line); command.Command(line);
                                        }
                                    }
                                }
                            }
                            
                            if (novel->fileOpened)
                            {
                                unsigned long i = 0;
                                for (; i < novel->position; ++i)
                                    novel->wif.ignore(std::numeric_limits<std::streamsize>::max(), wif.widen('\n'));
                            }
                            else cout << "Error :: Loading :: Couldn't find scenario with path: \"" << utf8(novel->scenarioPath) << "\"." << endl;
                        }
                        else if (nss::Command(command, L"=background"))
                        {
                            Background* component = novel->layers.PrioritizeComponent<Background>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->background.appearTime;
                                component->disappearTime = Skin::self->background.disappearTime;
                                component->maxAlpha = Skin::self->background.maxAlpha;
                                component->doParallax = Skin::self->background.doParallax;
                                component->parallaxPower = Skin::self->background.parallaxPower; }
                            component->folder = novel->folderPath;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->backgroundGroup.insert(novel->backgroundGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=dialogue"))
                        {
                            Dialogue* component = novel->layers.PrioritizeComponent<Dialogue>(layer, &novel->interface.guiDialogue);
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->dialogueGroup.insert(novel->dialogueGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=character"))
                        {
                            Character* component = novel->layers.PrioritizeComponent<Character>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->character.appearTime;
                                component->disappearTime = Skin::self->character.disappearTime;
                                component->maxAlpha = Skin::self->character.maxAlpha;
                                component->doParallax = Skin::self->character.doParallax;
                                component->parallaxPower = Skin::self->character.parallaxNormal; }
                            component->folder = novel->folderPath;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->characterGroup.insert(novel->characterGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=guisystem"))
                        {
                            GUISystem* component = novel->layers.PrioritizeComponent<GUISystem>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->character.appearTime;
                                component->disappearTime = Skin::self->character.disappearTime;
                                component->maxAlpha = Skin::self->character.maxAlpha; }
                            component->isNovel = true; component->folderPath = novel->folderPath;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->GUIGroup.insert(novel->GUIGroup.begin(), component);
                            component->ResetResize(); component->Resize(gs::width, gs::height);
                        }
                        else if (nss::Command(command, L"=music"))
                        {
                            MusicPlayer* component = novel->layers.PrioritizeComponent<MusicPlayer>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->music.appearTime;
                                component->disappearTime = Skin::self->music.disappearTime;
                                component->maxVolume = Skin::self->music.maxVolume; }
                            component->folderPath = novel->folderPath;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->musicGroup.insert(novel->musicGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=ambient"))
                        {
                            MusicPlayer* component = novel->layers.PrioritizeComponent<MusicPlayer>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->ambient.appearTime;
                                component->disappearTime = Skin::self->ambient.disappearTime;
                                component->maxVolume = Skin::self->ambient.maxVolume; }
                            component->folderPath = novel->folderPath; component->ambient = true;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->ambientGroup.insert(novel->ambientGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=sound"))
                        {
                            SoundPlayer* component = novel->layers.PrioritizeComponent<SoundPlayer>(layer);
                            if (Skin::self) {
                                component->appearTime = Skin::self->sound.appearTime;
                                component->disappearTime = Skin::self->sound.disappearTime;
                                component->maxVolume = Skin::self->sound.maxVolume; }
                            component->folderPath = novel->folderPath;
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->soundGroup.insert(novel->soundGroup.begin(), component);
                        }
                        else if (nss::Command(command, L"=choose"))
                        {
                            Choose* component = novel->layers.PrioritizeComponent<Choose>(layer, &novel->interface.guiChoose);
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                            novel->chooseGroup.insert(novel->chooseGroup.begin(), component);
                            component->InitChoose();
                        }
                        else if (nss::Command(command, L"=wait"))
                        {
                            Waiting* component = novel->layers.PrioritizeComponent<Waiting>(layer);
                            std::pair<std::wstring, bool> pr = component->Load(wif);
                            line = pr.first; command.Command(line); if (pr.second) novel->OnHold(component);
                        }
                        else { std::getline(wif, line); command.Command(line); }
                    } else { std::getline(wif, line); command.Command(line); }
                }
                for (auto it = novel->dialogueGroup.begin(); it != novel->dialogueGroup.end(); ++it)
                {
                    Dialogue* component = *it;
                    TemporarySettings::dialogue = &component->text;
                    TemporarySettings::name = &component->charText;
                    
                    if (Skin::self)
                    {
                        if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = true;
                        if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = true;
                        if (Skin::self->dialogue.nameRectH) Skin::self->dialogue.nameRectH->ignoreVariableChange = true;
                        if (Skin::self->dialogue.nameRectV) Skin::self->dialogue.nameRectV->ignoreVariableChange = true;
                    
                        if (!component->drawCharacterName && std::wstring(var::localVariables.at(L"@name")->value.asString) != L"")
                        {
                            if (Skin::self->dialogue.nameRectH) {
                                Skin::self->dialogue.nameRectH->SetFadings(GUIObject::disappearing, component->disappearTime);
                                Skin::self->dialogue.nameRectH->currentTime = component->currentTime; }
                            if (Skin::self->dialogue.nameRectV) {
                                Skin::self->dialogue.nameRectV->SetFadings(GUIObject::disappearing, component->disappearTime);
                                Skin::self->dialogue.nameRectV->currentTime = component->currentTime; }
                        }
                        if (component->drawCharacterName && std::wstring(var::localVariables.at(L"@name")->value.asString) == L"")
                        {
                            if (Skin::self->dialogue.nameRectH) {
                                Skin::self->dialogue.nameRectH->SetFadings(GUIObject::appearing, component->appearTime);
                                Skin::self->dialogue.nameRectH->currentTime = component->currentTime; }
                            if (Skin::self->dialogue.nameRectV) {
                                Skin::self->dialogue.nameRectV->SetFadings(GUIObject::appearing, component->appearTime);
                                Skin::self->dialogue.nameRectV->currentTime = component->currentTime; }
                        }
                    }
                    novel->LocalVariables_Set(L"@dialogue", component->textString);
                    novel->LocalVariables_Set(L"@name", component->charString);
                    if (Skin::self)
                    {
                        if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = false;
                        if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = false;
                        if (Skin::self->dialogue.nameRectH) Skin::self->dialogue.nameRectH->ignoreVariableChange = false;
                        if (Skin::self->dialogue.nameRectV) Skin::self->dialogue.nameRectV->ignoreVariableChange = false;
                    }
                    
                    component->SetDialogue(component->textString);
                    if (component->noguiSystem) { component->Draw(gs::window); component->guiSystem = nullptr; }
                }
                wif.close();
            } else { cout << "NovelLoading :: Can't open save file: " << utf8(fileName) << "." << endl; entity->SendMessage(MessageHolder("Novel :: Destroying")); }
            entity->PopComponent(this);
        }
        
        
        
        
        
        
        void Novel::VariableChange(const std::wstring& name)
        {
            interface.guiDialogue.VariableChange(name);
            interface.guiChoose.VariableChange(name);
            interface.guiPause.VariableChange(name);
            if (GUIGroup.size() != 0) for (auto g : GUIGroup) g->VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, std::wstring value)
        {
            NovelVariable *nvar;
            if (var::localVariables.find(name) == var::localVariables.end())
            {
                nvar = new NovelVariable();
                var::localVariables.insert({name, nvar});
            }
            else nvar = var::localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
            {
                if (nvar->type == nvar->String) callChange = (nvar->value.asString != value);
                else callChange = true;
            }
            
            nvar->Set(value);
            if (callChange) VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, bool value)
        {
            NovelVariable *nvar;
            if (var::localVariables.find(name) == var::localVariables.end())
            {
                nvar = new NovelVariable();
                var::localVariables.insert({name, nvar});
            }
            else nvar = var::localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Boolean)))
                    callChange = (nvar->value.asBoolean != value);
            
            nvar->Set(value);
            if (callChange) VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, int value)
        {
            NovelVariable *nvar;
            if (var::localVariables.find(name) == var::localVariables.end())
            {
                nvar = new NovelVariable();
                var::localVariables.insert({name, nvar});
            }
            else nvar = var::localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Integer)))
                    callChange = (nvar->value.asInt != value);
            
            nvar->Set(value);
            if (callChange) VariableChange(name);
        }
    }
}
