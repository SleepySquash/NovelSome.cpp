//
//  main.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 20/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
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

//TODO: Constrains and stuff like that, in States of .nschar too
//TODO: Close, normal and far default states in every character
//TODO: Make "..." quotes possibly to be on the next line (to make scenarios look eaiser to read) and parse all \n's as \n in lines
//TODO: Scroll the dialogue if text's not fitting
//TODO: .nschar having their own dialogue line's colors, so dialogues look better

//TODO: Logic's "if" and "else"
//TODO: .nsdata files and NovelInfo structure
//TODO: Main menu
//TODO: Parallax should have some default animation (like moving cursor) so the image is not static
//      (maybe only when cursor is not being moved??) and this might be an option in preferences and in novel's settings

//TODO: Colors in GUI system
//TODO: Minimum width and height. After that no scaling can be made.
//TODO: If the device NovelSome is running on is slow on loading images and stuff, then there should be an option to preload it.
//TODO: Threading in loading images?

//TODO: Choice menu and novel's ability to read commands from line[] array.
//TODO: HUD as GUISystem parsed from skin file with command.
//TODO: .mp3 working.
//TODO: Game Pause having some buttons.
//TODO: Make game pause's fingers be blocked from touching something on the screen.
//TODO: There is should be Text's scaling factor, which will be diffrent from the usual gs::scale to make text more readble on the big devices and screens.

//TODO: Double clicking also calls Game Pause.
//TODO: Individual scaling factor in Settings or stuff.
//TODO: GUI class with buttons.

//TODO: "dialogue hide" command hides all the dialogues with fade.
//TODO: Argument after dialogue command CHAR "..." or just "..." named as "new" or "add" should make dialogues that have been already created not to destroy. Instead it should change their "after input/wait switch to mode" and set it to "waiting" until dialogue that will destroy every dialogue will appear or until "dialogue hide" will be called. However, every dialogue has to draw it's own GUI, so dialogue's GUI will be drawn the times dialogues are presented (before Draw() SetAlpha() event should be called as well). So that way you can create "bubble"-like dialogues near characters or stuff like that.
//TODO: Make in every GUIConstrains array of bools that show dependence of other constrains (even of parent's constrains) or a variable.

//TODO: Make it possible for someones' lines to be between specified characters, ([ and ]) or (" and ") for example.

//DONE: Make nss::Command not case sensetive as an option in nss::CommandSettings
//DONE: Global scaling factor
//DONE: Text's new line when exceed the screen's width
//DONE: Scenario changing with "jump" or "scenario" command
//DONE: Variables in unordered_map that can be string, int, float, bool etc.
//      The type should be specified somewhere? So it's be own-made structure, I guess.
//      Ability to seamlessly declare and even use without declaring variables.
//DONE: Text's outline rescaling with ns::gs::scale
//DONE: Dialogue's GUI
//DONE: Ownership change in Dialogues
//DONE: Name box in GUI system
//DONE: Dialogue's GUISystem should be drawn level BENEATH the actual dialogue lines
//DONE: Condition of appearing the name box
//DONE: Width of name box depending on @dialogue.width
//DONE: LocalVariable_Set - use it everywhere where varialbles are changing.
//DONE: Image's global library so no dublicate will be loaded.
//DONE: Game Pause entered by clicking with two fingers.
//DONE: Smart global scaling factor.
//DONE: Getting the color and getting the alpha should have its own NSS functions.
//      (usage in GUI parsing and in Skin parsing)
//DONE: Correct scaling speed when ratio is big.

// PARSINGS:
//          NovelUpdate
//          NovelLibrary's .nschar
//          Character's LoadState
//          Skin's parsing
//          GUI's parsing

#ifdef __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <SFML/Main.hpp>
    #endif
#endif

#include <iostream>
#include <unordered_map>
#include <thread>

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



void DrawInParallel(sf::RenderWindow* window, ns::EntitySystem* system)
{
    window->setActive();
    
    window->clear();
    system->Draw(window);
    window->display();
}



