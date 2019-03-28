//
//  NovelComponents.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#ifndef NovelComponents_hpp
#define NovelComponents_hpp

#include <iostream>
#include <list>
#include <vector>

//NovelComponent
#include <locale>
#include <fstream>
#include <codecvt>

//Folder path
#include <dirent.h>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../../Essentials/Base.hpp"
#include "../../Engine/List.hpp"
#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/NovelSystem.hpp"
#include "../../Engine/Settings.hpp"
#include "../../Engine/GUIInterface.hpp"
#include "../../Engine/NovelSomeScript.hpp"
#include "../NSMenuComponents/NovelsLibrary.hpp"

#include "VariableSystem.hpp"
#include "CharacterLibrary.hpp"
#include "Skin.hpp"
#include "Interface.hpp"
#include "Audio.hpp"
#include "Background.hpp"
#include "Character.hpp"
#include "Dialogue.hpp"
#include "GUISystem.hpp"
#include "Misc.hpp"

using std::cin;
using std::cout;
using std::endl;
using base::utf8;
using base::utf16;
using std::list;
using std::vector;

namespace ns
{
    namespace NovelComponents
    {
        struct Novel;
        struct EventListener : NovelObject
        {
            Novel* novel;
            EventListener(Novel* novel);
            void ReceiveMessage(MessageHolder& message) override;
        };
        struct Novel : Component
        {
            std::wstring nsdataPath{ L"" }, folderPath{ L"" }, scenarioPath{ L"" }, line;
            std::wifstream wif;
            nss::CommandSettings command;
            list<std::wstring> lines;
            int preloadLinesAmount{ 12 };
            
            bool eof{ false }, fileOpened{ false };
            list<NovelObject*> onHold;
            
            NovelInfo* nvl{ nullptr };
            GamePause* gamePause{ nullptr };
            
            NovelSystem layers;
            Interface interface;
            
            list<Background*> backgroundGroup;
            list<Character*> characterGroup;
            list<Dialogue*> dialogueGroup;
            list<Choose*> chooseGroup;
            list<SoundPlayer*> soundGroup;
            list<MusicPlayer*> musicGroup;
            list<MusicPlayer*> ambientGroup;
            list<GUISystem*> GUIGroup;
            
            Novel(const std::wstring& path, NovelInfo* nvl = nullptr);
            Novel(NovelInfo* nvl);
            ~Novel();
            void Init() override;
            void Update(const sf::Time& elapsedTime) override;
            void ResourcesPreloading(list<std::wstring>& lines, std::wstring& line);
            void Draw(sf::RenderWindow* window) override;
            void Resize(unsigned int width, unsigned int height) override;
            void PollEvent(sf::Event& event) override;
            void ForwardMessage(MessageHolder& message);
            void ReceiveMessage(MessageHolder& message) override;
            void OnHold(NovelObject* component);
            void UnHold(NovelObject* component);
            void VariableChange(const std::wstring& name);
            void LocalVariables_Set(const std::wstring& name, std::wstring value);
            void LocalVariables_Set(const std::wstring& name, bool value);
            void LocalVariables_Set(const std::wstring& name, int value);
            template<typename T> void FreeGroup(list<T*>& elements) { elements.clear(); }
        };
        
        
    }
}

#endif /* NovelComponents_hpp */
