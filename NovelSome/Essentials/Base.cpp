//
//  Base.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melanholy Hill. All rights reserved.
//

#include "Base.hpp"


namespace ns
{
    namespace base
    {
        #ifdef _WIN32
            #include <direct.h>
            std::wstring GetCurrentWorkingDir( void )
            {
                wchar_t buff[FILENAME_MAX];
                _wgetcwd( buff, FILENAME_MAX );
                std::wstring current_working_dir(buff);
                return current_working_dir+L'\\';
            }
        #else
            #include <unistd.h>
            std::wstring GetCurrentWorkingDir( void )
            {
                char buff[FILENAME_MAX];
                getcwd( buff, FILENAME_MAX );
                std::string current_working_dir(buff);
                
                //TODO: change the way it converts
                std::wstring wcurrent_working_dir(current_working_dir.begin(), current_working_dir.end());
                
                return wcurrent_working_dir+L'\\';
            }
        #endif

        
        
        //TODO: Documentation
        std::wstring GetFolderPath(std::wstring path)
        {
            std::wstring folder = L"";
            
            int pos{ -1 };
            if (path.length() != 0)
                for (unsigned int i = path.length() - 1; i >= 0 && pos == -1; i--)
                    if (path[i] == '/' || path[i] == '\\')
                        pos = i;
            
            for (int i = 0; i <= pos; i++)
                folder += path[i];
            
            return folder;
        }
        
        
        
        std::wstring GetExtentionFromString(std::wstring filename)
        {
            for (int i = filename.size(); i >= 0; --i)
                if (filename[i] == L'.')
                {
                    if (i != filename.size())
                        filename.erase(0,i);
                    return filename;
                }
            return L"";
        }
        
        
        
        std::wstring GetStringWithNoExtention(std::wstring filename)
        {
            std::wstring returned = L"";
            std::wstring extention = GetExtentionFromString(filename);
            if (extention.length() != 0)
                for (int i = 0; i < filename.length() - extention.length(); i++)
                    returned += filename[i];
            if (returned != L"")
                return returned;
            return filename;
        }
        
        
        
        int64_t power(int num, int by)
        {
            if (by == 0)
                return 1;
            
            int64_t res = num;
            for (int i = 1; i < by; i++)
                res *= num;
            return res;
        }
        


        float ConvertToFloat(std::wstring str)
        {
            float flp = {0.f};
            bool after{false}, noerase{false};
            while (str.size()>0) {
                switch(str[0])
                {
                    case L'0': if (!after) {flp *= 10.f;} break;
                    case L'1': if (!after) {flp *= 10.f; flp += 1.f;} else {flp += 1.f/power(10,str.size());} break;
                    case L'2': if (!after) {flp *= 10.f; flp += 2.f;} else {flp += 2.f/power(10,str.size());} break;
                    case L'3': if (!after) {flp *= 10.f; flp += 3.f;} else {flp += 3.f/power(10,str.size());} break;
                    case L'4': if (!after) {flp *= 10.f; flp += 4.f;} else {flp += 4.f/power(10,str.size());} break;
                    case L'5': if (!after) {flp *= 10.f; flp += 5.f;} else {flp += 5.f/power(10,str.size());} break;
                    case L'6': if (!after) {flp *= 10.f; flp += 6.f;} else {flp += 6.f/power(10,str.size());} break;
                    case L'7': if (!after) {flp *= 10.f; flp += 7.f;} else {flp += 7.f/power(10,str.size());} break;
                    case L'8': if (!after) {flp *= 10.f; flp += 8.f;} else {flp += 8.f/power(10,str.size());} break;
                    case L'9': if (!after) {flp *= 10.f; flp += 9.f;} else {flp += 9.f/power(10,str.size());} break;
                    case L'.': if (!after) {after = true; str.erase(0,1); noerase=true; reverse(str.begin(),str.end());} break;
                    default: if (after) {flp *= 10.f;} break;
                }
                if (!noerase) {str.erase(0,1);} else {noerase = false;}
            } return flp;
        }



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
    }
}