void CalculateScaleRatios(unsigned int width, unsigned int height)
{
    float factorX = (float)width / ns::gs::relativeWidth;
    float factorY = (float)height / ns::gs::relativeHeight;
    
    float ratioFactorX = (float)width/(float)height;
    float ratioFactorY = (float)height/(float)width;
    
    ns::gs::scale = factorX > factorY ? factorX : factorY;
    if (ratioFactorY > 1)
    {
        float m = ns::gs::scale;
        if (ratioFactorY < 1.2)
            ns::gs::scale = ns::gs::scale - m*(ratioFactorY - 1)*0.72;
        else if (ratioFactorY < 2)
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(ratioFactorY - 1.2)*0.17;
        else if (ratioFactorY < 2.46)
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(ratioFactorY - 2)*0.12;
        else if (ratioFactorY < 3)
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(ratioFactorY - 2.46)*0.07;
        else if (ratioFactorY < 4.8)
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(ratioFactorY - 3)*0.04;
        else if (ratioFactorY < 8)
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(ratioFactorY - 4.8)*0.02;
        else
            ns::gs::scale = ns::gs::scale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(8 - 4.8)*0.02;
    }
    else if (ratioFactorX > 1)
    {
        float m = ns::gs::scale;
        if (ratioFactorX < 1.9)
            ns::gs::scale = ns::gs::scale - m*(ratioFactorX - 1)*0.175;
        else if (ratioFactorX < 3)
            ns::gs::scale = ns::gs::scale - m*(1.9 - 1)*0.175 - m*(ratioFactorX - 1.9)*0.27;
        else if (ratioFactorX < 4.24)
            ns::gs::scale = ns::gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(ratioFactorX - 3)*0.15;
        else if (ratioFactorX < 7)
            ns::gs::scale = ns::gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(ratioFactorX - 4.24)*0.05;
        else if (ratioFactorX < 11)
            ns::gs::scale = ns::gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(ratioFactorX - 7)*0.02;
        else
            ns::gs::scale = ns::gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(11 - 7)*0.02;
    }
    if (ns::gs::scale < 0.18)
        ns::gs::scale = 0.18;
}



#ifdef _MSC_VER
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
#ifdef SFML_SYSTEM_IOS
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "NovelSome", sf::Style::Default);
    ns::gs::isParallaxEnabled = false;
#else
    #ifdef SFML_SYSTEM_ANDROID
        sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "NovelSome", sf::Style::Default);
        ns::gs::isParallaxEnabled = false;
    #else
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width >= 1280 ? 1280 : sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height >= 880 ? 800 : sf::VideoMode::getDesktopMode().height - 80), "NovelSome");
    #endif
#endif
    ns::gs::window = &window;
    ns::gs::width = window.getSize().x;
    ns::gs::height = window.getSize().y;
    ns::gs::relativeWidth = 1280;
    ns::gs::relativeHeight = 800;
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
                else if (version <= 6)
                    ns::gs::resolutionClass = 1;
            }
        #endif
        
        cout << "main :: GlobalSettings :: Autocomputed ResolutionClass is " << ns::gs::resolutionClass << "@x." << endl;
    }
    else
        ns::gs::resolutionClass = ns::gs::resolutionClassSetting;
    
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
        Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 16");
#ifdef SFML_SYSTEM_IOS
        Shimakaze->AddComponent<ns::EssentialComponents::GyroscopeParallax>();
#endif
#ifdef SFML_SYSTEM_ANDROID
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
                    
                case sf::Event::MouseButtonReleased:
                    system.PollEvent(event);
                    break;
                    
                case sf::Event::TouchBegan:
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
                                Shimakaze->AddComponent<ns::EssentialComponents::DebugComponent>("Update 0 build 16");
#ifdef SFML_SYSTEM_IOS
                                Shimakaze->AddComponent<ns::EssentialComponents::GyroscopeParallax>();
#endif
#ifdef SFML_SYSTEM_ANDROID
                                Shimakaze->AddComponent<ns::EssentialComponents::GyroscopeParallax>();
#endif
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
