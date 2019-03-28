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

#include "Components/EssentialComponents.hpp"
#include "Components/NSMenuComponents/MainMenu.hpp"
#include "Components/NovelComponents/Novel.hpp"
//#include "Components/NekoComponents/NekoNinja.hpp"

#include <SFML/Config.hpp>

using std::cin;
using std::cout;
using std::endl;

using namespace ns;



void DrawInParallel(sf::RenderWindow* window, EntitySystem* system)
{
    window->setActive();
    
    window->clear();
    system->Draw(window);
    window->display();
}



void CalculateScaleRatios(unsigned int width, unsigned int height)
{
    float factorX = (float)width / gs::relativeWidth; gs::scalex = factorX;
    float factorY = (float)height / gs::relativeHeight; gs::scaley = factorY;
    
    float ratioFactorX = (float)width/(float)height;
    float ratioFactorY = (float)height/(float)width;
    gs::verticalOrientation = ratioFactorX < 1.23;
    
    gs::scale = factorX > factorY ? factorX : factorY;
    gs::scScale = gs::scale;
    if (ratioFactorY > 1)
    {
        float m = gs::scale;
        if (ratioFactorY < 1.2)
            gs::scale = gs::scale - m*(ratioFactorY - 1)*1.5;
        else if (ratioFactorY < 2)
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(ratioFactorY - 1.2)*0.17;
        else if (ratioFactorY < 2.46)
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(2 - 1.2)*0.17 - m*(ratioFactorY - 2)*0.12;
        else if (ratioFactorY < 3)
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(ratioFactorY - 2.46)*0.07;
        else if (ratioFactorY < 4.8)
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(ratioFactorY - 3)*0.04;
        else if (ratioFactorY < 8)
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(ratioFactorY - 4.8)*0.02;
        else
            gs::scale = gs::scale - m*(1.2 - 1)*1.5 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(8 - 4.8)*0.02;
        
        m = gs::scScale;
        if (ratioFactorY < 1.2)
            gs::scScale = gs::scScale - m*(ratioFactorY - 1)*0.72;
        else if (ratioFactorY < 2)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(ratioFactorY - 1.2)*0.17;
        else if (ratioFactorY < 2.46)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(ratioFactorY - 2)*0.12;
        else if (ratioFactorY < 3)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(ratioFactorY - 2.46)*0.07;
        else if (ratioFactorY < 4.8)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(ratioFactorY - 3)*0.04;
        else if (ratioFactorY < 8)
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(ratioFactorY - 4.8)*0.02;
        else
            gs::scScale = gs::scScale - m*(1.2 - 1)*0.72 - m*(2 - 1.2)*0.17 - m*(2.46 - 2)*0.12 - m*(3 - 2.46)*0.07 - m*(4.8 - 3)*0.04 - m*(8 - 4.8)*0.02;
    }
    else if (ratioFactorX > 1)
    {
        float m = gs::scale;
        if (ratioFactorX < 1.9)
            gs::scale = gs::scale - m*(ratioFactorX - 1)*0.175;
        else if (ratioFactorX < 3)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(ratioFactorX - 1.9)*0.27;
        else if (ratioFactorX < 4.24)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(ratioFactorX - 3)*0.15;
        else if (ratioFactorX < 7)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(ratioFactorX - 4.24)*0.05;
        else if (ratioFactorX < 11)
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(ratioFactorX - 7)*0.02;
        else
            gs::scale = gs::scale - m*(1.9 - 1)*0.175 - m*(3 - 1.9)*0.27 - m*(4.24 - 3)*0.15 - m*(7 - 4.24)*0.05 - m*(11 - 7)*0.02;
        gs::scScale = gs::scale;
    }
    if (gs::scale < 0.18) gs::scale = 0.18;
    if (gs::scScale < 0.18) gs::scScale = 0.18;
}



void SetResolutionClass()
{
    if (gs::resolutionClassSetting == -1)
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
        
        if (width <= 640 && height <= 480) gs::resolutionClass = 0;
        else if (width <= 1024 && height <= 768) gs::resolutionClass = 1;
        else if (width <= 1920 && height <= 1080) gs::resolutionClass = 2;
        else gs::resolutionClass = 3;
        
#ifdef SFML_SYSTEM_IOS
        std::string device = iOSDeviceName();
        if (nss::Command(device, "iPhone"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 6));
            if (version <= 4) gs::resolutionClass = 0;
            else if (version <= 7) gs::resolutionClass = 1;
            else if (version == 8)
            {
                int underVersion = base::ConvertToInt(nss::ParseUntil(device, '\0', 8));
                if (underVersion == 4) gs::resolutionClass = 1;
                else gs::resolutionClass = 2;
            }
        }
        else if (nss::Command(device, "iPad"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 4));
            if (version <= 1) gs::resolutionClass = 0;
            else if (version <= 2) gs::resolutionClass = 1;
        }
        else if (nss::Command(device, "iPod"))
        {
            int version = base::ConvertToInt(nss::ParseUntil(device, ',', 4));
            if (version <= 3) gs::resolutionClass = 0;
            else if (version <= 4) gs::resolutionClass = 1;
            else if (version <= 6) gs::resolutionClass = 1;
        }
