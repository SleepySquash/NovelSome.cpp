//
//  Audio.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 06/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Audio_hpp
#define Audio_hpp

#include <iostream>
#include <list>
#include <vector>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <minEH/Essentials/Base.hpp>
#include <minEH/Engine/List.hpp>
#include <minEH/Engine/EntitySystem.hpp>
#include <minEH/Engine/NovelSystem.hpp>
#include <minEH/Engine/Settings.hpp>
#include <minEH/Engine/Collectors/Sound.hpp>
#include <minEH/Engine/GUI/Button/SomeButtons.hpp>
#include <minEH/Engine/NovelSomeScript.hpp>

#include "Abstract/Skin.hpp"
#include "Abstract/Savable.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct SoundPlayer : NovelObject, Savable
        {
        private:
            std::unique_ptr<char[]> fileInMemory;
            
            sf::Sound sound;
            bool audioLoaded{ false };
            std::wstring audioPath{ L"" };
            float volume{ 0.f }, currentTime{ 0.f }, timeToStartDisappearing{ 0.f };
            
        public:
            enum modeEnum {appearing, playing, disappearing, deprecated};
            modeEnum mode{ appearing };
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ noMessage };
            
            std::wstring folderPath{ L"" };
            bool loop{ false };
            float maxVolume{ 100 }, rmaxVolume{ 100 }, appearTime{ 0.f }, disappearTime{ 0.f };
            sf::Time playingOffset;
            
            SoundPlayer();
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void LoadFromFile(const std::wstring& fileName);
            void SetStateMode(modeEnum newMode);
            void ReceiveMessage(MessageHolder& message) override;
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
        
        
        
        struct MusicPlayer : NovelObject, Savable
        {
        private:
            std::unique_ptr<char[]> fileInMemory;
            
            sf::Music music;
            bool audioLoaded{ false };
            std::wstring audioPath{ L"" };
            float volume{ 0.f }, currentTime{ 0.f }, timeToStartDisappearing{ 0.f };
            
        public:
            enum modeEnum {appearing, playing, disappearing, deprecated};
            modeEnum mode{ appearing };
            enum sendMessageBackEnum {noMessage, atAppearance, atDisappearing, atDeprecated};
            sendMessageBackEnum sendMessageBack{ noMessage };
            
            std::wstring folderPath{ L"" };
            bool loop{ true }, ambient{ false };
            float maxVolume{ 100 }, rmaxVolume{ 100 }, appearTime{ 1.f }, disappearTime{ 1.f };
            sf::Time playingOffset;
            
            MusicPlayer();
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void LoadFromFile(const std::wstring& fileName);
            void SetStateMode(modeEnum newMode);
            
            void Save(std::wofstream& wof) override;
            std::pair<std::wstring, bool> Load(std::wifstream& wof) override;
        };
    }
}

#endif /* Audio_h */
