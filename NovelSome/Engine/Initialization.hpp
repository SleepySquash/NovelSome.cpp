//
//  Initialization.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 01/07/2019.
//  Copyright © 2019 Melancholy Hill. All rights reserved.
//

#ifndef Initialization_hpp
#define Initialization_hpp

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

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "../Essentials/ResourcePath.hpp"
#include "EntitySystem.hpp"
#include "Settings.hpp"

#include <SFML/Config.hpp>

using std::cin;
using std::cout;
using std::endl;

using namespace ns;

void DrawInParallel(sf::RenderWindow* window, EntitySystem* system);
void CalculateScaleRatios(unsigned int width, unsigned int height);
void SetResolutionClass();

void ExecuteInitalization();
void AndroidInitalization();

#endif /* Initialization_hpp */