#endif
        
        cout << "main :: GlobalSettings :: Autocomputed ResolutionClass is " << gs::resolutionClass << "@x." << endl;
    }
    else
        gs::resolutionClass = gs::resolutionClassSetting;
}


#if defined(SFML_SYSTEM_ANDROID)
    char* androidFilesPath = new char[255];
    std::string documentsPath() { return std::string(androidFilesPath); }
    char* androidDataPath = new char[255];
    std::string externalDataPath() { return std::string(androidDataPath); }
#endif

#ifdef _MSC_VER
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main()
#endif
{
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "NovelSome", sf::Style::Default);
    gs::isParallaxEnabled = false;
#else
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width >= 1280 ? 1280 : sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height >= 880 ? 800 : sf::VideoMode::getDesktopMode().height - 80), "NovelSome");
#endif
    gs::window = &window;
    gs::width = window.getSize().x;
    gs::height = window.getSize().y;
    
    gs::relativeWidth = 1280;
    gs::relativeHeight = 800;
    CalculateScaleRatios(gs::width, gs::height);
    
#ifdef _WIN32
    if (sf::VideoMode::getDesktopMode().width == window.getSize().x)
        window.setPosition({- GlobalSettings::windowPositionOffset, window.getPosition().y});
#endif
    
    window.setFramerateLimit(gs::framerateLimit);
    window.setVerticalSyncEnabled(gs::isVerticalSyncEnabled);
    
    
#if defined(SFML_SYSTEM_ANDROID)
    const char* androidFilesPath1 = sf::getNativeActivity()->internalDataPath;
    int i; for (i = 0; androidFilesPath1[i] != '\0'; ++i) androidFilesPath[i] = androidFilesPath1[i];
    androidFilesPath[i++] = '/'; androidFilesPath[i] = '\0';
    LOGE("InternalDataPath: %s", androidFilesPath);
    
    const char* androidDataPath1 = sf::getNativeActivity()->externalDataPath;
    for (i = 0; androidDataPath1[i] != '\0'; ++i) androidDataPath[i] = androidDataPath1[i];
    androidDataPath[i++] = '/'; androidDataPath[i] = '\0';
    LOGE("ExternalDataPath: %s", androidDataPath);
    
    
    if (!base::FileExists(base::utf16(std::string(androidDataPath) + "Built-in Novels/")))
        base::CreateDirectory(base::utf16(std::string(androidDataPath) + "Built-in Novels/"));
    
    ANativeActivity* activity = sf::getNativeActivity();
    JNIEnv* lJNIEnv = activity->env;
    (activity->vm)->AttachCurrentThread(&lJNIEnv, NULL);
    
    // Retrieve the NativeActivity
    jobject ObjectNativeActivity = activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(ObjectNativeActivity);
    
    // Retrieve the ActivityInfo
    jmethodID MethodGetAssetManager = lJNIEnv->GetMethodID(ClassNativeActivity, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject ObjectAssetManager = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetAssetManager);
    
    AAssetManager* mgr = AAssetManager_fromJava(lJNIEnv, ObjectAssetManager);
    
    bool updateTheNovel{ true };
    std::ifstream fileVersion;
    fileVersion.open(std::string(androidDataPath) + "Built-in Novels/Bundle/version.novelversion");
    if (fileVersion.is_open())
    {
        std::string line; std::getline(fileVersion, line);
        int versionOld = std::atoi(line.c_str()); LOGE("Version OLD: %i", versionOld);
        AAsset* assetVersion = AAssetManager_open(mgr, "Novels/Bundle/version.novelversion", AASSET_MODE_BUFFER);
        if (assetVersion != NULL)
        {
            size_t fileLength = AAsset_getLength(assetVersion);
            char* fileContent = new char[fileLength+1];
            AAsset_read(assetVersion, fileContent, fileLength);
            fileContent[fileLength] = '\0';
            
            int versionNew = std::atoi(fileContent); LOGE("Version NEW: %i", versionNew);
            updateTheNovel = (versionOld != versionNew);
            
            AAsset_close(assetVersion);
        }
    }
    
    if (updateTheNovel)
    {
        std::vector<std::string> directories = { "/Bundle", "/Bundle/Backgrounds", "/Bundle/Characters", "/Bundle/Music", "/Bundle/Sounds", "/Example", base::utf8(L"/Чувственное познание") };
        for (auto& dir : directories)
        {
            if (!base::FileExists(base::utf16(std::string(androidDataPath) + "Built-in Novels" + dir)))
                base::CreateDirectory(base::utf16(std::string(androidDataPath) + "Built-in Novels" + dir));
            
            AAssetDir* assetDir = AAssetManager_openDir(mgr, ("Novels" + dir).c_str());
            const char* filename = (const char*)NULL;
            std::string fileto = std::string(std::string(androidDataPath) + "Built-in Novels" + dir + "/");
            
            while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
            {
                AAsset* asset = AAssetManager_open(mgr, ("Novels" + dir + "/" + std::string(filename)).c_str(), AASSET_MODE_BUFFER);
                if (asset != NULL)
                {
                    const char* filenameto = (fileto + std::string(filename)).c_str();
                    LOGE("filenameto: %s", filenameto);
                    
                    size_t fileLength = AAsset_getLength(asset);
                    char* fileContent = new char[fileLength+1];
                    AAsset_read(asset, fileContent, fileLength);
                    fileContent[fileLength] = '\0';
                    
                    FILE* out = fopen(filenameto, "w");
                    fwrite(fileContent, sizeof(char), fileLength+1, out);
                    fclose(out);
                    
                    delete [] fileContent;
                    AAsset_close(asset);
                }
            }
            AAssetDir_close(assetDir);
        }
    }
    
    (activity->vm)->DetachCurrentThread();
