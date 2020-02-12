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
        SoundPlayer::SoundPlayer() : Savable(L"Sound") { }
        void SoundPlayer::Update(const sf::Time& elapsedTime)
        {
            if (audioLoaded)
            {
                switch (mode)
                {
                    case Mode::Appear:
                        if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= appearTime) {
                            volume = rmaxVolume; currentTime = 0.f; mode = Mode::Exist;
                            if (messageBack == MessageBack::AtAppearance) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (rmaxVolume * (currentTime / appearTime));
                        sound.setVolume(volume);
                        break;
                        
                    case Mode::Exist:
                        if (!loop && (sound.getPlayingOffset().asSeconds() >= timeToStartDisappearing || sound.getStatus() == sf::Sound::Status::Stopped))
                        {
                            mode = Mode::Disapper;
                            if (messageBack == MessageBack::AtDisappearance) novelSystem->SendMessage({"UnHold", this});
                        }
                        break;
                        
                    case Mode::Disapper:
                        if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= disappearTime) {
                            volume = 0.f; currentTime = 0.f; mode = Mode::Deprecate;
                            if (messageBack == MessageBack::AtDeprecated) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (rmaxVolume - (rmaxVolume * (currentTime / disappearTime)));
                        sound.setVolume(volume);
                        break;
                        
                    case Mode::Deprecate: novelSystem->PopComponent(this); break;
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
                
                rmaxVolume = maxVolume * gs::maxVolumeGlobal * gs::maxVolumeSound;
                timeToStartDisappearing = (*buffer).getDuration().asSeconds() - disappearTime;
                currentTime = 0.f;
            }
            else
            {
                if (messageBack != MessageBack::No) novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
        }
        void SoundPlayer::SetStateMode(const Mode& newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == Mode::Disapper && messageBack == MessageBack::AtDisappearance)
                    novelSystem->SendMessage({"UnHold", this});
            }
        }
        void SoundPlayer::ReceiveMessage(MessageHolder& message)
        {
            if (message.info == "GamePause") { if (gs::isPause) sound.pause(); else sound.play(); }
        }
        void SoundPlayer::Save(std::wofstream& wof)
        {
            if (audioLoaded)
            {
                wof << L"path: " << nss::GetPathWOResourcePath(audioPath) << endl;
                wof << L"offset: " << sound.getPlayingOffset().asSeconds() << endl;
                //if (timeToStartDisappearing) wof << L"ttsd: " << timeToStartDisappearing << endl;
                if (loop) wof << L"loop: " << loop << endl;
                if ((!Skin::self && maxVolume != 100) || (Skin::self && maxVolume != Skin::self->sound.maxVolume)) wof << L"maxVolume: " << maxVolume << endl;
                
                if (mode != Mode::Exist)
                {
                    wof << L"mode: " << (int)mode << endl;
                    if (mode == Mode::Appear || mode == Mode::Disapper) wof << L"currentTime: " << currentTime << endl;
                    if (mode == Mode::Appear && ((!Skin::self && appearTime != 1.f) || (Skin::self && appearTime != Skin::self->sound.appearTime))) wof << L"appearTime: " << appearTime << endl;
                }
                if ((!Skin::self && disappearTime != 1.f) || (Skin::self && disappearTime != Skin::self->sound.disappearTime )) wof << L"disappearTime: " << disappearTime << endl;
                
            }
        }
        std::pair<std::wstring, bool> SoundPlayer::Load(std::wifstream& wif)
        {
            mode = Mode::Exist; std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"path: ")) audioPath = nss::ParseUntil(command, L'\n'); //imageName
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = Mode::Appear; break;
                        case 1: mode = Mode::Disapper; break;
                        case 3: mode = Mode::Deprecate; break;
                        default: mode = Mode::Exist; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"offset: ")) playingOffset = sf::seconds(nss::ParseAsFloat(command));
                else if (nss::Command(command, L"loop: ")) loop = nss::ParseAsBool(command);
                else if (nss::Command(command, L"maxvolume: ")) maxVolume = nss::ParseAsInt(command);
                else if (nss::Command(command, L"send: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: messageBack = MessageBack::No; break;
                        case 2: messageBack = MessageBack::AtDisappearance; break;
                        case 3: messageBack = MessageBack::AtDeprecated; break;
                        default: messageBack = MessageBack::AtAppearance; break;
                    }
                }
                if (wif.eof()) done = true;
            }
            
            float c = currentTime; LoadFromFile(audioPath); currentTime = c;
            if (mode == Mode::Appear) volume = (rmaxVolume * (currentTime / appearTime));
            else if (mode == Mode::Disapper) volume = (rmaxVolume - (rmaxVolume * (currentTime / disappearTime)));
            else volume = rmaxVolume; sound.setVolume(volume);
            bool onHold{ !((messageBack == MessageBack::No) || (messageBack == MessageBack::AtAppearance && (mode == Mode::Exist || mode == Mode::Disapper || mode == Mode::Deprecate)) || (messageBack == MessageBack::AtDisappearance && (mode == Mode::Disapper || mode == Mode::Deprecate))) };
            
            return { line, onHold };
        }
        
        
        
        
        
        
        
        MusicPlayer::MusicPlayer() : Savable(L"Music") { }
        void MusicPlayer::Update(const sf::Time& elapsedTime)
        {
            if (!audioLoaded) return;
                switch (mode)
                {
                    case Mode::Appear:
                        if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= appearTime) {
                            volume = rmaxVolume; currentTime = 0.f; mode = Mode::Exist;
                            if (messageBack == MessageBack::AtAppearance) novelSystem->SendMessage({"UnHold", this});
                        }
                        else volume = (rmaxVolume * (currentTime / appearTime));
                        music.setVolume(volume);
                        break;
                    case Mode::Exist:
                        if (!loop && (music.getPlayingOffset().asSeconds() >= timeToStartDisappearing || music.getStatus() == sf::Sound::Status::Stopped))
                        {
                            mode = Mode::Disapper;
                            if (messageBack == MessageBack::AtDisappearance) novelSystem->SendMessage({"UnHold", this});
                        }
                        break;
                    case Mode::Disapper:
                        if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= disappearTime) {
                            volume = 0.f; currentTime = 0.f; mode = Mode::Deprecate;
                            if (messageBack == MessageBack::AtDeprecated) novelSystem->SendMessage({"UnHold", this});
                        } else volume = (rmaxVolume - (rmaxVolume * (currentTime / disappearTime)));
                        music.setVolume(volume);
                        break;
                    case Mode::Deprecate: novelSystem->PopComponent(this); break;
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
                
                rmaxVolume = maxVolume * gs::maxVolumeGlobal * (ambient ? gs::maxVolumeAmbient : gs::maxVolumeMusic);
                if (!loop) timeToStartDisappearing = music.getDuration().asSeconds() - disappearTime;
                else timeToStartDisappearing = 0;
                currentTime = 0.f;
            }
            else
            {
                if (messageBack != MessageBack::No)
                    novelSystem->SendMessage({"UnHold", this});
                novelSystem->PopComponent(this);
            }
        }
        void MusicPlayer::SetStateMode(const Mode& newMode)
        {
            if (mode != newMode)
            {
                currentTime = 0.f; mode = newMode;
                if (newMode == Mode::Disapper && messageBack == MessageBack::AtDisappearance) novelSystem->SendMessage({"UnHold", this});
            }
        }
        void MusicPlayer::Save(std::wofstream& wof)
        {
            if (audioLoaded)
            {
                wof << L"path: " << nss::GetPathWOResourcePath(audioPath) << endl;
                wof << L"offset: " << music.getPlayingOffset().asSeconds() << endl;
                //if (timeToStartDisappearing) wof << L"ttsd: " << timeToStartDisappearing << endl;
                if (!loop) wof << L"loop: " << loop << endl;
                if ((!Skin::self && maxVolume != 100) || (Skin::self && ((ambient && maxVolume != Skin::self->ambient.maxVolume) || (!ambient && maxVolume != Skin::self->music.maxVolume)))) wof << L"maxVolume: " << maxVolume << endl;
                
                if (mode != Mode::Exist)
                {
                    wof << L"mode: " << (int)mode << endl;
                    if (mode == Mode::Appear || mode == Mode::Disapper) wof << L"currentTime: " << currentTime << endl;
                    if (mode == Mode::Appear && ((!Skin::self && appearTime != 1.f) || (Skin::self && ( (!ambient && appearTime != Skin::self->music.appearTime) || (ambient && appearTime != Skin::self->ambient.appearTime) ) )) ) wof << L"appearTime: " << appearTime << endl;
                }
                if ((!Skin::self && disappearTime != 1.f) || (Skin::self && ( (!ambient && disappearTime != Skin::self->music.disappearTime) || (ambient && disappearTime != Skin::self->ambient.disappearTime) ) )) wof << L"disappearTime: " << disappearTime << endl;
                    
            }
        }
        std::pair<std::wstring, bool> MusicPlayer::Load(std::wifstream& wif)
        {
            mode = Mode::Exist; std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"path: ")) audioPath = nss::ParseUntil(command, L'\n'); //imageName
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = Mode::Appear; break;
                        case 1: mode = Mode::Disapper; break;
                        case 3: mode = Mode::Deprecate; break;
                        default: mode = Mode::Exist; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"offset: ")) playingOffset = sf::seconds(nss::ParseAsFloat(command));
                else if (nss::Command(command, L"loop: ")) loop = nss::ParseAsBool(command);
                else if (nss::Command(command, L"maxvolume: ")) maxVolume = nss::ParseAsInt(command);
                else if (nss::Command(command, L"send: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: messageBack = MessageBack::No; break;
                        case 2: messageBack = MessageBack::AtDisappearance; break;
                        case 3: messageBack = MessageBack::AtDeprecated; break;
                        default: messageBack = MessageBack::AtAppearance; break;
                    }
                }
                if (wif.eof()) done = true;
            }
            
            float c = currentTime; LoadFromFile(audioPath); currentTime = c;
            if (mode == Mode::Appear) volume = (rmaxVolume * (currentTime / appearTime));
            else if (mode == Mode::Disapper) volume = (rmaxVolume - (rmaxVolume * (currentTime / disappearTime)));
            else volume = rmaxVolume; music.setVolume(volume);
            /// bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 1)) };
            bool onHold{ !((messageBack == MessageBack::No) || (messageBack == MessageBack::AtAppearance && (mode == Mode::Exist || mode == Mode::Disapper || mode == Mode::Deprecate)) || (messageBack == MessageBack::AtDisappearance && (mode == Mode::Disapper || mode == Mode::Deprecate))) };
            
            return { line, onHold };
        }
    }
}
