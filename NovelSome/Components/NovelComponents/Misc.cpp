//
//  Misc.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Misc.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Waiting::Waiting() : Savable(L"Wait") { }
        void Waiting::Update(const sf::Time& elapsedTime)
        {
            if (currentTime < waitingTime) currentTime += elapsedTime.asSeconds();
            else { novelSystem->SendMessage({"UnHold", this}); novelSystem->PopComponent(this); }
        }
        
        void Waiting::Save(std::wofstream& wof)
        {
            wof << L"w: " << waitingTime << endl;
            wof << L"c: " << currentTime << endl;
        }
        std::pair<std::wstring, bool> Waiting::Load(std::wifstream& wif)
        {
            std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"c: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"w: ")) waitingTime = nss::ParseAsFloat(command);
                if (wif.eof()) done = true;
            }
            
            bool onHold{ currentTime < waitingTime };
            
            return { line, onHold };
        }
    }
}
