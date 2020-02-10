//
//  GUI.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 11/09/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "GUISystem.hpp"

namespace ns
{
    namespace NovelComponents
    {
        GUISystem::~GUISystem() {  }
        GUISystem::GUISystem() : Savable(L"GUISystem") {  }
        void GUISystem::Destroy() { if (isNovel) novelSystem->SendMessage({"Destroy", this}); clear(); }
        void GUISystem::PollEvent(sf::Event& event)
        {
            for (auto g : guiObjects)
                if (g->visible && ((gs::trueVerticalOrientation && g->scaleVertical) || (!gs::trueVerticalOrientation && g->scaleHorizontal)))
                {
                    g->PollEvent(event);
                    if (g->child) g->child->PollEvent(event);
                }
        }
        void GUISystem::Update(const sf::Time& elapsedTime)
        {
            if (isNovel)
            {
                switch (mode)
                {
                    case appearing: gs::requestWindowRefresh = true;
                        if (currentTime < appearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= appearTime)
                        {
                            alpha = maxAlpha; currentTime = 0.f;
                            mode = existing;
                            if (sendMessageBack == atAppearance) novelSystem->SendMessage({"UnHold", this});
                        }
                        else alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
                        SetAlpha(alpha);
                        break; 
                    case disappearing: gs::requestWindowRefresh = true;
                        if (currentTime < disappearTime) currentTime += elapsedTime.asSeconds();
                        if (currentTime >= disappearTime)
                        {
                            alpha = 0; currentTime = 0.f;
                            mode = deprecated;
                            if (sendMessageBack == atDeprecated) novelSystem->SendMessage({"UnHold", this});
                        }
                        else alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
                        SetAlpha(alpha);
                        break;
                    case deprecated: gs::requestWindowRefresh = true; novelSystem->PopComponent(this); break;
                    default: break;
                }
            }
            for (auto g : guiObjects)
            {
                g->Update(elapsedTime);
                if (g->regulateFadings) g->FadingUpdate(elapsedTime);
                if (g->child) g->child->Update(elapsedTime);
            }
        }
        void GUISystem::Resize(const unsigned int& width, const unsigned int& height)
        {
            if (drawConstrains)
            {
                constrainsShape.setOutlineColor(sf::Color::Cyan);
                constrainsShape.setFillColor(sf::Color::Transparent);
                constrainsShape.setOutlineThickness(gs::scale);
            }
            if (width != lastWidth || height != lastHeight)
            {
                lastWidth = width; lastHeight = height;
                for (auto g : guiObjects)
                    if (g->visible && ((gs::trueVerticalOrientation && g->scaleVertical) || (!gs::trueVerticalOrientation && g->scaleHorizontal)))
                    {
                        //TODO: Make working good conditions to show something or not (constrains.displayWhenVariableIsTrue)
                        g->constrains.Recalculcalation(*g, width, height);
                        if (g->child) g->child->Resize(width, height);
                    }
            }
        }
        void GUISystem::Draw(sf::RenderWindow* window)
        {
            for (auto g : guiObjects)
                if (g->visible && ((gs::trueVerticalOrientation && g->scaleVertical) || (!gs::trueVerticalOrientation && g->scaleHorizontal)))
                {
                    g->Draw(window);
                    if (drawConstrains)
                    {
                        constrainsShape.setSize({ g->constrains.globalBounds.width, g->constrains.globalBounds.height });
                        constrainsShape.setPosition(g->constrains.globalBounds.left, g->constrains.globalBounds.top);
                        window->draw(constrainsShape);
                    }
                    if (g->child) g->child->Draw(window);
                }
        }
        void GUISystem::ReceiveMessage(MessageHolder& message)
        {
            for (auto g : guiObjects)
            {
                g->ReceiveMessage(message);
                if (g->child) g->child->ReceiveMessage(message);
            }
        }
        void GUISystem::VariableResize(const unsigned int& width, const unsigned int& height)
        {
            lastWidth = width;
            lastHeight = height;
            
            for (auto g : guiObjects)
                if (g->visible && !g->ignoreVariableChange)
                {
                    g->constrains.Recalculcalation(*g, width, height);
                    if (g->child) g->child->VariableResize(width, height);
                }
        }
        void GUISystem::VariableChange(const std::wstring& varName)
        {
            for (auto g : guiObjects)
                if (!g->ignoreVariableChange)
                {
                    g->VariableChange(varName);
                    if (g->child) g->child->VariableChange(varName);
                }
        }
        void GUISystem::SetAlpha(sf::Uint8 alpha)
        {
            lastAlpha = alpha;
            for (auto g : guiObjects)
                if (!g->regulateFadings)
                {
                    g->SetAlpha(alpha);
                    if (g->child) g->child->SetAlpha(alpha);
                }
        }
        void GUISystem::SetAlphaIfBigger(sf::Uint8 alpha)
        {
            if ((unsigned int)lastAlpha < (unsigned int)alpha)
            {
                lastAlpha = alpha;
                for (auto g : guiObjects)
                    if (!g->regulateFadings)
                    {
                        g->SetAlpha(alpha);
                        if (g->child) g->child->SetAlphaIfBigger(alpha);
                    }
            }
        }
        void GUISystem::clear()
        {
            for (auto g : guiObjects)
            {
                if (g->child)
                {
                    g->child->clear();
                    delete g->child;
                }
                g->Destroy();
                delete g;
            }
            guiObjects.clear();
        }
        void GUISystem::ResetResize()
        {
            lastWidth = 0; lastHeight = 0;
            for (auto g : guiObjects) if (g->child != nullptr) g->child->ResetResize();
        }
        
        
        
