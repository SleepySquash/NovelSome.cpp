//
//  Interface.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/03/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#include "Interface.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Interface::~Interface() { guiDialogue.clear(); guiChoose.clear(); guiPause.clear(); }
        void Interface::RestoreToDefaults(const RestoreInterface& restore)
        {
            //TODO: Defaults
            if (restore == RestoreInterface::All || restore == RestoreInterface::Dialogue)
            {
                guiDialogue.clear();
                GUIObjects::Rectangle* dialogueRect = guiDialogue.AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.dialogueRectH = Skin::self->dialogue.dialogueRectV = dialogueRect;
                dialogueRect->shape.setFillColor(sf::Color::Black);
                dialogueRect->maxAlpha = 170;
                dialogueRect->constrains.leftS = L"0";
                dialogueRect->constrains.rightS = L"0";
                dialogueRect->constrains.bottomS = L"0";
                dialogueRect->constrains.heightS = L".height/5 - 10";
                
                GUISystem* childSystem1 = dialogueRect->GetChildSystem();
                
                GUIObjects::DialogueConstrains* dialogueConstrains = childSystem1->AddComponent<GUIObjects::DialogueConstrains>();
                if (Skin::self) Skin::self->dialogue.textConstrainsH = Skin::self->dialogue.textConstrainsV = dialogueConstrains;
                dialogueConstrains->constrains.leftS = L"15";
                dialogueConstrains->constrains.rightS = L"15";
                dialogueConstrains->constrains.topS = L"5";
                dialogueConstrains->constrains.bottomS = L"5";
                
                GUIObjects::Rectangle* nameRect = childSystem1->AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.nameRectH = Skin::self->dialogue.nameRectV = nameRect;
                nameRect->shape.setFillColor(sf::Color::Black);
                nameRect->maxAlpha = 170;
                nameRect->constrains.leftS = L"10";
                nameRect->constrains.bottomS = L".height + 5";
                nameRect->constrains.widthS = L"@name.width + 20";
                nameRect->constrains.heightS = L"@name.height + 10";
                nameRect->SetFadings(GUIObject::offline);
                
                GUISystem* childSystem2 = nameRect->GetChildSystem();
                
                GUIObjects::DialogueConstrains* nameConstrains = childSystem2->AddComponent<GUIObjects::DialogueConstrains>();
                if (Skin::self) Skin::self->dialogue.nameConstrainsH = Skin::self->dialogue.nameConstrainsV = nameConstrains;
                nameConstrains->constrains.leftS = L"7";
                nameConstrains->constrains.rightS = L"5";
                nameConstrains->constrains.topS = L"-3";
                nameConstrains->constrains.bottomS = L"5";
            }
            
            
            
            if (restore == RestoreInterface::All || restore == RestoreInterface::Choose)
            {
                guiChoose.clear();
                GUIObjects::Rectangle* chooseRect = guiChoose.AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->choose.chooseRect = chooseRect;
                chooseRect->shape.setFillColor(sf::Color::Black);
                chooseRect->maxAlpha = 170;
                chooseRect->constrains.posXS = L".width/2 - 240";
                chooseRect->constrains.posYS = L".height/2 - 160";
                chooseRect->constrains.widthS = L"480";
                chooseRect->constrains.heightS = L"320";
            }
            
            
            
            if (restore == RestoreInterface::All || restore == RestoreInterface::Pause)
            {
                guiPause.clear();
                GUIObjects::Rectangle* pauseRect = guiPause.AddComponent<GUIObjects::Rectangle>();
                pauseRect->shape.setFillColor(sf::Color::Black);
                pauseRect->maxAlpha = 120;
                GUIObjects::Rectangle* centerRect = guiPause.AddComponent<GUIObjects::Rectangle>();
                centerRect->shape.setFillColor(sf::Color::Black);
                centerRect->constrains.posXS = L".width/2 - 210";
                centerRect->constrains.widthS = L"420";
                centerRect->maxAlpha = 170;
                
                GUIObjects::Button* menuButton = guiPause.AddComponent<GUIObjects::Button>(GUIObjects::Button::Text);
                menuButton->text->setColor(sf::Color::White);
                menuButton->text->setFont(L"Pacifica.ttf");
                menuButton->text->setCharacterSize(56);
                menuButton->text->setString(lang::get("Exit"));
                menuButton->actions.push_back(L"menu");
                menuButton->constrains.posXS = L".width/2";
                menuButton->constrains.posYS = L".height - .height/5";
                
                GUIObjects::Button* saveButton = guiPause.AddComponent<GUIObjects::Button>(GUIObjects::Button::Text);
                saveButton->text->setColor(sf::Color::White);
                saveButton->text->setFont(L"Pacifica.ttf");
                saveButton->text->setCharacterSize(56);
                saveButton->text->setString(lang::get("Save"));
                saveButton->actions.push_back(L"save");
                saveButton->constrains.posXS = L".width/2";
                saveButton->constrains.posYS = L".height/2 - 32";
                
                GUIObjects::Button* loadButton = guiPause.AddComponent<GUIObjects::Button>(GUIObjects::Button::Text);
                loadButton->text->setColor(sf::Color::White);
                loadButton->text->setFont(L"Pacifica.ttf");
                loadButton->text->setCharacterSize(56);
                loadButton->text->setString(lang::get("Load"));
                loadButton->actions.push_back(L"load");
                loadButton->constrains.posXS = L".width/2";
                loadButton->constrains.posYS = L".height/2 + 32";
            }
        }
        void Interface::LoadFromFile(const std::wstring& fileName, const std::wstring& folderPath)
        {
            std::wstring fullPath;
            if (folderPath == L"") fullPath = fileName; else fullPath = folderPath + fileName;
            if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
            
            bool loadDefaultGUI{ !ignoreDialogue };
            bool loadDefaultChoice{ !ignoreChoose };
            bool loadDefaultPause{ !ignorePause };
            if (base::FileExists(fullPath))
            {
                if (!ignoreDialogue) { guiDialogue.clear(); loadDefaultGUI = !guiDialogue.LoadFromFile(folderPath + fileName, L"dialogue"); }
                if (!ignoreChoose) { guiChoose.clear(); loadDefaultChoice = !guiChoose.LoadFromFile(folderPath + fileName, L"choose"); }
                if (!ignorePause) { guiPause.clear(); loadDefaultPause = !guiPause.LoadFromFile(folderPath + fileName, L"gamepause"); }
            }
            if (!ignoreDialogue) guiDialogue.trueFileName = defaultDialogue = fileName;
            if (!ignoreChoose) guiChoose.trueFileName = defaultChoose = fileName;
            if (!ignorePause) guiPause.trueFileName = defaultPause = fileName;
            
            if (loadDefaultGUI) RestoreToDefaults(RestoreInterface::Dialogue);
            if (loadDefaultChoice) RestoreToDefaults(RestoreInterface::Choose);
            if (loadDefaultPause) RestoreToDefaults(RestoreInterface::Pause);
            /*if (loadDefaultGUI)
            {
                //Adding the dialogue's box
                GUIObjects::Rectangle* dialogueRect = guiDialogue.AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.dialogueRectH = Skin::self->dialogue.dialogueRectV = dialogueRect;
                dialogueRect->shape.setFillColor(sf::Color::Black);
                dialogueRect->maxAlpha = 170;
                dialogueRect->constrains.leftS = L"30";
                dialogueRect->constrains.rightS = L"30";
                dialogueRect->constrains.bottomS = L"10";
                dialogueRect->constrains.heightS = L".height/5 - 10";
                
                //Requesting the child system to add the name's box
                GUISystem* childSystem = dialogueRect->GetChildSystem();
                GUIObjects::Rectangle* nameRect = childSystem->AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.nameRectH = Skin::self->dialogue.nameRectV = nameRect;
                nameRect->shape.setFillColor(sf::Color::Black);
                nameRect->maxAlpha = 170;
                nameRect->constrains.leftS = L"10";
                nameRect->constrains.bottomS = L".height + 5";
                nameRect->constrains.widthS = L"@name.width + 20";
                nameRect->constrains.heightS = L"@name.height + 10";
                nameRect->SetFadings(GUIObject::offline);
            }
            if (loadDefaultChoice)
            {
                GUIObjects::Rectangle* chooseRect = guiChoose.AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->choose.chooseRect = chooseRect;
                chooseRect->shape.setFillColor(sf::Color::Black);
                chooseRect->maxAlpha = 170;
                chooseRect->constrains.posXS = L".width/2 - 240";
                chooseRect->constrains.posYS = L".height/2 - 160";
                chooseRect->constrains.widthS = L"480";
                chooseRect->constrains.heightS = L"320";
            }
            if (loadDefaultPause)
            {
                GUIObjects::Rectangle* pauseRect = guiPause.AddComponent<GUIObjects::Rectangle>();
                pauseRect->shape.setFillColor(sf::Color::Black);
                pauseRect->maxAlpha = 170;
                GUIObjects::Button* menuButton = guiPause.AddComponent<GUIObjects::Button>(GUIObjects::Button::Text);
                menuButton->text->setColor(sf::Color::White);
                menuButton->text->setFont(L"Pacifica.ttf");
                menuButton->text->setCharacterSize(56);
                menuButton->text->setString(L"Back to menu");
                menuButton->actions.push_back(L"menu");
                menuButton->constrains.posXS = L".width/2";
                menuButton->constrains.posYS = L".height - .height/5";
            }*/
            ignoreDialogue = false;
            ignorePause = false;
            ignoreChoose = false;
        }
    }
}
