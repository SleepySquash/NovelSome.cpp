//
//  Audio.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        void SoundPlayer::Update(const sf::Time& elapsedTime)
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
                        sound.setVolume(volume);
                        break;
                        
                    case playing:
                        if (!loop)
                            if (sound.getPlayingOffset().asSeconds() >= timeToStartDisappearing || sound.getStatus() == sf::Sound::Status::Stopped)
                            {
                                mode = disappearing;
                                if (novel != nullptr)
                                    if (sendMessageBack == atDisappearing)
                                        novel->UnHold(this);
                            }
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
                        sound.setVolume(volume);
                        break;
                        
                    case deprecated: this->GetNovelSystem()->PopComponent(this); break;
                    default: break;
                }
            }
        }
        void SoundPlayer::Destroy() {
            if (novel != nullptr) sc::DeleteSound(novel->GetFolderPath() + audioPath);
            if (novel != nullptr) novel->RemoveFromGroup(groupPointer); }
        void SoundPlayer::SetNovel(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                appearTime = novel->skin.sound.appearTime;
                disappearTime = novel->skin.sound.disappearTime;
                maxVolume = novel->skin.sound.maxVolume * ns::gs::maxVolumeGlobal * ns::gs::maxVolumeSound;
            }
        }
        void SoundPlayer::SetGroup(const list<SoundPlayer*>::iterator& element) { this->groupPointer = element; }
        void SoundPlayer::LoadFromFile(const sf::String& fileName)
        {
            audioLoaded = false;
            if (novel != nullptr)
            {
                sf::SoundBuffer* buffer = sc::LoadSound(novel->GetFolderPath() + fileName);
                if ((audioLoaded = (buffer != nullptr)))
                {
                    audioPath = fileName;
                    sound.setBuffer(*buffer);
                    sound.setLoop(loop);
                    sound.setVolume(0);
                    sound.setPlayingOffset(playingOffset);
                    sound.play();
                    
                    timeToStartDisappearing = (*buffer).getDuration().asSeconds() - disappearTime;
                    
                    currentTime = 0.f;
                }
                else
                {
                    if (sendMessageBack != noMessage) novel->UnHold(this);
                    this->GetNovelSystem()->PopComponent(this);
                }
                /*std::wstring fullPath = sf::String(resourcePath() + novel->GetFolderPath() + fileName);
                
                if (!base::FileExists(fullPath))
                {
                    std::wstring onlyFolder = base::GetFolderPath(fullPath);
                    std::wstring onlyFileName = L"";
                    for (int i = onlyFolder.length(); i < fullPath.length(); i++)
                        onlyFileName += fullPath[i];
                    
                    if (base::FileExists(onlyFolder + L"sounds/" + onlyFileName))
                        fullPath = onlyFolder + L"sounds/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"sound/" + onlyFileName))
                        fullPath = onlyFolder + L"sound/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"audio/" + onlyFileName))
                        fullPath = onlyFolder + L"audio/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Sounds/" + onlyFileName))
                        fullPath = onlyFolder + L"Sounds/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Sound/" + onlyFileName))
                        fullPath = onlyFolder + L"Sound/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Audio/" + onlyFileName))
                        fullPath = onlyFolder + L"Audio/" + onlyFileName;
                }
                
#ifdef _WIN32
                std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::ConvertToUTF8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    fileInMemory.reset(new char[static_cast<unsigned int>(filesize)]);
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory.get(), filesize);
                    ifStream.close();
                    
                    audioLoaded = soundBuffer.loadFromMemory(fileInMemory.get(), filesize);
                }
#else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath);
                if (!(audioLoaded = soundBuffer.loadFromFile(u8str)))
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#endif
                
                if (audioLoaded)
                {
                    audioPath = fileName;
                    sound.setBuffer(soundBuffer);
                    sound.setLoop(loop);
                    sound.setVolume(0);
                    sound.setPlayingOffset(playingOffset);
                    sound.play();
                    
                    timeToStartDisappearing = soundBuffer.getDuration().asSeconds() - disappearTime;
                    
                    currentTime = 0.f;
                }
                else
                {
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
                    this->GetNovelSystem()->PopComponent(this);
                }*/
            }
            else
                cout << "Error :: SoundPlayer :: LoadFromFile :: No novel was loaded, pointer is NULL" << endl;
        }
        void SoundPlayer::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
                if (newMode == disappearing)
                    if (novel != nullptr)
                        if (sendMessageBack == atDisappearing)
                            novel->UnHold(this);
            }
        }
        void SoundPlayer::SetLoop(bool setLoop) { loop = setLoop; }
        void SoundPlayer::SetPlayingOffset(sf::Time timeOffset) { playingOffset = timeOffset; }
        
        
        
        
        
        
        
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
                            if (music.getPlayingOffset().asSeconds() >= timeToStartDisappearing || music.getStatus() == sf::Sound::Status::Stopped)
                            {
                                mode = disappearing;
                                if (novel != nullptr)
                                    if (sendMessageBack == atDisappearing)
                                        novel->UnHold(this);
                            }
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
                        
                    case deprecated: this->GetNovelSystem()->PopComponent(this); break;
                    default: break;
                }
            }
        }
        void MusicPlayer::Destroy() { if (novel != nullptr) novel->RemoveFromGroup(groupPointer); }
        void MusicPlayer::SetNovel(Novel* novel)
        {
            this->novel = novel;
            if (novel != nullptr)
            {
                appearTime = novel->skin.music.appearTime;
                disappearTime = novel->skin.music.disappearTime;
                maxVolume = novel->skin.music.maxVolume * ns::gs::maxVolumeGlobal * ns::gs::maxVolumeMusic;
            }
        }
        void MusicPlayer::SetGroup(const list<MusicPlayer*>::iterator& element) { this->groupPointer = element; }
        void MusicPlayer::LoadFromFile(const sf::String& fileName)
        {
            audioLoaded = false;
            if (novel != nullptr)
            {
                std::wstring fullPath = sf::String(resourcePath()) + novel->GetFolderPath() + fileName;
                if (!base::FileExists(fullPath))
                {
                    std::wstring onlyFolder = base::GetFolderPath(fullPath);
                    std::wstring onlyFileName = L"";
                    for (int i = onlyFolder.length(); i < fullPath.length(); i++)
                        onlyFileName += fullPath[i];
                    
                    if (base::FileExists(onlyFolder + L"music/" + onlyFileName))
                        fullPath = onlyFolder + L"music/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"ambient/" + onlyFileName))
                        fullPath = onlyFolder + L"ambient/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"audio/" + onlyFileName))
                        fullPath = onlyFolder + L"audio/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Music/" + onlyFileName))
                        fullPath = onlyFolder + L"Music/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Ambient/" + onlyFileName))
                        fullPath = onlyFolder + L"Ambient/" + onlyFileName;
                    else if (base::FileExists(onlyFolder + L"Audio/" + onlyFileName))
                        fullPath = onlyFolder + L"Audio/" + onlyFileName;
                }
                