        GUIConstrainsResult::GUIConstrainsResult() { }
        GUIConstrainsResult::GUIConstrainsResult(int res, bool depends, bool consta, bool needs, bool globfac) : result(res), dependsOnVariable(depends), constant(consta), needsToBeScaled(needs), scaledByGlobalFactor(globfac) { }
        GUIConstrainsResult GUIConstrains::Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height, std::wstring& line, float scalingFactor)
        {
            //TODO: Words parsing, so the can be compared to variables and keywords
            //      Seperators: ',' '+' '-' '*' '/' etc
            std::wstring finalLine = L"";
            bool mustBeRecalculated{ false }, reallyOnlyScaled{ false }, onlyNeedsScaling{ true }, dependVariable{ false },  scalByGlobalFactor{ false };
            
            bool itsBeingSummed{ true };
            wchar_t before = L'\0';
            
            unsigned long lastPos = 0;
            while (lastPos < line.length())
            {
                std::wstring word = L"";
                
                //looking for "words"
                bool wordFound{ false }, itHasAPoint{ false }, doScale{ true }, forceScale{ false }, itWasANumber{ true };
                for (unsigned long i = lastPos; i < line.length() && !wordFound; i++)
                {
                    if (line[i] == L'.')
                        itHasAPoint = true;
                    if ((line[i] == L'c' || line[i] == L'u') && itWasANumber && i != lastPos)
                        doScale = false;
                    if (line[i] == L'r' && itWasANumber && i != lastPos)
                        forceScale = true;
                    if (line[i] == L's' && itWasANumber && i != lastPos)
                        scalByGlobalFactor = true;
                    
                    if (line[i] == L'+' || line[i] == L'-' || line[i] == L'/' || line[i] == L'*')
                        wordFound = true;
                    else if (line[i] != L' ' && line[i] != L'(' && line[i] != L')')
                    {
                        itWasANumber = itWasANumber && ((line[i] >= 48 && line[i] <= 57) || line[i] == L'.' ||
                                                        (((line[i] == L'c' || line[i] == L'u')) && !doScale) ||
                                                        (line[i] == L'r' && forceScale) || (line[i] == L's' && scalByGlobalFactor));
                        word += line[i];
                    }
                    
                    if (wordFound)
                    {
                        itsBeingSummed = ((before == '\0' && (line[i] == L'+' || line[i] == L'-')) ||
                                          (line[i] == '\0' && (before == L'+' || before == L'-')) ||
                                          ((line[i] == L'+' || line[i] == L'-') && (before == L'+' || before == L'-')) );
                        before = line[i];
                        lastPos = i + 1;
                    }
                }
                if (!wordFound)
                {
                    itsBeingSummed = (before == '\0' || before == L'+' || before == L'-');
                    lastPos = line.length() + 1;
                }
                
                if (word.length() > 0)
                {
                    //analyzing the "word"
                    if (itHasAPoint && !itWasANumber)
                    {
                        std::wstring replaceBy = L"";
                        for (int i = 0; i < word.length(); i++)
                        {
                            if (word[i] == L'.')
                            {
                                NovelVariable *nvar{ nullptr };
                                std::wstring possibleVariable = L"";
                                for (int j = 0; j < i; j++) possibleVariable += word[j];
                                if (possibleVariable.length() != 0) nvar = VariableSystem::FindVariable(possibleVariable);
                                
                                bool availableToSelf{ false };
                                if (possibleVariable == L"@text") {
                                    GUIObjects::Text* textPtr = reinterpret_cast<GUIObjects::Text*>(&guiObject);
                                    if (textPtr) { availableToSelf = true; nvar = new NovelVariable(textPtr->textString); }
                                } else if (possibleVariable == L"@image") {
                                    GUIObjects::Image* textPtr = reinterpret_cast<GUIObjects::Image*>(&guiObject);
                                    if (textPtr) { availableToSelf = true; nvar = new NovelVariable(textPtr->imagePath); }
                                }
                                
                                if (possibleVariable.length() != 0 && !nvar)
                                    cout << "Warning :: GUI :: Unknown expression when recalculating: '" << base::utf8(possibleVariable) << "'." << endl;
                                else
                                {
                                    std::wstring possibleFunction = L"";
                                    for (int j = i; j < word.length(); j++) possibleFunction += word[j];
                                    if (possibleFunction == L".width")
                                    {
                                        mustBeRecalculated = true;
                                        onlyNeedsScaling = false;
                                        if (possibleVariable.length() == 0)
                                        {
                                            if (guiObject.guiSystem && guiObject.guiSystem->parent)
                                                replaceBy += std::to_wstring(guiObject.guiSystem->parent->constrains.globalBounds.width);
                                            else replaceBy += std::to_wstring(gs::width);
                                        }
                                        else
                                        {
                                            //TODO: If it's a string, then this code should calculate it's width.
                                            //      But the width depends on the font and the character size, so some virtual function
                                            //      should be called that is overwrited only by GUIText components.
                                            if (nvar->type == nvar->String)
                                            {
                                                dependVariable = true;
                                                dependOnVariables.insert({possibleVariable, true});
                                                
                                                if (possibleVariable == L"@name")
                                                {
                                                    if (TemporarySettings::name)
                                                    {
                                                        sf::Text text = *TemporarySettings::name;
                                                        text.setString(nvar->value.asString);
                                                        replaceBy += std::to_wstring((int)text.getLocalBounds().width);
                                                    }
                                                }
                                                else if (possibleVariable == L"@dialogue")
                                                {
                                                    if (TemporarySettings::dialogue)
                                                        replaceBy += std::to_wstring((int)TemporarySettings::dialogue->getLocalBounds().width);
                                                }
                                                else if (possibleVariable == L"@text")
                                                {
                                                    GUIObjects::Text* textPtr = reinterpret_cast<GUIObjects::Text*>(&guiObject);
                                                    if (textPtr) replaceBy += std::to_wstring((int)textPtr->text.getLocalBounds().width);
                                                    else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                                }
                                                else if (possibleVariable == L"@image")
                                                {
                                                    GUIObjects::Image* imagePtr = reinterpret_cast<GUIObjects::Image*>(&guiObject);
                                                    if (imagePtr) replaceBy += std::to_wstring((int)imagePtr->sprite.getGlobalBounds().width);
                                                    else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                                }
                                                else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                            }
                                        }
                                    }
                                    else if (possibleFunction == L".height")
                                    {
                                        mustBeRecalculated = true;
                                        onlyNeedsScaling = false;
                                        
                                        if (possibleVariable.length() == 0)
                                        {
                                            if (guiObject.guiSystem && guiObject.guiSystem->parent)
                                                replaceBy += std::to_wstring(guiObject.guiSystem->parent->constrains.globalBounds.height);
                                            else replaceBy += std::to_wstring(gs::height);
                                        }
                                        else
                                        {
                                            //TODO: If it's a string, then this code should calculate it's height.
                                            //      But the width depends on the font and the character size, so some virtual function
                                            //      should be called that is overwrited only by GUIText components.
                                            if (nvar->type == nvar->String)
                                            {
                                                dependVariable = true;
                                                dependOnVariables.insert({possibleVariable, true});
                                                
                                                if (possibleVariable == L"@name")
                                                {
                                                    if (TemporarySettings::name)
                                                    {
                                                        sf::Text text = *TemporarySettings::name;
                                                        text.setString(nvar->value.asString);
                                                        replaceBy += std::to_wstring((int)text.getLocalBounds().height);
                                                    }
                                                }
                                                else if (possibleVariable == L"@dialogue")
                                                {
                                                    if (TemporarySettings::dialogue)
                                                        replaceBy += std::to_wstring((int)TemporarySettings::dialogue->getLocalBounds().height);
                                                }
                                                else if (possibleVariable == L"@text")
                                                {
                                                    GUIObjects::Text* textPtr = reinterpret_cast<GUIObjects::Text*>(&guiObject);
                                                    if (textPtr) replaceBy += std::to_wstring((int)textPtr->text.getLocalBounds().height);
                                                    else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                                }
                                                else if (possibleVariable == L"@image")
                                                {
                                                    GUIObjects::Image* imagePtr = reinterpret_cast<GUIObjects::Image*>(&guiObject);
                                                    if (imagePtr) replaceBy += std::to_wstring((int)imagePtr->sprite.getGlobalBounds().height);
                                                    else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                                }
                                                else cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                            }
                                        }
                                    }
                                    else if (possibleFunction == L".length")
                                    {
                                        mustBeRecalculated = true;
                                        onlyNeedsScaling = false;
                                        
                                        if (possibleVariable.length() != 0)
                                            if (nvar->type == nvar->String)
                                            {
                                                dependOnVariables.insert({possibleVariable, true});
                                                dependVariable = true;
                                                replaceBy += std::to_wstring(std::wstring(nvar->value.asString).length());
                                            }
                                    }
                                    
                                    if (availableToSelf) delete nvar;
                                }
                            }
                        }
                        
                        //replacing the whole text by calculated expression
                        if (replaceBy.length() != 0)
                        {
                            //line.replace(wordStartPos, lastPos - 1 - wordStartPos, replaceBy);
                            finalLine += replaceBy;
                            if (lastPos <= line.length() && lastPos > 0)
                                finalLine += line[lastPos - 1];
                        }
                    }
                    else
                    {
                        if (!itWasANumber)
                        {
                            NovelVariable *nvar{ VariableSystem::FindVariable(word) };
                            if (word.length() != 0 && !nvar)
                                cout << "Warning :: GUI :: Unknown expression when recalculating: '" << base::utf8(word) << "'." << endl;
                            else if (nvar)
                            {
                                dependVariable = true;
                                mustBeRecalculated = true;
                                onlyNeedsScaling = false;
                                
                                dependOnVariables.insert({word, true});
                                
                                switch (nvar->type)
                                {
                                    case NovelVariable::String: doScale = false;
                                        word = std::wstring(nvar->value.asString);
                                        break;
                                    case NovelVariable::Boolean: doScale = false;
                                        if (nvar->value.asBoolean) word = L"true";
                                        else word = L"false";
                                        break;
                                    case NovelVariable::Integer:
                                        word = std::to_wstring(nvar->value.asInt);
                                        break;
                                    default: doScale = false; word = L"NotSet"; break;
                                }
                            }
                        }
                        
                        
                        if ((doScale && itsBeingSummed) || forceScale)
                        {
                            mustBeRecalculated = true;
                            if (onlyNeedsScaling) reallyOnlyScaled = true;
                            
                            float left = base::atof(word);
                            if (scalByGlobalFactor) left = left * scalingFactor;
                            else left = left * gs::scale;
                            finalLine += std::to_wstring(left);
                            
                            if (lastPos <= line.length() && lastPos > 0)
                                finalLine += line[lastPos - 1];
                        }
                        else
                        {
                            if (!mustBeRecalculated)
                            {
                                for (int j = 0; j < word.length(); ++j)
                                    if ((word[j] >= 48 && word[j] <= 57) || word[j] == '.')
                                        finalLine += word[j];
                            }
                            else finalLine += word;
                            
                            if (lastPos <= line.length() && lastPos > 0)
                                finalLine += line[lastPos - 1];
                        }
                    }
                }
                else if (lastPos <= line.length() && lastPos > 0)
                    finalLine += line[lastPos - 1];
            }
            
            if (!onlyNeedsScaling) { reallyOnlyScaled = false; scalByGlobalFactor = false; }
            float left = nss::MathParser(finalLine);
            return GUIConstrainsResult(left, dependVariable, !mustBeRecalculated, reallyOnlyScaled, scalByGlobalFactor);
        }
        float GUIConstrains::RecalculateCompact(GUIObject& guiObject, GUIConstrainsResult& res, const unsigned int& width, const unsigned int& height, const unsigned int& i, std::wstring& analyze, int& sresult)
        {
            float result = 0;
            if (!notSet[i])
            {
                if (constant[i]) result = sresult;
                else if (scaledByGlobalFactor[i]) result = sresult * gs::scalex;
                else if (onlyNeedsToBeScaled[i]) result = sresult * gs::scale;
                else if (analyze.length() != 0)
                {
                    res = Recalculate(guiObject, width, height, analyze, gs::scalex);
                    result = res.result;
                    
                    constant[i] = res.constant;
                    onlyNeedsToBeScaled[i] = res.needsToBeScaled;
                    scaledByGlobalFactor[i] = res.scaledByGlobalFactor;
                    if (scaledByGlobalFactor[i]) sresult = (float)result/gs::scalex;
                    else if (constant[i] || onlyNeedsToBeScaled[i]) sresult = (float)result/gs::scale;
                    isDependsOnVariable[i] = res.dependsOnVariable;
                }
                else notSet[i] = true;
            }
            return result;
        }
        void GUIConstrains::Recalculcalation(GUIObject& guiObject, unsigned int width, unsigned int height)
        {
            guiObject.PreCalculate(width, height);
            
            GUIConstrainsResult res;
            localBounds.left = localBounds.width = localBounds.top = localBounds.height = globalBounds.left = globalBounds.width = globalBounds.top = globalBounds.height = 0;
            
            localBounds.left = RecalculateCompact(guiObject, res, width, height, 0, leftS, sleft);
            globalBounds.width = RecalculateCompact(guiObject, res, width, height, 4, widthS, swidth);
            localBounds.width = RecalculateCompact(guiObject, res, width, height, 1, rightS, sright);
            
            localBounds.top = RecalculateCompact(guiObject, res, width, height, 3, topS, stop);
            globalBounds.height = RecalculateCompact(guiObject, res, width, height, 5, heightS, sheight);
            localBounds.height = RecalculateCompact(guiObject, res, width, height, 2, bottomS, sbottom);
            
            if (guiObject.guiSystem && guiObject.guiSystem->parent)
            {
                if (notSet[4] || globalBounds.width <= 0)
                    globalBounds.width += guiObject.guiSystem->parent->constrains.globalBounds.width - localBounds.left - localBounds.width;
                if (notSet[5] || globalBounds.height <= 0)
                    globalBounds.height += guiObject.guiSystem->parent->constrains.globalBounds.height - localBounds.top - localBounds.height;
            }
            else
            {
                if (notSet[4] || globalBounds.width <= 0)
                    globalBounds.width = width - localBounds.left - localBounds.width;
                if (notSet[5] || globalBounds.height <= 0)
                    globalBounds.height = height - localBounds.top - localBounds.height;
            }
            
            guiObject.PreCalculatedSize(width, height);
            localBounds.left += RecalculateCompact(guiObject, res, width, height, 6, posXS, sposX);
            localBounds.top += RecalculateCompact(guiObject, res, width, height, 7, posYS, sposY);
            
            if (guiObject.guiSystem && guiObject.guiSystem->parent)
            {
                if (notSet[6]) {
                    if (notSet[0] && !notSet[1]) localBounds.left = guiObject.guiSystem->parent->constrains.globalBounds.width - globalBounds.width - localBounds.width;
                    else if (!notSet[0] && !notSet[1]) localBounds.left = guiObject.guiSystem->parent->constrains.globalBounds.width/2 - (-localBounds.left + localBounds.width)/2 - globalBounds.width/2;
                } globalBounds.left = guiObject.guiSystem->parent->constrains.globalBounds.left + localBounds.left;
                
                if (notSet[7]) {
                    if (notSet[3] && !notSet[2])
                        localBounds.top = guiObject.guiSystem->parent->constrains.globalBounds.height - globalBounds.height - localBounds.height;
                    else if (!notSet[2] && !notSet[3]) localBounds.top = guiObject.guiSystem->parent->constrains.globalBounds.height/2 - (-localBounds.top + localBounds.height)/2 - globalBounds.height/2;
                } globalBounds.top = guiObject.guiSystem->parent->constrains.globalBounds.top + localBounds.top;
            }
            else
            {
                if (notSet[4] || globalBounds.width <= 0)
                    globalBounds.width = width - localBounds.left - localBounds.width;
                if (notSet[5] || globalBounds.height <= 0)
                    globalBounds.height = height - localBounds.top - localBounds.height;
                
                if (notSet[6]) {
                    if (notSet[0] && !notSet[1]) localBounds.left = width - globalBounds.width - localBounds.width;
                    else if (!notSet[0] && !notSet[1]) localBounds.left = width/2 - (-localBounds.left + localBounds.width)/2 - globalBounds.width/2;
                } globalBounds.left = localBounds.left;
                
                if (notSet[7]) {
                    if (notSet[3] && !notSet[2])
                        localBounds.top = height - globalBounds.height - localBounds.height;
                    else if (!notSet[2] && !notSet[3]) localBounds.top = height/2 - (-localBounds.top + localBounds.height)/2 - globalBounds.height/2;
                } globalBounds.top = localBounds.top;
            }
            
            //cout << this << " " << globalBounds.left << " (" << localBounds.left << ") to " << globalBounds.width << " (" << localBounds.width << "); " << globalBounds.top << " (" << localBounds.top << ") to " << globalBounds.height << " (" << localBounds.height << ")" << endl;
            guiObject.Resize(width, height);
        }
        
        
        
