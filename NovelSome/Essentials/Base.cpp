//
//  Base.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Base.hpp"

#ifdef _WIN32
void systemclear()
{
    system("cls");
}
void systempause()
{
    system("pause");
}
#else
void systempause()
{
    std::cout << '\n' << "-----Нажмите Enter, чтобы продолжить-----";
    do {
        /* nothing */
    } while (getchar() != '\n');
}
void systemclear()
{
    system("clear");
}
#endif

//----------------------------------------------------------
//char* InputString()
//Возвращает введённую пользователем строку с признаком завершения строки, память для хранения которой выделяется динамически.
//----------------------------------------------------------
char *InputString()
{
    char *str{ nullptr }; //Переменная для хранения введённой строки
    char wc;       //Временная переменная для хранения введённого пользователем символа.
    int strLen{ 0 };  //Временная переменная для хранения длины строки, учитываем сразу
    
    bool MemError{ false }; //Флаг ошибки выделения памяти
    
    while ((wc = getchar()) != '\n' && !MemError)
    {
        if (wc == 8)
        {
            if (strLen > 0)
            {
                strLen--;
                char *temp = (char*)realloc(str, (strLen + 1) * sizeof(char));
                if (temp != nullptr)
                {
                    str = temp;
                }
                else
                {
                    free(str); str = nullptr;
                    MemError = true;
                    std::cout << "Ошибка выделения памяти для хранения введённой строки." << std::endl;
                }
            }
        }
        else
        {
            /*Пока производится ввод пользователем, добавляем введённый символ к строке*/
            /*Добавляем на одну ячейку памяти больше, чтобы потом учесть признак завершения строки*/
            char *temp = (char*)realloc(str, (strLen + 2) * sizeof(char));
            if (temp != nullptr)
            {
                str = temp;
                str[strLen] = wc;
                strLen++;
            }
            else
            {
                free(str); str = nullptr;
                MemError = true;
                std::cout << "Ошибка выделения памяти для хранения введённой строки." << std::endl;
            }
        }
    }
    
    if (!MemError)
    {
        if (strLen == 0)
        {
            /*Если не произошло ошибки выделения памяти, но не было введено ни одного символа...*/
            str = (char*)malloc(sizeof(char));
        }
        /*Учитываем в конце признак завершения строки*/
        str[strLen] = '\0';
    }
    
    fflush(stdin); std::cin.clear();
    return str;
}

//----------------------------------------------------------
//int GetLength(char*)
//Возвращает длину строки, имеющую признак завершения строки, без учёта этого самого признака завершения строки.
//----------------------------------------------------------
int GetLength(const char *str)
{
    int i{ 0 };
    if (str != nullptr)
    {
        while (*(str + i) != '\0')
        {
            i++;
        }
    }
    return i;
}
