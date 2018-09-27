//
//  GUI.cpp
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
        GUISystem::~GUISystem()
        {
            Clear();
        }
        GUISystem::GUISystem(Novel* novel)
        {
            this->novel = novel;
        }
        void GUISystem::Update(const sf::Time& elapsedTime)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    list->data->Update(elapsedTime);
                    if (list->data->regulateFadings)
                        list->data->FadingUpdate(elapsedTime);
                    if (list->data->child != nullptr)
                        list->data->child->Update(elapsedTime);
                }
        }
        void GUISystem::Draw(sf::RenderWindow* window)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    if (list->data->visible)
                    {
                        list->data->Draw(window);
                        if (list->data->child != nullptr)
                            list->data->child->Draw(window);
                    }
                }
        }
        void GUISystem::Resize(unsigned int width, unsigned int height)
        {
            if (width != lastWidth || height != lastHeight)
            {
                lastWidth = width;
                lastHeight = height;
                
                List<GUIObject>* next = nullptr;
                if (guiObjects != nullptr)
                    for (auto* list = guiObjects; list != nullptr; list = next)
                    {
                        next = list->next;
                        
                        if (list->data->visible)
                        {
                            //TODO: Make working good conditions to show something or not (constrains.displayWhenVariableIsTrue)
                            /*if (list->data->constrains.displayWhenVariableIsTrue.length() != 0)
                            {
                                NovelVariable* nvar = novel->FindVariable(list->data->constrains.displayWhenVariableIsTrue);
                                if (nvar != nullptr)
                                {
                                    switch (nvar->type)
                                    {
                                        case NovelVariable::String:
                                            list->data->visible = std::wstring(nvar->value.asString).length() > 0;
                                            break;
                                        case NovelVariable::Integer:
                                            list->data->visible = nvar->value.asInt != 0;
                                            break;
                                        case NovelVariable::Boolean:
                                            list->data->visible = nvar->value.asBoolean;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                            }*/
                            
                            list->data->constrains.Recalculate(*list->data, width, height);
                            list->data->Resize(width, height);
                            if (list->data->child != nullptr)
                                list->data->child->Resize(width, height);
                        }
                    }
            }
        }
        void GUISystem::VariableResize(unsigned int width, unsigned int height)
        {
            lastWidth = width;
            lastHeight = height;
            
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    
                    if (list->data->visible && !list->data->ignoreVariableChange)
                    {
                        list->data->constrains.Recalculate(*list->data, width, height);
                        list->data->Resize(width, height);
                        if (list->data->child != nullptr)
                            list->data->child->Resize(width, height);
                    }
                }
        }
        void GUISystem::VariableChange(const std::wstring& varName)
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    if (!list->data->ignoreVariableChange)
                    {
                        list->data->VariableChange(varName);
                        if (list->data->child != nullptr)
                            list->data->child->VariableChange(varName);
                    }
                }
        }
        void GUISystem::SetAlpha(sf::Int8 alpha)
        {
            lastAlpha = alpha;
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    if (!list->data->regulateFadings)
                    {
                        list->data->SetAlpha(alpha);
                        if (list->data->child != nullptr)
                            list->data->child->SetAlpha(alpha);
                    }
                }
        }
        void GUISystem::SetAlphaIfBigger(sf::Int8 alpha)
        {
            if ((unsigned int)lastAlpha < (unsigned int)alpha)
            {
                lastAlpha = alpha;
                List<GUIObject>* next = nullptr;
                if (guiObjects != nullptr)
                    for (auto* list = guiObjects; list != nullptr; list = next)
                    {
                        next = list->next;
                        if (!list->data->regulateFadings)
                        {
                            list->data->SetAlpha(alpha);
                            if (list->data->child != nullptr)
                                list->data->child->SetAlphaIfBigger(alpha);
                        }
                    }
            }
        }
        void GUISystem::Clear()
        {
            List<GUIObject>* next = nullptr;
            if (guiObjects != nullptr)
                for (auto* list = guiObjects; list != nullptr; list = next)
                {
                    next = list->next;
                    
                    if (list->data->child != nullptr)
                    {
                        list->data->child->Clear();
                        delete list->data->child;
                        list->data->child = nullptr;
                    }
                    list->data->Destroy();
                    delete list->data;
                    delete list;
                }
            guiObjects = nullptr;
            lastGuiObjects = nullptr;
        }
        void GUISystem::SetNovel(Novel* novel)
        {
            this->novel = novel;
        }
        void GUISystem::SetParent(GUIObject* parent)
        {
            this->parent = parent;
        }
        
        
        
        GUIConstrainsResult GUIConstrains::Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height, std::wstring& line)
        {
            //TODO: Words parsing, so the can be compared to variables and keywords
            //      Seperators: ',' '+' '-' '*' '/' etc
            std::wstring finalLine = L"";
            bool mustBeRecalculated{ false };
            bool dependVariable{ false };
            
            bool itsBeingSummed{ true };
            wchar_t before = L'\0';
            
            int lastPos = 0;
            while (lastPos < line.length())
            {
                std::wstring word = L"";
                
                //looking for "words"
                bool wordFound{ false };
                bool itHasAPoint{ false };
                bool doScale{ true };
                bool forceScale{ false };
                bool itWasANumber{ true };
                
                for (int i = lastPos; i < line.length() && !wordFound; i++)
                {
                    if (line[i] == L'.')
                        itHasAPoint = true;
                    if ((line[i] == L'c' || line[i] == L'u') && itWasANumber && i != lastPos)
                        doScale = false;
                    if (line[i] == L'r' && itWasANumber && i != lastPos)
                        forceScale = true;
                    
                    if (line[i] == L'+' || line[i] == L'-' || line[i] == L'/' || line[i] == L'*')
                        wordFound = true;
                    else if (line[i] != L' ' && line[i] != L'(' && line[i] != L')')
                    {
                        itWasANumber = itWasANumber && ((line[i] >= 48 && line[i] <= 57) || line[i] == L'.' ||
                                                        (((line[i] == L'c' || line[i] == L'u')) && !doScale) ||
                                                        (line[i] == L'r' && forceScale));
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
                    if (itHasAPoint)
                    {
                        std::wstring replaceBy = L"";
                        for (int i = 0; i < word.length(); i++)
                        {
                            if (word[i] == L'.')
                            {
                                NovelVariable *nvar{ nullptr };
                                std::wstring possibleVariable = L"";
                                for (int j = 0; j < i; j++)
                                    possibleVariable += word[j];
                                if (possibleVariable.length() != 0)
                                    if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr)
                                        nvar = guiObject.guiSystem->novel->FindVariable(possibleVariable);
                                
                                if (possibleVariable.length() != 0 && nvar == nullptr)
                                    cout << "Warning :: GUI :: Unknown expression when recalculating: '" << base::ConvertToUTF8(possibleVariable) << "'." << endl;
                                else
                                {
                                    std::wstring possibleFunction = L"";
                                    for (int j = i; j < word.length(); j++)
                                        possibleFunction += word[j];
                                    
                                    if (possibleFunction == L".width")
                                    {
                                        mustBeRecalculated = true;
                                        if (possibleVariable.length() == 0)
                                        {
                                            if (guiObject.guiSystem != nullptr && guiObject.guiSystem->parent != nullptr)
                                                replaceBy += std::to_wstring(guiObject.guiSystem->parent->constrains.width);
                                            else
                                                replaceBy += std::to_wstring(ns::gs::width);
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
                                                    if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr && guiObject.guiSystem->novel->dialogueGroup != nullptr)
                                                        replaceBy += std::to_wstring((int)guiObject.guiSystem->novel->dialogueGroup->data->charText.getLocalBounds().width);
                                                }
                                                else if (possibleVariable == L"@dialogue")
                                                {
                                                    if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr && guiObject.guiSystem->novel->dialogueGroup != nullptr)
                                                        replaceBy += std::to_wstring((int)guiObject.guiSystem->novel->dialogueGroup->data->text.getLocalBounds().width);
                                                }
                                                else
                                                    cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                            }
                                        }
                                    }
                                    else if (possibleFunction == L".height")
                                    {
                                        mustBeRecalculated = true;
                                        if (possibleVariable.length() == 0)
                                        {
                                            if (guiObject.guiSystem != nullptr && guiObject.guiSystem->parent != nullptr)
                                                replaceBy += std::to_wstring(guiObject.guiSystem->parent->constrains.height);
                                            else
                                                replaceBy += std::to_wstring(ns::gs::height);
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
                                                    if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr && guiObject.guiSystem->novel->dialogueGroup != nullptr)
                                                        replaceBy += std::to_wstring((int)guiObject.guiSystem->novel->dialogueGroup->data->charText.getLocalBounds().height);
                                                }
                                                else if (possibleVariable == L"@dialogue")
                                                {
                                                    if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr && guiObject.guiSystem->novel->dialogueGroup != nullptr)
                                                        replaceBy += std::to_wstring((int)guiObject.guiSystem->novel->dialogueGroup->data->text.getLocalBounds().height);
                                                }
                                                else
                                                    cout << "Notification :: GUI :: I can't do that for now :'C" << endl;
                                            }
                                        }
                                    }
                                    else if (possibleFunction == L".length")
                                    {
                                        if (possibleVariable.length() != 0)
                                            if (nvar->type == nvar->String)
                                            {
                                                dependVariable = true;
                                                dependOnVariables.insert({possibleVariable, true});
                                                mustBeRecalculated = true;
                                                
                                                replaceBy += std::to_wstring(std::wstring(nvar->value.asString).length());
                                            }
                                    }
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
                            NovelVariable *nvar{ nullptr };
                            if (guiObject.guiSystem != nullptr && guiObject.guiSystem->novel != nullptr)
                                nvar = guiObject.guiSystem->novel->FindVariable(word);
                            
                            if (word.length() != 0 && nvar == nullptr)
                                cout << "Warning :: GUI :: Unknown expression when recalculating: '" << base::ConvertToUTF8(word) << "'." << endl;
                            else if (nvar != nullptr)
                            {
                                dependVariable = true;
                                dependOnVariables.insert({word, true});
                                
                                switch (nvar->type)
                                {
                                    case NovelVariable::String:
                                        doScale = false;
                                        word = std::wstring(nvar->value.asString);
                                        break;
                                    case NovelVariable::Boolean:
                                        doScale = false;
                                        if (nvar->value.asBoolean)
                                            word = L"true";
                                        else
                                            word = L"false";
                                        break;
                                    case NovelVariable::Integer:
                                        word = std::to_wstring(nvar->value.asInt);
                                        break;
                                    default:
                                        doScale = false;
                                        word = L"NotSet";
                                        break;
                                }
                            }
                        }
                            
                        
                        if ((doScale && itsBeingSummed) || forceScale)
                        {
                            mustBeRecalculated = true;
                            
                            float left = base::ConvertToFloat(word);
                            left = left * ns::gs::scale;
                            finalLine += std::to_wstring(left);
                            
                            if (lastPos <= line.length() && lastPos > 0)
                                finalLine += line[lastPos - 1];
                        }
                        else
                        {
                            finalLine += word;
                            if (lastPos <= line.length() && lastPos > 0)
                                finalLine += line[lastPos - 1];
                        }
                    }
                    
                }
            }
            
            float left = nss::MathParser(finalLine);
            return GUIConstrainsResult(left, dependVariable, !mustBeRecalculated);
        }
        void GUIConstrains::Recalculate(GUIObject& guiObject, unsigned int width, unsigned int height)
        {
            GUIConstrainsResult res;
            if (!leftC && leftS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, leftS);
                left = res.result;
                leftC = res.constant;
                isDependsOnVariable[0] = res.dependsOnVariable;
            }
            if (!rightC && rightS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, rightS);
                right = res.result;
                rightC = res.constant;
                isDependsOnVariable[1] = res.dependsOnVariable;
            }
            if (!topC && topS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, topS);
                top = res.result;
                topC = res.constant;
                isDependsOnVariable[2] = res.dependsOnVariable;
            }
            if (!bottomC && bottomS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, bottomS);
                bottom = res.result;
                bottomC = res.constant;
                isDependsOnVariable[3] = res.dependsOnVariable;
            }
            
            if (!widthC && widthS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, widthS);
                this->width = res.result;
                widthC = res.constant;
                isDependsOnVariable[4] = res.dependsOnVariable;
            }
            if (!heightC && heightS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, heightS);
                this->height = res.result;
                heightC = res.constant;
                isDependsOnVariable[5] = res.dependsOnVariable;
            }
            
            if (!posXC && posXS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, posXS);
                posX = res.result;
                posXC = res.constant;
                isDependsOnVariable[6] = res.dependsOnVariable;
            }
            if (!posYC && posYS.length() != 0)
            {
                res = Recalculate(guiObject, width, height, posYS);
                posY = res.result;
                posYC = res.constant;
                isDependsOnVariable[7] = res.dependsOnVariable;
            }
            
            if (guiObject.guiSystem != nullptr && guiObject.guiSystem->parent != nullptr)
            {
                if (!widthC && (this->width == 0 || widthS.length() == 0))
                    this->width = guiObject.guiSystem->parent->constrains.width - right - left;
                if (!heightC && (this->height || heightS.length() == 0) == 0)
                    this->height = guiObject.guiSystem->parent->constrains.height - top - bottom;
                
                if (!posXC && posXS.length() == 0)
                    posX = guiObject.guiSystem->parent->constrains.posX + left;
                if (!posYC && posYS.length() == 0)
                    posY = guiObject.guiSystem->parent->constrains.posY + guiObject.guiSystem->parent->constrains.height - bottom - this->height;
                
                left += guiObject.guiSystem->parent->constrains.left;
                right += guiObject.guiSystem->parent->constrains.right;
                top += guiObject.guiSystem->parent->constrains.top;
                bottom += guiObject.guiSystem->parent->constrains.bottom;
            }
            else
            {
                if (!widthC && (this->width == 0 || widthS.length() == 0))
                    this->width = width - right - left;
                if (!heightC && (this->height == 0 || heightS.length() == 0))
                    this->height = height - top - bottom;
                
                if (!posXC && posXS.length() == 0)
                    posX = left;
                if (!posYC && posYS.length() == 0)
                    posY = height - bottom - this->height;
            }
        }
        
        
        
        void GUIObject::FadingUpdate(const sf::Time& elapsedTime)
        {
            sf::Int8 alpha;
            switch (fadingsMode)
            {
                case appearing:
                    currentTime += elapsedTime.asSeconds();
                    if (currentTime >= appearTime)
                    {
                        currentTime = 0.f;
                        fadingsMode = offline;
                        regulateFadings = false;
                        
                        alpha = 255;
                        SetAlpha(alpha);
                        if (child != nullptr)
                            child->SetAlpha(alpha);
                    }
                    else
                    {
                        alpha = (sf::Int8)(255 * currentTime/appearTime);
                        SetAlpha(alpha);
                        if (child != nullptr)
                            child->SetAlpha(alpha);
                    }
                    break;
                    
                case disappearing:
                    currentTime += elapsedTime.asSeconds();
                    if (currentTime >= disappearTime)
                    {
                        currentTime = 0.f;
                        fadingsMode = offline;
                        
                        alpha = 0;
                        SetAlpha(alpha);
                        if (child != nullptr)
                            child->SetAlpha(alpha);
                    }
                    else
                    {
                        alpha = (sf::Int8)(255 - 255 * currentTime/disappearTime);
                        SetAlpha(alpha);
                        if (child != nullptr)
                            child->SetAlpha(alpha);
                    }
                    break;
                    
                default:
                    break;
            }
        }
        void GUIObject::SetFadings(fadingsModeEnum newMode, float forTime)
        {
            if (newMode == appearing)
                ignoreVariableChange = false;
            else
                ignoreVariableChange = true;
            
            if (fadingsMode != newMode)
                currentTime = 0.f;
            
            if (newMode == appearing)
            {
                appearTime = forTime;
                fadingsMode = newMode;
            }
            else if (newMode == disappearing)
            {
                disappearTime = forTime;
                
                if ((unsigned char)guiSystem->lastAlpha == 0)
                    fadingsMode = offline;
                else
                    fadingsMode = newMode;
            }
            
            if (newMode == offline)
            {
                SetAlpha(0);
                if (child != nullptr)
                    child->SetAlpha(0);
            }
            
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
                
                /*constrains.Recalculate(*this, ns::gs::width, ns::gs::height);
                if (child != nullptr)
                    child->VariableResize(ns::gs::width, ns::gs::height);
                Resize(ns::gs::width, ns::gs::height);*/
                
                bool changed{ false };
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
                }
            }
        }
        void GUIObject::SetGUISystem(GUISystem *system)
        {
            this->guiSystem = system;
        }
        GUISystem* GUIObject::GetChildSystem()
        {
            if (child == nullptr)
            {
                child = new GUISystem();
                child->SetParent(this);
                child->SetNovel(guiSystem->novel);
                return child;
            }
            else
                return child;
        }
        
        
        
        namespace GUIObjects
        {
            void Rectangle::Init()
            {
                shape.setPosition(100, 100);
                shape.setFillColor(sf::Color::Black);
            }
            void Rectangle::Update(const sf::Time& elapsedTime)
            {
                
            }
            void Rectangle::Draw(sf::RenderWindow* window)
            {
                window->draw(shape);
            }
            void Rectangle::Resize(unsigned int width, unsigned int height)
            {
                shape.setSize({static_cast<float>(constrains.width), static_cast<float>(constrains.height)});
                shape.setPosition(constrains.posX, constrains.posY);
            }
            void Rectangle::SetAlpha(sf::Int8 alpha)
            {
                unsigned char realAlpha = sf::Int8((unsigned char)alpha * ((float)maxAlpha/255));
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, realAlpha));
            }
            
            
            
            void DialogueRectangle::Init()
            {
                shape.setFillColor(sf::Color::Black);
            }
            void DialogueRectangle::Update(const sf::Time& elapsedTime)
            {
                
            }
            void DialogueRectangle::Draw(sf::RenderWindow* window)
            {
                window->draw(shape);
            }
            void DialogueRectangle::Resize(unsigned int width, unsigned int height)
            {
                shape.setSize({static_cast<float>(constrains.width), static_cast<float>(constrains.height)});
                shape.setPosition(constrains.posX, constrains.posY);
            }
            void DialogueRectangle::SetAlpha(sf::Int8 alpha)
            {
                unsigned char realAlpha = sf::Int8((unsigned char)alpha * ((float)maxAlpha/255));
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, realAlpha));
            }
            
            
            
            void NameRectangle::Init()
            {
                shape.setPosition(100, 100);
                shape.setFillColor(sf::Color::Black);
            }
            void NameRectangle::Update(const sf::Time& elapsedTime)
            {
                
            }
            void NameRectangle::Draw(sf::RenderWindow* window)
            {
                window->draw(shape);
            }
            void NameRectangle::Resize(unsigned int width, unsigned int height)
            {
                shape.setSize({static_cast<float>(constrains.width), static_cast<float>(constrains.height)});
                shape.setPosition(constrains.posX, constrains.posY);
            }
            void NameRectangle::SetAlpha(sf::Int8 alpha)
            {
                unsigned char realAlpha = sf::Int8((unsigned char)alpha * ((float)maxAlpha/255));
                shape.setFillColor(sf::Color(shape.getFillColor().r, shape.getFillColor().g, shape.getFillColor().b, realAlpha));
            }
        }
    }
}
