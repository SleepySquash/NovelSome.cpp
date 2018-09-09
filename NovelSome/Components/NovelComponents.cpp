//
//  NovelComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "NovelComponents.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void Waiting::Update(const sf::Time& elapsedTime)
        {
            if (novel != nullptr)
            {
                if (currentTime < waitingTime)
                    currentTime += elapsedTime.asSeconds();
                else
                {
                    novel->UnHold(this);
                    GetNovelSystem()->PopComponent(this);
                }
            }
        }
        void Waiting::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        
        
        
        void Background::LoadImage(sf::String path)
        {
            spriteLoaded = false;
            if (novel != nullptr)
            {
                sf::String fullPath = sf::String(resourcePath()) + novel->GetFolderPath() + path;
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath.toWideString());
                
                if (image.loadFromFile(u8str))
                {
                    imagePath = sf::String(path);
                    if (texture.loadFromImage(image))
                    {
                        spriteLoaded = true;
                        texture.setSmooth(true);
                        sprite.setTexture(texture);
                        
                        Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
                    }
                }
                
                if (!spriteLoaded)
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void Background::Resize(unsigned int width, unsigned int height)
        {
            CalculateScale(width, height);
        }
        void Background::Update(const sf::Time& elapsedTime)
        {
            switch (mode)
            {
                case appearing:
                    if (currentTime < appearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha;
                        currentTime = 0.f;
                        mode = afterAppearSwitchTo;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atAppearance)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case disappearing:
                    if (currentTime < disappearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = deprecated;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDeprecated)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, alpha));
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
                    break;
                    
                default:
                    break;
            }
        }
        void Background::Draw(sf::RenderWindow* window)
        {
            if (spriteLoaded && visible)
                window->draw(sprite);
        }
        void Background::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void Background::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void Background::SetGroup(List<Background>* element)
        {
            this->groupPointer = element;
        }
        void Background::CalculateScale(unsigned int width, unsigned int height)
        {
            if (spriteLoaded)
            {
                float scaleFactorX, scaleFactorY, scaleFactor;
                scaleFactorX = (float)width / image.getSize().x;
                scaleFactorY = (float)height / image.getSize().y;
                switch (fitMode)
                {
                    case defaultFit:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        break;
                        
                    case fillCentre:
                        scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                        sprite.setScale(scaleFactor, scaleFactor);
                        sprite.setPosition((float)width/2 - sprite.getLocalBounds().width/2*scaleFactor - sprite.getOrigin().x*scaleFactor,
                                           (float)height/2 - sprite.getLocalBounds().height/2*scaleFactor - sprite.getOrigin().y*scaleFactor);
                        break;
                        
                    case stretch:
                        sprite.setScale(scaleFactorX, scaleFactorY);
                        break;
                        
                    default:
                        break;
                }
            }
        }
        void Background::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
            }
        }
        
        
        
        void Dialogue::Update(const sf::Time& elapsedTime)
        {
            switch (mode)
            {
                case appearing:
                    if (currentTime < appearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= appearTime)
                    {
                        alpha = maxAlpha;
                        currentTime = 0.f;
                        mode = afterAppearSwitchTo;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atAppearance)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha * (currentTime / appearTime));
                    
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
                    }
                    break;
                    
                case disappearing:
                    if (currentTime < disappearTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= disappearTime)
                    {
                        alpha = 0;
                        currentTime = 0.f;
                        mode = deprecated;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDeprecated)
                                novel->UnHold(this);
                    }
                    else
                        alpha = (sf::Int8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                    
                    text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, alpha));
                    shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, alpha));
                    if (drawCharacterName)
                    {
                        charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
                        charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
                    }
                    break;
                    
                case deprecated:
                    this->GetNovelSystem()->PopComponent(this);
                    break;
                    
                case waitingForTime:
                    if (currentTime < waitingTime)
                        currentTime += elapsedTime.asSeconds();
                    
                    if (currentTime >= waitingTime)
                    {
                        currentTime = 0.f;
                        mode = disappearing;
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDisappearing)
                                novel->UnHold(this);
                    }
                    break;
                    
                default:
                    break;
            }
        }
        void Dialogue::PollEvent(sf::Event& event)
        {
            if (mode == waitingForInput)
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                {
                    mode = (!forcePressInsideDialogue || (event.mouseButton.x > 0 && event.mouseButton.x < ns::GlobalSettings::width && event.mouseButton.y < ns::GlobalSettings::height && event.mouseButton.y > ns::GlobalSettings::height - ns::GlobalSettings::height/5)) ? disappearing : waitingForInput;
                    
                    if (mode == disappearing)
                    {
                        event = sf::Event();
                        
                        if (novel != nullptr)
                            if (sendMessageBack == atDisappearing)
                                novel->UnHold(this);
                    }
                }
        }
        void Dialogue::Draw(sf::RenderWindow* window)
        {
            window->draw(shape);
            if (fontLoaded)
            {
                window->draw(text);
                if (drawCharacterName)
                {
                    window->draw(charShape);
                    window->draw(charText);
                }
            }
        }
        void Dialogue::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void Dialogue::Resize(unsigned int width, unsigned int height)
        {
            text.setPosition(30, height - height/5 + 10);
            shape.setPosition(0, height - height/5);
            shape.setSize({static_cast<float>(width), static_cast<float>(height/5)});
            
            if (drawCharacterName)
            {
                charText.setPosition(35, height - height/5 - 10 - 10 - charText.getLocalBounds().height);
                charShape.setPosition(30, height - height/5 - 10 - 5 - charText.getLocalBounds().height);
                charShape.setSize({static_cast<float>(charText.getLocalBounds().width + 15), static_cast<float>(charText.getLocalBounds().height + 10)});
            }
        }
        void Dialogue::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void Dialogue::SetCharacter(sf::String character)
        {
            charString = character;
            drawCharacterName = true;
            
            charText.setString(charString);
            charText.setFont(ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            charText.setCharacterSize(characterSize);
            charText.setFillColor(sf::Color::White);
            charShape.setFillColor(sf::Color(0,0,0,150));
            
            charText.setFillColor(sf::Color(charText.getFillColor().r, charText.getFillColor().g, charText.getFillColor().b, alpha));
            charShape.setFillColor(sf::Color(charShape.getFillColor().r, charShape.getFillColor().g, charShape.getFillColor().b, alpha));
            
            Resize(ns::GlobalSettings::width, ns::GlobalSettings::height);
        }
        void Dialogue::SetGroup(List<Dialogue>* element)
        {
            this->groupPointer = element;
        }
        void Dialogue::SetDialogue(sf::String dialogue)
        {
            text.setString(dialogue);
            text.setFont(ns::FontCollector::GetFont(fontName));
            fontLoaded = (text.getFont() != nullptr);
            text.setCharacterSize(characterSize);
            text.setFillColor(sf::Color::White);
            shape.setFillColor(sf::Color(0,0,0,150));
        }
        void Dialogue::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
            }
        }
        
        
        
        void MusicPlayer::Update(const sf::Time& elapsedTime)
        {
            if (audioLoaded)
            {
                switch (mode)
                {
                    case appearing:
                        if (currentTime < appearTime)
                            currentTime += elapsedTime.asSeconds();
                        
                        if (currentTime >= appearTime)
                        {
                            volume = maxVolume;
                            currentTime = 0.f;
                            mode = playing;
                            
                            if (novel != nullptr)
                                if (sendMessageBack == atAppearance)
                                    novel->UnHold(this);
                        }
                        else
                            volume = (maxVolume * (currentTime / appearTime));
                        music.setVolume(volume);
                        break;
                        
                    case playing:
                        if (!loop)
                            if (music.getPlayingOffset().asSeconds() >= timeToStartDisappearing)
                                mode = disappearing;
                        break;
                        
                    case disappearing:
                        if (currentTime < disappearTime)
                            currentTime += elapsedTime.asSeconds();
                        
                        if (currentTime >= disappearTime)
                        {
                            volume = 0.f;
                            currentTime = 0.f;
                            mode = deprecated;
                            
                            if (novel != nullptr)
                                if (sendMessageBack == atDeprecated)
                                    novel->UnHold(this);
                        }
                        else
                            volume = (maxVolume - (maxVolume * (currentTime / disappearTime)));
                        music.setVolume(volume);
                        break;
                        
                    case deprecated:
                        this->GetNovelSystem()->PopComponent(this);
                        break;
                        
                    default:
                        break;
                }
            }
        }
        void MusicPlayer::Destroy()
        {
            if (groupPointer != nullptr && novel != nullptr)
                novel->RemoveFromGroup(groupPointer);
        }
        void MusicPlayer::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void MusicPlayer::SetGroup(List<MusicPlayer>* element)
        {
            this->groupPointer = element;
        }
        void MusicPlayer::LoadFromFile(sf::String fileName)
        {
            audioLoaded = false;
            if (novel != nullptr)
            {
                sf::String fullPath = sf::String(resourcePath() + novel->GetFolderPath() + fileName);
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath.toWideString());
                
                if ((audioLoaded = (music.openFromFile(u8str))))
                {
                    audioPath = fileName;
                    music.setLoop(loop);
                    music.setVolume(0);
                    music.play();
                    
                    timeToStartDisappearing = music.getDuration().asSeconds() - disappearTime;
                    
                    currentTime = 0.f;
                }
                else if (sendMessageBack != noMessage)
                    novel->UnHold(this);
            }
            else
                cout << "Error :: BackgroundComponent :: LoadImage :: No novel was loaded, pointer is NULL" << endl;
        }
        void MusicPlayer::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
            }
        }
        
        
        
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
                                        newFolder->data = new sf::String(entryName);
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
                                                        nss::ParseUntil(command, '"');
                                                        std::wstring nameParsed = nss::ParseUntil(command, '"');
                                                        charData->name = nameParsed;
                                                    }
                                                    else if (nss::Command(command, L"dname ") || nss::Command(command, L"display "))
                                                    {
                                                        nss::ParseUntil(command, '"');
                                                        std::wstring nameParsed = nss::ParseUntil(command, '"');
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
        
        
        
        Novel::Novel(sf::String path) : nsdataPath(path)
        {
            folderPath = ns::base::GetFolderPath(path);
            
            library.SetNovel(this);
            library.ScanForCharacters();
         
            sf::String filePath = (resourcePath() + path);
#ifdef _WIN32
            wif.open(filePath.toWideString());
#else
            std::wstring _wpath = filePath;
            std::string _path(_wpath.begin(), _wpath.end());
            wif.open(_path);
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            if (!(fileOpened = wif.is_open()))
                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
        }
        Novel::~Novel()
        {
            wif.close();
            layers.Destroy();
            
            FreeGroup<Background>(backgroundGroup);
            FreeGroup<Character>(characterGroup);
            FreeGroup<Dialogue>(dialogueGroup);
            FreeGroup<SoundPlayer>(soundGroup);
            FreeGroup<MusicPlayer>(musicGroup);
            FreeGroup<GUISystem>(GUIGroup);
            
            FreeGroup<NovelObject>(onHold);
        }
        void Novel::Update(const sf::Time& elapsedTime)
        {
            while (fileOpened && !eof && (onHold == nullptr || onHoldSize == 0) &&
                   !(ns::GlobalSettings::isPause && ns::GlobalSettings::isPauseEnabled))
            {
                if (wif.fail() || wif.bad())
                    cout << "Warning :: NovelComponent :: Stream.fail() or Stream.bad() caught: " << nsdataPath.toAnsiString() << endl;
                
                if (!wif.eof())
                {
                    std::getline(wif, line);
                    command.Command(line);
                    
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    if (nss::Command(command, L"\""))
                    {
                        command.lastPos = command.lastPos - 1;
                        std::wstring dialogueLine = nss::ParseAsQuoteString(command);
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000);
                        OnHold(component);
                        
                        component->SetNovel(this);
                        component->fontName = "NotoSansCJK-Regular.ttc";
                        component->characterSize = 40;
                        component->forcePressInsideDialogue = true;
                        component->afterAppearSwitchTo = component->waitingForInput;
                        component->sendMessageBack = component->atDisappearing;
                        
                        if (arguments != nullptr)
                            for (int i = 0; arguments[i] != nullptr; i++)
                            {
                                nss::CommandSettings argument;
                                argument.Command(arguments[i]);
                                
                                if (nss::Command(argument, L"fade:"))
                                {
                                    float value = nss::ArgumentAsFloat(argument);
                                    component->appearTime = value;
                                    component->disappearTime = value;
                                }
                                else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:"))
                                    component->appearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:"))
                                    component->disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"alpha:") || nss::Command(argument, L"maxalpha:"))
                                    component->maxAlpha = nss::ArgumentAsInt(argument);
                                
                                delete arguments[i];
                            }
                        if (arguments != nullptr)
                            delete arguments;
                        
                        dialogueGroup = ns::list::Insert<ns::NovelComponents::Dialogue>(dialogueGroup);
                        dialogueGroup->data = component;
                        component->SetGroup(dialogueGroup);
                        
                        component->SetDialogue(dialogueLine);
                    }
                    ///----------------------------------------MISC----------------------------------------
                    ///----------------------------------------MISC----------------------------------------
                    ///----------------------------------------MISC----------------------------------------
                    else if (nss::Command(command, L"wait "))
                    {
                        nss::SkipSpaces(command);
                        float amount = ArgumentAsFloat(command);
                        
                        if (amount > 0.f)
                        {
                            auto* component = layers.PrioritizeComponent<Waiting>(-32000);
                            OnHold(component);
                            
                            component->SetNovel(this);
                            component->waitingTime = amount;
                        }
                        else
                            cout << "Warning :: NovelComponent :: Couldn't init \"wait\" command for " << amount << " seconds." << endl;
                    }
                    ///--------------------------------------BACKGROUND--------------------------------------
                    ///--------------------------------------BACKGROUND--------------------------------------
                    ///--------------------------------------BACKGROUND--------------------------------------
                    else if (nss::Command(command, L"background hide"))
                    {
                        if (backgroundGroup != nullptr)
                        {
                            List<Background>* temp = backgroundGroup;
                            wchar_t** arguments = nss::ParseArguments(command);
                            
                            float disappearTime{ -1.f };
                            if (arguments != nullptr)
                                for (int i = 0; arguments[i] != nullptr; i++)
                                {
                                    nss::CommandSettings argument;
                                    argument.Command(arguments[i]);
                                    
                                    if (nss::Command(argument, L"fade:")||
                                        nss::Command(argument, L"fadeout:") ||
                                        nss::Command(argument, L"disappear:"))
                                        disappearTime = nss::ArgumentAsFloat(argument);
                                    
                                    free(arguments[i]);
                                }
                            if (arguments != nullptr)
                                free(arguments);
                            
                            while (temp != nullptr)
                            {
                                if (temp->data != nullptr)
                                {
                                    OnHold(temp->data);
                                    temp->data->sendMessageBack = temp->data->atDeprecated;
                                    temp->data->SetStateMode(temp->data->disappearing);
                                    if (disappearTime >= 0)
                                        temp->data->disappearTime = disappearTime;
                                }
                                temp = temp->next;
                            }
                        }
                    }
                    else if (nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Background>(0);
                        OnHold(component);
                        
                        component->SetNovel(this);
                        component->fitMode = component->fillCentre;
                        
                        if (arguments != nullptr)
                            for (int i = 0; arguments[i] != nullptr; i++)
                            {
                                nss::CommandSettings argument;
                                argument.Command(arguments[i]);
                                
                                if (nss::Command(argument, L"fade:"))
                                {
                                    float value = nss::ArgumentAsFloat(argument);
                                    component->appearTime = value;
                                    component->disappearTime = value;
                                }
                                else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:"))
                                    component->appearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:"))
                                    component->disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"alpha:") || nss::Command(argument, L"maxalpha:"))
                                    component->maxAlpha = nss::ArgumentAsInt(argument);
                                else if (nss::Command(argument, L"fit:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"fill")
                                        component->fitMode = component->fillCentre;
                                    else if (stringValue == L"center")
                                        component->fitMode = component->fillCentre;
                                    else if (stringValue == L"fillCenter")
                                        component->fitMode = component->fillCentre;
                                    else if (stringValue == L"default")
                                        component->fitMode = component->defaultFit;
                                    else if (stringValue == L"no")
                                        component->fitMode = component->noFit;
                                    else if (stringValue == L"noFit")
                                        component->fitMode = component->noFit;
                                    else if (stringValue == L"stretch")
                                        component->fitMode = component->stretch;
                                }
                                
                                free(arguments[i]);
                            }
                        if (arguments != nullptr)
                            free(arguments);
                        
                        backgroundGroup = ns::list::Insert<ns::NovelComponents::Background>(backgroundGroup);
                        backgroundGroup->data = component;
                        component->SetGroup(backgroundGroup);
                        
                        component->LoadImage(filePath);
                    }
                    else if (nss::Command(command, L"music stop") || nss::Command(command, L"ambient stop"))
                    {
                        if (musicGroup != nullptr)
                        {
                            List<MusicPlayer>* temp = musicGroup;
                            wchar_t** arguments = nss::ParseArguments(command);
                            
                            float disappearTime{ -1.f };
                            if (arguments != nullptr)
                                for (int i = 0; arguments[i] != nullptr; i++)
                                {
                                    nss::CommandSettings argument;
                                    argument.Command(arguments[i]);
                                    
                                    if (nss::Command(argument, L"fade:")||
                                        nss::Command(argument, L"fadeout:") ||
                                        nss::Command(argument, L"disappear:"))
                                        disappearTime = nss::ArgumentAsFloat(argument);
                                    
                                    free(arguments[i]);
                                }
                            if (arguments != nullptr)
                                free(arguments);
                            
                            while (temp != nullptr)
                            {
                                if (temp->data != nullptr)
                                {
                                    //OnHold(temp->data);
                                    temp->data->sendMessageBack = temp->data->atDeprecated;
                                    temp->data->SetStateMode(temp->data->disappearing);
                                    if (disappearTime >= 0)
                                        temp->data->disappearTime = disappearTime;
                                }
                                temp = temp->next;
                            }
                        }
                    }
                    else if (nss::Command(command, L"music ") || nss::Command(command, L"ambient "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::MusicPlayer>(0);
                        component->SetNovel(this);
                        
                        if (arguments != nullptr)
                            for (int i = 0; arguments[i] != nullptr; i++)
                            {
                                nss::CommandSettings argument;
                                argument.Command(arguments[i]);
                                
                                if (nss::Command(argument, L"fade:"))
                                {
                                    float value = nss::ArgumentAsFloat(argument);
                                    component->appearTime = value;
                                    component->disappearTime = value;
                                }
                                else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:"))
                                    component->appearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:"))
                                    component->disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"volume:") || nss::Command(argument, L"maxvolume:"))
                                    component->maxVolume = nss::ArgumentAsFloat(argument);
                                
                                free(arguments[i]);
                            }
                        if (arguments != nullptr)
                            free(arguments);
                        
                        musicGroup = ns::list::Insert<ns::NovelComponents::MusicPlayer>(musicGroup);
                        musicGroup->data = component;
                        component->SetGroup(musicGroup);
                        
                        component->LoadFromFile(filePath);
                    }
                    else
                    {
                        //TODO: Parse if character's replic or variable
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            std::wstring possibleDialogue = nss::ParseAsQuoteString(command);
                            if (possibleDialogue.length() != 0)
                            {
                                sf::String characterName = (library.characterLibrary.find(possibleName) != library.characterLibrary.end()) ? library.characterLibrary.at(possibleName)->displayName : sf::String(possibleName);
                                wchar_t** arguments = nss::ParseArguments(command);
                                
                                auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000);
                                OnHold(component);
                                
                                component->SetNovel(this);
                                component->fontName = "NotoSansCJK-Regular.ttc";
                                component->characterSize = 40;
                                component->forcePressInsideDialogue = true;
                                component->afterAppearSwitchTo = component->waitingForInput;
                                component->sendMessageBack = component->atDisappearing;
                                
                                if (arguments != nullptr)
                                    for (int i = 0; arguments[i] != nullptr; i++)
                                    {
                                        nss::CommandSettings argument;
                                        argument.Command(arguments[i]);
                                        
                                        if (nss::Command(argument, L"fade:"))
                                        {
                                            float value = nss::ArgumentAsFloat(argument);
                                            component->appearTime = value;
                                            component->disappearTime = value;
                                        }
                                        else if (nss::Command(argument, L"fadein:") || nss::Command(argument, L"appear:"))
                                            component->appearTime = nss::ArgumentAsFloat(argument);
                                        else if (nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:"))
                                            component->disappearTime = nss::ArgumentAsFloat(argument);
                                        else if (nss::Command(argument, L"alpha:") || nss::Command(argument, L"maxalpha:"))
                                            component->maxAlpha = nss::ArgumentAsInt(argument);
                                        
                                        delete arguments[i];
                                    }
                                if (arguments != nullptr)
                                    delete arguments;
                                
                                dialogueGroup = ns::list::Insert<ns::NovelComponents::Dialogue>(dialogueGroup);
                                dialogueGroup->data = component;
                                component->SetGroup(dialogueGroup);
                                
                                component->SetCharacter(characterName);
                                component->SetDialogue(possibleDialogue);
                            }
                        }
                    }
                }
                else
                    eof = true;
            }
            
            if (fileOpened && !(ns::GlobalSettings::isPause && ns::GlobalSettings::isPauseEnabled))
                layers.Update(elapsedTime);
        }
        void Novel::Draw(sf::RenderWindow* window)
        {
            if (fileOpened)
                layers.Draw(window);
        }
        void Novel::Resize(unsigned int width, unsigned int height)
        {
            if (fileOpened)
                layers.Resize(width, height);
        }
        void Novel::PollEvent(sf::Event& event)
        {
            if (fileOpened && !(ns::GlobalSettings::isPause))
                layers.PollEvent(event);
        }
        void Novel::OnHold(NovelObject* component)
        {
            onHold = ns::list::Insert<ns::NovelObject>(onHold);
            onHold->data = component;
            
            onHoldSize++;
        }
        void Novel::UnHold(NovelObject* component)
        {
            List<NovelObject>* temp = onHold;
            while (temp != nullptr)
            {
                List<NovelObject>* next = temp->next;
                if (temp->data == component)
                {
                    if (temp == onHold)
                        onHold = temp->next;
                    if (temp->next != nullptr)
                        temp->next->prev = temp->prev;
                    if (temp->prev != nullptr)
                        temp->prev->next = temp->next;
                    delete temp;
                    
                    onHoldSize--;
                    next = nullptr;
                }
                
                temp = next;
            }
        }
        sf::String Novel::GetFolderPath()
        {
            return folderPath;
        }
        void Novel::RemoveFromGroup(List<Background>* groupPointer)
        {
            if (groupPointer == backgroundGroup)
            {
                backgroundGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<Dialogue>* groupPointer)
        {
            if (groupPointer == dialogueGroup)
            {
                dialogueGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<Character>* groupPointer)
        {
            if (groupPointer == characterGroup)
            {
                characterGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<SoundPlayer>* groupPointer)
        {
            if (groupPointer == soundGroup)
            {
                soundGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<MusicPlayer>* groupPointer)
        {
            if (groupPointer == musicGroup)
            {
                musicGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<GUISystem>* groupPointer)
        {
            if (groupPointer == GUIGroup)
            {
                GUIGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
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
