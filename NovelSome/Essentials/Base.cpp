//
//  Base.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 07/06/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "Base.hpp"


namespace ns
{
    namespace base
    {
        std::string utf8(const std::wstring& str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.to_bytes(str);
        }
        std::wstring utf16(const std::string& str)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            return converter.from_bytes(str);
        }
        std::string utf8(const wchar_t& wchar) { std::wstring str = L""; str[0] = wchar; return utf8(str); }
        std::wstring utf16(const char& wchar) { std::string str = ""; str[0] = wchar; return utf16(str); }
        
        
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
        bool FileExists(const std::wstring& path)
        {
#ifdef __ANDROID__
            if (path.length() != 0)
            {
                if (path[0] != L'/')
                {
                    ANativeActivity* activity = sf::getNativeActivity();
                    JNIEnv* lJNIEnv = NULL;
                    (activity->vm)->AttachCurrentThread(&lJNIEnv, 0);
                    
                    // Retrieve the NativeActivity
                    jobject ObjectNativeActivity = activity->clazz;
                    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(ObjectNativeActivity);
                    
                    // Retrieve the ActivityInfo
                    jmethodID MethodGetAssetManager = lJNIEnv->GetMethodID(ClassNativeActivity, "getAssets", "()Landroid/content/res/AssetManager;");
                    jobject ObjectAssetManager = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetAssetManager);
                    
                    AAssetManager* mgr = AAssetManager_fromJava(lJNIEnv, ObjectAssetManager);
                    AAsset* asset = AAssetManager_open(mgr, (utf8(path)).c_str(), AASSET_MODE_UNKNOWN);
                    
                    (activity->vm)->DetachCurrentThread();
                    return (asset != nullptr);
                }
            }
#endif
            
#ifdef _WIN32
            return _waccess_s(path.c_str(), 0) == 0;
#else
            struct stat buffer;
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::string u8str = converter.to_bytes(path);
            return (stat(u8str.c_str(), &buffer) == 0);
#endif
        }
        /// Thanks to Maxim Suslov ( https://stackoverflow.com/users/3364871/maxim-suslov ) ///
        bool CreateDirectory(const std::wstring& path)
        {
#ifdef _WIN32
            int ret = _wmkdir(path.c_str());
#else
            std::string upath = utf8(path);
            mode_t mode = 0755;
            int ret = mkdir(upath.c_str(), mode);
#endif
            if (ret == 0) return true;
            switch (errno)
            {
                case ENOENT:
                    // parent didn't exist, try to create it
                {
                    int pos = path.find_last_of('/');
                    if (pos == std::string::npos)
#ifdef _WIN32
                        pos = path.find_last_of('\\');
                    if (pos == std::string::npos)
#endif
                        return false;
                    if (!CreateDirectory( path.substr(0, pos) ))
                        return false;
                }
                    // now, try to create again
#ifdef _WIN32
                    return (0 == _wmkdir(path.c_str()));
#else
                    return (0 == mkdir(upath.c_str(), mode));
#endif
                    
                case EEXIST: return FileExists(path); // done!
                default: return false;
            }
        }
        
        
        
        bool DoesFileExistWithResolutionClass(const std::wstring& currentPath)
        {
            if (!FileExists(currentPath))
            {
                std::wstring getExtention = GetExtentionFromString(currentPath);
                std::wstring woExtention = GetStringWithNoExtention(currentPath);
                
                if (FileExists(woExtention + L"@3x" + getExtention)) return true;
                else if (FileExists(woExtention + L"@2x" + getExtention)) return true;
                else if (FileExists(woExtention + L"@1x" + getExtention)) return true;
                else if (FileExists(woExtention + L"@0x" + getExtention)) return true;
            } else return true;
            
            return false;
        }
        
        
        
        
        //TODO: Documentation
        std::wstring GetFolderPath(const std::wstring& path)
        {
            unsigned long pos = path.find_last_of(L'/');
            if (pos != std::wstring::npos)
            {
                std::wstring p = path.substr(0, pos);
                if (p.length() && p[p.length() - 1] != L'/') p += L"/";
                return p;
            }
            else {
                pos = path.find_last_of(L'\\');
                if (pos != std::wstring::npos)
                {
                    std::wstring p = path.substr(0, pos);
                    if (p.length() && p[p.length() - 1] != L'\\') p += L"\\";
                    return p;
                }
            }
            return path;
        }
        std::wstring GetExtentionFromString(const std::wstring& filename)
        {
            unsigned long pos = filename.find_last_of(L'.');
            if (pos != std::wstring::npos)
            {
                std::wstring fp = filename.substr(pos, filename.length());
                return fp;
            }
            else return L"";
        }
        std::wstring GetStringWithNoExtention(const std::wstring& filename)
        {
            unsigned long pos = filename.find_last_of(L'.');
            if (pos != std::wstring::npos) return filename.substr(0, pos);
            else return filename;
        }
        
        
        
        size_t GetLengthWONewLinesAndSpaces(const std::wstring& string)
        {
            size_t size = 0, i = 0;
            while (string[i] != '\0') {
                if (string[i] != '\n' && string[i] != ' ') size++;
                ++i; }
            return size;
        }
        
        
        
        std::wstring LowercaseTheString(const std::wstring& str)
        {
            std::wstring result = L"";
            for (int i = 0; i < str.length(); i++)
            {
                switch (str[i])
                {
                    case L'Q': result += L'q'; break;
                    case L'W': result += L'w'; break;
                    case L'E': result += L'e'; break;
                    case L'R': result += L'r'; break;
                    case L'T': result += L't'; break;
                    case L'Y': result += L'y'; break;
                    case L'U': result += L'u'; break;
                    case L'I': result += L'i'; break;
                    case L'O': result += L'o'; break;
                    case L'P': result += L'p'; break;
                    case L'A': result += L'a'; break;
                    case L'S': result += L's'; break;
                    case L'D': result += L'd'; break;
                    case L'F': result += L'f'; break;
                    case L'G': result += L'g'; break;
                    case L'H': result += L'h'; break;
                    case L'J': result += L'j'; break;
                    case L'K': result += L'k'; break;
                    case L'L': result += L'l'; break;
                    case L'Z': result += L'z'; break;
                    case L'X': result += L'x'; break;
                    case L'C': result += L'c'; break;
                    case L'V': result += L'v'; break;
                    case L'B': result += L'b'; break;
                    case L'N': result += L'n'; break;
                    case L'M': result += L'm'; break;
                        
                    case L'Й': result += L'й'; break;
                    case L'Ц': result += L'ц'; break;
                    case L'У': result += L'у'; break;
                    case L'К': result += L'к'; break;
                    case L'Е': result += L'е'; break;
                    case L'Н': result += L'н'; break;
                    case L'Г': result += L'г'; break;
                    case L'Ш': result += L'ш'; break;
                    case L'Щ': result += L'щ'; break;
                    case L'З': result += L'з'; break;
                    case L'Ё': result += L'ё'; break;
                    case L'Х': result += L'х'; break;
                    case L'Ъ': result += L'ъ'; break;
                    case L'Ф': result += L'ф'; break;
                    case L'Ы': result += L'ы'; break;
                    case L'В': result += L'в'; break;
                    case L'А': result += L'а'; break;
                    case L'П': result += L'п'; break;
                    case L'Р': result += L'р'; break;
                    case L'О': result += L'о'; break;
                    case L'Л': result += L'л'; break;
                    case L'Д': result += L'д'; break;
                    case L'Ж': result += L'ж'; break;
                    case L'Э': result += L'э'; break;
                    case L'Я': result += L'я'; break;
                    case L'Ч': result += L'ч'; break;
                    case L'С': result += L'с'; break;
                    case L'М': result += L'м'; break;
                    case L'И': result += L'и'; break;
                    case L'Т': result += L'т'; break;
                    case L'Ь': result += L'ь'; break;
                    case L'Б': result += L'б'; break;
                    case L'Ю': result += L'ю'; break;
                        
                    default: result += str[i]; break;
                }
            }
            return result;
        }
        
        
        
        
        int atoi(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            return std::atoi(parsingString.c_str());
        }
        int ConvertToInt(const std::string& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            return std::atoi(parsingString.c_str());
        }
        float atof(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 44 || stringValue[i] == 46 || stringValue[i] == '-')
                    parsingString += (char)stringValue[i];
            
            return std::atof(parsingString.c_str());
        }
        bool atob(const std::wstring& stringValue)
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
    }
}
