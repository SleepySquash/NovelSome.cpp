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
        
        
        
        bool FileExists(std::wstring path)
        {
#ifdef _WIN32
            return _waccess_s(path.c_str(), 0) == 0;
#else
            struct stat buffer;
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(path);
            return (stat (u8str.c_str(), &buffer) == 0);
#endif
        }
        
        
        
        std::wstring LowercaseTheString(const std::wstring& str)
        {
            std::wstring result = L"";
            for (int i = 0; i < str.length(); i++)
            {
                switch (str[i])
                {
                    case L'Q':
                        result += L'q';
                        break;
                    case L'W':
                        result += L'w';
                        break;
                    case L'E':
                        result += L'e';
                        break;
                    case L'R':
                        result += L'r';
                        break;
                    case L'T':
                        result += L't';
                        break;
                    case L'Y':
                        result += L'y';
                        break;
                    case L'U':
                        result += L'u';
                        break;
                    case L'I':
                        result += L'i';
                        break;
                    case L'O':
                        result += L'o';
                        break;
                    case L'P':
                        result += L'p';
                        break;
                    case L'A':
                        result += L'a';
                        break;
                    case L'S':
                        result += L's';
                        break;
                    case L'D':
                        result += L'd';
                        break;
                    case L'F':
                        result += L'f';
                        break;
                    case L'G':
                        result += L'g';
                        break;
                    case L'H':
                        result += L'h';
                        break;
                    case L'J':
                        result += L'j';
                        break;
                    case L'K':
                        result += L'k';
                        break;
                    case L'L':
                        result += L'l';
                        break;
                    case L'Z':
                        result += L'z';
                        break;
                    case L'X':
                        result += L'x';
                        break;
                    case L'C':
                        result += L'c';
                        break;
                    case L'V':
                        result += L'v';
                        break;
                    case L'B':
                        result += L'b';
                        break;
                    case L'N':
                        result += L'n';
                        break;
                    case L'M':
                        result += L'm';
                        break;
                        
                    case L'Й':
                        result += L'й';
                        break;
                    case L'Ц':
                        result += L'ц';
                        break;
                    case L'У':
                        result += L'у';
                        break;
                    case L'К':
                        result += L'к';
                        break;
                    case L'Е':
                        result += L'е';
                        break;
                    case L'Н':
                        result += L'н';
                        break;
                    case L'Г':
                        result += L'г';
                        break;
                    case L'Ш':
                        result += L'ш';
                        break;
                    case L'Щ':
                        result += L'щ';
                        break;
                    case L'З':
                        result += L'з';
                        break;
                    case L'Ё':
                        result += L'ё';
                        break;
                    case L'Х':
                        result += L'х';
                        break;
                    case L'Ъ':
                        result += L'ъ';
                        break;
                    case L'Ф':
                        result += L'ф';
                        break;
                    case L'Ы':
                        result += L'ы';
                        break;
                    case L'В':
                        result += L'в';
                        break;
                    case L'А':
                        result += L'а';
                        break;
                    case L'П':
                        result += L'п';
                        break;
                    case L'Р':
                        result += L'р';
                        break;
                    case L'О':
                        result += L'о';
                        break;
                    case L'Л':
                        result += L'л';
                        break;
                    case L'Д':
                        result += L'д';
                        break;
                    case L'Ж':
                        result += L'ж';
                        break;
                    case L'Э':
                        result += L'э';
                        break;
                    case L'Я':
                        result += L'я';
                        break;
                    case L'Ч':
                        result += L'ч';
                        break;
                    case L'С':
                        result += L'с';
                        break;
                    case L'М':
                        result += L'м';
                        break;
                    case L'И':
                        result += L'и';
                        break;
                    case L'Т':
                        result += L'т';
                        break;
                    case L'Ь':
                        result += L'ь';
                        break;
                    case L'Б':
                        result += L'б';
                        break;
                    case L'Ю':
                        result += L'ю';
                        break;
                        
                    default:
                        result += str[i];
                        break;
                }
            }
            return result;
        }
        
        
        
        std::string ConvertToUTF8(const std::wstring& str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.to_bytes(str);
        }
        std::string ConvertToUTF8(const wchar_t& wchar)
        {
            std::wstring str = L"";
            str[0] = wchar;
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.to_bytes(str);
        }
        std::wstring ConvertFromUTF8(const std::string& str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.from_bytes(str);
        }
        std::wstring ConvertFromUTF8(const char& wchar)
        {
            std::string str = "";
            str[0] = wchar;
            
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.from_bytes(str);
        }
        int ConvertToInt(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            return std::atoi(parsingString.c_str());
        }
        float ConvertToFloat(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 44 || stringValue[i] == 46)
                    parsingString += (char)stringValue[i];
            
            return std::atof(parsingString.c_str());
        }
        bool ConvertToBool(const std::wstring& stringValue)
        {
            if (stringValue == L"true" || stringValue == L"True" || stringValue == L"T" || stringValue == L"истина" || stringValue == L"1")
                return true;
            else
                return false;
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
        


        float obsoleteConvertToFloat(std::wstring str)
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
