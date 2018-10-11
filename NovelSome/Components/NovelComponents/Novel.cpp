//
//  NovelComponents.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 31/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Novel.hpp"

namespace ns
{
    namespace NovelComponents
    {
        Novel::Novel(sf::String path) : nsdataPath(path)
        {
            ns::ic::FreeImages();
            
            folderPath = ns::base::GetFolderPath(path);
            
            localVariables.insert({L"@dialogue", new NovelVariable(std::wstring(L""))});
            localVariables.insert({L"@name", new NovelVariable(std::wstring(L""))});
            localVariables.insert({L"version", new NovelVariable(std::wstring(L"Update 0 build 14"))});
            
            library.SetNovel(this);
            library.ScanForCharacters();
            
            gamePause.novel = this;
            skin.dialogue.gui.SetNovel(this);
            skin.LoadFromFile(folderPath + L"skin.nskin");
            
            sf::String filePath = (resourcePath() + path);
#ifdef _WIN32
            wif.open(filePath.toWideString());
#else
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(filePath.toWideString());
            wif.open(u8str);
#endif
            wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
            
            if (!(fileOpened = wif.is_open()))
                cout << "Error :: NovelComponent :: File couldn't be opened, path: " << path.toAnsiString() << endl;
        }
        Novel::~Novel()
        {
            wif.close();
            layers.Destroy();
            skin.dialogue.gui.Clear();
            skin.gamePauseGUI.Clear();
            ns::ic::FreeImages();
            
            for (auto& key : localVariables)
                if (key.second != nullptr)
                    delete key.second;
            
            FreeGroup<Background>(backgroundGroup);
            FreeGroup<Character>(characterGroup);
            FreeGroup<Dialogue>(dialogueGroup);
            FreeGroup<SoundPlayer>(soundGroup);
            FreeGroup<MusicPlayer>(musicGroup);
            FreeGroup<GUISystem>(GUIGroup);
            
            FreeGroup<NovelObject>(onHold);
        }
        void Novel::Draw(sf::RenderWindow* window)
        {
            if (fileOpened)
                layers.Draw(window);
            gamePause.Draw(window);
        }
        void Novel::Resize(unsigned int width, unsigned int height)
        {
            if (fileOpened)
                layers.Resize(width, height);
            gamePause.Resize(width, height);
        }
        void Novel::PollEvent(sf::Event& event)
        {
            if (fileOpened && !(ns::GlobalSettings::isPause))
                layers.PollEvent(event);
            gamePause.PollEvent(event);
        }
        void Novel::OnHold(NovelObject* component)
        {
            onHold = ns::list::Insert<ns::NovelObject>(onHold);
            onHold->data = component;
            
            onHoldSize++;
        }
        void Novel::UnHold(NovelObject* component)
        {
            List<NovelObject>* temp = onHold;
            while (temp != nullptr)
            {
                List<NovelObject>* next = temp->next;
                if (temp->data == component)
                {
                    if (temp == onHold)
                        onHold = temp->next;
                    if (temp->next != nullptr)
                        temp->next->prev = temp->prev;
                    if (temp->prev != nullptr)
                        temp->prev->next = temp->next;
                    delete temp;
                    
                    onHoldSize--;
                    next = nullptr;
                }
                
                temp = next;
            }
        }
        sf::String Novel::GetFolderPath()
        {
            return folderPath;
        }
        
        
        
        
        
        
        NovelVariable* Novel::FindVariable(const std::wstring& variableName)
        {
            return (localVariables.find(variableName) != localVariables.end()) ? localVariables.at(variableName) : nullptr;
        }
        void Novel::VariableChange(const std::wstring& name)
        {
            skin.dialogue.gui.VariableChange(name);
            if (GUIGroup != nullptr)
            {
                List<GUISystem>* temp = GUIGroup;
                while (temp != nullptr)
                {
                    if (temp->data != nullptr)
                        temp->data->VariableChange(name);
                    temp = temp->next;
                }
            }
        }
        void Novel::LocalVariables_Set(const std::wstring& name, std::wstring value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
            {
                if (nvar->type == nvar->String)
                    callChange = (nvar->value.asString != value);
                else
                    callChange = true;
            }
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, bool value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Boolean)))
                    callChange = (nvar->value.asBoolean != value);
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        void Novel::LocalVariables_Set(const std::wstring& name, int value)
        {
            NovelVariable *nvar;
            if (localVariables.find(name) == localVariables.end())
            {
                nvar = new NovelVariable();
                localVariables.insert({name, nvar});
            }
            else
                nvar = localVariables.at(name);
            
            bool callChange{ false };
            if (nvar->type != nvar->NotSet)
                if (!(callChange = (nvar->type != nvar->Integer)))
                    callChange = (nvar->value.asInt != value);
            
            nvar->Set(value);
            if (callChange)
                VariableChange(name);
        }
        
        
        
        
        
        
        void Novel::RemoveFromGroup(List<Background>* groupPointer)
        {
            if (groupPointer == backgroundGroup)
            {
                backgroundGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<Dialogue>* groupPointer)
        {
            if (groupPointer == dialogueGroup)
            {
                dialogueGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<Character>* groupPointer)
        {
            if (groupPointer == characterGroup)
            {
                characterGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<SoundPlayer>* groupPointer)
        {
            if (groupPointer == soundGroup)
            {
                soundGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<MusicPlayer>* groupPointer)
        {
            if (groupPointer == musicGroup)
            {
                musicGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
        void Novel::RemoveFromGroup(List<GUISystem>* groupPointer)
        {
            if (groupPointer == GUIGroup)
            {
                GUIGroup = groupPointer->next;
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = nullptr;
                
                delete groupPointer;
            }
            else
            {
                if (groupPointer->next != nullptr)
                    groupPointer->next->prev = groupPointer->prev;
                if (groupPointer->prev != nullptr)
                    groupPointer->prev->next = groupPointer->next;
                
                delete groupPointer;
            }
        }
    }
}

/*void NovelLibrary::ScanForCharacters()
 {
 if (novel != nullptr)
 {
 DIR *dir;
 struct dirent *ent;
 
 sf::String path{ executablePath() + novel->GetFolderPath() };
 if ((dir = opendir(path.toAnsiString().c_str())) != NULL)
 {
 while ((ent = readdir (dir)) != NULL)
 std::cout << "Entry found: " << ent->d_name << std::endl;
 closedir (dir);
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Could not open directory: '" << path.toAnsiString() << '\'' << std::endl;
 }
 else
 std::cout << "NovelLibrary :: ScanForCharacters :: Novel not set: '" << '\'' << std::endl;
 }*/