#endif
    
    gs::Load(L"UserDefined.nsoptions");
    SetResolutionClass();
    
    EntitySystem system;
    
    ///----------------------------------------------------------
    /// \brief Entity to hold Neko Ninja components
    ///
    /// Entity holds components like Neko Ninja controller, its background etc.
    ///
    ///----------------------------------------------------------
    /*Entity* Chocola = system.AddEntity();
    {
        Chocola->AddComponent<NekoNinja::SceneBackground>();
        auto* control = Chocola->AddComponent<NekoNinja::Controller>();
        Chocola->AddComponent<NekoNinja::GUIOverlay>(control);
    }*/
    
    ///----------------------------------------------------------
    /// \brief Entity to hold novel and stuff it depends on
    ///
    /// Entity holds the novel itself in order to have some ierarchy of entities.
    /// It also may hold some libraries etc that novel depends on.
    ///
    ///----------------------------------------------------------
    Entity* Elizabeth = system.AddEntity();
    {
/*#ifdef SFML_SYSTEM_ANDROID
        LOGE("Novel loading path: %s", (externalDataPath() + "Built-in Novels/Bundle/scen.nsdat").c_str());
        entity->AddComponent<NovelComponents::Novel>(externalDataPath() + "Built-in Novels/Bundle/scen.nsdat");
#else
        entity->AddComponent<NovelComponents::Novel>("Novels/Bundle/scen.nsdat");
#endif*/
        Elizabeth->AddComponent<NSMenuComponents::MainMenu>();
    }
    
    ///----------------------------------------------------------
    /// \brief Entity to hold essential components
    ///
    /// Entity holds components like always-on debug UI layer, system's components and other essential stuff.
    /// It also may hold components like NovelComponent that runs the novel, cuz it has to be essential component.
    ///
    ///----------------------------------------------------------
    Entity* Shimakaze = system.AddEntity();
    {
        Shimakaze->AddComponent<EssentialComponents::DebugComponent>("Update 0 build 19");
#if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
        Shimakaze->AddComponent<EssentialComponents::GyroscopeParallax>();
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
                    
                case sf::Event::KeyPressed:
                    system.PollEvent(event);
                    switch (event.key.code)
                    {
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
                                Elizabeth->AddComponent<NovelComponents::Novel>(L"Novels/Bundle/scen.nsdat");
                            }
                            Shimakaze = system.AddEntity();
                            {
                                Shimakaze->AddComponent<EssentialComponents::DebugComponent>("Update 0 build 18");
#ifdef SFML_SYSTEM_IOS
                                Shimakaze->AddComponent<EssentialComponents::GyroscopeParallax>();
#endif
#ifdef SFML_SYSTEM_ANDROID
                                Shimakaze->AddComponent<EssentialComponents::GyroscopeParallax>();
#endif
                            }
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
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
