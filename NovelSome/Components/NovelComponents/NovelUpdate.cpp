//
//  NovelUpdate.cpp
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
        void Novel::Update(const sf::Time& elapsedTime)
        {
            while (fileOpened && !eof && onHold.size() == 0 &&
                   !(ns::GlobalSettings::isPause && ns::GlobalSettings::isPauseEnabled))
            {
                if ((wif.fail() || wif.bad()) && lines.empty())
                    cout << "Warning :: NovelComponent :: Stream.fail() or Stream.bad() caught: " << nsdataPath.toAnsiString() << endl;
                
                if (!wif.eof() || !lines.empty())
                {
                    if (lines.empty())
                    {
                        ResourcesPreloading(lines, line);
                        if (!lines.empty()) { line = lines.back(); lines.pop_back(); } else line = L"";
                    }
                    else
                    {
                        if (!nss::Command(lines.front(), L"choose"))
                            ResourcesPreloading(lines, line);
                        line = lines.back(); lines.pop_back();
                    }
                    
                    command.Command(line);
                    
                    bool backgroundAddingMode{ false };
                    if (nss::Command(command, L"//")) { /* oh, that's a comment... */ }
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    else if (nss::Command(command, L"\""))
                    {
                        command.lastPos = command.lastPos - 1;
                        std::wstring dialogueLine = nss::ParseAsQuoteString(command);
                        
                        if (dialogueGroup.size() != 0)
                            for (auto d : dialogueGroup)
                            {
                                ++d->priority;
                                d->SetGUISystem(nullptr);
                            }
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000, this);
                        
                        vector<std::wstring> arguments;
                        nss::ParseArguments(command, arguments);
                        for (auto it = arguments.begin(); it != arguments.end(); ++it)
                        {
                            nss::CommandSettings argument; argument.Command(*it);
                            
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
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->sendMessageBack = component->atAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->sendMessageBack = component->atDisappearing;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->sendMessageBack = component->atDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->sendMessageBack = component->noMessage;
                            }
                            else if (nss::Command(argument, L"afterappearswitchto:") || nss::Command(argument, L"switchby:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"waitingforinput" || stringValue == L"input")
                                    component->afterAppearSwitchTo = component->waitingForInput;
                                else if (stringValue == L"waitingfortime" || stringValue == L"time")
                                    component->afterAppearSwitchTo = component->waitingForTime;
                            }
                            else if (nss::Command(argument, L"waitingtime:") || nss::Command(argument, L"time:"))
                                component->waitingTime = nss::ArgumentAsFloat(argument);
                        }
                        
                        if (component->sendMessageBack != component->noMessage) OnHold(component);
                        
                        component->SetGroup(dialogueGroup.insert(dialogueGroup.begin(), component));
                        
                        if (skin.dialogue.dialogueRect != nullptr)
                            skin.dialogue.dialogueRect->ignoreVariableChange = true;
                        
                        if (std::wstring(localVariables.at(L"@name")->value.asString) != L"")
                            if (skin.dialogue.nameRect != nullptr)
                                skin.dialogue.nameRect->SetFadings(GUIObject::disappearing, component->disappearTime);
                        LocalVariables_Set(L"@dialogue", dialogueLine);
                        LocalVariables_Set(L"@name", std::wstring(L""));
                        
                        if (skin.dialogue.dialogueRect != nullptr)
                            skin.dialogue.dialogueRect->ignoreVariableChange = false;
                        
                        component->SetDialogue(dialogueLine);
                    }
                    else if (nss::Command(command, L"choose"))
                    {
                        int openedWithBracket{ 0 };
                        int spacesCount{ 0 };
                        for (int i = 0; i < line.length(); ++i)
                        {
                            if (line[i] == L'{')
                                ++openedWithBracket;
                            else if (line[i] == L'}')
                                if (openedWithBracket > 0)
                                    --openedWithBracket;
                        }
                        if (openedWithBracket == 0)
                            spacesCount = command.lastPos - 6;
                        else
                            spacesCount = -1;
                        
                        bool mightBeOnNextLine{ (openedWithBracket == 0) };
                        int lastChooseSpaces = -1;
                        
                        bool chooseEnded{ false };
                        bool explicitChoose{ false };
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Choose>(10000, this);
                        
                        std::wstring question = L"";
                        bool dialogueShouldBeStill{ false }; //TODO: Default to Skin
                        
                        vector<std::wstring> arguments;
                        nss::ParseArguments(command, arguments);
                        for (auto it = arguments.begin(); it != arguments.end(); ++it)
                        {
                            nss::CommandSettings argument;
                            argument.Command(*it);
                            
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
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->sendMessageBack = component->atAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->sendMessageBack = component->atDisappearing;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->sendMessageBack = component->atDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->sendMessageBack = component->noMessage;
                            }
                            else if (nss::Command(argument, L"still") || nss::Command(argument, L"dialogue"))
                                dialogueShouldBeStill = true;
                            else question = nss::ParseAsMaybeQuoteString(argument);
                        }
                        
                        cout << "openedWithBracket: " << openedWithBracket << "  spacesCount: " << spacesCount << "  question: " << ns::base::utf8(question) << endl;
                        while (!chooseEnded && !eof)
                        {
                            if (wif.eof())
                                eof = true;
                            else
                            {
                                std::wstring parsed{ L"" };
                                if (lines.empty()) std::getline(wif, parsed);
                                else { parsed = lines.back(); lines.pop_back(); }
                                
                                if (nss::Command(parsed, L"end"))
                                    chooseEnded = true;
                                else
                                {
                                    if (mightBeOnNextLine || openedWithBracket != 0)
                                    {
                                        for (int i = 0; i < parsed.length(); ++i)
                                        {
                                            if (parsed[i] == L'{')
                                                ++openedWithBracket;
                                            else if (parsed[i] == L'}')
                                                --openedWithBracket = false;
                                        }
                                        
                                        if (mightBeOnNextLine && openedWithBracket != 0)
                                        {
                                            std::wstring newParsed; bool found{ false };
                                            for (int i = 0; i < parsed.length() && !found; ++i)
                                            {
                                                if (parsed[i] == L'{')
                                                {
                                                    found = true;
                                                    for (int j = i + 1; j < parsed.length(); ++j)
                                                        newParsed += parsed[j];
                                                }
                                            }
                                            parsed = newParsed;
                                            
                                            spacesCount = -1;
                                            mightBeOnNextLine = false;
                                        }
                                        
                                        if (!mightBeOnNextLine && openedWithBracket == 0)
                                        {
                                            std::wstring newParsed; bool found{ false };
                                            for (int i = parsed.length() - 1; i >= 0 && !found; --i)
                                            {
                                                if (parsed[i] == L'}')
                                                {
                                                    found = true;
                                                    for (int j = 0; j < i; ++j)
                                                        newParsed += parsed[j];
                                                }
                                            }
                                            parsed = newParsed;
                                            chooseEnded = true;
                                        }
                                    }
                                    
                                    if (openedWithBracket == 0 && spacesCount != -1)
                                    {
                                        int currentSpaces = -1;
                                        for (int i = 0; i < parsed.length() && currentSpaces == -1; ++i)
                                            if (parsed[i] != L' ')
                                                currentSpaces = i;
                                        
                                        if (currentSpaces != -1 && currentSpaces <= spacesCount)
                                            chooseEnded = true;
                                        
                                        if (chooseEnded) { lines.push_back(parsed); parsed = L""; }
                                    }
                                    
                                    if (parsed.length() != 0)
                                    {
                                        nss::CommandSettings settings;
                                        settings.Command(parsed);
                                        
                                        if (nss::ContainsUsefulInformation(settings))
                                        {
                                            mightBeOnNextLine = false;
                                            
                                            if (nss::Command(settings, L"choice "))
                                            {
                                                int currentSpaces = -1;
                                                for (int i = 0; i < parsed.length() && currentSpaces == -1; ++i)
                                                    if (parsed[i] != L' ')
                                                        currentSpaces = i;
                                                lastChooseSpaces = currentSpaces;
                                                
                                                explicitChoose = true;
                                                component->AddChoice(nss::ParseAsMaybeQuoteStringFull(settings));
                                            }
                                            else
                                            {
                                                if (lastChooseSpaces == -1)
                                                {
                                                    int currentSpaces = -1;
                                                    for (int i = 0; i < parsed.length() && currentSpaces == -1; ++i)
                                                        if (parsed[i] != L' ')
                                                            currentSpaces = i;
                                                    
                                                    if (currentSpaces != -1 && currentSpaces > spacesCount)
                                                    {
                                                        lastChooseSpaces = currentSpaces;
                                                        component->AddChoice(nss::ParseAsMaybeQuoteStringFull(settings));
                                                    }
                                                    else
                                                        component->AddAction(parsed);
                                                }
                                                else
                                                {
                                                    int currentSpaces = -1;
                                                    for (int i = 0; i < parsed.length() && currentSpaces == -1; ++i)
                                                        if (parsed[i] != L' ')
                                                            currentSpaces = i;
                                                    
                                                    if (currentSpaces != -1 && currentSpaces > lastChooseSpaces)
                                                        { component->AddAction(parsed); explicitChoose = false; }
                                                    else if (currentSpaces != -1 && currentSpaces <= lastChooseSpaces)
                                                    {
                                                        if ((currentSpaces == lastChooseSpaces || currentSpaces > spacesCount) && !explicitChoose)
                                                            component->AddChoice(nss::ParseAsMaybeQuoteStringFull(settings));
                                                        else
                                                            component->AddAction(parsed);
                                                    }
                                                    else
                                                        component->AddAction(parsed);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        
                        if (dialogueShouldBeStill && dialogueGroup.size() != 0)
                        {
                            for (auto d : dialogueGroup)
                                if (d->mode != Dialogue::modeEnum::disappearing && d->mode != Dialogue::modeEnum::deprecated)
                                {
                                    if (d->mode == Dialogue::modeEnum::appearing)
                                        d->afterAppearSwitchTo = Dialogue::modeEnum::waitingForChoose;
                                    else
                                        d->SetStateMode(Dialogue::modeEnum::waitingForChoose);
                                }
                        }
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
                        component->SetGroup(chooseGroup.insert(chooseGroup.begin(), component));
                        component->InitChoose();
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
                    else if (nss::Command(command, L"scenario ") || nss::Command(command, L"jump "))
                    {
                        nss::SkipSpaces(command);
                        std::wstring path = nss::ParseAsQuoteString(command);
                        sf::String filePath = sf::String(resourcePath()) + GetFolderPath() + path;
                        
                        if (base::FileExists(filePath))
                        {
                            wif.close();
#ifdef _WIN32
                            wif.open(filePath.toWideString());
#else
                            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                            std::string u8str = converter.to_bytes(filePath.toWideString());
                            wif.open(u8str);
#endif
                            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
                            
                            lines.clear();
                            
                            if (!(fileOpened = wif.is_open()))
                                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << filePath.toAnsiString() << endl;
                        }
                        else
                            cout << "Error :: NovelComponent :: Couldn't find scenario with path: \"" << filePath.toAnsiString() << "\"." << endl;
                    }
                    ///--------------------------------------BACKGROUND--------------------------------------
                    ///--------------------------------------BACKGROUND--------------------------------------
                    ///--------------------------------------BACKGROUND--------------------------------------
                    else if (nss::Command(command, L"background hide"))
                    {
                        if (backgroundGroup.size() != 0)
                        {
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        sendMessageBack = atDisappearing;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        sendMessageBack = atDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        sendMessageBack = noMessage;
                                }
                            }
                            
                            for (auto b : backgroundGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(b);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: b->sendMessageBack = b->atDeprecated; break;
                                    case atDisappearing: b->sendMessageBack = b->atDisappearing; break;
                                    case noMessage: b->sendMessageBack = b->noMessage; break;
                                    default: b->sendMessageBack = b->atDeprecated; break;
                                }
                                b->SetStateMode(b->disappearing);
                                if (disappearTime >= 0) b->disappearTime = disappearTime;
                            }
                        }
                    }
                    else if ((backgroundAddingMode = nss::Command(command, L"background add ")) || nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        if (!backgroundAddingMode && backgroundGroup.size() != 0)
                            for (auto b : backgroundGroup)
                            {
                                b->sendMessageBack = b->noMessage;
                                b->SetStateMode(b->disappearing);
                            }
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Background>(0);
                        component->SetNovel(this);
                        component->fitMode = component->fillCentre;
                        
                        vector<std::wstring> arguments;
                        nss::ParseArguments(command, arguments);
                        for (auto it = arguments.begin(); it != arguments.end(); ++it)
                        {
                            nss::CommandSettings argument;
                            argument.Command(*it);
                            
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
                                else if (stringValue == L"fillcenter")
                                    component->fitMode = component->fillCentre;
                                else if (stringValue == L"default")
                                    component->fitMode = component->defaultFit;
                                else if (stringValue == L"no")
                                    component->fitMode = component->noFit;
                                else if (stringValue == L"nofit")
                                    component->fitMode = component->noFit;
                                else if (stringValue == L"stretch")
                                    component->fitMode = component->stretch;
                            }
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->sendMessageBack = component->atAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->sendMessageBack = component->atDisappearing;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->sendMessageBack = component->atDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->sendMessageBack = component->noMessage;
                            }
                            else if (nss::Command(argument, L"parallax:"))
                            {
                                std::wstring possibleParallax = nss::ArgumentAsString(argument);
                                if (possibleParallax == L"normal" || possibleParallax == L"n")
                                    component->parallaxPower = ns::GlobalSettings::defaultParallaxNormal;
                                else if (possibleParallax == L"close" || possibleParallax == L"c")
                                    component->parallaxPower = ns::GlobalSettings::defaultParallaxClose;
                                else if (possibleParallax == L"far" || possibleParallax == L"f")
                                    component->parallaxPower = ns::GlobalSettings::defaultParallaxFar;
                                else if (possibleParallax == L"background" || possibleParallax == L"back" || possibleParallax == L"b")
                                    component->parallaxPower = ns::GlobalSettings::defaultParallaxBackground;
                                else if (possibleParallax == L"frontground" || possibleParallax == L"front" || possibleParallax == L"f")
                                    component->parallaxPower = ns::GlobalSettings::defaultParallaxFrontground;
                                else
                                    component->parallaxPower = base::ConvertToFloat(possibleParallax);
                            }
                        }
                        
                        if (component->sendMessageBack != component->noMessage) OnHold(component);
                        
                        component->SetGroup(backgroundGroup.insert(backgroundGroup.begin(), component));
                        component->LoadImage(filePath);
                    }
                    ///---------------------------------------CHARACTER--------------------------------------
                    ///---------------------------------------CHARACTER--------------------------------------
                    ///---------------------------------------CHARACTER--------------------------------------
                    else if (nss::Command(command, L"show "))
                    {
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            if (library.characterLibrary.find(possibleName) != library.characterLibrary.end())
                            {
                                auto* component = layers.PrioritizeComponent<ns::NovelComponents::Character>(5000);
                                component->SetNovel(this);
                                component->SetCharacterData(library.characterLibrary.at(possibleName));
                                component->position = component->center;
                                
                                std::wstring state{ L"" };
                                
                                vector<std::wstring> arguments;
                                nss::ParseArguments(command, arguments);
                                for (auto it = arguments.begin(); it != arguments.end(); ++it)
                                {
                                    nss::CommandSettings argument;
                                    argument.Command(*it);
                                    
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
                                    else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                    {
                                        std::wstring stringValue = nss::ArgumentAsString(argument);
                                        if (stringValue == L"atappearance" || stringValue == L"appearance")
                                            component->sendMessageBack = component->atAppearance;
                                        else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                            component->sendMessageBack = component->atDisappearing;
                                        else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                            component->sendMessageBack = component->atDeprecated;
                                        else if (stringValue == L"nomessage" || stringValue == L"no")
                                            component->sendMessageBack = component->noMessage;
                                    }
                                    else
                                    {
                                        std::wstring possibleStateOrPos;
                                        if (argument.line[0] == L'"')
                                            possibleStateOrPos = nss::ParseAsQuoteString(argument);
                                        else possibleStateOrPos = argument.line;
                                        
                                        if (state.length() == 0) state = possibleStateOrPos;
                                        else
                                        {
                                            if (possibleStateOrPos == L"l" || possibleStateOrPos == L"left")
                                                component->position = component->left;
                                            else if (possibleStateOrPos == L"cl" || possibleStateOrPos == L"cleft")
                                                component->position = component->cleft;
                                            else if (possibleStateOrPos == L"c" || possibleStateOrPos == L"center")
                                                component->position = component->center;
                                            else if (possibleStateOrPos == L"cr" || possibleStateOrPos == L"cright")
                                                component->position = component->cright;
                                            else if (possibleStateOrPos == L"r" || possibleStateOrPos == L"right")
                                                component->position = component->right;
                                        }
                                    }
                                }
                                
                                if (component->sendMessageBack != component->noMessage) OnHold(component);
                                
                                
                                component->SetGroup(characterGroup.insert(characterGroup.begin(), component));
                                component->LoadState(state);
                            }
                        }
                    }
                    else if (nss::Command(command, L"hide all"))
                    {
                        if (characterGroup.size() != 0)
                        {
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        sendMessageBack = atDisappearing;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        sendMessageBack = atDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        sendMessageBack = noMessage;
                                }
                            }
                            
                            for (auto c : characterGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(c);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: c->sendMessageBack = c->atDeprecated; break;
                                    case atDisappearing: c->sendMessageBack = c->atDisappearing; break;
                                    case noMessage: c->sendMessageBack = c->noMessage; break;
                                    default: c->sendMessageBack = c->atDeprecated; break;
                                }
                                c->SetStateMode(c->disappearing);
                                if (disappearTime >= 0) c->disappearTime = disappearTime;
                            }
                        }
                    }
                    else if (nss::Command(command, L"hide "))
                    {
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            if (library.characterLibrary.find(possibleName) != library.characterLibrary.end())
                            {
                                if (characterGroup.size() != 0)
                                {
                                    float disappearTime{ -1.f };
                                    enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                                    sendMessageBackEnum sendMessageBack{ atDeprecated };
                                    vector<std::wstring> arguments;
                                    nss::ParseArguments(command, arguments);
                                    for (auto it = arguments.begin(); it != arguments.end(); ++it)
                                    {
                                        nss::CommandSettings argument;
                                        argument.Command(*it);
                                        
                                        if (nss::Command(argument, L"fade:")||
                                            nss::Command(argument, L"fadeout:") ||
                                            nss::Command(argument, L"disappear:"))
                                            disappearTime = nss::ArgumentAsFloat(argument);
                                        else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                        {
                                            std::wstring stringValue = nss::ArgumentAsString(argument);
                                            if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                                sendMessageBack = atDisappearing;
                                            else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                                sendMessageBack = atDeprecated;
                                            else if (stringValue == L"nomessage" || stringValue == L"no")
                                                sendMessageBack = noMessage;
                                        }
                                    }
                                    
                                    for (auto c : characterGroup)
                                    {
                                        if (c->GetCharacterData() != nullptr && c->GetCharacterData()->name == possibleName)
                                        {
                                            if (sendMessageBack != noMessage) OnHold(c);
                                            switch (sendMessageBack)
                                            {
                                                case atDeprecated: c->sendMessageBack = c->atDeprecated; break;
                                                case atDisappearing: c->sendMessageBack = c->atDisappearing; break;
                                                case noMessage: c->sendMessageBack = c->noMessage; break;
                                                default: c->sendMessageBack = c->atDeprecated; break;
                                            }
                                            c->SetStateMode(c->disappearing);
                                            if (disappearTime >= 0) c->disappearTime = disappearTime;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    else if (nss::Command(command, L"music stop") || nss::Command(command, L"ambient stop") || nss::Command(command, L"stop"))
                    {
                        if (musicGroup.size() != 0)
                        {
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        sendMessageBack = atDisappearing;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        sendMessageBack = atDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        sendMessageBack = noMessage;
                                }
                            }
                            
                            for (auto m : musicGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(m);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: m->sendMessageBack = m->atDeprecated; break;
                                    case atDisappearing: m->sendMessageBack = m->atDisappearing; break;
                                    case noMessage: m->sendMessageBack = m->noMessage; break;
                                    default: m->sendMessageBack = m->atDeprecated; break;
                                }
                                m->SetStateMode(m->disappearing);
                                if (disappearTime >= 0) m->disappearTime = disappearTime;
                            }
                        }
                    }
                    else if (nss::Command(command, L"music ") || nss::Command(command, L"ambient ") || nss::Command(command, L"play "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::MusicPlayer>(0);
                        component->SetNovel(this);
                        
                        vector<std::wstring> arguments;
                        nss::ParseArguments(command, arguments);
                        for (auto it = arguments.begin(); it != arguments.end(); ++it)
                        {
                            nss::CommandSettings argument;
                            argument.Command(*it);
                            
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
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->sendMessageBack = component->atAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->sendMessageBack = component->atDisappearing;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->sendMessageBack = component->atDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->sendMessageBack = component->noMessage;
                            }
                            else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isloop:"))
                                component->SetLoop(nss::ArgumentAsBool(argument));
                            else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                component->SetPlayingOffset(sf::seconds(nss::ArgumentAsFloat(argument)));
                        }
                        
                        if (component->sendMessageBack != component->noMessage) OnHold(component);
                        
                        component->SetGroup(musicGroup.insert(musicGroup.begin(), component));
                        component->LoadFromFile(filePath);
                    }
                    else if (nss::Command(command, L"sound stop"))
                    {
                        if (soundGroup.size() != 0)
                        {
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ noMessage };
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument;
                                argument.Command(*it);
                                
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        sendMessageBack = atDisappearing;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        sendMessageBack = atDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        sendMessageBack = noMessage;
                                }
                            }
                            
                            for (auto m : soundGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(m);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: m->sendMessageBack = m->atDeprecated; break;
                                    case atDisappearing: m->sendMessageBack = m->atDisappearing; break;
                                    case noMessage: m->sendMessageBack = m->noMessage; break;
                                    default: m->sendMessageBack = m->atDeprecated; break;
                                }
                                m->SetStateMode(m->disappearing);
                                if (disappearTime >= 0) m->disappearTime = disappearTime;
                            }
                        }
                    }
                    else if (nss::Command(command, L"sound "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::SoundPlayer>(0);
                        component->SetNovel(this);
                        
                        vector<std::wstring> arguments;
                        nss::ParseArguments(command, arguments);
                        for (auto it = arguments.begin(); it != arguments.end(); ++it)
                        {
                            nss::CommandSettings argument;
                            argument.Command(*it);
                            
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
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->sendMessageBack = component->atAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->sendMessageBack = component->atDisappearing;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->sendMessageBack = component->atDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->sendMessageBack = component->noMessage;
                            }
                            else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isloop:"))
                                component->SetLoop(nss::ArgumentAsBool(argument));
                            else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                component->SetPlayingOffset(sf::seconds(nss::ArgumentAsFloat(argument)));
                        }
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
                        component->SetGroup(soundGroup.insert(soundGroup.begin(), component));
                        component->LoadFromFile(filePath);
                    }
                    ///----------------------------------------LOGIC----------------------------------------
                    ///----------------------------------------LOGIC----------------------------------------
                    ///----------------------------------------LOGIC----------------------------------------
                    else if (nss::Command(command, L"var "))
                    {
                        std::wstring varName;
                        if (command.line[command.lastPos] == '"')
                            varName = nss::ParseAsQuoteString(command);
                        else
                            varName = nss::ParseUntil(command, ' ');
                        if (varName.length() != 0)
                        {
                            if (varName != L"@dialogue" && varName != L"@name")
                            {
                                nss::SkipSpaces(command);
                                if (command.line[command.lastPos] == '=')
                                {
                                    command.lastPos++;
                                    nss::SkipSpaces(command);
                                }
                                
                                std::wstring valueString;
                                bool asString{ false };
                                if ((asString = (command.line[command.lastPos] == '"')))
                                    valueString = nss::ParseAsQuoteString(command);
                                else
                                    valueString = nss::ParseUntil(command, ' ');
                                
                                if (valueString.length() != 0)
                                {
                                    if (asString)
                                        LocalVariables_Set(varName, valueString);
                                    else
                                    {
                                        if (valueString == L"true" || valueString == L"false")
                                            LocalVariables_Set(varName, ns::base::ConvertToBool(valueString));
                                        else
                                            LocalVariables_Set(varName, ns::base::ConvertToInt(valueString));
                                    }
                                }
                            }
                            else
                                cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                    else if (nss::Command(command, L"delete "))
                    {
                        std::wstring varName;
                        if (command.line[command.lastPos] == '"')
                            varName = nss::ParseAsQuoteString(command);
                        else
                            varName = nss::ParseUntil(command, ' ');
                        
                        if (varName.length() != 0)
                        {
                            if (varName != L"@dialogue" && varName != L"@name")
                            {
                                if (localVariables.find(varName) != localVariables.end())
                                {
                                    NovelVariable* nvar = localVariables.at(varName);
                                    localVariables.erase(varName);
                                    delete nvar;
                                }
                            }
                            else
                                cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                    ///----------------------------------------DEBUG----------------------------------------
                    ///----------------------------------------DEBUG----------------------------------------
                    ///----------------------------------------DEBUG----------------------------------------
                    else if (nss::Command(command, L"debug::outputvariables"))
                    {
                        cout << endl;
                        for (const auto& key : localVariables)
                            if (key.second != nullptr)
                                cout << base::utf8(key.first) << ": " << (key.second->type == key.second->String ? "\"" : "") << *key.second << (key.second->type == key.second->String ? "\"" : "") << endl;
                    }
                    ///---------------------------------------SPECIFIC---------------------------------------
                    ///---------------------------------------SPECIFIC---------------------------------------
                    ///---------------------------------------SPECIFIC---------------------------------------
                    else
                    {
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            if (possibleName != L"@name" && possibleName != L"@dialogue")
                            {
                                nss::SkipSpaces(command);
                                
                                if (nss::Command(command, L"="))
                                {
                                    nss::ParseWhile(command, ' '); nss::SkipSpaces(command);
                                    
                                    std::wstring valueString;
                                    bool asString{ false };
                                    if ((asString = (command.line[command.lastPos] == '"')))
                                        valueString = nss::ParseAsQuoteString(command);
                                    else
                                        valueString = nss::ParseUntil(command, ' ');
                                    
                                    if (valueString.length() != 0)
                                    {
                                        if (asString)
                                            LocalVariables_Set(possibleName, valueString);
                                        else
                                        {
                                            if (valueString == L"true" || valueString == L"false")
                                                LocalVariables_Set(possibleName, ns::base::ConvertToBool(valueString));
                                            else
                                                LocalVariables_Set(possibleName, ns::base::ConvertToInt(valueString));
                                        }
                                    }
                                }
                                else if (nss::Command(command, L"+="))
                                {
                                    //TODO: +=
                                }
                                else if (nss::Command(command, L"-="))
                                {
                                    //TODO: -=
                                }
                                else if (nss::Command(command, L"*="))
                                {
                                    //TODO: *=
                                }
                                else if (nss::Command(command, L"/="))
                                {
                                    //TODO: /=
                                }
                                else if (nss::Command(command, L"++"))
                                {
                                    //TODO: ++
                                }
                                else if (nss::Command(command, L"--"))
                                {
                                    //TODO: --
                                }
                                else
                                {
                                    std::wstring possibleDialogue = nss::ParseAsQuoteString(command);
                                    if (possibleDialogue.length() != 0)
                                    {
                                        CharacterData* characterData = (library.characterLibrary.find(possibleName) != library.characterLibrary.end()) ? library.characterLibrary.at(possibleName) : nullptr;
                                        sf::String characterName = (characterData != nullptr) ? characterData->displayName : sf::String(possibleName);
                                        
                                        if (dialogueGroup.size() != 0)
                                            for (auto d : dialogueGroup)
                                            {
                                                ++d->priority;
                                                d->SetGUISystem(nullptr);
                                            }
                                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000, this);
                                        
                                        vector<std::wstring> arguments;
                                        nss::ParseArguments(command, arguments);
                                        for (auto arg : arguments)
                                        {
                                            nss::CommandSettings argument;
                                            argument.Command(arg);
                                            
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
                                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                            {
                                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                                    component->sendMessageBack = component->atAppearance;
                                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                                    component->sendMessageBack = component->atDisappearing;
                                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                                    component->sendMessageBack = component->atDeprecated;
                                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                                    component->sendMessageBack = component->noMessage;
                                            }
                                            else if (nss::Command(argument, L"afterappearswitchto:") || nss::Command(argument, L"switchby:"))
                                            {
                                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                                if (stringValue == L"waitingforinput" || stringValue == L"input")
                                                    component->afterAppearSwitchTo = component->waitingForInput;
                                                else if (stringValue == L"waitingfortime" || stringValue == L"time")
                                                    component->afterAppearSwitchTo = component->waitingForTime;
                                            }
                                            else if (nss::Command(argument, L"waitingtime:") || nss::Command(argument, L"time:"))
                                                component->waitingTime = nss::ArgumentAsFloat(argument);
                                        }
                                        
                                        if (component->sendMessageBack != component->noMessage)
                                            OnHold(component);
                                        
                                        component->SetGroup(dialogueGroup.insert(dialogueGroup.begin(), component));
                                        component->SetCharacterName(characterName);
                                        if (characterData != nullptr)
                                            component->SetCharacter(characterData);
                                        
                                        if (skin.dialogue.dialogueRect != nullptr)
                                            skin.dialogue.dialogueRect->ignoreVariableChange = true;
                                        if (skin.dialogue.nameRect != nullptr)
                                            skin.dialogue.nameRect->ignoreVariableChange = true;
                                        
                                        if (std::wstring(localVariables.at(L"@name")->value.asString) == L"")
                                            if (skin.dialogue.nameRect != nullptr)
                                                skin.dialogue.nameRect->SetFadings(GUIObject::appearing, component->appearTime);
                                        LocalVariables_Set(L"@dialogue", possibleDialogue);
                                        LocalVariables_Set(L"@name", characterName.toWideString());
                                        
                                        if (skin.dialogue.dialogueRect != nullptr)
                                            skin.dialogue.dialogueRect->ignoreVariableChange = false;
                                        if (skin.dialogue.nameRect != nullptr)
                                            skin.dialogue.nameRect->ignoreVariableChange = false;
                                        
                                        component->SetDialogue(possibleDialogue);
                                    }
                                }
                            }
                            else
                                cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                }
                else
                    eof = true;
            }
            
            if (fileOpened && !(ns::GlobalSettings::isPause && ns::GlobalSettings::isPauseEnabled))
                layers.Update(elapsedTime);
            gamePause.Update(elapsedTime);
        }
    }
}
