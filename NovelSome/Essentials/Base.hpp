//
//  Base.hpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#ifndef BaseFunctions_hpp
#define BaseFunctions_hpp

#include <iostream>

/*Для кроссплатформенности между консолью (cmd.exe) Windows и терминалом (Terminal.app) UNIX*/
void systempause();
void systemclear();

//MACROS
// _WIN32 - both 32-bit and 64-bit
// _WIN64
// __APPLE__
// TARGET_OS_IPHONE
// TARGET_OS_MAC
// __linux__
// __unix__

/*Объявляем вспомогательные функции*/
char* InputString();
int GetLength(const char*);

#endif /* BaseFunctions_hpp */
