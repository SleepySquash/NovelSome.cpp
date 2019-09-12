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
//TODO: Parallax should have some default animation (like moving cursor) so the image is not static
//      (maybe only when cursor is not being moved??) and this might be an option in preferences and in novel's settings

//TODO: Colors in GUI system
//TODO: Minimum width and height. After that no scaling can be made.

//TODO: Choice menu and novel's ability to read commands from line[] array.
//TODO: HUD as GUISystem parsed from skin file with command.
//TODO: .mp3 working.
//TODO: Game Pause having some buttons.
//TODO: Make game pause's fingers be blocked from touching something on the screen.

//TODO: Double clicking also calls Game Pause (only with touch).
//TODO: Individual scaling factor in Settings or stuff.
//TODO: GUI class with buttons.

//TODO: "dialogue hide" command hides all the dialogues with fade.
//TODO: Argument after dialogue command CHAR "..." or just "..." named as "new" or "add" should make dialogues that have been already created not to destroy. Instead it should change their "after input/wait switch to mode" and set it to "waiting" until dialogue that will destroy every dialogue will appear or until "dialogue hide" will be called. However, every dialogue has to draw it's own GUI, so dialogue's GUI will be drawn the times dialogues are presented (before Draw() SetAlpha() event should be called as well). So that way you can create "bubble"-like dialogues near characters or stuff like that.
//TODO: Make in every GUIConstrains array of bools that show dependence of other constrains (even of parent's constrains) or a variable.

//TODO: Make it possible for someones' lines to be between specified characters, ([ and ]) or (" and ") for example.


//TODO: Make choices be dependent on the "as choose" GUIObject.
//TODO: Properly make GUISystem's images adding, showing etc.
//TODO: If one wants to make their own choices, them should use GUISystem => adding HUDs. NovelSome's "choose" command only supports list-like choosing (for now) and skin marked as "as choose" is only for limiting the space list will take.
//TODO: Settings in the GamePause (and in the menu for future): changing the ResolutionClass.



//TODO: Change the way negative offset is being made.
//TODO: Make main menu draw only novel buttons that are in the scope.



//TODO: Make GUIConstrains calculate in parts:
//       Call GUIObject's PreCalculate()
//      1) Calculate left, right, top, bottom, width and height
//       Call GUIObject's function like PreCalculatedSize()
//      3) Calculate posx, posy
//       Call GUIObject's Resize()




// TODO: Кастомные шрифты из папки новеллы.
// TODO: Изображения чтоб можно было image:"..."
// TODO: Элементы интерфейса - поиск также в папках GUI, Interface и так далее.
// TODO: Отвязать as Name от ширины/высоты @name.
// DONE: Проверить, почему Arial.ttf не загрузился, когда не было шрифта.
// TODO: Макросы для скейлинга только по X и только по Y.
// TODO: Возможность ГУИ объектам быть только горизонтальными или только вертикальными.

// DONE: Возможность менять настройки диалога при загрузке его гуи системы (т.е. при смене, например).
// DONE: Возможность менять skin (+ сохранение и загрузка, если было изменено с дефолта).




//DONE: Make nss::Command not case sensetive as an option in nss::CommandSettings
//DONE: Global scaling factor
//DONE: Text's new line when exceed the screen's width
//DONE: Scenario changing with "jump" or "scenario" command
//DONE: Variables in unordered_map that can be string, int, float, bool etc.
//      The type should be specified somewhere? So it's be own-made structure, I guess.
//      Ability to seamlessly declare and even use without declaring variables.
//DONE: Text's outline rescaling with gs::scale
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
//DONE: Make minEH be able to "send messages" to other components. Like, for example, message to the MainMenu from Novel about its ending and exiting, so the MainMenu starts to draw itself again. Or if there's no MainMenu, then the message is destroyed.
//DONE: "music" and "ambient" commands should stop currently playing music.
//DONE: Adding the main menu and the novel select screen.
//DONE: There is should be Text's scaling factor, which will be diffrent from the usual gs::scale to make text more readble on the big devices and screens.
//DONE: If the device NovelSome is running on is slow on loading images and stuff, then there should be an option to preload it.
//DONE: Threading in loading images?
//DONE: .nsdata files and NovelInfo structure
//DONE: Main menu

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
#ifdef SFML_SYSTEM_ANDROID
    #include <jni.h>
    #include <android/native_activity.h>
    #include <android/log.h>
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sfml-activity", __VA_ARGS__))
    #include <SFML/System/NativeActivity.hpp>
