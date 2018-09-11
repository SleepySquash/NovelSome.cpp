//
//  Audio.cpp
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
                    music.setPlayingOffset(playingOffset);
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
                if (newMode == disappearing)
                    if (novel != nullptr)
                        if (sendMessageBack == atDisappearing)
                            novel->UnHold(this);
            }
        }
        void MusicPlayer::SetLoop(bool setLoop)
        {
            loop = setLoop;
        }
        void MusicPlayer::SetPlayingOffset(sf::Time timeOffset)
        {
            playingOffset = timeOffset;
        }
    }
}