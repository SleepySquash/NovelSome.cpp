//
//  NovelUpdate.cpp
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
                                else if (nss::Command(argument, L"messageBack:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atAppearance" || stringValue == L"appearance")
                                        component->sendMessageBack = component->atAppearance;
                                    else if (stringValue == L"atDisappearing" || stringValue == L"disappearing")
                                        component->sendMessageBack = component->atDisappearing;
                                    else if (stringValue == L"atDeprecated" || stringValue == L"deprecated")
                                        component->sendMessageBack = component->atDeprecated;
                                    else if (stringValue == L"noMessage" || stringValue == L"no")
                                        component->sendMessageBack = component->noMessage;
                                }
                                
                                delete arguments[i];
                            }
                        if (arguments != nullptr)
                            delete arguments;
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
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
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            if (arguments != nullptr)
                                for (int i = 0; arguments[i] != nullptr; i++)
                                {
                                    nss::CommandSettings argument;
                                    argument.Command(arguments[i]);
                                    
                                    if (nss::Command(argument, L"fade:")||
                                        nss::Command(argument, L"fadeout:") ||
                                        nss::Command(argument, L"disappear:"))
                                        disappearTime = nss::ArgumentAsFloat(argument);
                                    else if (nss::Command(argument, L"messageBack:") || nss::Command(argument, L"message:"))
                                    {
                                        std::wstring stringValue = nss::ArgumentAsString(argument);
                                        if (stringValue == L"atDisappearing" || stringValue == L"disappearing")
                                            sendMessageBack = atDisappearing;
                                        else if (stringValue == L"atDeprecated" || stringValue == L"deprecated")
                                            sendMessageBack = atDeprecated;
                                        else if (stringValue == L"noMessage" || stringValue == L"no")
                                            sendMessageBack = noMessage;
                                    }
                                    
                                    free(arguments[i]);
                                }
                            if (arguments != nullptr)
                                free(arguments);
                            
                            while (temp != nullptr)
                            {
                                if (temp->data != nullptr)
                                {
                                    if (sendMessageBack != noMessage)
                                        OnHold(temp->data);
                                    switch (sendMessageBack)
                                    {
                                        case atDeprecated:
                                            temp->data->sendMessageBack = temp->data->atDeprecated;
                                            break;
                                        case atDisappearing:
                                            temp->data->sendMessageBack = temp->data->atDisappearing;
                                            break;
                                        case noMessage:
                                            temp->data->sendMessageBack = temp->data->noMessage;
                                            break;
                                        default:
                                            temp->data->sendMessageBack = temp->data->atDeprecated;
                                            break;
                                    }
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
                                else if (nss::Command(argument, L"messageBack:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atAppearance" || stringValue == L"appearance")
                                        component->sendMessageBack = component->atAppearance;
                                    else if (stringValue == L"atDisappearing" || stringValue == L"disappearing")
                                        component->sendMessageBack = component->atDisappearing;
                                    else if (stringValue == L"atDeprecated" || stringValue == L"deprecated")
                                        component->sendMessageBack = component->atDeprecated;
                                    else if (stringValue == L"noMessage" || stringValue == L"no")
                                        component->sendMessageBack = component->noMessage;
                                }
                                
                                free(arguments[i]);
                            }
                        if (arguments != nullptr)
                            free(arguments);
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
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
                                else if (nss::Command(argument, L"messageBack:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atAppearance" || stringValue == L"appearance")
                                        component->sendMessageBack = component->atAppearance;
                                    else if (stringValue == L"atDisappearing" || stringValue == L"disappearing")
                                        component->sendMessageBack = component->atDisappearing;
                                    else if (stringValue == L"atDeprecated" || stringValue == L"deprecated")
                                        component->sendMessageBack = component->atDeprecated;
                                    else if (stringValue == L"noMessage" || stringValue == L"no")
                                        component->sendMessageBack = component->noMessage;
                                }
                                else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isLoop:"))
                                    component->SetLoop(nss::ArgumentAsBool(argument));
                                else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                    component->SetPlayingOffset(sf::seconds(nss::ArgumentAsFloat(argument)));
                                
                                free(arguments[i]);
                            }
                        if (arguments != nullptr)
                            free(arguments);
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
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
    }
}
