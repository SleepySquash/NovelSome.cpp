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
        void EventListener::ReceiveMessage(MessageHolder& message) { if (message.info == "UnHold" || message.info == "Destroy" || message.info == "Push" || message.info == "Execute" || message.info == "ExecuteNow" || message.info == "ExecuteInsert" || message.info == "ChooseUnHold" || message.info == "Save" || message.info == "Load") novel->ForwardMessage(message);
            else if (message.info == "FinishedExecute" || message.info == "GamePause") novel->gamePause->ReceiveMessage(message);
            else if (message.info == "GamePause :: Return to menu") novel->entity->ReceiveMessage(message);
        }
        void EventListener::Save(std::wofstream& wof) { novel->Save(wof); }
        
        Novel::Novel(const std::wstring& path, NovelInfo* nvl) : nsdataPath(path), nvl(nvl) { }
        Novel::Novel(NovelInfo* nvl) : nvl(nvl) { }
        void Novel::Init()
        {
            layers.PrioritizeComponent<EventListener>(-31000, this);
            
            scenarioPath = nsdataPath;
            if (!(nsdataPath.length() == 0 || nsdataPath == L"") && nvl) nsdataPath = nvl->path;
            if ((nsdataPath.length() == 0 || nsdataPath == L"") && nvl)
            {
                std::wstring scenario = L"";
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
                
                if (scenario != L"")
                {
                    //std::wstring path = nvl->path + scenario;
                    scenarioPath = base::GetFolderPath(nvl->path) + scenario;
                    nsdataPath = nvl->path;
                }
            }
            
            folderPath = base::GetFolderPath(nsdataPath);
            
            VariableSystem::localVariables.insert({L"@dialogue", new NovelVariable(std::wstring(L""))});
            VariableSystem::localVariables.insert({L"@name", new NovelVariable(std::wstring(L""))});
            VariableSystem::localVariables.insert({L"version", new NovelVariable(std::wstring(L"Update 0 build 18"))});
            CharacterLibrary::ScanForCharacters(folderPath);
            interface.guiChoose.novelSystem = interface.guiPause.novelSystem = interface.guiDialogue.novelSystem = &layers;
            
            gamePause = entity->AddComponent<GamePause>(&interface.guiPause);
            std::wstring skinPath = L"skin.nskin";
            if (nvl)
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
                        if (nss::Command(command, L"skin ")) { skinPath = nss::ParseAsMaybeQuoteString(command); done = true; }
                    }
                    wifn.close();
                }
            }
            
            if (Skin::self) delete Skin::self;
            Skin::self = new Skin();
            Skin::self->LoadFromFile(folderPath + skinPath);
            interface.LoadFromFile(folderPath + skinPath);
            
            
            std::wstring filePath = scenarioPath;
            if (!base::FileExists(scenarioPath)) filePath = base::utf16(resourcePath()) + filePath;
#ifdef _WIN32
            wif.open(filePath);
#else
            wif.open(base::utf8(filePath));
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
            
            if (!(fileOpened = wif.is_open()))
                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << base::utf8(scenarioPath) << endl;
        }
        Novel::~Novel()
        {
            entity->SendMessage(MessageHolder("NovelComponents :: Novel :: Destroying"));
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
                std::getline(wif, line);
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
                    sc::PreloadSound(folderPath + filePath);
                }
                
                if (shouldPush) lines.push_front(line);
            }
        }
        void Novel::Draw(sf::RenderWindow* window) { if (fileOpened) layers.Draw(window); }
        void Novel::Resize(const unsigned int& width, const unsigned int& height) { if (fileOpened) layers.Resize(width, height); }
        void Novel::PollEvent(sf::Event& event) { if (fileOpened && !gs::isPause) layers.PollEvent(event); }
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
                wof << L"folderPath: " << folderPath << endl;
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
            else if (message.info == "Save")
            {
                SaveGameState(utf16(documentsPath()) + L"Saves/Bundle/save.nsave", layers);
                message = MessageHolder();
            }
            else if (message.info == "Load")
            {
                LoadGameState(utf16(documentsPath()) + L"Saves/Bundle/save.nsave", layers);
                message = MessageHolder();
            }
        }
        void Novel::ReceiveMessage(MessageHolder &message) { if (message.info == "GamePause :: Return to menu") entity->PopComponent(this); }
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
        
        
        
        
        
        
        
        
        void LoadGameState(std::wstring fileName, NovelSystem& system)
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
                std::wstring line; nss::CommandSettings command;
                while (!wif.eof())
                {
                    std::getline(wif, line); command.Command(line);
                    if (nss::Command(command, L"tag("))
                    {
                        int layer = base::atoi(nss::ParseUntil(command, L')'));
                        if (nss::Command(command, L"=background"))
                        {
                            Background* background = new Background();
                            background->Load(wif);
                            delete background;
                        }
                        else if (nss::Command(command, L"=character"))
                        {
                            Character* character = new Character();
                            character->Load(wif);
                            delete character;
                        }
                    }
                }
                wif.close();
            }
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

/*void NovelLibrary::ScanForCharacters()
 {
 if (novel != nullptr)
 {
 DIR *dir;
 struct dirent *ent;
 
 sf::String path{ executablePath() + novel->GetFolderPath() };
 if ((dir = opendir(path.toAnsiString().c_str())) != NULL)
 {
 while ((ent = readdir (dir)) != NULL)
 std::cout << "Entry found: " << ent->d_name << std::endl;
 closedir (dir);
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Could not open directory: '" << path.toAnsiString() << '\'' << std::endl;
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Novel not set: '" << '\'' << std::endl;
 }*/