#endif

#include <iostream>
#include <unordered_map>
#include <thread>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "Essentials/ResourcePath.hpp"
#include "Engine/EntitySystem.hpp"
#include "Engine/Settings.hpp"
#include "Engine/Initialization.hpp"

#include "Components/Helpers/EssentialComponents.hpp"
#include "Components/Helpers/LanguageLibrary.hpp"
#include "Components/NSMenuComponents/MainMenu.hpp"
#include "Components/NSMenuComponents/NovelLoader.hpp"
#include "Components/NovelComponents/Novel.hpp"
#include "Components/ClientComponents/ClientImpl.hpp"

#include "Components/Interfaces/SaveLoadUI.hpp"
#include "Components/Interfaces/SettingsUI.hpp"
#include "Components/Interfaces/HeavensGateUI.hpp"

#include <SFML/Config.hpp>

using std::cin;
using std::cout;
using std::endl;

using namespace ns;

#ifdef _MSC_VER
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "NovelSome", sf::Style::Default);
    gs::isParallaxEnabled = gs::buttonHovering = false;
#else
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width >= 1280 ? 1280 : sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height >= 880 ? 800 : sf::VideoMode::getDesktopMode().height - 80), "NovelSome");
#endif
    gs::window = &window;
    gs::width = window.getSize().x;
    gs::height = window.getSize().y;
    gs::relativeWidth = 1280;
    gs::relativeHeight = 800;
    
#ifdef _WIN32
    if (sf::VideoMode::getDesktopMode().width == window.getSize().x)
        window.setPosition({- GlobalSettings::windowPositionOffset, window.getPosition().y});
#endif
    
    ExecuteInitalization();
    
    window.setFramerateLimit(gs::framerateLimit);
    window.setVerticalSyncEnabled(gs::isVerticalSyncEnabled);
    
    EntitySystem system;
    
    ///----------------------------------------------------------
    /// \brief Entity to hold NekoUI components
    ///
    /// Entity holds networking components.
    ///
    ///----------------------------------------------------------
    Entity* Tenshi = system.AddEntity();
    {
        Tenshi->AddComponent<HeavensGate::ClientComponent>();
    }
    
    ///----------------------------------------------------------
    /// \brief Entity to hold novel and stuff it depends on
    ///
    /// Entity holds the novel itself in order to have some ierarchy of entities.
    /// It also may hold some interfaces etc that novel depends on.
    ///
    ///----------------------------------------------------------
    Entity* Elizabeth = system.AddEntity();
    {
        ic::globalRequestSender = Elizabeth;
        Language::Default(); Languages::LoadData();
        Elizabeth->AddComponent<SaveLoadUI>();
        Elizabeth->AddComponent<SettingsUI>();
        Elizabeth->AddComponent<NSMenuComponents::MainMenu>();
        Elizabeth->AddComponent<NovelLoader>();
    }
    
    ///----------------------------------------------------------
    /// \brief Entity to hold essential components
    ///
    /// Entity holds components like always-on debug UI layer, system's components and other essential stuff.
    ///
    ///----------------------------------------------------------
    Entity* Shimakaze = system.AddEntity();
    {
        Shimakaze->AddComponent<EssentialComponents::DebugComponent>("Update 0 build 21");
        Shimakaze->AddComponent<EssentialComponents::FadingFromBlackScreen>();
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
        // Shimakaze->AddComponent<EssentialComponents::GyroscopeParallax>();
#endif
    }
    gs::lastMousePos = { sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y };
    
    bool active{ true };
    sf::Clock clock;