#ifdef _WIN32
                std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::ConvertToUTF8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    fileInMemory.reset(new char[static_cast<unsigned int>(filesize)]);
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory.get(), filesize);
                    ifStream.close();
                    
                    audioLoaded = music.openFromMemory(fileInMemory.get(), filesize);
                }
#else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath);
                if (!(audioLoaded = music.openFromFile(u8str)))
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#endif
                
                if (audioLoaded)
                {
                    audioPath = fileName;
                    music.setLoop(loop);
                    music.setVolume(0);
                    music.setPlayingOffset(playingOffset);
                    music.play();
                    
                    timeToStartDisappearing = music.getDuration().asSeconds() - disappearTime;
                    
                    currentTime = 0.f;
                }
                else
                {
                    if (sendMessageBack != noMessage)
                        novel->UnHold(this);
                    this->GetNovelSystem()->PopComponent(this);
                }
            }
            else
                cout << "Error :: MusicPlayer :: LoadFromFile :: No novel was loaded, pointer is NULL" << endl;
        }
        void MusicPlayer::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f;
                mode = newMode;
                if (newMode == disappearing)
                    if (novel != nullptr)
                        if (sendMessageBack == atDisappearing)
                            novel->UnHold(this);
            }
        }
        void MusicPlayer::SetLoop(bool setLoop) { loop = setLoop; }
        void MusicPlayer::SetPlayingOffset(sf::Time timeOffset) { playingOffset = timeOffset; }
    }
}
