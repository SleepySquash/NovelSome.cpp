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

//TODO: Colors in GUI system
//TODO: Minimum width and height. After that no scaling can be made.
//TODO: If the device NovelSome is running on is slow on loading images and stuff, then there should be an option to preload it.

//DONE: Make nss::Command not case sensetive as an option in nss::CommandSettings
//DONE: Global scaling factor
//DONE: Text's new line when exceed the screen's width
//DONE: Scenario changing with "jump" or "scenario" command
//DONE: Variables in unordered_map that can be string, int, float, bool etc.
//      The type should be specified somewhere? So it's be own-made structure, I guess.
//      Ability to seamlessly declare and even use without declaring variables.
//DONE: Text's outline rescaling with ns::gs::scale
//DONE: Ownership change in Dialogues
//DONE: Name box in GUI system
//DONE: Dialogue's GUISystem should be drawn level BENEATH the actual dialogue lines
//DONE: Condition of appearing the name box
//DONE: Width of name box depending on @dialogue.width
//DONE: LocalVariable_Set - use it everywhere where varialbles are changing.
//DONE: Image's global library so no dublicate will be loaded.

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <SFML/Main.hpp>
    #endif
#endif

#include <iostream>
#include <unordered_map>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Essentials/ResourcePath.hpp"
#include "Engine/EntitySystem.hpp"
#include "Engine/StaticMethods.hpp"

#include "Components/EssentialComponents.hpp"
#include "Components/NovelComponents/Novel.hpp"

#include <SFML/Config.hpp>

using std::cin;
using std::cout;
using std::endl;

void CalculateScaleRatios(unsigned int width, unsigned int height)
{
    float factorX = (float)width / ns::gs::relativeWidth;
    float factorY = (float)height / ns::gs::relativeHeight;
    
    float ratioFactorX = (float)width/(float)height;
    float ratioFactorY = (float)height/(float)width;
    
    if (ratioFactorY > 1.2)
        ns::GlobalSettings::scale = factorY - factorY * 1/(ratioFactorY - 0.2)*0.4;
    else if (ratioFactorX > 1.2)
        ns::GlobalSettings::scale = factorX * ((1.2 + ratioFactorX*0.3) / ratioFactorX);
    else
        ns::GlobalSettings::scale = factorX > factorY ? factorX : factorY;
}

int main()
{
#ifdef SFML_SYSTEM_IOS
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "NovelSome", sf::Style::Default);
    ns::GlobalSettings::width = sf::VideoMode::getDesktopMode().width;
    ns::GlobalSettings::height = sf::VideoMode::getDesktopMode().height;
#else
    sf::RenderWindow window(sf::VideoMode(1280, 800), "NovelSome");
    ns::GlobalSettings::window = &window;
    ns::GlobalSettings::width = 1280;
    ns::GlobalSettings::height = 800;
#endif
    ns::GlobalSettings::relativeWidth = 1280;
    ns::GlobalSettings::relativeHeight = 800;
    CalculateScaleRatios(ns::gs::width, ns::gs::height);
    
#ifdef _WIN32
    if (sf::VideoMode::getDesktopMode().width == window.getSize().x)
        window.setPosition({- ns::GlobalSettings::windowPositionOffset, window.getPosition().y});
#endif
    
    if (ns::gs::resolutionClassSetting == -1)
    {
        unsigned int width;
        unsigned int height;
        
        if (sf::VideoMode::getDesktopMode().width >= sf::VideoMode::getDesktopMode().height)
        {
            width = sf::VideoMode::getDesktopMode().width;
            height = sf::VideoMode::getDesktopMode().height;
        }
        else
        {
            width = sf::VideoMode::getDesktopMode().height;
            height = sf::VideoMode::getDesktopMode().width;
        }
        
        if (width <= 640 && height <= 480)
            ns::gs::resolutionClass = 0;
        else if (width <= 1024 && height <= 768)
            ns::gs::resolutionClass = 1;
        else if (width <= 1920 && height <= 1080)
            ns::gs::resolutionClass = 2;
        else
            ns::gs::resolutionClass = 3;
        
        #ifdef SFML_SYSTEM_IOS
            std::string device = iOSDeviceName();
            if (nss::Command(device, "iPhone"))
            {
                int version = ns::base::ConvertToInt(nss::ParseUntil(device, ',', 6));
                if (version <= 4)
                    ns::gs::resolutionClass = 0;
                else if (version <= 7)
                    ns::gs::resolutionClass = 1;
                else if (version == 8)
                {
                    int underVersion = ns::base::ConvertToInt(nss::ParseUntil(device, '\0', 8));
                    if (underVersion == 4)
                        ns::gs::resolutionClass = 1;
                    else
                        ns::gs::resolutionClass = 2;
                }
            }
            else if (nss::Command(device, "iPad"))
            {
                int version = ns::base::ConvertToInt(nss::ParseUntil(device, ',', 4));
                if (version <= 2)
                    ns::gs::resolutionClass = 0;
            }
            else if (nss::Command(device, "iPod"))
            {
                int version = ns::base::ConvertToInt(nss::ParseUntil(device, ',', 4));
                if (version <= 3)
                    ns::gs::resolutionClass = 0;
                else if (version <= 4)
                    ns::gs::resolutionClass = 1;
            }
        #endif
        
        cout << "main :: GlobalSettings :: Autocomputed ResolutionClass is " << ns::gs::resolutionClass << "@x." << endl;
    }
    else
        ns::gs::resolutionClass = ns::gs::resolutionClassSetting;
    
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
        Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 13");
#ifdef SFML_SYSTEM_IOS
        ns::gs::isParallaxEnabled = false;
        Shimakaze->AddComponent<ns::EssentialComponents::GyroscopeParallax>();
#endif
    }
    
    sf::Clock clock;
    sf::FloatRect visibleArea;
    
    window.setActive();
    while (window.isOpen())
    {
        sf::Event event;
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
                    
                case sf::Event::TouchEnded:
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
                                Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 13");
                            }
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                case sf::Event::Resized:
                    ns::GlobalSettings::width = event.size.width;
                    ns::GlobalSettings::height = event.size.height;
                    CalculateScaleRatios(event.size.width, event.size.height);
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
