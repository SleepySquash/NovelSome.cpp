//
//  Audio.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Audio.hpp"

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
                        if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= appearTime) {
                            volume = maxVolume; currentTime = 0.f; mode = playing;
                            if (sendMessageBack == atAppearance) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (maxVolume * (currentTime / appearTime));
                        sound.setVolume(volume);
                        break;
                        
                    case playing:
                        if (!loop && (sound.getPlayingOffset().asSeconds() >= timeToStartDisappearing || sound.getStatus() == sf::Sound::Status::Stopped))
                        {
                            mode = disappearing;
                            if (sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
                        }
                        break;
                        
                    case disappearing:
                        if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= disappearTime) {
                            volume = 0.f; currentTime = 0.f; mode = deprecated;
                            if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (maxVolume - (maxVolume * (currentTime / disappearTime)));
                        sound.setVolume(volume);
                        break;
                        
                    case deprecated: novelSystem->PopComponent(this); break;
                    default: break;
                }
            }
        }
        void SoundPlayer::Destroy() { sc::DeleteSound(folderPath + audioPath); novelSystem->SendMessage({"Destroy", this}); }
        void SoundPlayer::LoadFromFile(const std::wstring& fileName)
        {
            audioLoaded = false;
            sf::SoundBuffer* buffer = sc::LoadSound(folderPath + fileName);
            if ((audioLoaded = buffer))
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
                if (sendMessageBack != noMessage) novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
        }
        void SoundPlayer::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == disappearing && sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
            }
        }
        
        
        
        
        
        
        
        void MusicPlayer::Update(const sf::Time& elapsedTime)
        {
            if (!audioLoaded) return;
                switch (mode)
                {
                    case appearing:
                        if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= appearTime) {
                            volume = maxVolume;
                            currentTime = 0.f;
                            mode = playing;
                            if (sendMessageBack == atAppearance) novelSystem->SendMessage({"UnHold", this});
                        }
                        else volume = (maxVolume * (currentTime / appearTime));
                        music.setVolume(volume);
                        break;
                    case playing:
                        if (!loop && (music.getPlayingOffset().asSeconds() >= timeToStartDisappearing || music.getStatus() == sf::Sound::Status::Stopped))
                        {
                            mode = disappearing;
                            if (sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
                        }
                        break;
                    case disappearing:
                        if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= disappearTime) {
                            volume = 0.f; currentTime = 0.f; mode = deprecated;
                            if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (maxVolume - (maxVolume * (currentTime / disappearTime)));
                        music.setVolume(volume);
                        break;
                    case deprecated: novelSystem->PopComponent(this); break;
                    default: break;
                }
        }
        void MusicPlayer::Destroy() { novelSystem->SendMessage({"Destroy", this}); }
        void MusicPlayer::LoadFromFile(const std::wstring& fileName)
        {
            audioLoaded = false;
            if ((audioLoaded = sc::LoadMusic(music, folderPath + fileName, fileInMemory)))
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
                if (sendMessageBack != noMessage) novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
        }
        void MusicPlayer::SetStateMode(modeEnum newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == disappearing && sendMessageBack == atDisappearing) novelSystem->SendMessage({"UnHold", this});
            }
        }
    }
}
