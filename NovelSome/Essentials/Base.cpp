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
            try {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.to_bytes(str);
            } catch(...) { throw; }
        }
        std::wstring utf16(const std::string& str)
        {
            try {
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                return converter.from_bytes(str);
            } catch(...) { throw; }
        }
        std::string utf8(const wchar_t& wchar) { std::wstring str = L""; str[0] = wchar; try { return utf8(str); } catch(...) { throw; } }
        std::wstring utf16(const char& wchar) { std::string str = ""; str[0] = wchar; try { return utf16(str); } catch(...) { throw; } }
        
        
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
            try
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
            } catch(std::exception& e) { cout << "Error :: FileExists :: " << e.what() << endl; return false; }
        }
        /// Thanks to Maxim Suslov ( https://stackoverflow.com/users/3364871/maxim-suslov ) ///
        bool CreateDirectory(const std::wstring& path)
        {
            try
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
                    case ENOENT: // parent didn't exist, try to create it
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
            } catch(...) { throw; }
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
            else
            {
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
        std::wstring GetFilenameWOProhibited(const std::wstring& filename)
        {
            if (!filename.length()) return filename;
            std::wstring wstr; std::size_t pos;
#ifdef _WIN32
            for (unsigned long i = 0; i < 254 && i < filename.length(); ++i)
                if (filename[i] >= 32 && filename[i] != 58 && filename[i] != 60 && filename[i] != 62 && filename[i] != 63 && filename[i] != 34 && filename[i] != 124 && filename[i] != 42 && filename[i] != 47 && filename[i] != 92) wstr += filename[i]; else wstr += L"_";
            std::wstring lowercase = LowercaseTheString(wstr);
            if (lowercase == L"con" || lowercase == L"prn" || lowercase == L"aux" || lowercase == L"prn" || lowercase == L"nul" || lowercase == L"com1" || lowercase == L"com2" || lowercase == L"com3" || lowercase == L"com4" || lowercase == L"com5" || lowercase == L"com6" || lowercase == L"com7" || lowercase == L"com8" || lowercase == L"com9" || lowercase == L"lpt1" || lowercase == L"lpt2" || lowercase == L"lpt3" || lowercase == L"lpt4" || lowercase == L"lpt5" || lowercase == L"lpt6" || lowercase == L"lpt7" || lowercase == L"lpt8" || lowercase == L"lpt9") wstr += L"q";
            else if (wstr[wstr.length() - 1] == L' ' || wstr[wstr.length() - 1] == L'.') wstr += L"q";
#else
            wstr = filename;
            pos = wstr.find('\0'); if (pos != std::string::npos) wstr = wstr.replace(pos, 1, L"_");
            pos = wstr.find('/'); if (pos != std::string::npos)  wstr = wstr.replace(pos, 1, L"_");
            pos = wstr.find('\\'); if (pos != std::string::npos) wstr = wstr.replace(pos, 1, L"_");
#endif
            return wstr;
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
        
        
        
        
        int ConvertToInt(const std::string& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            try { return std::atoi(parsingString.c_str()); } catch(...) { throw; }
        }
        int atoi(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            try { return std::atoi(parsingString.c_str()); } catch(...) { throw; }
        }
        long atol(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 45)
                    parsingString += (char)stringValue[i];
            
            try { return std::atol(parsingString.c_str()); } catch(...) { throw; }
        }
        float atof(const std::wstring& stringValue)
        {
            std::string parsingString = "";
            for (int i = 0; stringValue[i] != '\0'; i++)
                if ((stringValue[i] >= 48 && stringValue[i] <= 57) || stringValue[i] == 44 || stringValue[i] == 46 || stringValue[i] == '-')
                    parsingString += (char)stringValue[i];
            
            try { return std::atof(parsingString.c_str()); } catch(...) { throw; }
        }
        bool atob(const std::wstring& stringValue)
        {
            if (stringValue == L"true" || stringValue == L"True" || stringValue == L"T" || stringValue == L"истина" || stringValue == L"1") return true;
            else return false;
        }
        
        // double atan(double x) { return 0.79f*x - x*(std::fabs(x) - 1)*(0.2447 + 0.0663*std::fabs(x)); }
        double atan(double x) { return x * (0.9724 - 0.1919 * x * x); }
    }
}
