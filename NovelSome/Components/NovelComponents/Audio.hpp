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

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/Collectors/Sound.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;

namespace ns
{
    namespace NovelComponents
    {
        struct SoundPlayer : NovelObject
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
            float maxVolume{ 100 }, appearTime{ 0.f }, disappearTime{ 0.f };
            sf::Time playingOffset;
            
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void LoadFromFile(const std::wstring& fileName);
            void SetStateMode(modeEnum newMode);
        };
        
        
        
        struct MusicPlayer : NovelObject
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
            bool loop{ true };
            float maxVolume{ 100 }, appearTime{ 1.f }, disappearTime{ 1.f };
            sf::Time playingOffset;
            
            void Update(const sf::Time& elapsedTime) override;
            void Destroy() override;
            void LoadFromFile(const std::wstring& fileName);
            void SetStateMode(modeEnum newMode);
        };
    }
}

#endif /* Audio_h */
