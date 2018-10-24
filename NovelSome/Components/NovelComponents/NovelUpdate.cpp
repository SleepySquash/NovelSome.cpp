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
                    if (lines.IsEmpty())
                        std::getline(wif, line);
                    else
                        line = lines.Pop();
                    
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
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        if (dialogueGroup != nullptr)
                        {
                            List<Dialogue>* temp = dialogueGroup;
                            while (temp != nullptr)
                            {
                                if (temp->data != nullptr)
                                {
                                    temp->data->priority++;
                                    temp->data->SetGUISystem(nullptr);
                                }
                                temp = temp->next;
                            }
                        }
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000, this);
                        
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
                                
                                delete arguments[i];
                            }
                        if (arguments != nullptr)
                            delete arguments;
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
                        dialogueGroup = ns::list::Insert<ns::NovelComponents::Dialogue>(dialogueGroup);
                        dialogueGroup->data = component;
                        component->SetGroup(dialogueGroup);
                        
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
                        
                        wchar_t** arguments = nss::ParseArguments(command);
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
                                else
                                    question = nss::ParseAsMaybeQuoteString(argument);
                                
                                delete arguments[i];
                            }
                        if (arguments != nullptr)
                            delete arguments;
                        
                        cout << "openedWithBracket: " << openedWithBracket << "  spacesCount: " << spacesCount << "  question: " << ns::base::ConvertToUTF8(question) << endl;
                        while (!chooseEnded && !eof)
                        {
                            if (wif.eof())
                                eof = true;
                            else
                            {
                                std::wstring parsed{ L"" };
                                if (lines.IsEmpty())
                                    std::getline(wif, parsed);
                                else
                                    parsed = lines.Pop();
                                
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
                                        
                                        if (chooseEnded)
                                            lines.Push(parsed), parsed = L"";
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
                                                        component->AddAction(parsed), explicitChoose = false;
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
                        
                        if (dialogueShouldBeStill)
                        {
                            if (dialogueGroup != nullptr)
                            {
                                List<Dialogue>* temp = dialogueGroup;
                                while (temp != nullptr)
                                {
                                    if (temp->data != nullptr)
                                    {
                                        if (temp->data->mode != Dialogue::modeEnum::disappearing && temp->data->mode != Dialogue::modeEnum::deprecated)
                                        {
                                            if (temp->data->mode == Dialogue::modeEnum::appearing)
                                                temp->data->afterAppearSwitchTo = Dialogue::modeEnum::waitingForChoose;
                                            else
                                                temp->data->SetStateMode(Dialogue::modeEnum::waitingForChoose);
                                        }
                                    }
                                    temp = temp->next;
                                }
                            }
                        }
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
                        chooseGroup = ns::list::Insert<ns::NovelComponents::Choose>(chooseGroup);
                        chooseGroup->data = component;
                        component->SetGroup(chooseGroup);
                        
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
                            
                            lines.Clear();
                            
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
                    else if ((backgroundAddingMode = nss::Command(command, L"background add ")) || nss::Command(command, L"background ") || nss::Command(command, L"задний фон "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        if (!backgroundAddingMode)
                            if (backgroundGroup != nullptr)
                            {
                                List<Background>* temp = backgroundGroup;
                                while (temp != nullptr)
                                {
                                    if (temp->data != nullptr)
                                    {
                                        temp->data->sendMessageBack = temp->data->noMessage;
                                        temp->data->SetStateMode(temp->data->disappearing);
                                    }
                                    temp = temp->next;
                                }
                            }
                        
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
                                wchar_t** arguments = nss::ParseArguments(command);
                                
                                auto* component = layers.PrioritizeComponent<ns::NovelComponents::Character>(5000);
                                component->SetNovel(this);
                                component->SetCharacterData(library.characterLibrary.at(possibleName));
                                component->position = component->center;
                                
                                std::wstring state{ L"" };
                                
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
                                            else
                                                possibleStateOrPos = argument.line;
                                            
                                            if (state.length() == 0)
                                                state = possibleStateOrPos;
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
                                        
                                        free(arguments[i]);
                                    }
                                if (arguments != nullptr)
                                    free(arguments);
                                
                                if (component->sendMessageBack != component->noMessage)
                                    OnHold(component);
                                
                                characterGroup = ns::list::Insert<ns::NovelComponents::Character>(characterGroup);
                                characterGroup->data = component;
                                component->SetGroup(characterGroup);
                                
                                component->LoadState(state);
                            }
                        }
                    }
                    else if (nss::Command(command, L"hide all"))
                    {
                        if (characterGroup != nullptr)
                        {
                            List<Character>* temp = characterGroup;
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
                    else if (nss::Command(command, L"hide "))
                    {
                        std::wstring possibleName = nss::ParseUntil(command, ' ');
                        if (possibleName.length() != 0)
                        {
                            if (library.characterLibrary.find(possibleName) != library.characterLibrary.end())
                            {
                                if (characterGroup != nullptr)
                                {
                                    List<Character>* temp = characterGroup;
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
                                            
                                            free(arguments[i]);
                                        }
                                    if (arguments != nullptr)
                                        free(arguments);
                                    
                                    while (temp != nullptr)
                                    {
                                        if (temp->data != nullptr)
                                        {
                                            if (temp->data->GetCharacterData() != nullptr && temp->data->GetCharacterData()->name == possibleName)
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
                                        }
                                        temp = temp->next;
                                    }
                                }
                            }
                        }
                    }
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    ///----------------------------------------AUDIO----------------------------------------
                    else if (nss::Command(command, L"music stop") || nss::Command(command, L"ambient stop"))
                    {
                        if (musicGroup != nullptr)
                        {
                            List<MusicPlayer>* temp = musicGroup;
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
                    else if (nss::Command(command, L"sound stop"))
                    {
                        if (soundGroup != nullptr)
                        {
                            List<SoundPlayer>* temp = soundGroup;
                            wchar_t** arguments = nss::ParseArguments(command);
                            
                            float disappearTime{ -1.f };
                            enum sendMessageBackEnum{ atDisappearing, atDeprecated, noMessage };
                            sendMessageBackEnum sendMessageBack{ noMessage };
                            if (arguments != nullptr)
                                for (int i = 0; arguments[i] != nullptr; i++)
                                {
                                    nss::CommandSettings argument;
                                    argument.Command(arguments[i]);
                                    
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
                    else if (nss::Command(command, L"sound "))
                    {
                        std::wstring filePath = nss::ParseAsQuoteString(command);
                        wchar_t** arguments = nss::ParseArguments(command);
                        
                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::SoundPlayer>(0);
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
                                
                                free(arguments[i]);
                            }
                        if (arguments != nullptr)
                            free(arguments);
                        
                        if (component->sendMessageBack != component->noMessage)
                            OnHold(component);
                        
                        soundGroup = ns::list::Insert<ns::NovelComponents::SoundPlayer>(soundGroup);
                        soundGroup->data = component;
                        component->SetGroup(soundGroup);
                        
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
                                cout << base::ConvertToUTF8(key.first) << ": " << (key.second->type == key.second->String ? "\"" : "") << *key.second << (key.second->type == key.second->String ? "\"" : "") << endl;
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
                                        wchar_t** arguments = nss::ParseArguments(command);
                                        
                                        if (dialogueGroup != nullptr)
                                        {
                                            List<Dialogue>* temp = dialogueGroup;
                                            while (temp != nullptr)
                                            {
                                                if (temp->data != nullptr)
                                                {
                                                    temp->data->priority++;
                                                    temp->data->SetGUISystem(nullptr);
                                                }
                                                temp = temp->next;
                                            }
                                        }
                                        auto* component = layers.PrioritizeComponent<ns::NovelComponents::Dialogue>(10000, this);
                                        
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
                                                
                                                delete arguments[i];
                                            }
                                        if (arguments != nullptr)
                                            delete arguments;
                                        
                                        if (component->sendMessageBack != component->noMessage)
                                            OnHold(component);
                                        
                                        dialogueGroup = ns::list::Insert<ns::NovelComponents::Dialogue>(dialogueGroup);
                                        dialogueGroup->data = component;
                                        component->SetGroup(dialogueGroup);
                                        
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
