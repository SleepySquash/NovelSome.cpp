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
        void Interface::RestoreToDefaults()
        {
            //TODO: Defaults
            guiDialogue.clear();
            GUIObjects::Rectangle* dialogueRect = guiDialogue.AddComponent<GUIObjects::Rectangle>();
            if (Skin::self) Skin::self->dialogue.dialogueRect = dialogueRect;
            dialogueRect->shape.setFillColor(sf::Color::Black);
            dialogueRect->maxAlpha = 170;
            dialogueRect->constrains.leftS = L"30";
            dialogueRect->constrains.rightS = L"30";
            dialogueRect->constrains.bottomS = L"10";
            dialogueRect->constrains.heightS = L".height/5 - 10";
            
            GUISystem* childSystem = dialogueRect->GetChildSystem();
            GUIObjects::Rectangle* nameRect = childSystem->AddComponent<GUIObjects::Rectangle>();
            if (Skin::self) Skin::self->dialogue.nameRect = nameRect;
            nameRect->shape.setFillColor(sf::Color::Black);
            nameRect->maxAlpha = 170;
            nameRect->constrains.leftS = L"10";
            nameRect->constrains.rightS = L"0";
            nameRect->constrains.bottomS = L".height + 5";
            nameRect->constrains.widthS = L"@name.width + 20";
            nameRect->constrains.heightS = L"@name.height + 10";
            nameRect->SetFadings(GUIObject::offline);
            
            
            guiChoose.clear();
            GUIObjects::Rectangle* chooseRect = guiChoose.AddComponent<GUIObjects::Rectangle>();
            if (Skin::self) Skin::self->choose.chooseRect = chooseRect;
            chooseRect->shape.setFillColor(sf::Color::Black);
            chooseRect->maxAlpha = 170;
            chooseRect->constrains.posXS = L".width/2 - 240";
            chooseRect->constrains.posYS = L".height/2 - 160";
            chooseRect->constrains.widthS = L"480";
            chooseRect->constrains.heightS = L"320";
            
            
            guiPause.clear();
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
        }
        void Interface::LoadFromFile(const std::wstring& fileName)
        {
            std::wstring fullPath = fileName;
            if (!base::FileExists(fullPath)) fullPath = base::utf16(resourcePath()) + fullPath;
            
            bool loadDefaultGUI{ true };
            bool loadDefaultChoice{ true };
            bool loadDefaultPause{ true };
            if (base::FileExists(fullPath))
            {
                guiDialogue.clear(); loadDefaultGUI = !guiDialogue.LoadFromFile(fileName, L"dialogue");
                guiChoose.clear(); loadDefaultChoice = !guiChoose.LoadFromFile(fileName, L"choose");
                guiPause.clear(); loadDefaultPause = !guiPause.LoadFromFile(fileName, L"gamepause");
            }
            if (loadDefaultGUI)
            {
                //Adding the dialogue's box
                GUIObjects::Rectangle* dialogueRect = guiDialogue.AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.dialogueRect = dialogueRect;
                dialogueRect->shape.setFillColor(sf::Color::Black);
                dialogueRect->maxAlpha = 170;
                dialogueRect->constrains.leftS = L"30";
                dialogueRect->constrains.rightS = L"30";
                dialogueRect->constrains.bottomS = L"10";
                dialogueRect->constrains.heightS = L".height/5 - 10";
                
                //Requesting the child system to add the name's box
                GUISystem* childSystem = dialogueRect->GetChildSystem();
                GUIObjects::Rectangle* nameRect = childSystem->AddComponent<GUIObjects::Rectangle>();
                if (Skin::self) Skin::self->dialogue.nameRect = nameRect;
                nameRect->shape.setFillColor(sf::Color::Black);
                nameRect->maxAlpha = 170;
                nameRect->constrains.leftS = L"10";
                nameRect->constrains.rightS = L"0";
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
            }
        }
    }
}