#if defined(SFML_SYSTEM_IOS)
    window.setActive();
#endif
    while (window.isOpen())
    {
        sf::Event event;
        while (active ? window.pollEvent(event) : window.waitEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed: window.close(); break;
                case sf::Event::GainedFocus: active = true; clock.restart(); window.setFramerateLimit(gs::framerateLimit);
#ifdef SFML_SYSTEM_ANDROID
                    window.setActive();
#endif
                    break;
                case sf::Event::LostFocus: active = false; system.PollEvent(event); window.setFramerateLimit(gs::framerateNoFocus);
#ifdef SFML_SYSTEM_ANDROID
                    window.setActive(false);
#endif
                    break;
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
                case sf::Event::MouseEntered: active = true; clock.restart(); window.setFramerateLimit(gs::framerateLimit);
#ifdef SFML_SYSTEM_ANDROID
                    window.setActive();
#endif
                    break;
                case sf::Event::MouseLeft: active = false; system.PollEvent(event); window.setFramerateLimit(gs::framerateNoFocus);
#ifdef SFML_SYSTEM_ANDROID
                    window.setActive(false);
#endif
                    break;
#endif
                    
                case sf::Event::MouseWheelScrolled: system.PollEvent(event); break;
                case sf::Event::TouchEnded: case sf::Event::TouchBegan:
                    gs::lastMousePos = { event.touch.x, event.touch.y };
                    system.PollEvent(event);
                    break;
                case sf::Event::MouseButtonReleased: case sf::Event::MouseButtonPressed:
                    gs::lastMousePos = { event.mouseButton.x, event.mouseButton.y };
                    system.PollEvent(event);
                    break;
                case sf::Event::TouchMoved:
                    system.PollEvent(event);
                    gs::lastMousePos = { event.touch.x, event.touch.y };
                    break;
                case sf::Event::MouseMoved:
                    system.PollEvent(event);
                    gs::lastMousePos = { event.mouseMove.x, event.mouseMove.y };
                    break;
                    
                case sf::Event::TextEntered: if (gs::listenForTextInput) system.PollEvent(event); break; 
                case sf::Event::KeyPressed: system.PollEvent(event); break;
                case sf::Event::KeyReleased: if (event.key.code == sf::Keyboard::Key::Enter) system.PollEvent(event); break;
                    
                case sf::Event::Resized:
                    gs::width = event.size.width;
                    gs::height = event.size.height;
                    CalculateScaleRatios(event.size.width, event.size.height);
                    system.Resize(event.size.width, event.size.height);
                    window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
#ifdef SFML_SYSTEM_ANDROID
                    if (active)
                    {
                        window.clear();
                        system.Draw(&window);
                        window.display(); //TODO: Might crash here if app is not running
                    }
#endif
                    break;
                    
                default: break;
            }
            gs::requestWindowRefresh = true;
        }
        
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
        if (active)
        {
            system.Update(clock.restart());
            
            if (gs::requestWindowRefresh)
            {
                window.clear();
                system.Draw(&window);
                window.display(); //TODO: Might crash here if app is not running
                
                gs::requestWindowRefresh = false;
            } else sf::sleep(sf::milliseconds(10));
        } else sf::sleep(sf::milliseconds(100));
#else
        system.Update(clock.restart());
        
        if (gs::requestWindowRefresh)
        {
            window.clear();
            system.Draw(&window);
            window.display();
            
            gs::requestWindowRefresh = false;
        } else sf::sleep(sf::milliseconds(10));
#endif
    }
    
    system.clear();

    return EXIT_SUCCESS;
}
