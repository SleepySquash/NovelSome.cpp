//
//  main.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 20/08/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

//TODO: If not _WIN32, then replace all \ slashes to / in filenames (background, music)

//TODO: .mp3 working
//TODO: Smart global scaling factor
//TODO: Dialogue's GUI
//TODO: Constrains and stuff like that, in States of .nschar too
//TODO: Close, normal and far default states in every character
//TODO: Make "..." quotes possibly to be on the next line (to make scenarios look eaiser to read) and parse all \n's as \n in lines
//TODO: Scroll the dialogue if text's not fitting
//TODO: .nschar having their own dialogue line's colors, so dialogues look better

//TODO: Choice menu and novel's ability to read commands from line[] array
//TODO: HUD
//TODO: Logic's "if" and "else"
//TODO: .nsdata files and NovelInfo structure
//TODO: Main menu
//TODO: Parallax should have some default animation (like moving cursor) so the image is not static
//      (maybe only when cursor is not being moved??) and this might be an option in preferences and in novel's settings

//TODO: Text's outline rescaling with ns::gs::scale
//TODO: Colors in GUI system
//TODO: Ownership change in Dialogues
//TODO: Name box in GUI system
//TODO: Dialogue's GUISystem should be drawn level BENEATH the actual dialogue lines

//TODO: Condition of appearing the name box
//TODO: Width of name box depending on @dialogue.width
//TODO: Minimum width and height. After that no scaling can be made.
//TODO: LocalVariable_Set - use it everywhere where varialbles are changing.

//DONE: Make nss::Command not case sensetive as an option in nss::CommandSettings
//DONE: Global scaling factor
//DONE: Text's new line when exceed the screen's width
//DONE: Scenario changing with "jump" or "scenario" command
//DONE: Variables in unordered_map that can be string, int, float, bool etc.
//      The type should be specified somewhere? So it's be own-made structure, I guess.
//      Ability to seamlessly declare and even use without declaring variables.

#include <iostream>
#include <unordered_map>

#include <SFML/Main.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Essentials/ResourcePath.hpp"
#include "Engine/EntitySystem.hpp"
#include "Engine/StaticMethods.hpp"

#include "Components/EssentialComponents.hpp"
#include "Components/NovelComponents/Novel.hpp"

using std::cin;
using std::cout;
using std::endl;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 800), "NovelSome");
    ns::GlobalSettings::window = &window;
    ns::GlobalSettings::width = 1280;
    ns::GlobalSettings::height = 800;
    
    ns::GlobalSettings::relativeWidth = 1280;
    ns::GlobalSettings::relativeHeight = 800;
#ifdef _WIN32
    if (sf::VideoMode::getDesktopMode().width == window.getSize().x)
        window.setPosition({- ns::GlobalSettings::windowPositionOffset, window.getPosition().y});
#endif

    sf::Image icon;
    if (icon.loadFromFile(resourcePath() + "Data/Images/macos@2x.png"))
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    window.setFramerateLimit(ns::gs::framerateLimit);
    window.setVerticalSyncEnabled(ns::gs::isVerticalSyncEnabled);

    ns::EntitySystem system;
    
    ///----------------------------------------------------------
    /// \brief Entity to hold novel and stuff it depends on
    ///
    /// Entity holds the novel itself in order to have some ierarchy of entities.
    /// It also may hold some libraries etc that novel depends on.
    ///
    ///----------------------------------------------------------
    ns::Entity* Elizabeth = system.AddEntity();
    {
        Elizabeth->AddComponent<ns::NovelComponents::Novel>("Novels/Bundle/scen.nsdat");
    }
    
    ///----------------------------------------------------------
    /// \brief Entity to hold essential components
    ///
    /// Entity holds components like always-on debug UI layer, system's components and other essential stuff.
    /// It also may hold components like NovelComponent that runs the novel, cuz it has to be essential component.
    ///
    ///----------------------------------------------------------
    ns::Entity* Shimakaze = system.AddEntity();
    {
        Shimakaze->AddComponent<ns::EssentialComponents::GamePause>();
        Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 12");
    }
    
    sf::Clock clock;
    sf::FloatRect visibleArea;
    while (window.isOpen())
    {
        sf::Event event;
        float factorX, factorY;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                    
                case sf::Event::GainedFocus:
                    window.setFramerateLimit(ns::gs::framerateLimit);
                    break;
                    
                case sf::Event::LostFocus:
                    window.setFramerateLimit(ns::gs::framerateNoFocus);
                    break;
                    
                case sf::Event::MouseMoved:
                    system.PollEvent(event);
                    break;
                    
                case sf::Event::MouseButtonPressed:
                    system.PollEvent(event);
                    break;
                    
                case sf::Event::KeyPressed:
                    system.PollEvent(event);
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Space:
                            system.AddEntity();
                            break;
                        case sf::Keyboard::E:
                            system.PopEntity(Shimakaze);
                            Shimakaze = nullptr;
                            system.PopEntity(Elizabeth);
                            Elizabeth = nullptr;
                            break;
                        case sf::Keyboard::R:
                            if (Shimakaze != nullptr)
                                system.PopEntity(Shimakaze);
                            if (Elizabeth != nullptr)
                                system.PopEntity(Elizabeth);
                            Elizabeth = system.AddEntity();
                            {
                                Elizabeth->AddComponent<ns::NovelComponents::Novel>("Novels/Bundle/scen.nsdat");
                            }
                            Shimakaze = system.AddEntity();
                            {
                                Shimakaze->AddComponent<ns::EssentialComponents::GamePause>();
                                Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 12");
                            }
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case sf::Event::Resized:
                    ns::GlobalSettings::width = event.size.width;
                    ns::GlobalSettings::height = event.size.height;
                    
                    factorX = (float)event.size.width / ns::GlobalSettings::relativeWidth;
                    factorY = (float)event.size.height / ns::GlobalSettings::relativeHeight;
                    ns::GlobalSettings::scale = factorX > factorY ? factorX : factorY;
                    
                    system.Resize(event.size.width, event.size.height);
                    
                    visibleArea = sf::FloatRect(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(visibleArea));
                    break;
                    
                default:
                    break;
            }
        }

        system.Update(clock.restart());
        
        window.clear();
        system.Draw(&window);
        window.display();
    }
    
    system.Destroy();

    return EXIT_SUCCESS;
}