        void GUIObject::FadingUpdate(const sf::Time& elapsedTime)
        {
            switch (fadingsMode)
            {
                case appearing: gs::requestWindowRefresh = true;
                    currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime)
                    {
                        currentTime = 0.f;
                        fadingsMode = offline;
                        regulateFadings = false;
                        
                        alpha = 255; SetAlpha(alpha);
                        if (child) child->SetAlpha(alpha);
                    }
                    else
                    {
                        alpha = (sf::Uint8)(255 * currentTime/appearTime);
                        SetAlpha(alpha);
                        if (child) child->SetAlpha(alpha);
                    }
                    break;
                    
                case disappearing: gs::requestWindowRefresh = true;
                    currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime)
                    {
                        currentTime = 0.f;
                        fadingsMode = offline;
                        
                        alpha = 0; SetAlpha(alpha);
                        if (child) child->SetAlpha(alpha);
                    }
                    else
                    {
                        alpha = (sf::Uint8)(255 - 255 * currentTime/disappearTime);
                        SetAlpha(alpha);
                        if (child) child->SetAlpha(alpha);
                    }
                    break;
                    
                default: break;
            }
        }
        void GUIObject::SetFadings(fadingsModeEnum newMode, float forTime)
        {
            if (newMode == appearing) ignoreVariableChange = false;
            else ignoreVariableChange = true;
            
            if (fadingsMode != newMode) currentTime = 0.f;
            if (newMode == appearing) { appearTime = forTime; fadingsMode = newMode; }
            else if (newMode == disappearing)
            {
                disappearTime = forTime;
                if (guiSystem->lastAlpha == 0) { fadingsMode = offline; SetAlpha(0); if (child) child->SetAlpha(0); }
                else fadingsMode = newMode;
            }
            if (newMode == offline) { SetAlpha(0); if (child) child->SetAlpha(0); }
            regulateFadings = true;
        }
        void GUIObject::VariableChange(const std::wstring &varName)
        {
            if (constrains.dependOnVariables.find(varName) != constrains.dependOnVariables.end())
            {
                //TODO: Make working good conditions to show something or not (constrains.displayWhenVariableIsTrue)
                /*if (constrains.displayWhenVariableIsTrue.length() != 0 && varName == constrains.displayWhenVariableIsTrue)
                 {
                 bool lastVisible = visible;
                 NovelVariable* nvar = guiSystem->novel->FindVariable(constrains.displayWhenVariableIsTrue);
                 if (nvar != nullptr)
                 {
                 switch (nvar->type)
                 {
                 case NovelVariable::String:
                 visible = std::wstring(nvar->value.asString).length() > 0;
                 break;
                 case NovelVariable::Integer:
                 visible = nvar->value.asInt != 0;
                 break;
                 case NovelVariable::Boolean:
                 visible = nvar->value.asBoolean;
                 break;
                 default:
                 break;
                 }
                 }
                 
                 if (visible != lastVisible)
                 {
                 constrains.Recalculate(*this, ns::gs::width, ns::gs::height);
                 Resize(ns::gs::width, ns::gs::height);
                 }
                 }*/
                
                cout << "VariableResize's Resize" << endl;
                //PreCalculate(gs::width, gs::height);
                //constrains.Recalculate(*this, gs::width, gs::height, 1);
                //PreCalculatedSize(gs::width, gs::height);
                //constrains.Recalculate(*this, gs::width, gs::height, 2);
                constrains.Recalculcalation(*this, gs::width, gs::height);
                if (child) child->VariableResize(gs::width, gs::height);
                //Resize(gs::width, gs::height);
                
                /*bool changed{ false };
                if (!constrains.leftC && constrains.isDependsOnVariable[0] && constrains.leftS.length() != 0)
                {
                    int oldConstraint = constrains.left;
                    constrains.left = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.leftS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        constrains.left += guiSystem->parent->constrains.left;
                    if (oldConstraint != constrains.left)
                        changed = true;
                }
                if (!constrains.rightC && constrains.isDependsOnVariable[1] && constrains.rightS.length() != 0)
                {
                    int oldConstraint = constrains.right;
                    constrains.right = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.rightS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        constrains.right += guiSystem->parent->constrains.right;
                    if (oldConstraint != constrains.right)
                        changed = true;
                }
                if (!constrains.topC && constrains.isDependsOnVariable[2] && constrains.topS.length() != 0)
                {
                    int oldConstraint = constrains.top;
                    constrains.top = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.topS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        constrains.top += guiSystem->parent->constrains.top;
                    if (oldConstraint != constrains.top)
                        changed = true;
                }
                if (!constrains.bottomC && constrains.isDependsOnVariable[3] && constrains.bottomS.length() != 0)
                {
                    int oldConstraint = constrains.bottom;
                    constrains.bottom = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.bottomS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        constrains.bottom += guiSystem->parent->constrains.bottom;
                    if (oldConstraint != constrains.bottom)
                        changed = true;
                }
                if (!constrains.widthC && constrains.isDependsOnVariable[4] && constrains.widthS.length() != 0)
                {
                    int oldConstraint = constrains.width;
                    constrains.width = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.widthS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                    {
                        if (constrains.width == 0)
                            constrains.width = guiSystem->parent->constrains.width - constrains.right - constrains.left;
                    }
                    else if (constrains.width == 0)
                        constrains.width = ns::gs::width - constrains.right - constrains.left;
                    if (oldConstraint != constrains.width)
                    {
                        changed = true;
                        //cuz posX depends on width
                        if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        {
                            if (!constrains.posXC && constrains.posXS.length() == 0)
                                constrains.posX = guiSystem->parent->constrains.posX - guiSystem->parent->constrains.left + constrains.left;
                        }
                        else if (!constrains.posXC && constrains.posXS.length() == 0)
                            constrains.posX = constrains.left;
                    }
                }
                if (!constrains.heightC && constrains.isDependsOnVariable[5] && constrains.heightS.length() != 0)
                {
                    int oldConstraint = constrains.height;
                    constrains.height = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.heightS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                    {
                        if (constrains.height == 0)
                            constrains.height = guiSystem->parent->constrains.height - constrains.top - constrains.bottom;
                    }
                    else if (constrains.height == 0)
                        constrains.height = ns::gs::height - constrains.top - constrains.bottom;
                    
                    if (oldConstraint != constrains.height)
                    {
                        changed = true;
                        //cuz posY depends on height
                        if (guiSystem != nullptr && guiSystem->parent != nullptr)
                        {
                            if (!constrains.posYC && constrains.posYS.length() == 0)
                                constrains.posY = guiSystem->parent->constrains.posY + guiSystem->parent->constrains.height + guiSystem->parent->constrains.bottom - constrains.bottom - constrains.height;
                        }
                        else if (!constrains.posYC && constrains.posYS.length() == 0)
                            constrains.posY = ns::gs::height - constrains.bottom - constrains.height;
                    }
                }
                if (!constrains.posXC && constrains.isDependsOnVariable[6] && constrains.posXS.length() != 0)
                {
                    int oldConstraint = constrains.posX;
                    constrains.posX = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.posXS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                    {
                        if (!constrains.posXC && constrains.posXS.length() == 0)
                            constrains.posX = guiSystem->parent->constrains.posX + constrains.left;
                    }
                    else if (!constrains.posXC && constrains.posXS.length() == 0)
                        constrains.posX = constrains.left;
                    if (oldConstraint != constrains.posX)
                        changed = true;
                }
                if (!constrains.posYC && constrains.isDependsOnVariable[7] && constrains.posYS.length() != 0)
                {
                    int oldConstraint = constrains.posY;
                    constrains.posY = constrains.Recalculate(*this, ns::gs::width, ns::gs::height, constrains.posYS).result;
                    if (guiSystem != nullptr && guiSystem->parent != nullptr)
                    {
                        if (!constrains.posYC && constrains.posYS.length() == 0)
                            constrains.posY = guiSystem->parent->constrains.posY + guiSystem->parent->constrains.height - constrains.bottom - constrains.height;
                    }
                    else if (!constrains.posYC && constrains.posYS.length() == 0)
                        constrains.posY = ns::gs::height - constrains.bottom - constrains.height;
                    if (oldConstraint != constrains.posY)
                        changed = true;
                }
                
                if (changed)
                {
                    if (child != nullptr)
                        child->VariableResize(ns::gs::width, ns::gs::height);
                    Resize(ns::gs::width, ns::gs::height);
                }*/
            }
        }
        GUISystem* GUIObject::GetChildSystem()
        {
            if (!child)
            {
                child = new GUISystem();
                child->parent = this;
                return child;
            }
            else return child;
        }
        bool GUISystem::LoadFromFile(const std::wstring& fileName, std::wstring guiScope)
        {
            bool skinLoaded{ false };
            std::wstring fullPath = fileName; trueFileName = fileName;
            if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
            
            std::wifstream wif;
#ifdef _WIN32
            wif.open(fullPath);
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(fullPath);
            wif.open(u8str);
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            if (!wif.is_open())
                cout << "Error :: Skin :: File couldn't be opened, path: " << base::utf8(fileName) << endl;
            else
            {
                scope = guiScope; this->fileName = fileName;
                bool eof{ false }; std::wstring line;
                nss::CommandSettings command;
                std::wstring folderPath = base::GetFolderPath(fileName);
                
                bool parsingGUI{ (guiScope == L"") };
                list<GUIScopeStruct> scope; scope.emplace_front(nullptr);
                unsigned int knownType = 0;
                std::wstring forArgumentsParsing{ L"" }, defaultFont{ L"" };
                GUIObject* component{ nullptr };
                
                while (!eof)
                {
                    if (!wif.eof())
                    {
                        std::getline(wif, line); command.Command(line, true);
                        if (!parsingGUI)
                        {
                            if (nss::Command(command, L"//")) { /* that's a comment */ }
                            else if (nss::Command(command, L"gui "))
                            {
                                std::wstring nameParsed = nss::ParseAsMaybeQuoteString(command);
                                parsingGUI = (nameParsed == guiScope);
                                if (parsingGUI)
                                {
                                    skinLoaded = true; bool typeFound{ false };
                                    for (int i = 0; i < line.length() && !typeFound; i++)
                                        typeFound = (line[i] == L'{');
                                    if (!typeFound) knownType = 1;
                                }
                            }
                            else if (nss::Command(line, L"font ", true, false) || nss::Command(line, L"font:", true, false)) {
                                command.lastPos += 5; defaultFont = nss::ParseAsMaybeQuoteString(command); }
                        }
                        else
                        {
                            nss::RemoveSpaces(line);
                            bool thatsAScope{ false }, onlyOneLine{ false };
                            if (!nss::Command(command, L"action") && !nss::Command(command, L"action:"))
                                for (int i = 0; i < line.length(); ++i)
                                {
                                    if (line[i] == L'{' && knownType != 1) thatsAScope = true;
                                    else if (line[i] == L'{') knownType = 0;
                                    else if (line[i] == L'}')
                                    {
                                        if (thatsAScope) { thatsAScope = false; onlyOneLine = true; }
                                        else if (scope.size() != 0) scope.erase(scope.begin());
                                    }
                                }
                            else command.lastPos = 0;
                            
                            if (scope.size() != 0)
                            {
                                if (thatsAScope || onlyOneLine)
                                {
                                    if (knownType == 0)
                                    {
                                        if (nss::Command(command, L"rectangle"))        knownType = 2;
                                        else if (nss::Command(command, L"text"))        knownType = 3;
                                        else if (nss::Command(command, L"image"))       knownType = 4;
                                        else if (nss::Command(command, L"@dialogue"))   knownType = 5;
                                        else if (nss::Command(command, L"@name"))       knownType = 6;
                                        else if (command.line == L"background" ||
                                                 nss::Command(command, L"background ")) knownType = 10;
                                        else if (nss::Command(command, L"button:"))     knownType = 7;
                                        else if (nss::Command(command, L"button") ||
                                                 nss::Command(command, L"sbutton"))     knownType = 8;
                                        else if (nss::Command(command, L"rbutton") ||
                                                 nss::Command(command, L"rectbutton"))  knownType = 9;
                                        if (knownType != 0) forArgumentsParsing = line;
                                    }
                                    
                                    if (knownType != 0)
                                    {
                                        GUISystem* guiSystemToAddTo{ nullptr };
                                        if (scope.front().object.obj) guiSystemToAddTo = scope.front().object.obj->GetChildSystem();
                                        else if (!scope.front().object.obj && scope.size() == 1) guiSystemToAddTo = this;
                                        GUIObject* parent = scope.front().object.obj;
                                        
                                        if (guiSystemToAddTo)
                                        {
                                            GUIObjects::Rectangle* rect{ nullptr };
                                            GUIObjects::Image* img{ nullptr };
                                            GUIObjects::Text* text{ nullptr };
                                            GUIObjects::DialogueConstrains* dconstr{ nullptr };
                                            GUIObjects::Button* button{ nullptr };
                                            switch (knownType)
                                            {
                                                case 2: /// rectangle
                                                    rect = guiSystemToAddTo->AddComponent<GUIObjects::Rectangle>();
                                                    rect->shape.setFillColor(sf::Color(0,0,0,0));
                                                    component = rect; scope.emplace_front(component, GUIScopeStruct::Rectangle);
                                                    break;
                                                case 3: /// text
                                                    text = guiSystemToAddTo->AddComponent<GUIObjects::Text>();
                                                    if (defaultFont.length() && defaultFont != L"") text->SetFont(defaultFont);
                                                    else if (Skin::self) text->SetFont(Skin::self->defaultFontName);
                                                    else text->SetFont(lang::menufont);
                                                    component = text; scope.emplace_front(component, GUIScopeStruct::Text);
                                                    break;
                                                case 4: /// image
                                                    img = guiSystemToAddTo->AddComponent<GUIObjects::Image>();
                                                    component = img; scope.emplace_front(component, GUIScopeStruct::Image);
                                                    break;
                                                case 10: /// image (background)
                                                    img = guiSystemToAddTo->AddComponent<GUIObjects::Image>();
                                                    img->fitMode = GUIObjects::Image::fillCentre; img->doParallax = gs::isParallaxEnabled;
                                                    component = img; scope.emplace_front(component, GUIScopeStruct::Image);
                                                    break;
                                                case 5: /// @dialogue
                                                    dconstr = guiSystemToAddTo->AddComponent<GUIObjects::DialogueConstrains>();
                                                    component = dconstr; scope.emplace_front(component, GUIScopeStruct::DialogueConstrains);
                                                    //if (Skin::self) Skin::self->dialogue.textConstrains = component;
                                                    break;
                                                case 6: /// @name
                                                    dconstr = guiSystemToAddTo->AddComponent<GUIObjects::DialogueConstrains>();
                                                    component = dconstr; scope.emplace_front(component, GUIScopeStruct::DialogueConstrains);
                                                    //if (Skin::self) Skin::self->dialogue.nameConstrains = component;
                                                    break;
                                                case 7: /// button: (text button)
                                                    button = guiSystemToAddTo->AddComponent<GUIObjects::Button>(GUIObjects::Button::Text);
                                                    button->text->correctBoundaries = true;
                                                    if (defaultFont.length() && defaultFont != L"") button->text->setFont(defaultFont);
                                                    component = button; scope.emplace_front(component, GUIScopeStruct::Button);
                                                    break;
                                                case 8: /// button (sprite button)
                                                    button = guiSystemToAddTo->AddComponent<GUIObjects::Button>(GUIObjects::Button::Sprite);
                                                    component = button; scope.emplace_front(component, GUIScopeStruct::Button);
                                                    break;
                                                case 9: /// button: (rectangle button)
                                                    button = guiSystemToAddTo->AddComponent<GUIObjects::Button>(GUIObjects::Button::Rect);
                                                    if (defaultFont.length() && defaultFont != L"") button->rect->setFont(defaultFont);
                                                    component = button; scope.emplace_front(component, GUIScopeStruct::Button);
                                                    break;
                                                default: component = nullptr; scope.emplace_front(component); break;
                                            }
                                            if (component && parent)
                                            {
                                                component->scaleHorizontal = parent->scaleHorizontal;
                                                component->scaleVertical = parent->scaleVertical;
                                            }
                                            
                                            nss::CommandSettings argumentLine;
                                            argumentLine.Command(forArgumentsParsing);
                                            
                                            vector<std::wstring> arguments;
                                            nss::ParseArguments(argumentLine, arguments);
                                            bool asName{ false }, asDialogue{ false };
                                            for (auto arg : arguments)
                                            {
                                                nss::CommandSettings argument; argument.Command(arg);
                                                if (nss::Command(argument, L"rectangle") || nss::Command(argument, L"image") || nss::Command(argument, L"background") || nss::Command(argument, L"text") || nss::Command(argument, L"@dialogue") || nss::Command(argument, L"@name") || nss::Command(argument, L"sbutton") || nss::Command(argument, L"rbutton") || nss::Command(argument, L"rectbutton")) { /* ignoring */ }
                                                else if (nss::Command(argument, L"button:")) button->text->setString(nss::ParseAsMaybeQuoteString(argument));
                                                else if (nss::Command(argument, L"button")) { /* ignoring */ }
                                                else if (nss::Command(argument, L"texture:") || nss::Command(argument, L"sprite:") || nss::Command(argument, L"spr:"))
                                                    button->sprite->setTexture(nss::ParseAsMaybeQuoteString(argument));
                                                else if (nss::Command(argument, L"\""))
                                                {
                                                    --argument.lastPos; std::wstring str = nss::ParseAsQuoteString(argument);
                                                    if (knownType == 3 && text) text->SetString(str);
                                                    else if ((knownType == 4 || knownType == 10) && img) img->LoadImage(folderPath + str, /*this*/ic::globalRequestSender);
                                                    else if (knownType == 8 && button) button->sprite->setTexture(folderPath + str, /*this*/ic::globalRequestSender);
                                                }
                                                else if (nss::Command(argument, L"}")) { /* ignoring */ }
                                                else if (nss::Command(argument, L"{")) { /* ignoring */ }
                                                else if (nss::Command(argument, L"name") && guiScope == L"dialogue") asName = true;
                                                /*{
                                                    if (Skin::self) { Skin::self->dialogue.nameRect = component;
                                                        Skin::self->dialogue.nameRect->SetFadings(GUIObject::offline); }
                                                }*/
                                                else if (nss::Command(argument, L"dialogue") && guiScope == L"dialogue") asDialogue = true; /*&& Skin::self)
                                                    Skin::self->dialogue.dialogueRect = component;*/
                                                else if (nss::Command(argument, L"choose") && guiScope == L"choose" && Skin::self)
                                                    Skin::self->choose.chooseRect = component;
                                                else if (nss::Command(argument, L"horizontal")) component->scaleVertical = false;
                                                else if (nss::Command(argument, L"vertical")) component->scaleHorizontal = false;
                                                else
                                                {
                                                    std::wstring str = nss::ParseAsMaybeQuoteString(argument);
                                                    if (knownType == 3 && text && text->textString == L"") text->SetString(str);
                                                    else if ((knownType == 4 || knownType == 10) && img && !img->spriteLoaded)
                                                        img->LoadImage(folderPath + str, /*this*/ic::globalRequestSender);
                                                    else if (knownType == 8 && button && !button->sprite->loaded)
                                                        button->sprite->setTexture(folderPath + str, /*this*/ic::globalRequestSender);
                                                }
                                            }
                                            if (!component->scaleVertical && !component->scaleHorizontal) component->scaleVertical = component->scaleHorizontal = true;
                                            if (Skin::self)
                                            {
                                                if (asName) {
                                                    if (component->scaleVertical) { Skin::self->dialogue.nameRectV = component;
                                                        Skin::self->dialogue.nameRectV->SetFadings(GUIObject::offline); }
                                                    if (component->scaleHorizontal) { Skin::self->dialogue.nameRectH = component;
                                                        Skin::self->dialogue.nameRectH->SetFadings(GUIObject::offline); } }
                                                if (asDialogue) {
                                                    if (component->scaleVertical) Skin::self->dialogue.dialogueRectV = component;
                                                    if (component->scaleHorizontal) Skin::self->dialogue.dialogueRectH = component; }
                                                switch (knownType)
                                                {
                                                    case 5:
                                                        if (component->scaleVertical) Skin::self->dialogue.textConstrainsV = component;
                                                        if (component->scaleHorizontal) Skin::self->dialogue.textConstrainsH = component;
                                                        break;
                                                    case 6:
                                                        if (component->scaleVertical) Skin::self->dialogue.nameConstrainsV = component;
                                                        if (component->scaleHorizontal) Skin::self->dialogue.nameConstrainsH = component;
                                                        break;
                                                    default: break;
                                                }
                                            }
                                        }
                                    }
                                    if (onlyOneLine) scope.erase(scope.begin());
                                    knownType = 0;
                                }
                                else
                                {
                                    if (nss::Command(command, L"//")) { /* that's a comment */ }
                                    else if (nss::Command(command, L"}")) { /* we've already handled this situation */ }
                                    else if (nss::Command(command, L"rectangle")) { forArgumentsParsing = line; knownType = 2; }
                                    else if (command.line == L"text" || nss::Command(command, L"text ")) { forArgumentsParsing = line; knownType = 3; }
                                    else if (nss::Command(command, L"image")) { forArgumentsParsing = line; knownType = 4; }
                                    else if (command.line == L"background" || nss::Command(command, L"background ")) { forArgumentsParsing = line; knownType = 10; }
                                    else if (nss::Command(command, L"@dialogue")) { forArgumentsParsing = line; knownType = 5; }
                                    else if (nss::Command(command, L"@name")) { forArgumentsParsing = line; knownType = 6; }
                                    else if (nss::Command(command, L"button:")) { forArgumentsParsing = line; knownType = 7; }
                                    else if (nss::Command(command, L"button") || nss::Command(command, L"sbutton")) { forArgumentsParsing = line; knownType = 8; }
                                    else if (nss::Command(command, L"rbutton") || nss::Command(command, L"rectbutton")) { forArgumentsParsing = line; knownType = 9; }
                                    else if (nss::ContainsUsefulInformation(command))
                                    {
                                        knownType = 0;
                                        if (scope.front().object.obj)
                                        {
                                            ///------------------------------- CONSTRAINS -------------------------------
                                            ///------------------------------- CONSTRAINS -------------------------------
                                            ///------------------------------- CONSTRAINS -------------------------------
                                            if (nss::Command(command, L"left:") || nss::Command(command, L"left ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.leftS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"right:") || nss::Command(command, L"right ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.rightS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"top:") || nss::Command(command, L"top ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.topS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"bottom:") || nss::Command(command, L"bottom ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.bottomS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"width:") || nss::Command(command, L"width ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.widthS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"height:") || nss::Command(command, L"height ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.heightS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"posx:") || nss::Command(command, L"positionx:") ||
                                                       nss::Command(command, L"posx ") || nss::Command(command, L"positionx ") ||
                                                       nss::Command(command, L"x:") || nss::Command(command, L"x ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.posXS = nss::ParseAsMaybeQuoteStringFull(command);
                                            } else if (nss::Command(command, L"posy:") || nss::Command(command, L"positiony:") ||
                                                       nss::Command(command, L"posy ") || nss::Command(command, L"positiony ") ||
                                                       nss::Command(command, L"y:") || nss::Command(command, L"y ")) {
                                                nss::SkipSpaces(command); scope.front()->constrains.posYS = nss::ParseAsMaybeQuoteStringFull(command);
                                            }
                                            else if (nss::Command(command, L"alpha:") || nss::Command(command, L"maxalpha:") ||
                                                     nss::Command(command, L"alpha ") || nss::Command(command, L"maxalpha "))
                                            {
                                                int possibleValue = nss::ParseAlpha(command);
                                                if (possibleValue != -1) { scope.front()->maxAlpha = possibleValue; /*scope.front()->SetAlpha(255);*/ }
                                            }
                                            else if (nss::Command(command, L"fillcolor ") ||
                                                     nss::Command(command, L"color ") || nss::Command(command, L"colour ") ||
                                                     nss::Command(command, L"fillcolor:") ||
                                                     nss::Command(command, L"color:") || nss::Command(command, L"colour:") ||
                                                     (scope.front().type != GUIScopeStruct::Image && (nss::Command(command, L"fill ") || nss::Command(command, L"fill:"))))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255) scope.front()->SetColor(possibleColor);
                                            }
                                            ///------------------------------- SPECIFIC -------------------------------
                                            ///------------------------------- SPECIFIC -------------------------------
                                            ///------------------------------- SPECIFIC -------------------------------
                                            else if (nss::Command(command, L"outline ") || nss::Command(command, L"outlinecolor ") ||
                                                     nss::Command(command, L"ocolor ") || nss::Command(command, L"ocolour ") ||
                                                     nss::Command(command, L"outline:") || nss::Command(command, L"outlinecolor:") ||
                                                     nss::Command(command, L"ocolor:") || nss::Command(command, L"ocolour:"))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->text.setOutlineColor(possibleColor);
                                                    else if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) { button->text->onormalColor = possibleColor; button->text->text.setOutlineColor(sf::Color(possibleColor.r, possibleColor.g, possibleColor.b, button->text->text.getOutlineColor().a)); }
                                                        else if (button->type == GUIObjects::Button::Rect) { /*button->rect->onormalColor = possibleColor;*/ button->rect->text.setOutlineColor(sf::Color(possibleColor.r, possibleColor.g, possibleColor.b, button->alpha)); }
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"hover ") || nss::Command(command, L"hovercolor ") ||
                                                     nss::Command(command, L"hcolor ") || nss::Command(command, L"hcolour ") ||
                                                     nss::Command(command, L"hover:") || nss::Command(command, L"hovercolor:") ||
                                                     nss::Command(command, L"hcolor:") || nss::Command(command, L"hcolour:"))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255 && scope.front().type == GUIScopeStruct::Button)
                                                    scope.front().object.but->button->hoverColor = possibleColor;
                                            }
                                            else if (nss::Command(command, L"outlinehover ") || nss::Command(command, L"outlinehover:") ||
                                                     nss::Command(command, L"ohcolor ") || nss::Command(command, L"ohcolor:") ||
                                                     nss::Command(command, L"ohovercolor ") || nss::Command(command, L"ohovercolor:") ||
                                                     nss::Command(command, L"outlinehcolor ") || nss::Command(command, L"outlinehcolor:"))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) button->text->ohoverColor = possibleColor;
                                                        // else if (button->type == GUIObjects::Button::Rect) button->rect->ohoverColor = possibleColor;
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"presscolor ") || nss::Command(command, L"presscolor:") ||
                                                     nss::Command(command, L"pcolor ") || nss::Command(command, L"pcolor:") ||
                                                     nss::Command(command, L"pcolour ") || nss::Command(command, L"pcolour:") ||
                                                     nss::Command(command, L"presscolour ") || nss::Command(command, L"presscolour:"))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255 && scope.front().type == GUIScopeStruct::Button)
                                                    scope.front().object.but->button->pressColor = possibleColor;
                                            }
                                            else if (nss::Command(command, L"outlinepress ") || nss::Command(command, L"outlinepress:") ||
                                                     nss::Command(command, L"opcolor ") || nss::Command(command, L"opcolor:") ||
                                                     nss::Command(command, L"opresscolor ") || nss::Command(command, L"opresscolor:") ||
                                                     nss::Command(command, L"outlinepcolor ") || nss::Command(command, L"outlinepcolor:"))
                                            {
                                                sf::Color possibleColor = nss::ParseColor(command);
                                                if (possibleColor.a != 255)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) button->text->opressColor = possibleColor;
                                                        // else if (button->type == GUIObjects::Button::Rect) button->rect->ohoverColor = possibleColor;
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"halign ") || nss::Command(command, L"halign:") ||
                                                     nss::Command(command, L"h ") || nss::Command(command, L"h:"))
                                            {
                                                std::wstring align = nss::ArgumentAsString(command);
                                                if (align.length() && align != L"")
                                                {
                                                    if (nss::Command(align, L"center") || nss::Command(align, L"c") ||
                                                        nss::Command(align, L"middle") || nss::Command(align, L"m"))
                                                    {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->halign = Halign::Center;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->halign = Halign::Center;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->halign = Halign::Center;
                                                        }
                                                    } else if (nss::Command(align, L"left") || nss::Command(align, L"l")) {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->halign = Halign::Left;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->halign = Halign::Left;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->halign = Halign::Left;
                                                        }
                                                    } else if (nss::Command(align, L"right") || nss::Command(align, L"r")) {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->halign = Halign::Right;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->halign = Halign::Right;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->halign = Halign::Right;
                                                        }
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"valign ") || nss::Command(command, L"valign:") ||
                                                     nss::Command(command, L"v ") || nss::Command(command, L"v:"))
                                            {
                                                std::wstring align = nss::ArgumentAsString(command);
                                                if (align.length() && align != L"")
                                                {
                                                    if (nss::Command(align, L"center") || nss::Command(align, L"c") ||
                                                        nss::Command(align, L"middle") || nss::Command(align, L"m"))
                                                    {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->valign = Valign::Center;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->valign = Valign::Center;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->valign = Valign::Center;
                                                        }
                                                    } else if (nss::Command(align, L"top") || nss::Command(align, L"t")) {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->valign = Valign::Top;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->valign = Valign::Top;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->valign = Valign::Top;
                                                        }
                                                    } else if (nss::Command(align, L"bottom") || nss::Command(align, L"b")) {
                                                        if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->valign = Valign::Bottom;
                                                        else if (scope.front().type == GUIScopeStruct::Button) {
                                                            GUIObjects::Button* button = scope.front().object.but;
                                                            if (button->type == GUIObjects::Button::Text) button->text->valign = Valign::Bottom;
                                                            else if (button->type == GUIObjects::Button::Rect) button->rect->valign = Valign::Bottom;
                                                        }
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"thickness ") || nss::Command(command, L"thickness:"))
                                            {
                                                float thickness = nss::ArgumentAsFloat(command);
                                                if (thickness >= 0)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->SetOutlineThickness(thickness);
                                                    else if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) button->text->thickness = thickness;
                                                        else if (button->type == GUIObjects::Button::Rect) button->rect->thickness = thickness;
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"size ") || nss::Command(command, L"size:") ||
                                                     nss::Command(command, L"charactersize ") || nss::Command(command, L"charactersize:"))
                                            {
                                                unsigned int characterSize = nss::ArgumentAsInt(command);
                                                if (characterSize > 0)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->SetCharacterSize(characterSize);
                                                    else if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) button->text->setCharacterSize(characterSize);
                                                        else if (button->type == GUIObjects::Button::Rect) button->rect->setCharacterSize(characterSize);
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"wrap ") || nss::Command(command, L"wrap:") ||
                                                     nss::Command(command, L"textwrap ") || nss::Command(command, L"textwrap:"))
                                            {
                                                nss::SkipSpaces(command); bool wrap = nss::ArgumentAsBool(command);
                                                if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->allowWrap = wrap;
                                            }
                                            else if (nss::Command(command, L"font ") || nss::Command(command, L"font:") ||
                                                     nss::Command(command, L"fontname ") || nss::Command(command, L"fontname:"))
                                            {
                                                std::wstring fontName = nss::ParseAsMaybeQuoteStringFull(command);
                                                if (fontName.length() != 0)
                                                {
                                                    if (scope.front().type == GUIScopeStruct::Text) scope.front().object.text->SetFont(fontName);
                                                    else if (scope.front().type == GUIScopeStruct::Button) {
                                                        GUIObjects::Button* button = scope.front().object.but;
                                                        if (button->type == GUIObjects::Button::Text) button->text->setFont(fontName);
                                                        else if (button->type == GUIObjects::Button::Rect) button->rect->setFont(fontName);
                                                    }
                                                }
                                            }
                                            else if (nss::Command(command, L"action:") || nss::Command(command, L"action"))
                                            {
                                                if (scope.front().type == GUIScopeStruct::Button && scope.front().object.but)
                                                {
                                                    GUIObjects::Button* button = scope.front().object.but;
                                                    std::wstring possibleCommand = nss::ParseUntil(command, 13);
                                                    int brackets = 0; for (unsigned int i = 0; i < line.length(); ++i)
                                                        if (line[i] == L'{') ++brackets; else if (line[i] == L'}') --brackets;
                                                    if (brackets == 0 && !nss::ContainsUsefulInformation(possibleCommand))
                                                    {
                                                        std::getline(wif, possibleCommand); nss::RemoveSpaces(possibleCommand);
                                                        command.Command(possibleCommand);
                                                        for (unsigned int i = 0; i < command.line.length(); ++i)
                                                            if (command.line[i] == L'{') ++brackets; else if (command.line[i] == L'}') --brackets;
                                                    }
                                                    if (brackets > 0)
                                                    {
                                                        command.Command(possibleCommand, true);
                                                        std::wstring newPossibleCommand = L"";
                                                        std::wstring maybeBeginOfBrackets = nss::ParseUntil(command, L'{');
                                                        for (unsigned long i = maybeBeginOfBrackets.length() + 2; i < possibleCommand.length(); ++i)
                                                            newPossibleCommand += possibleCommand[i];
                                                        possibleCommand = newPossibleCommand;
                                                        if (nss::ContainsUsefulInformation(possibleCommand)) button->actions.push_back(possibleCommand);
                                                        while (brackets > 0 && !wif.eof())
                                                        {
                                                            std::getline(wif, line); nss::RemoveSpaces(line); command.Command(line, false);
                                                            for (unsigned int i = 0; i < command.line.length(); ++i)
                                                                if (command.line[i] == L'{') ++brackets; else if (command.line[i] == L'}') --brackets;
                                                            if (brackets == 0)
                                                            {
                                                                std::wstring maybeItsInBrackets = nss::ParseUntilReverse(command.line, L'}', command.line.length() - 1);
                                                                if (!nss::ContainsUsefulInformation(maybeItsInBrackets))
                                                                {
                                                                    std::wstring newPossibleCommand = L"";
                                                                    for (unsigned int i = 0; i < line.length() - (maybeItsInBrackets.length() + 1); ++i) newPossibleCommand += line[i];
                                                                    line = newPossibleCommand;
                                                                }
                                                            }
                                                            if (nss::ContainsUsefulInformation(line)) button->actions.push_back(line);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        command.Command(possibleCommand, true);
                                                        std::wstring maybeItsInBrackets = nss::ParseUntilReverse(command.line, L'}', command.line.length() - 1);
                                                        if (!nss::ContainsUsefulInformation(maybeItsInBrackets))
                                                        {
                                                            std::wstring newPossibleCommand = L"";
                                                            std::wstring maybeBeginOfBrackets = nss::ParseUntil(command, L'{');
                                                            for (unsigned int i = maybeBeginOfBrackets.length() + 2; i < possibleCommand.length() - (maybeItsInBrackets.length() + 1); ++i) newPossibleCommand += possibleCommand[i];
                                                            possibleCommand = newPossibleCommand;
                                                        }
                                                        if (nss::ContainsUsefulInformation(possibleCommand)) button->actions.push_back(possibleCommand);
                                                    }
                                                    
                                                    /*cout << "ACTIONS BEGIN" << endl;
                                                    for (auto& w : buttonPtr->actions) cout << base::utf8(w) << endl;
                                                    cout << "ACTIONS END" << endl;*/
                                                }
                                            }
                                            else if (nss::Command(command, L"sprite ") || nss::Command(command, L"sprite:") ||
                                                     nss::Command(command, L"texture ") || nss::Command(command, L"texture:") ||
                                                     nss::Command(command, L"spr ") || nss::Command(command, L"spr:"))
                                            {
                                                std::wstring imageName = nss::ParseAsMaybeQuoteStringFull(command);
                                                if (imageName.length() != 0 && scope.front().type == GUIScopeStruct::Button && scope.front().object.but->type == GUIObjects::Button::Sprite) scope.front().object.but->sprite->setTexture(folderPath + imageName, this);
                                            }
                                            else if (nss::Command(command, L"scale.:") || nss::Command(command, L"scale. "))
                                            {
                                                nss::SkipSpaces(command);
                                                if (scope.front().type == GUIScopeStruct::Button && scope.front().object.but->type == GUIObjects::Button::Sprite)
                                                {
                                                    float fromScope{ 1.f };
                                                    if (scope.size() > 1)
                                                    {
                                                        auto it = scope.begin();
                                                        std::advance(it, 1);
                                                        if (it->type == GUIScopeStruct::Image) fromScope = it->object.img->sprite.getScale().x;
                                                    }
                                                    cout << fromScope << endl;
                                                    scope.front().object.but->sprite->setScale(fromScope * nss::ParseAsFloat(command));
                                                }
                                            }
                                            else if (nss::Command(command, L"scale ") || nss::Command(command, L"scale:"))
                                            {
                                                nss::SkipSpaces(command);
                                                if (scope.front().type == GUIScopeStruct::Button && scope.front().object.but->type == GUIObjects::Button::Sprite) scope.front().object.but->sprite->setScale(nss::ParseAsFloat(command));
                                            }
                                            else if (nss::Command(command, L"nocolorchanging ") || nss::Command(command, L"nocolor"))
                                            {
                                                nss::SkipSpaces(command);
                                                if (scope.front().type == GUIScopeStruct::Button && scope.front().object.but->type == GUIObjects::Button::Sprite) scope.front().object.but->sprite->noColorChanging = true;
                                            }
                                            else if (nss::Command(command, L"fit ") || nss::Command(command, L"fit:") ||
                                                     nss::Command(command, L"fill ") || nss::Command(command, L"fill:") ||
                                                     nss::Command(command, L"mode ") || nss::Command(command, L"mode:") ||
                                                     nss::Command(command, L"fitmode ") || nss::Command(command, L"fitmode:"))
                                            {
                                                nss::SkipSpaces(command); std::wstring fitMode = nss::ParseAsString(command);
                                                if (fitMode.length() != 0 && scope.front().type == GUIScopeStruct::Image)
                                                {
                                                    GUIObjects::Image* img = scope.front().object.img;
                                                    if (nss::Command(fitMode, L"fill") || nss::Command(fitMode, L"fit")) img->fitMode = GUIObjects::Image::fill;
                                                    else if (nss::Command(fitMode, L"fillcenter") || nss::Command(fitMode, L"center")) img->fitMode = GUIObjects::Image::fillCentre;
                                                    else if (nss::Command(fitMode, L"keep") || nss::Command(fitMode, L"keepaspect") || nss::Command(fitMode, L"aspect")) img->fitMode = GUIObjects::Image::keepAspect;
                                                    else if (nss::Command(fitMode, L"stretch")) img->fitMode = GUIObjects::Image::stretch;
                                                    else if (nss::Command(fitMode, L"default")) img->fitMode = GUIObjects::Image::defaultFit;
                                                }
                                            }
                                            else if (nss::Command(command, L"parallax: ") || nss::Command(command, L"parallax "))
                                            {
                                                nss::SkipSpaces(command); std::wstring parallaxPower = nss::ParseAsString(command);
                                                if (parallaxPower.length() != 0 && scope.front().type == GUIScopeStruct::Image)
                                                {
                                                    GUIObjects::Image* img = scope.front().object.img;
                                                    img->doParallax = gs::isParallaxEnabled; img->parallaxPower = base::atof(parallaxPower);
                                                    if (img->parallaxPower == 0) img->doParallax = false;
                                                }
                                            }
                                        }
                                        else if (nss::Command(command, L"font ") || nss::Command(command, L"font:")) defaultFont = nss::ParseAsMaybeQuoteString(command);
                                    }
                                }
                            }
                            else
                            {
                                parsingGUI = false;
                                scope.emplace_back(nullptr);
                                knownType = 0;
                            }
                        }
                    }
                    else eof = true;
                }
            }
            wif.close();
            
            return skinLoaded;
        }
        GUIScopeStruct::GUIScopeStruct(GUIObject* obj, const typeEnum& type) : type(type) { object.obj = obj; }
        GUIObject& GUIScopeStruct::operator*() { return *object.obj; }
        GUIObject* GUIScopeStruct::operator->() { return object.obj; }
        void GUISystem::PrintIerarchy()
        {
            /*cout << "GUISystem {" << endl;
            for (List<GUIObject>* list = guiObjects; list != nullptr; list = list->next)
            {
                cout << "---GUIObject {" << endl;
                if (list->data->child != nullptr)
                    for (List<GUIObject>* list1 = list->data->child->guiObjects; list1 != nullptr; list1 = list1->next)
                    {
                        cout << "------GUIObject {" << endl;
                        if (list1->data->child != nullptr)
                            for (List<GUIObject>* list2 = list1->data->child->guiObjects; list2 != nullptr; list2 = list2->next)
                            {
                                cout << "---------GUIObject {" << endl;
                                if (list2->data->child != nullptr)
                                    for (List<GUIObject>* list3 = list2->data->child->guiObjects; list3 != nullptr; list3 = list3->next)
                                    {
                                        cout << "------------asd" << endl;
                                    }
                                cout << "---------}" << endl;
                            }
                        cout << "------}" << endl;
                    }
                cout << "---}" << endl;
            }
            cout << "}" << endl;*/
        }
        void GUISystem::Save(std::wofstream& wof)
        {
            wof << L"path: " << trueFileName << endl;
            wof << L"scope: " << scope << endl;
            //if ((Skin::self && maxAlpha != Skin::self->dialogue.maxAlpha) || (!Skin::self && maxAlpha != 255)) wof << L"maxAlpha: " << maxAlpha << endl;
            
            if (mode != existing)
            {
                wof << L"mode: " << mode << endl;
                if (mode == appearing || mode == disappearing) wof << L"currentTime: " << currentTime << endl;
                if (mode == appearing && appearTime != 0.6f) wof << L"appearTime: " << appearTime << endl;
                if (disappearTime != 0.6f) wof << L"disappearTime: " << disappearTime << endl;
                // TODO: Добавить в Skin настройки GUI систем.
                /*if (mode == appearing && ((Skin::self && appearTime != Skin::self->character.appearTime) || (!Skin::self && appearTime != 0.6f))) wof << L"appearTime: " << appearTime << endl;*/
            }
            /*if ((Skin::self && disappearTime != Skin::self->character.disappearTime) || (!Skin::self && disappearTime != 0.6f)) wof << L"disappearTime: " << disappearTime << endl;*/
            if (sendMessageBack != atAppearance) wof << L"send: " << sendMessageBack << endl;
        }
        std::pair<std::wstring, bool> GUISystem::Load(std::wifstream& wif)
        {
            mode = existing; std::wstring line; nss::CommandSettings command; bool done{ false };
            while (!done)
            {
                std::getline(wif, line); command.Command(line);
                if (nss::Command(command, L"tag(")) done = true;
                else if (nss::Command(command, L"path: ")) { trueFileName = nss::ParseUntil(command, L'\n'); fileName = folderPath + trueFileName; }
                else if (nss::Command(command, L"scope: ")) LoadFromFile(fileName, nss::ParseUntil(command, L'\n'));
                else if (nss::Command(command, L"mode: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: mode = modeEnum::appearing; break;
                        case 2: mode = modeEnum::disappearing; break;
                        case 3: mode = modeEnum::deprecated; break;
                        default: mode = modeEnum::existing; break;
                    }
                }
                else if (nss::Command(command, L"currenttime: ")) currentTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"appeartime: ")) appearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"disappeartime: ")) disappearTime = nss::ParseAsFloat(command);
                else if (nss::Command(command, L"maxalpha: ")) maxAlpha = nss::ParseAsInt(command);
                else if (nss::Command(command, L"send: "))
                {
                    int md = nss::ParseAsInt(command);
                    switch (md)
                    {
                        case 0: sendMessageBack = sendMessageBackEnum::noMessage; break;
                        case 2: sendMessageBack = sendMessageBackEnum::atDisappearing; break;
                        case 3: sendMessageBack = sendMessageBackEnum::atDeprecated; break;
                        default: sendMessageBack = sendMessageBackEnum::atAppearance; break;
                    }
                }
                if (wif.eof()) done = true;
            }
            
            if (mode == modeEnum::appearing) alpha = (sf::Uint8)(maxAlpha * (currentTime / appearTime));
            else if (mode == modeEnum::disappearing) alpha = (sf::Uint8)(maxAlpha - (maxAlpha * (currentTime / disappearTime)));
            else alpha = maxAlpha; SetAlpha(alpha);
            bool onHold{ !((sendMessageBack == sendMessageBackEnum::noMessage) || (sendMessageBack == sendMessageBackEnum::atAppearance && mode > 0) || (sendMessageBack == sendMessageBackEnum::atDisappearing && mode > 1)) };
            
            return { line, onHold };
        }
        
        
        
        
        
        
        void GUIObject::Init() { }
        void GUIObject::PollEvent(sf::Event& event) { }
        void GUIObject::Update(const sf::Time& elapsedTime) { }
        void GUIObject::Draw(sf::RenderWindow* window) { }
        void GUIObject::PreCalculate(const unsigned int& width, const unsigned int& height) { }
        void GUIObject::PreCalculatedSize(const unsigned int& width, const unsigned int& height) { }
        void GUIObject::Resize(const unsigned int& width, const unsigned int& height) { }
        void GUIObject::Destroy() { }
        void GUIObject::ReceiveMessage(MessageHolder& message) { }
        void GUIObject::SetAlpha(sf::Uint8 alpha) { }
        void GUIObject::SetColor(const sf::Color& fillColour) { }
        NovelSystem* GUIObject::FindNovelSystem()
        {
            GUISystem* sys = guiSystem;
            while (sys && sys->parent) sys = sys->parent->guiSystem;
            if (sys) return sys->novelSystem; else return nullptr;
        }
        NovelObject* GUIObject::FindNovelObject()
        {
            GUISystem* sys = guiSystem;
            while (sys && sys->parent) sys = sys->parent->guiSystem;
            if (sys) return sys; else return nullptr;
        }
        
        
        
        
        
        
        
        namespace GUIObjects
        {
            void Rectangle::Init() { shape.setFillColor(sf::Color::Black); }
            void Rectangle::Draw(sf::RenderWindow* window) { window->draw(shape); }
            void Rectangle::Resize(const unsigned int& width, const unsigned int& height)
            {
                shape.setSize({static_cast<float>(constrains.globalBounds.width), static_cast<float>(constrains.globalBounds.height)});
                shape.setPosition(constrains.globalBounds.left, constrains.globalBounds.top);
            }
            void Rectangle::SetAlpha(sf::Uint8 alpha)
            {
                sf::Uint8 realAlpha = sf::Uint8(alpha * ((float)maxAlpha/255));
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, realAlpha));
            }
            void Rectangle::SetColor(const sf::Color& fillColour) { shape.setFillColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, shape.getFillColor().a)); }
            
            
            
            void Text::Init()
            {
                text.setFillColor(sf::Color::White);
                text.setString(textString);
                text.setPosition(0, 0);
            }
            void Text::Draw(sf::RenderWindow* window) { if (fontLoaded) window->draw(text); }
            void Text::PreCalculate(const unsigned int& width, const unsigned int& height)
            {
                text.setCharacterSize((unsigned int)(characterSize * gs::scale));
                if (thickness != 0) text.setOutlineThickness(thickness * gs::scale);
                
                if (guiSystem && guiSystem->parent)
                {
                    bounds.left = guiSystem->parent->constrains.globalBounds.left;
                    bounds.width = guiSystem->parent->constrains.globalBounds.width;
                    bounds.top = guiSystem->parent->constrains.globalBounds.top;
                    bounds.height = guiSystem->parent->constrains.globalBounds.height;
                }
                else { bounds.left = 0; bounds.width = width; bounds.top = 0; bounds.height = height; }
                
                //cout << bounds.left << " " << bounds.width << " " << bounds.top << " " << bounds.height << endl;
            }
            void Text::PreCalculatedSize(const unsigned int& width, const unsigned int& height)
            {
                
                //constrains.width = text.getGlobalBounds().width;
                //constrains.height = text.getGlobalBounds().height;
            }
            void Text::Resize(const unsigned int& width, const unsigned int& height) {
                if (allowWrap) { text.setScale(1, 1);
                    text.setString(nss::GetStringWithLineBreaks(text, textString, -constrains.localBounds.left + bounds.width, bounds.height)); }
                
                setPosition(constrains.globalBounds.left, constrains.globalBounds.top);
                constrains.globalBounds.width = text.getGlobalBounds().width;
                constrains.globalBounds.height = text.getGlobalBounds().height;
                constrains.globalBounds.left = text.getGlobalBounds().left;
                constrains.globalBounds.top = text.getGlobalBounds().top;
                
                // Текст должен быть ограничен прямоугольником, находящимся над ним. Bounds текста - это исключительно границы текста.
            }
            void Text::SetAlpha(sf::Uint8 alpha)
            {
                sf::Uint8 realAlpha = sf::Uint8(alpha * ((float)maxAlpha/255));
                text.setFillColor(sf::Color(text.getFillColor().r, text.getFillColor().g, text.getFillColor().b, realAlpha));
                text.setOutlineColor(sf::Color(text.getOutlineColor().r, text.getOutlineColor().g, text.getOutlineColor().b, realAlpha));
            }
            void Text::SetColor(const sf::Color& fillColour) { text.setFillColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, text.getFillColor().a)); }
            void Text::SetString(const std::wstring& wstr)
            {
                textString = wstr;
                text.setString(textString);
            }
            void Text::SetFont(const std::wstring& font)
            {
                fontName = font;
                if ((fontLoaded = (ns::FontCollector::GetFont(font))))
                    text.setFont(*ns::FontCollector::GetFont(font));
            }
            void Text::SetOutlineThickness(const float& thickness)
            {
                this->thickness = thickness;
                text.setOutlineThickness(thickness * gs::scale);
            }
            void Text::SetCharacterSize(const unsigned int& characterSize)
            {
                this->characterSize = characterSize;
                text.setCharacterSize(characterSize * gs::scale);
            }
            void Text::setPosition(const float& x, const float& y)
            {
                switch (halign)
                {
                    case Halign::Left: text.setOrigin(0, text.getOrigin().y); break;
                    case Halign::Center: text.setOrigin(text.getLocalBounds().width/2, text.getOrigin().y); break;
                    case Halign::Right: text.setOrigin(text.getLocalBounds().width, text.getOrigin().y); break;
                }
                switch (valign)
                {
                    case Valign::Top: text.setOrigin(text.getOrigin().x, 0); break;
                    case Valign::Center: text.setOrigin(text.getOrigin().x, text.getLocalBounds().height/2); break;
                    case Valign::Bottom: text.setOrigin(text.getOrigin().x, text.getLocalBounds().height); break;
                }
                text.setPosition(x, y); CorrectBoundaries();
            }
            void Text::CorrectBoundaries()
            {
                text.setScale(1, 1);
                if (text.getGlobalBounds().width > bounds.width && text.getGlobalBounds().left < bounds.left)
                    text.setScale((float)bounds.width/text.getGlobalBounds().width, 1);
                else if (text.getGlobalBounds().width - text.getOrigin().x > bounds.width - constrains.localBounds.left)
                    text.setScale((float)(bounds.width - constrains.localBounds.left)/(text.getGlobalBounds().width - text.getOrigin().x), 1);
                else if (text.getGlobalBounds().left < bounds.left && text.getOrigin().x > 0)
                    text.setScale((float)((text.getOrigin().x + text.getGlobalBounds().left - bounds.left))/(text.getGlobalBounds().width), 1);
                if (text.getGlobalBounds().left < bounds.left) text.setPosition(bounds.left + text.getOrigin().x*text.getScale().x, text.getPosition().y);
                
                /*if (text.getGlobalBounds().width > bounds.width - constrains.localBounds.left && text.getGlobalBounds().left < bounds.left)
                    text.setScale((float)bounds.width/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().width > bounds.width - constrains.localBounds.left)
                    text.setScale((float)(bounds.width - constrains.localBounds.left)/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().left < bounds.left && text.getOrigin().x > 0)
                    text.setScale((float)((text.getOrigin().x + text.getGlobalBounds().left - bounds.left))/(text.getGlobalBounds().width), 1);
                if (text.getGlobalBounds().left < bounds.left) text.setPosition(bounds.left + text.getOrigin().x, text.getPosition().y);*/
                
                /*cout << constrains.globalBounds.width << " " << text.getGlobalBounds().width << " " << screen.x << " " << text.getGlobalBounds().left << constrains.globalBounds.left << endl;
                
                if (text.getGlobalBounds().width > screen.x - constrains.localBounds.left && text.getGlobalBounds().left < constrains.globalBounds.left)
                    text.setScale((float)(screen.x)/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().width > screen.x - constrains.localBounds.left)
                    text.setScale((float)(screen.x - constrains.localBounds.left)/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().left < constrains.globalBounds.left && text.getOrigin().x > 0)
                    text.setScale((float)((constrains.globalBounds.left - text.getGlobalBounds().left))/(text.getGlobalBounds().width), 1);
                if (text.getGlobalBounds().left < constrains.globalBounds.left) text.setPosition(constrains.globalBounds.left - text.getOrigin().x, text.getPosition().y);*/
                
                /*if (text.getGlobalBounds().width > screen.x - text.getGlobalBounds().left && text.getGlobalBounds().left < 0)
                    text.setScale((float)screen.x/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().width > screen.x - text.getGlobalBounds().left)
                    text.setScale((float)(screen.x - text.getGlobalBounds().left)/(text.getGlobalBounds().width), 1);
                else if (text.getGlobalBounds().left < 0 && text.getOrigin().x > 0) //TODO: Correct this
                    text.setScale((float)(text.getOrigin().x + text.getGlobalBounds().left)/(text.getGlobalBounds().width), 1);
                if (text.getGlobalBounds().left < 0) text.setPosition(text.getPosition().x - text.getGlobalBounds().left, text.getPosition().y);*/
            }
            
            
            
            void Image::Destroy() { if (spriteLoaded && imagePath != L"" && imagePath.length() != 0) ic::DeleteImage(imagePath); }
            void Image::PollEvent(sf::Event &event)
            {
                if (event.type == sf::Event::MouseMoved && doParallax && parallaxPower > 0 && visible)
                    CalculateParallax(event.mouseMove.x, event.mouseMove.y);
            }
            void Image::Draw(sf::RenderWindow* window) { if (spriteLoaded) window->draw(sprite); }
            void Image::PreCalculate(const unsigned int& width, const unsigned int& height) { sprite.setScale(1, 1); }
            void Image::PreCalculatedSize(const unsigned int& width, const unsigned int& height)
            {
                CalculateScale(constrains.globalBounds.width, constrains.globalBounds.height);
                if (fitMode != fillCentre) {
                    constrains.globalBounds.width = sprite.getGlobalBounds().width;
                    constrains.globalBounds.height = sprite.getGlobalBounds().height;
                }
            }
            void Image::Resize(const unsigned int& width, const unsigned int& height)
            {
                if (spriteLoaded)
                {
                    //sprite.setScale({static_cast<float>(constrains.width) / sprite.getTexture()->getSize().x, static_cast<float>(constrains.height)  / sprite.getTexture()->getSize().y});
                    sprite.setPosition(constrains.globalBounds.left, constrains.globalBounds.top);
                    CalculateScale(constrains.globalBounds.width, constrains.globalBounds.height);
                    //if (fitMode != fillCentre) {
                        constrains.globalBounds.left = sprite.getGlobalBounds().left;
                        constrains.globalBounds.top = sprite.getGlobalBounds().top;
                        constrains.globalBounds.width = sprite.getGlobalBounds().width;
                        constrains.globalBounds.height = sprite.getGlobalBounds().height;
                    //}
                    if (doParallax) CalculateParallax(sf::Mouse::getPosition(*gs::window).x, sf::Mouse::getPosition(*gs::window).y);
                }
            }
            void Image::SetAlpha(sf::Uint8 alpha)
            {
                sf::Uint8 realAlpha = sf::Uint8(alpha * ((float)maxAlpha/255));
                sprite.setColor(sf::Color(sprite.getColor().r, sprite.getColor().g, sprite.getColor().b, realAlpha));
            }
            void Image::SetColor(const sf::Color& fillColour) { sprite.setColor(sf::Color(fillColour.r, fillColour.g, fillColour.b, sprite.getColor().a)); }
            void Image::LoadImage(const std::wstring& path, MessageSender* sender)
            {
                spriteLoaded = false; sf::Texture* texture;
                if (sender) texture = ic::RequestHigherTexture(path, sender); else texture = ic::LoadTexture(path);
                if ((spriteLoaded = texture)) { imagePath = path; sprite.setTexture(*texture, true); Resize(gs::width, gs::height); }
            }
            void Image::ReceiveMessage(MessageHolder &message)
            {
                if (nss::Command(message.info, "Request") && message.additional == imagePath)
                {
                    sf::Texture* texture = ic::LoadTexture(imagePath);
                    if (texture) { sprite.setTexture(*texture, true); Resize(gs::width, gs::height); }
                }
            }
            void Image::CalculateParallax(int mouseX, int mouseY)
            {
                if (mouseX >= 0 && mouseY >= 0 && mouseX <= gs::width && mouseY <= gs::height)
                {
                    float posX = defaultPositionX + (int)(mouseX - gs::width/2) * parallaxPower;
                    float posY = defaultPositionY + (int)(mouseY - gs::height/2) * parallaxPower;
                    sprite.setPosition(posX, posY);
                }
            }
            void Image::CalculateScale(unsigned int width, unsigned int height)
            {
                if (spriteLoaded)
                {
                    float scaleFactorX, scaleFactorY, scaleFactor;
                    scaleFactorX = (float)width / (sprite.getTexture()->getSize().x) * (doParallax? 1 + parallaxPower : 1) * scaleX;
                    scaleFactorY = (float)height / (sprite.getTexture()->getSize().y) * (doParallax? 1 + parallaxPower : 1) * scaleY;
                    switch (fitMode)
                    {
                        case defaultFill:
                            scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                            sprite.setScale(scaleFactor, scaleFactor);
                            break;
                        case defaultFit:
                            scaleFactor = (scaleFactorX < scaleFactorY) ? scaleFactorX : scaleFactorY;
                            sprite.setScale(scaleFactor, scaleFactor);
                            break;
                        case fill:
                            scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                            sprite.setScale(scaleFactor, scaleFactor);
                            defaultPositionX = (float)width/2 - (sprite.getLocalBounds().width/2 + sprite.getOrigin().x)*sprite.getScale().x;
                            defaultPositionY = (float)height/2 - (sprite.getLocalBounds().height/2 + sprite.getOrigin().y)*sprite.getScale().y;
                            sprite.setPosition(defaultPositionX, defaultPositionY);
                            break;
                        case fillCentre:
                            scaleFactor = (scaleFactorX > scaleFactorY) ? scaleFactorX : scaleFactorY;
                            sprite.setScale(scaleFactor, scaleFactor);
                            defaultPositionX = (float)width/2 - (sprite.getLocalBounds().width/2 + sprite.getOrigin().x)*sprite.getScale().x;
                            defaultPositionY = (float)height/2 - (sprite.getLocalBounds().height/2 + sprite.getOrigin().y)*sprite.getScale().y;
                            sprite.setPosition(defaultPositionX, defaultPositionY);
                            break;
                        case stretch: sprite.setScale(scaleFactorX, scaleFactorY); break;
                        case keepAspect: sprite.setScale(scaleX * gs::scale, scaleY * gs::scale); break;
                        case keepAspectSc: sprite.setScale(scaleX * gs::scScale, scaleY * gs::scScale); break;
                        default: break;
                    }
                }
            }
            
            
            
            
            Button::Button(const typeEnum& type)
            {
                this->type = type;
                switch (type)
                {
                    case Text:
                        text = new GUI::TextButton(); button = text;
                        if (Skin::self) text->setFont(Skin::self->defaultFontName);
                        else text->setFont(lang::menufont);
                        text->setCharacterSize(48);
                        text->setString(L"Button");
                        break;
                    case Rect:
                        rect = new GUI::RectangleButton(); button = rect;
                        if (Skin::self) rect->setFont(Skin::self->defaultFontName);
                        else text->setFont(lang::menufont);
                        rect->setCharacterSize(48);
                        rect->setString(L"Button");
                        break;
                    case Sprite: sprite = new GUI::SpriteButton(); button = sprite; break;
                    default: break;
                }
                if (button) button->regulateBounds = true;
            }
            void Button::Destroy() { delete button; button = nullptr; }
            void Button::PollEvent(sf::Event& event)
            {
                if (maxAlpha == button->getAlpha() && button->PollEvent(event))
                {
                    nss::CommandSettings command; MessageSender* sender;
                    NovelSystem* system = FindNovelSystem();
                    if (system) sender = system; else sender = ic::globalRequestSender;
                    if (sender)
                    {
                        std::list<std::wstring*> toTheRanch;
                        for (auto it = actions.begin(); it != actions.end(); ++it)
                        {
                            command.Command(*it);
                            if (nss::Command(command, L"page")) cout << "page" << endl;
                            else if (nss::Command(command, L"switch interface") || nss::Command(command, L"interface switch") || nss::Command(command, L"switch")) sender->SendMessage({"Show/Hide Interface"});
                            else if (nss::Command(command, L"menu") || nss::Command(command, L"go to menu")) {
                                gs::isPause = false; sender->SendMessage({"GamePause :: Return to menu"}); }
                            else if (nss::Command(command, L"pause off") || nss::Command(command, L"pause false")) {
                                if (gs::isPause) { gs::isPause = false; sender->SendMessage({"GamePause"}); } }
                            else if (nss::Command(command, L"pause on") || nss::Command(command, L"game true")) {
                                if (!gs::isPause) { gs::isPause = true; sender->SendMessage({"GamePause"}); } }
                            else if (nss::Command(command, L"pause") || nss::Command(command, L"game pause")) {
                                gs::isPause = !gs::isPause; sender->SendMessage({"GamePause"}); }
                            else if (nss::Command(command, L"save")) sender->SendMessage({"Save"});
                            else if (nss::Command(command, L"load")) sender->SendMessage({"Load"});
                            else if (!system && (nss::Command(command, L"play") || nss::Command(command, L"read"))) sender->SendMessage({"NM :: Start"});
                            else toTheRanch.push_back(&(*it));
                        }
                        if ((eventPolling = toTheRanch.size()) && system)
                        {
                            sender->SendMessage({"ExecuteInsert"});
                            for (auto it = toTheRanch.rbegin(); it != toTheRanch.rend(); ++it) sender->SendMessage({"Execute", *(*it)});
                            button->active = false;
                        }
                    }
                }
            }
            void Button::Update(const sf::Time& elapsedTime) { }
            void Button::PreCalculate(const unsigned int& width, const unsigned int& height)
            {
                if (guiSystem && guiSystem->parent)
                {
                    button->bounds.left = guiSystem->parent->constrains.globalBounds.left;
                    button->bounds.width = guiSystem->parent->constrains.globalBounds.width;
                    button->bounds.top = guiSystem->parent->constrains.globalBounds.top;
                    button->bounds.height = guiSystem->parent->constrains.globalBounds.height;
                    button->leftBound = constrains.localBounds.left;
                }
                else { button->bounds.left = 0; button->bounds.width = width; button->bounds.top = 0; button->bounds.height = height; }
            }
            void Button::PreCalculatedSize(const unsigned int& width, const unsigned int& height)
            {
            }
            void Button::Resize(const unsigned int& width, const unsigned int& height)
            {
                button->Resize(width, height);
                button->setPosition(constrains.globalBounds.left, constrains.globalBounds.top);
                if (type == Text) { constrains.globalBounds.left = text->text.getGlobalBounds().left;
                                    constrains.globalBounds.top = text->text.getGlobalBounds().top; }
                else if (type == Rect) { constrains.globalBounds.left = rect->text.getGlobalBounds().left;
                                         constrains.globalBounds.top = rect->text.getGlobalBounds().top; }
                else if (type == Sprite) { constrains.globalBounds.left = sprite->sprite.getGlobalBounds().left;
                                           constrains.globalBounds.top = sprite->sprite.getGlobalBounds().top; }
                if (type == Text) {
                    constrains.globalBounds.width = text->text.getGlobalBounds().width;
                    constrains.globalBounds.height = text->text.getGlobalBounds().height; }
                else if (type == Rect) {
                    constrains.globalBounds.width = rect->text.getGlobalBounds().width;
                    constrains.globalBounds.height = rect->text.getGlobalBounds().height; }
                else if (type == Sprite) {
                    constrains.globalBounds.width = sprite->sprite.getGlobalBounds().width;
                    constrains.globalBounds.height = sprite->sprite.getGlobalBounds().height; }
            }
            void Button::Draw(sf::RenderWindow* window) { button->draw(window); }
            void Button::ReceiveMessage(MessageHolder& message)
            {
                if (message.info == "FinishedExecute" && eventPolling) { eventPolling = false; button->active = true; }
                else if (nss::Command(message.info, "Request")) button->ReceiveMessage(message);
            }
            void Button::SetAlpha(sf::Uint8 alpha)
            {
                sf::Uint8 realAlpha = sf::Uint8(alpha * ((float)maxAlpha/255));
                button->setAlpha(realAlpha);
            }
            void Button::SetColor(const sf::Color& fillColour) { button->setColor(fillColour); }
        }
    }
}
