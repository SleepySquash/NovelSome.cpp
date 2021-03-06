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
            while (fileOpened && (!eof || !lines.empty()) && (onHold.empty() || (executeOnHold > 0 && onExecute.empty())) && !(gs::isPause && gs::isPauseEnabled))
            {
                if ((wif.fail() || wif.bad()) && lines.empty())
                    cout << "Warning :: NovelComponent :: Stream.fail() or Stream.bad() caught: " << base::utf8(scenarioPath) << endl;
                
                if (!wif.eof() || !lines.empty())
                {
                    if (!execute.empty()) line = execute.back();
                    else {
                        if (lines.empty())
                        {
                            ResourcesPreloading(lines, line);
                            if (!lines.empty()) { line = lines.back(); lines.pop_back(); } else line = L"";
                        }
                        else
                        {
                            if (!nss::Command(lines.front(), L"choose")) ResourcesPreloading(lines, line);
                            line = lines.back(); lines.pop_back();
                        }
                    } command.Command(line);
                    
                    bool backgroundAddingMode{ false };
                    if (nss::Command(command, L"//")) { /* oh, that's a comment... */ }
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    ///---------------------------------------DIALOGUE---------------------------------------
                    else if (nss::Command(command, L"\""))
                    {
                        command.lastPos = command.lastPos - 1;
                        std::wstring dialogueLine = nss::ParseAsQuoteString(command);
                        
                        if (dialogueGroup.size()) for (auto d : dialogueGroup) { ++d->priority; d->guiSystem = nullptr; }
                        auto* component = layers.PrioritizeComponent<Dialogue>(10000, &interface.guiDialogue);
                        if (command.ExplicitNoMessage()) component->messageBack = MessageBack::No;
                        
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
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
                            }
                            else if (nss::Command(argument, L"afterappearswitchto:") || nss::Command(argument, L"switchby:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"waitingforinput" || stringValue == L"input")
                                    component->switchTo = Mode::WaitingForInput;
                                else if (stringValue == L"waitingfortime" || stringValue == L"time")
                                    component->switchTo = Mode::WaitingForTime;
                            }
                            else if (nss::Command(argument, L"waitingtime:") || nss::Command(argument, L"time:"))
                                component->waitingTime = nss::ArgumentAsFloat(argument);
                            else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"layer:") || nss::Command(argument, L"d:"))
                                layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        
                        dialogueGroup.insert(dialogueGroup.begin(), component);
                        TemporarySettings::dialogue = &component->text;
                        TemporarySettings::name = &component->charText;
                        
                        if (Skin::self)
                        {
                            if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = true;
                            if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = true;
                        
                            if (std::wstring(var::localVariables.at(L"@name")->value.asString) != L"")
                            {
                                if (Skin::self->dialogue.nameRectH)
                                    Skin::self->dialogue.nameRectH->SetFadings(GUIObject::disappearing, component->disappearTime);
                                if (Skin::self->dialogue.nameRectV)
                                    Skin::self->dialogue.nameRectV->SetFadings(GUIObject::disappearing, component->disappearTime);
                            }
                        }
                        LocalVariables_Set(L"@dialogue", dialogueLine);
                        LocalVariables_Set(L"@name", std::wstring(L""));
                        if (Skin::self)
                        {
                            if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = false;
                            if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = false;
                        }
                        
                        component->SetDialogue(dialogueLine);
                    }
                    else if (nss::Command(command, L"choose"))
                    {
                        int openedWithBracket{ 0 }, spacesCount{ 0 };
                        for (int i = 0; i < line.length(); ++i) {
                            if (line[i] == L'{') ++openedWithBracket;
                            else if (line[i] == L'}') if (openedWithBracket > 0) --openedWithBracket; }
                        if (openedWithBracket == 0) spacesCount = command.lastPos - 6;
                        else spacesCount = -1;
                        
                        bool mightBeOnNextLine{ (openedWithBracket == 0) };
                        int lastChooseSpaces = -1;
                        
                        bool chooseEnded{ false }, explicitChoose{ false };
                        auto* component = layers.PrioritizeComponent<Choose>(10000, &interface.guiChoose);
                        if (command.ExplicitNoMessage()) component->messageBack = MessageBack::No;
                        
                        std::wstring question = L"";
                        bool dialogueShouldBeStill{ false }; //TODO: Default to Skin
                        
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
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
                            }
                            else if (nss::Command(argument, L"still") || nss::Command(argument, L"dialogue"))
                                dialogueShouldBeStill = true;
                            else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"layer:") || nss::Command(argument, L"d:"))
                                layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                            else question = nss::ParseAsMaybeQuoteString(argument);
                        }
                        
                        //cout << "openedWithBracket: " << openedWithBracket << "  spacesCount: " << spacesCount << "  question: " << ns::base::utf8(question) << endl;
                        while (!chooseEnded && !eof)
                        {
                            if (wif.eof()) eof = true;
                            else
                            {
                                std::wstring parsed{ L"" };
                                if (lines.empty()) {
                                    std::getline(wif, parsed); ++position;
                                    if (line.length() && line[line.length() - 1] == 13) line.erase(line.begin() + (line.size() - 1));
                                } else { parsed = lines.back(); lines.pop_back(); }
                                
                                if (nss::Command(parsed, L"end")) chooseEnded = true;
                                else
                                {
                                    if (mightBeOnNextLine || openedWithBracket != 0)
                                    {
                                        for (int i = 0; i < parsed.length(); ++i)
                                        {
                                            if (parsed[i] == L'{') ++openedWithBracket;
                                            else if (parsed[i] == L'}') --openedWithBracket;// = false;
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
                                            if (parsed[i] != L' ' && parsed[i] != L'\t') currentSpaces = i;
                                        
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
                                                    if (parsed[i] != L' ' && parsed[i] != L'\t') currentSpaces = i;
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
                                                        if (parsed[i] != L' ' && parsed[i] != L'\t') currentSpaces = i;
                                                    
                                                    if (currentSpaces != -1 && currentSpaces > spacesCount)
                                                    {
                                                        lastChooseSpaces = currentSpaces;
                                                        component->AddChoice(nss::ParseAsMaybeQuoteStringFull(settings));
                                                    }
                                                    else component->AddAction(parsed);
                                                }
                                                else
                                                {
                                                    int currentSpaces = -1;
                                                    for (int i = 0; i < parsed.length() && currentSpaces == -1; ++i)
                                                        if (parsed[i] != L' ' && parsed[i] != L'\t') currentSpaces = i;
                                                    
                                                    if (currentSpaces != -1 && currentSpaces > lastChooseSpaces)
                                                        { component->AddAction(parsed); explicitChoose = false; }
                                                    else if (currentSpaces != -1 && currentSpaces <= lastChooseSpaces)
                                                    {
                                                        if ((currentSpaces == lastChooseSpaces || currentSpaces > spacesCount) && !explicitChoose)
                                                            component->AddChoice(nss::ParseAsMaybeQuoteStringFull(settings));
                                                        else component->AddAction(parsed);
                                                    }
                                                    else component->AddAction(parsed);
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
                                if (d->mode != Mode::Disapper && d->mode != Mode::Deprecate)
                                {
                                    if (d->mode == Mode::Appear)
                                        d->switchTo = Mode::WaitingForChoose;
                                    else d->SetStateMode(Mode::WaitingForChoose);
                                }
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        chooseGroup.insert(chooseGroup.begin(), component);
                        component->InitChoose();
                    }
                    ///----------------------------------------MISC----------------------------------------
                    ///----------------------------------------MISC----------------------------------------
                    ///----------------------------------------MISC----------------------------------------
                    else if (nss::Command(command, L"event block")) gs::ignoreEvent = true;
                    else if (nss::Command(command, L"event unblock")) gs::ignoreEvent = false;
                    else if (nss::Command(command, L"draw block")) gs::ignoreDraw = true;
                    else if (nss::Command(command, L"draw unblock")) gs::ignoreDraw = false;
                    else if (nss::Command(command, L"wait"))
                    {
                        nss::SkipSpaces(command);
                        float amount{ 1.f };
                        if (nss::ContainsUsefulInformation(command)) amount = ArgumentAsFloat(command);
                        if (amount > 0.f) {
                            auto* component = layers.PrioritizeComponent<Waiting>(-32000);
                            OnHold(component); component->waitingTime = amount;
                        } else cout << "Warning :: NovelComponent :: Couldn't init \"wait\" command for " << amount << " seconds." << endl;
                    }
                    else if (nss::Command(command, L"fadeout") || nss::Command(command, L"fade") || nss::Command(command, L"disappear"))
                    {
                        nss::SkipSpaces(command);
                        float disappearTime{ 1.f };
                        
                        MessageBack messageBack{ MessageBack::AtDeprecated };
                        if (command.ExplicitNoMessage()) messageBack = MessageBack::No;
                        if (nss::ContainsUsefulInformation(command)) disappearTime = ArgumentAsFloat(command);
                        
                        if (disappearTime >= 0.f)
                        {
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"for") || nss::Command(argument, L"seconds")) { }
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing") messageBack = MessageBack::AtDisappearance;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated") messageBack = MessageBack::AtDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no") messageBack = MessageBack::No;
                                }
                            }
                            
                            if (backgroundGroup.size())
                                for (auto b : backgroundGroup)
                                {
                                    if (messageBack != MessageBack::No) OnHold(b);
                                    switch (messageBack)
                                    {
                                        case MessageBack::AtDisappearance: b->messageBack = MessageBack::AtDisappearance; break;
                                        case MessageBack::No: b->messageBack = MessageBack::No; break;
                                        default: b->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) b->disappearTime = disappearTime;
                                    b->SetStateMode(Mode::Disapper);
                                    b->hideAfter = nullptr;
                                }
                            if (characterGroup.size())
                                for (auto c : characterGroup)
                                {
                                    if (messageBack != MessageBack::No) OnHold(c);
                                    switch (messageBack)
                                    {
                                        case MessageBack::AtDisappearance: c->messageBack = MessageBack::AtDisappearance; break;
                                        case MessageBack::No: c->messageBack = MessageBack::No; break;
                                        default: c->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) c->disappearTime = disappearTime;
                                    c->SetStateMode(Mode::Disapper);
                                }
                            if (musicGroup.size())
                                for (auto m : musicGroup)
                                {
                                    if (messageBack != MessageBack::No) OnHold(m);
                                    switch (messageBack)
                                    {
                                        case MessageBack::AtDisappearance: m->messageBack = MessageBack::AtDisappearance; break;
                                        case MessageBack::No: m->messageBack = MessageBack::No; break;
                                        default: m->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) m->disappearTime = disappearTime;
                                    m->SetStateMode(Mode::Disapper);
                                }
                            if (ambientGroup.size())
                                for (auto m : ambientGroup)
                                {
                                    if (messageBack != MessageBack::No) OnHold(m);
                                    switch (messageBack)
                                    {
                                        case MessageBack::AtDisappearance: m->messageBack = MessageBack::AtDisappearance; break;
                                        case MessageBack::No: m->messageBack = MessageBack::No; break;
                                        default: m->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) m->disappearTime = disappearTime;
                                    m->SetStateMode(Mode::Disapper);
                                }
                            if (soundGroup.size())
                                for (auto m : soundGroup)
                                {
                                    if (messageBack != MessageBack::No) OnHold(m);
                                    switch (messageBack)
                                    {
                                        case MessageBack::AtDisappearance: m->messageBack = MessageBack::AtDisappearance; break;
                                        case MessageBack::No: m->messageBack = MessageBack::No; break;
                                        default: m->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) m->disappearTime = disappearTime;
                                    m->SetStateMode(Mode::Disapper);
                                }
                        }
                        else
                            cout << "Warning :: NovelComponent :: Couldn't init \"fade\" command for " << disappearTime << " seconds." << endl;
                    }
                    else if (nss::Command(command, L"scenario ") || nss::Command(command, L"jump "))
                    {
                        nss::SkipSpaces(command);
                        std::wstring fileName = nss::ParseAsMaybeQuoteString(command), filePath = folder + fileName;
                        if (!base::FileExists(filePath)) filePath = utf16(resourcePath()) + filePath;
                        if (!base::FileExists(filePath)) filePath = base::GetFolderPath(scenarioPath) + fileName;
                        if (base::FileExists(filePath))
                        {
                            wif.close(); wif.clear(); eof = false;
                            scenario = fileName; scenarioPath = filePath;
                            
                            std::wifstream wif1;
#ifdef _WIN32
                            wif1.open(filePath);
#else
                            wif1.open(utf8(filePath));
#endif
                            wif1.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t, 0x10FFFF, std::consume_header>));
                            lines.clear(); position = 0;
                            if (!(fileOpened = wif1.is_open()))
                                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << utf8(filePath) << endl;
                            wif.swap(wif1);
                        }
                        else cout << "Error :: NovelComponent :: Couldn't find scenario with path: \"" << utf8(filePath) << "\"." << endl;
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
                            if (command.ExplicitNoMessage()) sendMessageBack = noMessage;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing") sendMessageBack = atDisappearing;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated") sendMessageBack = atDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no") sendMessageBack = noMessage;
                                }
                            }
                            
                            for (auto b : backgroundGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(b);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: b->messageBack = MessageBack::AtDeprecated; break;
                                    case atDisappearing: b->messageBack = MessageBack::AtDisappearance; break;
                                    case noMessage: b->messageBack = MessageBack::No; break;
                                    default: b->messageBack = MessageBack::AtDeprecated; break;
                                }
                                if (disappearTime >= 0) b->disappearTime = disappearTime;
                                b->SetStateMode(Mode::Disapper);
                                b->hideAfter = nullptr;
                            }
                        }
                    }
                    else if ((backgroundAddingMode = nss::Command(command, L"background add ")) || nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Background>(0);
                        
                        if (!backgroundAddingMode && backgroundGroup.size() != 0)
                            for (auto b : backgroundGroup)
                            {
                                b->messageBack = MessageBack::No;
                                b->hideAfter = component;
                                // b->SetStateMode(b->disappearing);
                            }
                        
                        component->folder = folder;
                        if (Skin::self) {
                            component->appearTime = Skin::self->background.appearTime;
                            component->disappearTime = Skin::self->background.disappearTime;
                            component->maxAlpha = Skin::self->background.maxAlpha;
                            component->doParallax = Skin::self->background.doParallax;
                            component->parallaxPower = Skin::self->background.parallaxPower; }
                        component->fitMode = component->fillCentre;
                        if (command.ExplicitNoMessage()) component->messageBack = MessageBack::No;
                        
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
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
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
                                    component->parallaxPower = base::atof(possibleParallax);
                            }
                            else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"layer:") || nss::Command(argument, L"d:"))
                                layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        
                        backgroundGroup.insert(backgroundGroup.begin(), component);
                        component->LoadImage(filePath);
                    }
                    ///---------------------------------------CHARACTER--------------------------------------
                    ///---------------------------------------CHARACTER--------------------------------------
                    ///---------------------------------------CHARACTER--------------------------------------
                    else if (nss::Command(command, L"show "))
                    {
                        bool noMessage{ false }; if (command.ExplicitNoMessage()) noMessage = true;
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            if (CharacterLibrary::exists(possibleName))
                            {
                                auto* component = layers.PrioritizeComponent<ns::NovelComponents::Character>(5000);
                                component->folder = folder;
                                if (Skin::self) {
                                    component->appearTime = Skin::self->character.appearTime;
                                    component->disappearTime = Skin::self->character.disappearTime;
                                    component->maxAlpha = Skin::self->character.maxAlpha;
                                    component->doParallax = Skin::self->character.doParallax;
                                    component->parallaxPower = Skin::self->character.parallaxNormal; }
                                component->characterData = CharacterLibrary::at(possibleName);
                                component->position = Position::Center;
                                if (noMessage) component->messageBack = MessageBack::No;
                                
                                std::wstring state{ L"" };
                                
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
                                            component->messageBack = MessageBack::AtAppearance;
                                        else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                            component->messageBack = MessageBack::AtDisappearance;
                                        else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                            component->messageBack = MessageBack::AtDeprecated;
                                        else if (stringValue == L"nomessage" || stringValue == L"no")
                                            component->messageBack = MessageBack::No;
                                    }
                                    else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"layer:") || nss::Command(argument, L"d:"))
                                        layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                                    else
                                    {
                                        std::wstring possibleStateOrPos;
                                        if (argument.line[0] == L'"')
                                            possibleStateOrPos = nss::ParseAsQuoteString(argument);
                                        else possibleStateOrPos = argument.line;
                                        
                                        if (possibleStateOrPos == L"l" || possibleStateOrPos == L"left")
                                            component->position = Position::Left;
                                        else if (possibleStateOrPos == L"cl" || possibleStateOrPos == L"cleft")
                                            component->position = Position::CLeft;
                                        else if (possibleStateOrPos == L"c" || possibleStateOrPos == L"center")
                                            component->position = Position::Center;
                                        else if (possibleStateOrPos == L"cr" || possibleStateOrPos == L"cright")
                                            component->position = Position::CRight;
                                        else if (possibleStateOrPos == L"r" || possibleStateOrPos == L"right")
                                            component->position = Position::Right;
                                        else state = possibleStateOrPos;
                                    }
                                }
                                //if (arguments.size() && arguments[arguments.size() - 1].length() && arguments[arguments.size() - 1][arguments[arguments.size() - 1].length() - 1] == L'!') component->sendMessageBack = component->noMessage;
                                
                                if (component->messageBack != MessageBack::No) OnHold(component);
                                characterGroup.insert(characterGroup.begin(), component);
                                component->LoadState(state);
                            }
                        }
                    }
                    else if (nss::Command(command, L"hide all"))
                    {
                        if (characterGroup.size())
                        {
                            std::optional<float> disappearTime;
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            MessageBack messageBack{ MessageBack::AtDeprecated };
                            if (command.ExplicitNoMessage()) messageBack = MessageBack::No;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"fade:")||
                                    nss::Command(argument, L"fadeout:") ||
                                    nss::Command(argument, L"disappear:"))
                                    disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing") messageBack = MessageBack::AtDisappearance;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated") messageBack = MessageBack::AtDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no") messageBack = MessageBack::No;
                                }
                            }
                            
                            for (auto c : characterGroup)
                            {
                                if (messageBack != MessageBack::No) OnHold(c);
                                switch (messageBack)
                                {
                                    case MessageBack::AtDisappearance: c->messageBack = MessageBack::AtDisappearance; break;
                                    case MessageBack::No: c->messageBack = MessageBack::No; break;
                                    default: c->messageBack = MessageBack::AtDeprecated; break;
                                }
                                if (disappearTime.has_value()) c->disappearTime = *disappearTime;
                                c->SetStateMode(Mode::Disapper);
                            }
                        }
                    }
                    else if (nss::Command(command, L"hide "))
                    {
                        enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                        sendMessageBackEnum sendMessageBack{ atDeprecated };
                        if (command.ExplicitNoMessage()) sendMessageBack = noMessage;
                        
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0 && CharacterLibrary::exists(possibleName) && characterGroup.size())
                        {
                            float disappearTime{ -1.f };
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"fade:") || nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:")) disappearTime = nss::ArgumentAsFloat(argument);
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
                                if (c->characterData && c->characterData->name == possibleName)
                                {
                                    if (sendMessageBack != noMessage) OnHold(c);
                                    switch (sendMessageBack)
                                    {
                                        case atDisappearing: c->messageBack = MessageBack::AtDisappearance; break;
                                        case noMessage: c->messageBack = MessageBack::No; break;
                                        default: c->messageBack = MessageBack::AtDeprecated; break;
                                    }
                                    if (disappearTime >= 0) c->disappearTime = disappearTime;
                                    c->SetStateMode(Mode::Disapper);
                                }
                        }
                    }
                    ///---------------------------------------G U I--------------------------------------
                    ///---------------------------------------G U I--------------------------------------
                    ///---------------------------------------G U I--------------------------------------
                    else if (nss::Command(command, L"gui hide all"))
                    {
                        if (GUIGroup.size())
                        {
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            if (command.ExplicitNoMessage()) sendMessageBack = noMessage;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
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
                            
                            for (auto c : GUIGroup)
                            {
                                if (sendMessageBack != noMessage) OnHold(c);
                                switch (sendMessageBack)
                                {
                                    case atDeprecated: c->sendMessageBack = c->atDeprecated; break;
                                    case atDisappearing: c->sendMessageBack = c->atDisappearing; break;
                                    case noMessage: c->sendMessageBack = c->noMessage; break;
                                    default: c->sendMessageBack = c->atDeprecated; break;
                                }
                                c->mode = c->disappearing;
                                if (disappearTime >= 0) c->disappearTime = disappearTime;
                            }
                        }
                    }
                    else if (nss::Command(command, L"gui hide "))
                    {
                        if (GUIGroup.size())
                        {
                            std::wstring possibleScope = nss::ParseAsMaybeQuoteString(command);
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ atDeprecated };
                            if (command.ExplicitNoMessage()) sendMessageBack = noMessage;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
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
                            
                            for (auto c : GUIGroup)
                                if (c->scope == possibleScope)
                                {
                                    if (sendMessageBack != noMessage) OnHold(c);
                                    switch (sendMessageBack)
                                    {
                                        case atDeprecated: c->sendMessageBack = c->atDeprecated; break;
                                        case atDisappearing: c->sendMessageBack = c->atDisappearing; break;
                                        case noMessage: c->sendMessageBack = c->noMessage; break;
                                        default: c->sendMessageBack = c->atDeprecated; break;
                                    }
                                    c->mode = c->disappearing;
                                    if (disappearTime >= 0) c->disappearTime = disappearTime;
                                }
                        }
                    }
                    else if (nss::Command(command, L"skin "))
                    {
                        std::wstring possibleStr1 = nss::ParseAsMaybeQuoteString(command);
                        if (possibleStr1.length() != 0)
                        {
                            nss::SkipSpaces(command);
                            std::wstring possibleStr2 = nss::ParseAsMaybeQuoteString(command);
                            std::wstring filename, scopename;
                            if (possibleStr2.length() != 0)
                            {
                                if (base::FileExists(folder + possibleStr2) || base::FileExists(utf16(resourcePath()) + folder + possibleStr2)) { scopename = possibleStr1; filename = possibleStr2; }
                                else { scopename = possibleStr2; filename = possibleStr1; }
                            } else { scopename = L""; filename = possibleStr1; }
                            if (scopename != L"dialogue" && scopename != L"choose") Skin::self->RestoreToDefaults(scopename);
                            Skin::self->LoadFromFile(folder + filename, scopename);
                            if (scopename == L"") { Skin::self->changedDialogue = Skin::self->changedAmbient = Skin::self->changedBackground = Skin::self->changedCharacter = Skin::self->changedMusic = Skin::self->changedSound = Skin::self->changedChoose = true; Skin::self->defaultDialogue = Skin::self->defaultAmbient = Skin::self->defaultBackground = Skin::self->defaultCharacter = Skin::self->defaultMusic = Skin::self->defaultSound = Skin::self->defaultChoose = filename; }
                            else if (scopename == L"dialogue") { Skin::self->changedDialogue = true; Skin::self->defaultDialogue = filename; }
                            else if (scopename == L"choose") { Skin::self->changedChoose = true; Skin::self->defaultChoose = filename; }
                            else if (scopename == L"ambient") { Skin::self->changedAmbient = true; Skin::self->defaultAmbient = filename; }
                            else if (scopename == L"sound") { Skin::self->changedSound = true; Skin::self->defaultSound = filename; }
                            else if (scopename == L"music") { Skin::self->changedMusic = true; Skin::self->defaultMusic = filename; }
                            else if (scopename == L"background") { Skin::self->changedBackground = true; Skin::self->defaultBackground = filename; }
                            else if (scopename == L"character") { Skin::self->changedCharacter = true; Skin::self->defaultCharacter = filename; }
                        }
                    }
                    else if (nss::Command(command, L"gui "))
                    {
                        bool noMessage{ false }; if (command.ExplicitNoMessage()) noMessage = true;
                        std::wstring possibleStr1 = nss::ParseAsMaybeQuoteString(command);
                        if (possibleStr1.length() != 0)
                        {
                            nss::SkipSpaces(command);
                            std::wstring possibleStr2 = nss::ParseAsMaybeQuoteString(command);
                            if (possibleStr2.length() != 0)
                            {
                                std::wstring filename, scopename;
                                if (base::FileExists(folder + possibleStr2) || base::FileExists(utf16(resourcePath()) + folder + possibleStr2)) { scopename = possibleStr1; filename = possibleStr2; }
                                else { scopename = possibleStr2; filename = possibleStr1; }
                                
                                if (scopename == L"dialogue")
                                {
                                    sf::Uint8 alpha{ interface.guiDialogue.alpha };
                                    interface.guiDialogue.clear();
                                    LocalVariables_Set(L"@name", std::wstring(L""));
                                    Skin::self->RestoreToDefaults(scopename);
                                    Skin::self->LoadFromFile(folder + filename, scopename);
                                    Skin::self->changedDialogue = false;
                                    interface.guiDialogue.LoadFromFile(folder + filename, scopename);
                                    interface.guiDialogue.ResetResize(); interface.guiDialogue.Resize(gs::width, gs::height);
                                    interface.guiDialogue.trueFileName = filename;
                                    interface.guiDialogue.SetAlpha(alpha);
                                }
                                else if (scopename == L"choose")
                                {
                                    /// not supported yet - dunno the consequenses.
                                    /*sf::Uint8 alpha{ interface.guiDialogue.alpha };
                                    interface.guiDialogue.clear();
                                    LocalVariables_Set(L"@name", std::wstring(L""));
                                    Skin::self->LoadFromFile(folderPath + filename, scopename);
                                    Skin::self->changedDialogue = false;
                                    interface.guiDialogue.LoadFromFile(folderPath + filename, scopename);
                                    interface.guiDialogue.ResetResize(); interface.guiDialogue.Resize(gs::width, gs::height);
                                    interface.guiDialogue.trueFileName = filename;
                                    interface.guiDialogue.SetAlpha(alpha);*/
                                }
                                else if (scopename == L"gamepause")
                                {
                                    sf::Uint8 alpha{ interface.guiPause.alpha };
                                    interface.guiPause.clear();
                                    interface.guiPause.LoadFromFile(folder + filename, scopename);
                                    interface.guiPause.ResetResize(); interface.guiPause.Resize(gs::width, gs::height);
                                    interface.guiPause.trueFileName = filename;
                                    interface.guiPause.SetAlpha(alpha);
                                }
                                else
                                {
                                    auto* component = layers.PrioritizeComponent<ns::NovelComponents::GUISystem>(16000);
                                    component->isNovel = true;
                                    // TODO: Сделать свои Skin для GUI.
                                    if (Skin::self) {
                                        component->appearTime = Skin::self->character.appearTime;
                                        component->disappearTime = Skin::self->character.disappearTime;
                                        component->maxAlpha = Skin::self->character.maxAlpha; }
                                    if (noMessage) component->sendMessageBack = component->noMessage;
                                    
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
                                        else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"layer:") || nss::Command(argument, L"d:"))
                                            layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                                    }
                                    
                                    if (component->sendMessageBack != component->noMessage) OnHold(component);
                                    GUIGroup.insert(GUIGroup.begin(), component);
                                    component->LoadFromFile(folder + filename, scopename); component->trueFileName = filename;
                                    component->SetAlpha(255); component->ResetResize(); component->Resize(gs::width, gs::height);
                                }
                            }
                        }
                    }
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    else if ((backgroundAddingMode = nss::Command(command, L"ambient stop")) || nss::Command(command, L"music stop") || nss::Command(command, L"stop"))
                    {
                        if ((musicGroup.size() != 0 && !backgroundAddingMode) || (ambientGroup.size() != 0 && backgroundAddingMode))
                        {
                            float disappearTime{ -1.f };
                            MessageBack messageBack{ MessageBack::AtDeprecated };
                            if (command.ExplicitNoMessage()) messageBack = MessageBack::No;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"fade:") || nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:")) disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        messageBack = MessageBack::AtDisappearance;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        messageBack = MessageBack::AtDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        messageBack = MessageBack::No;
                                }
                            }
                            
                            list<MusicPlayer*>& group = musicGroup;
                            if (backgroundAddingMode) group = ambientGroup;
                            for (auto m : group)
                            {
                                if (messageBack != MessageBack::No) OnHold(m);
                                switch (messageBack)
                                {
                                    case MessageBack::AtDisappearance: m->messageBack = MessageBack::AtDisappearance; break;
                                    case MessageBack::No: m->messageBack = MessageBack::No; break;
                                    default: m->messageBack = MessageBack::AtDeprecated; break;
                                }
                                if (disappearTime >= 0) m->disappearTime = disappearTime;
                                m->SetStateMode(Mode::Disapper);
                            }
                        }
                    }
                    else if ((backgroundAddingMode = nss::Command(command, L"music add ")) || (backgroundAddingMode = nss::Command(command, L"play add ")) || nss::Command(command, L"music ") || nss::Command(command, L"play "))
                    {
                        bool noMessage{ false }; if (command.ExplicitNoMessage()) noMessage = true;
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        if (!backgroundAddingMode && musicGroup.size() != 0)
                            for (auto b : musicGroup)
                            {
                                b->messageBack = MessageBack::No;
                                b->SetStateMode(Mode::Disapper);
                            }
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::MusicPlayer>(0);
                        component->folderPath = folder;
                        if (Skin::self) {
                            component->appearTime = Skin::self->music.appearTime;
                            component->disappearTime = Skin::self->music.disappearTime;
                            component->maxVolume = Skin::self->music.maxVolume; }
                        if (noMessage) component->messageBack = MessageBack::No;
                        
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
                            else if (nss::Command(argument, L"volume:") || nss::Command(argument, L"maxvolume:"))
                                component->maxVolume = nss::ArgumentAsFloat(argument);
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
                            }
                            else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isloop:"))
                                component->loop = nss::ArgumentAsBool(argument);
                            else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                component->playingOffset = sf::seconds(nss::ArgumentAsFloat(argument));
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        musicGroup.insert(musicGroup.begin(), component);
                        component->LoadFromFile(filePath);
                    }
                    else if ((backgroundAddingMode = nss::Command(command, L"ambient add ")) || nss::Command(command, L"ambient "))
                    {
                        bool noMessage{ false }; if (command.ExplicitNoMessage()) noMessage = true;
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        if (!backgroundAddingMode && musicGroup.size() != 0)
                            for (auto b : ambientGroup)
                            {
                                b->messageBack = MessageBack::No;
                                b->SetStateMode(Mode::Disapper);
                            }
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::MusicPlayer>(0);
                        component->folderPath = folder; component->tag = L"Ambient"; component->ambient = true;
                        if (Skin::self) {
                            component->appearTime = Skin::self->music.appearTime;
                            component->disappearTime = Skin::self->music.disappearTime;
                            component->maxVolume = Skin::self->music.maxVolume; }
                        if (noMessage) component->messageBack = MessageBack::No;
                        
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
                            else if (nss::Command(argument, L"volume:") || nss::Command(argument, L"maxvolume:"))
                                component->maxVolume = nss::ArgumentAsFloat(argument);
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
                            }
                            else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isloop:"))
                                component->loop = nss::ArgumentAsBool(argument);
                            else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                component->playingOffset = sf::seconds(nss::ArgumentAsFloat(argument));
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        ambientGroup.insert(ambientGroup.begin(), component);
                        component->LoadFromFile(filePath);
                    }
                    else if (nss::Command(command, L"sound stop"))
                    {
                        if (soundGroup.size() != 0)
                        {
                            float disappearTime{ -1.f };
                            MessageBack messageBack{ MessageBack::No };
                            if (command.ExplicitNoMessage()) messageBack = MessageBack::No;
                            
                            vector<std::wstring> arguments;
                            nss::ParseArguments(command, arguments);
                            for (auto it = arguments.begin(); it != arguments.end(); ++it)
                            {
                                nss::CommandSettings argument; argument.Command(*it);
                                if (nss::Command(argument, L"fade:") || nss::Command(argument, L"fadeout:") || nss::Command(argument, L"disappear:")) disappearTime = nss::ArgumentAsFloat(argument);
                                else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                                {
                                    std::wstring stringValue = nss::ArgumentAsString(argument);
                                    if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                        messageBack = MessageBack::AtDisappearance;
                                    else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                        messageBack = MessageBack::AtDeprecated;
                                    else if (stringValue == L"nomessage" || stringValue == L"no")
                                        messageBack = MessageBack::No;
                                }
                            }
                            
                            for (auto m : soundGroup)
                            {
                                if (messageBack != MessageBack::No) OnHold(m);
                                switch (messageBack)
                                {
                                    case MessageBack::AtDisappearance: m->messageBack = MessageBack::AtDisappearance; break;
                                    case MessageBack::No: m->messageBack = MessageBack::No; break;
                                    default: m->messageBack = MessageBack::AtDeprecated; break;
                                }
                                if (disappearTime >= 0) m->disappearTime = disappearTime;
                                m->SetStateMode(Mode::Disapper);
                            }
                        }
                    }
                    else if (nss::Command(command, L"sound "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::SoundPlayer>(0);
                        if (Skin::self) {
                            component->appearTime = Skin::self->sound.appearTime;
                            component->disappearTime = Skin::self->sound.disappearTime;
                            component->maxVolume = Skin::self->sound.maxVolume; }
                        component->folderPath = folder;
                        if (command.ExplicitNoMessage()) component->messageBack = MessageBack::No;
                        
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
                            else if (nss::Command(argument, L"volume:") || nss::Command(argument, L"maxvolume:"))
                                component->maxVolume = nss::ArgumentAsFloat(argument);
                            else if (nss::Command(argument, L"messageback:") || nss::Command(argument, L"message:"))
                            {
                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                if (stringValue == L"atappearance" || stringValue == L"appearance")
                                    component->messageBack = MessageBack::AtAppearance;
                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                    component->messageBack = MessageBack::AtDisappearance;
                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                    component->messageBack = MessageBack::AtDeprecated;
                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                    component->messageBack = MessageBack::No;
                            }
                            else if (nss::Command(argument, L"loop:") || nss::Command(argument, L"isloop:"))
                                component->loop = nss::ArgumentAsBool(argument);
                            else if (nss::Command(argument, L"from:") || nss::Command(argument, L"start:"))
                                component->playingOffset = sf::seconds(nss::ArgumentAsFloat(argument));
                        }
                        
                        if (component->messageBack != MessageBack::No) OnHold(component);
                        soundGroup.insert(soundGroup.begin(), component);
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
                        else varName = nss::ParseUntil(command, ' ');
                        if (varName.length() != 0)
                        {
                            if (varName != L"@dialogue" && varName != L"@name")
                            {
                                nss::SkipSpaces(command);
                                if (command.line[command.lastPos] == '=') { command.lastPos++; nss::SkipSpaces(command); }
                                
                                std::wstring valueString;
                                bool asString{ false };
                                if ((asString = (command.line[command.lastPos] == '"')))
                                    valueString = nss::ParseAsQuoteString(command);
                                else valueString = nss::ParseUntil(command, ' ');
                                
                                if (valueString.length() != 0)
                                {
                                    if (asString) LocalVariables_Set(varName, valueString);
                                    else
                                    {
                                        if (valueString == L"true" || valueString == L"false")
                                            LocalVariables_Set(varName, base::atob(valueString));
                                        else LocalVariables_Set(varName, base::atoi(valueString));
                                    }
                                }
                            }
                            else cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                    else if (nss::Command(command, L"delete "))
                    {
                        std::wstring varName;
                        if (command.line[command.lastPos] == '"')
                            varName = nss::ParseAsQuoteString(command);
                        else varName = nss::ParseUntil(command, ' ');
                        
                        if (varName.length() != 0)
                        {
                            if (varName != L"@dialogue" && varName != L"@name")
                            {
                                if (var::localVariables.find(varName) != var::localVariables.end())
                                {
                                    NovelVariable* nvar = var::localVariables.at(varName);
                                    var::localVariables.erase(varName);
                                    delete nvar;
                                }
                            }
                            else cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                    ///----------------------------------------DEBUG----------------------------------------
                    ///----------------------------------------DEBUG----------------------------------------
                    ///----------------------------------------DEBUG----------------------------------------
                    else if (nss::Command(command, L"debug::outputvariables"))
                    {
                        cout << endl;
                        for (const auto& key : var::localVariables)
                            if (key.second)
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
                                    
                                    std::wstring valueString; bool asString{ false };
                                    if ((asString = (command.line[command.lastPos] == '"')))
                                        valueString = nss::ParseAsQuoteString(command);
                                    else valueString = nss::ParseUntil(command, ' ');
                                    
                                    if (valueString.length() != 0)
                                    {
                                        if (asString) LocalVariables_Set(possibleName, valueString);
                                        else
                                        {
                                            if (valueString == L"true" || valueString == L"false")
                                                LocalVariables_Set(possibleName, base::atob(valueString));
                                            else LocalVariables_Set(possibleName, base::atoi(valueString));
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
                                        CharacterData* characterData = CharacterLibrary::at(possibleName);
                                        std::wstring characterName = characterData ? characterData->display : possibleName;
                                        
                                        if (dialogueGroup.size()) for (auto d : dialogueGroup) { ++d->priority; d->guiSystem = nullptr; }
                                        auto* component = layers.PrioritizeComponent<Dialogue>(10000, &interface.guiDialogue);
                                        if (command.ExplicitNoMessage()) component->messageBack = MessageBack::No;
                                        
                                        vector<std::wstring> arguments;
                                        nss::ParseArguments(command, arguments);
                                        for (auto arg : arguments)
                                        {
                                            nss::CommandSettings argument; argument.Command(arg);
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
                                                    component->messageBack = MessageBack::AtAppearance;
                                                else if (stringValue == L"atdisappearing" || stringValue == L"disappearing")
                                                    component->messageBack = MessageBack::AtDisappearance;
                                                else if (stringValue == L"atdeprecated" || stringValue == L"deprecated")
                                                    component->messageBack = MessageBack::AtDeprecated;
                                                else if (stringValue == L"nomessage" || stringValue == L"no")
                                                    component->messageBack = MessageBack::No;
                                            }
                                            else if (nss::Command(argument, L"afterappearswitchto:") || nss::Command(argument, L"switchto:"))
                                            {
                                                std::wstring stringValue = nss::ArgumentAsString(argument);
                                                if (stringValue == L"waitingforinput" || stringValue == L"input")
                                                    component->switchTo = Mode::WaitingForInput;
                                                else if (stringValue == L"waitingfortime" || stringValue == L"time")
                                                    component->switchTo = Mode::WaitingForTime;
                                            }
                                            else if (nss::Command(argument, L"waitingtime:") || nss::Command(argument, L"time:"))
                                                component->waitingTime = nss::ArgumentAsFloat(argument);
                                            else if (nss::Command(argument, L"depth:") || nss::Command(argument, L"d:"))
                                                layers.ChangePriorityOf(component, nss::ArgumentAsInt(argument));
                                        }
                                        
                                        if (component->messageBack != MessageBack::No) OnHold(component);
                                        
                                        dialogueGroup.insert(dialogueGroup.begin(), component);
                                        TemporarySettings::dialogue = &component->text;
                                        TemporarySettings::name = &component->charText;
                                        component->SetCharacterName(characterName);
                                        if (characterData) component->SetCharacter(characterData);
                                        
                                        if (Skin::self)
                                        {
                                            if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = true;
                                            if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = true;
                                            if (Skin::self->dialogue.nameRectH) Skin::self->dialogue.nameRectH->ignoreVariableChange = true;
                                            if (Skin::self->dialogue.nameRectV) Skin::self->dialogue.nameRectV->ignoreVariableChange = true;
                                        
                                            if (std::wstring(var::localVariables.at(L"@name")->value.asString) == L"")
                                            {
                                                if (Skin::self->dialogue.nameRectH)
                                                    Skin::self->dialogue.nameRectH->SetFadings(GUIObject::appearing, component->appearTime);
                                                if (Skin::self->dialogue.nameRectV)
                                                    Skin::self->dialogue.nameRectV->SetFadings(GUIObject::appearing, component->appearTime);
                                            }
                                        }
                                        LocalVariables_Set(L"@dialogue", possibleDialogue);
                                        LocalVariables_Set(L"@name", characterName);
                                        if (Skin::self)
                                        {
                                            if (Skin::self->dialogue.dialogueRectH) Skin::self->dialogue.dialogueRectH->ignoreVariableChange = false;
                                            if (Skin::self->dialogue.dialogueRectV) Skin::self->dialogue.dialogueRectV->ignoreVariableChange = false;
                                            if (Skin::self->dialogue.nameRectH) Skin::self->dialogue.nameRectH->ignoreVariableChange = false;
                                            if (Skin::self->dialogue.nameRectV) Skin::self->dialogue.nameRectV->ignoreVariableChange = false;
                                        }
                                        
                                        component->SetDialogue(possibleDialogue);
                                    }
                                }
                            }
                            else cout << "NSS :: Prohibited change of @dialogue or @name variables." << endl;
                        }
                    }
                    if (executeOnHold) { --executeOnHold; if (!execute.empty()) execute.pop_back();
                        if (executeOnHold <= 0 && onExecute.empty()) layers.SendMessage({"FinishedExecute"}); }
                }
                else eof = true;
            }
            
            if (fileOpened && !(gs::isPause && gs::isPauseEnabled))
            {
                layers.Update(elapsedTime); timeReading += elapsedTime;
                if (nextAutosave && timeReading.asSeconds() > nextAutosave)
                {
                    nextAutosave += gs::autosaveDeltaTime;
                    cout << "Autosaving ..." << endl;
                    // Also save timeReading as a backup.
                }
            }
            if (eof && onHold.empty() && lines.empty()) entity->PopComponent(this);
        }
    }
}
