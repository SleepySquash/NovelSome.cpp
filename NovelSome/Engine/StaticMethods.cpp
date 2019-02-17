//
//  StaticMethods.cpp
//  NovelSome
//
//  Created by Никита Исаенко on 26/08/2018.
//  Copyright © 2018 Melancholy Hill. All rights reserved.
//

#include "StaticMethods.hpp"

namespace ns
{
    sf::Font* FontCollector::GetFont(const std::wstring& fontName)
    {
        if (fonts.find(fontName) != fonts.end()) return fonts.at(fontName);
        else if (fonts.find(L"Data/Fonts/" + fontName) != fonts.end()) return fonts.at(L"Data/Fonts/" + fontName);
        else
        {
            std::wstring fullPath = fontName;
            if (!base::FileExists(fontName)) fullPath = base::utf16(resourcePath()) + L"Data/Fonts/" + fontName;
            if (base::FileExists(fullPath))
            {
                sf::Font* font = new sf::Font();
                bool fontLoaded{ false };
    #ifdef _WIN32
                //TODO: Fix memory leak
                std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory, filesize);
                    ifStream.close();
                    
                    fontLoaded = font->loadFromMemory(fileInMemory, filesize);
                }
    #else
                std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                std::string u8str = converter.to_bytes(fullPath);
                if (!(fontLoaded = font->loadFromFile(u8str)))
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
    #endif
                
                if (fontLoaded)
                {
                    fonts.emplace(L"Data/Fonts/" + fontName, font);
                    return font;
                }
            }
            return nullptr;
        }
    }
    void FontCollector::FreeFonts()
    {
        for (const auto& key : fonts)
            if (key.second != nullptr)
                delete key.second;
        fonts.clear();
    }
    std::unordered_map<std::wstring, sf::Font*> FontCollector::fonts = { {L"", nullptr} };
    
    
    
    
    
    
    
    bool Collector_UseFullPath = false;
    //////////////////////////////////////////
    ///
    /// Modes:
    ///     1 - background
    ///     2 - sprites/images
    ///     3 - sounds
    ///     4 - music/ambient
    ///
    //////////////////////////////////////////
    std::wstring PathWithFolderDetermination(const std::wstring& imageName, unsigned int mode)
    {
        std::wstring fullPath = imageName;
        if (mode != 0 && !base::DoesFileExistWithResolutionClass(fullPath))
        {
            if (!base::DoesFileExistWithResolutionClass(base::utf16(resourcePath()) + fullPath))
            {
                bool found{ false };
                std::wstring onlyFolder = base::GetFolderPath(fullPath);
                std::wstring onlyFileName = L"";
                for (unsigned long i = onlyFolder.length(); i < fullPath.length(); i++) onlyFileName += fullPath[i];
                
                for (int j = 0; j <= 1 && !found; ++j)
                {
                    if (j == 1) {onlyFolder = base::utf16(resourcePath()) + onlyFolder; fullPath = base::utf16(resourcePath()) + fullPath;}
                    switch (mode)
                    {
                        case 1:
                            for (int i = 0; i <= 1 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0: currentPath = onlyFolder + L"backgrounds/" + onlyFileName; break;
                                    case 1: currentPath = onlyFolder + L"Backgrounds/" + onlyFileName; break;
                                    default: break;
                                }
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath)))) fullPath = currentPath;
                            }
                            break;
                            
                        case 2:
                            for (int i = 0; i <= 3 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0: currentPath = onlyFolder + L"images/" + onlyFileName; break;
                                    case 1: currentPath = onlyFolder + L"sprites/" + onlyFileName; break;
                                    case 2: currentPath = onlyFolder + L"Images/" + onlyFileName; break;
                                    case 3: currentPath = onlyFolder + L"Sprites/" + onlyFileName; break;
                                    default: break;
                                }
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath)))) fullPath = currentPath;
                            }
                            break;
                        case 3:
                            for (int i = 0; i <= 5 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0: currentPath = onlyFolder + L"sounds/" + onlyFileName; break;
                                    case 1: currentPath = onlyFolder + L"sound/" + onlyFileName; break;
                                    case 2: currentPath = onlyFolder + L"audio/" + onlyFileName; break;
                                    case 3: currentPath = onlyFolder + L"Sounds/" + onlyFileName; break;
                                    case 4: currentPath = onlyFolder + L"Sound/" + onlyFileName; break;
                                    case 5: currentPath = onlyFolder + L"Audio/" + onlyFileName; break;
                                    default: break;
                                }
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath)))) fullPath = currentPath;
                            }
                            break;
                        case 4:
                            for (int i = 0; i <= 5 && !found; i++)
                            {
                                std::wstring currentPath;
                                switch (i)
                                {
                                    case 0: currentPath = onlyFolder + L"music/" + onlyFileName; break;
                                    case 1: currentPath = onlyFolder + L"ambient/" + onlyFileName; break;
                                    case 2: currentPath = onlyFolder + L"audio/" + onlyFileName; break;
                                    case 3: currentPath = onlyFolder + L"Music/" + onlyFileName; break;
                                    case 4: currentPath = onlyFolder + L"Ambient/" + onlyFileName; break;
                                    case 5: currentPath = onlyFolder + L"Audio/" + onlyFileName; break;
                                    default: break;
                                }
                                if (((found = base::DoesFileExistWithResolutionClass(currentPath)))) fullPath = currentPath;
                            }
                            break;
                        default: break;
                    }
                }
            } else fullPath = base::utf16(resourcePath()) + fullPath;
        }
        
        return fullPath;
    }
    std::wstring PathWithResolutionDetermination(const std::wstring& imageName, unsigned int mode)
    {
        std::wstring fullPath = PathWithFolderDetermination(imageName, mode);
        if (gs::isResolutionClassEnabled)
        {
            std::wstring getExtention = base::GetExtentionFromString(fullPath);
            std::wstring woExtention = base::GetStringWithNoExtention(fullPath);
            
            switch (ns::gs::resolutionClass)
            {
                case -1: break;
                case 0:
                    if (base::FileExists(woExtention + L"@0x" + getExtention)) fullPath = woExtention + L"@0x" + getExtention;
                    else if (base::FileExists(woExtention + L"@1x" + getExtention)) fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(woExtention + L"@2x" + getExtention)) fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(woExtention + L"@3x" + getExtention)) fullPath = woExtention + L"@3x" + getExtention;
                    break;
                    
                case 1:
                    if (base::FileExists(woExtention + L"@1x" + getExtention)) fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(woExtention + L"@2x" + getExtention)) fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(woExtention + L"@0x" + getExtention)) fullPath = woExtention + L"@0x" + getExtention;
                    else if (base::FileExists(woExtention + L"@3x" + getExtention)) fullPath = woExtention + L"@3x" + getExtention;
                    break;
                    
                case 2:
                    if (base::FileExists(woExtention + L"@2x" + getExtention)) fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(woExtention + L"@1x" + getExtention)) fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(woExtention + L"@3x" + getExtention)) fullPath = woExtention + L"@3x" + getExtention;
                    else if (base::FileExists(woExtention + L"@0x" + getExtention)) fullPath = woExtention + L"@0x" + getExtention;
                    break;
                    
                case 3:
                    if (base::FileExists(woExtention + L"@3x" + getExtention)) fullPath = woExtention + L"@3x" + getExtention;
                    else if (base::FileExists(woExtention + L"@2x" + getExtention)) fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(woExtention + L"@1x" + getExtention)) fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(woExtention + L"@0x" + getExtention)) fullPath = woExtention + L"@0x" + getExtention;
                    break;
                    
                default:
                    if (base::FileExists(woExtention + L"@" + std::to_wstring(ns::gs::resolutionClass) + L"x" + getExtention)) fullPath = woExtention + L"@" + std::to_wstring(ns::gs::resolutionClass) + L"x" + getExtention;
                    else if (base::FileExists(woExtention + L"@3x" + getExtention)) fullPath = woExtention + L"@3x" + getExtention;
                    else if (base::FileExists(woExtention + L"@2x" + getExtention)) fullPath = woExtention + L"@2x" + getExtention;
                    else if (base::FileExists(woExtention + L"@1x" + getExtention)) fullPath = woExtention + L"@1x" + getExtention;
                    else if (base::FileExists(woExtention + L"@0x" + getExtention)) fullPath = woExtention + L"@0x" + getExtention;
                    break;
            }
        }
        else
        {
            if (!base::FileExists(fullPath))
            {
                std::wstring getExtention = base::GetExtentionFromString(fullPath);
                std::wstring woExtention = base::GetStringWithNoExtention(fullPath);
                
                if (base::FileExists(sf::String(woExtention + L"@3x" + getExtention))) fullPath = woExtention + L"@3x" + getExtention;
                else if (base::FileExists(sf::String(woExtention + L"@2x" + getExtention))) fullPath = woExtention + L"@2x" + getExtention;
                else if (base::FileExists(sf::String(woExtention + L"@1x" + getExtention))) fullPath = woExtention + L"@1x" + getExtention;
                else if (base::FileExists(sf::String(woExtention + L"@0x" + getExtention))) fullPath = woExtention + L"@0x" + getExtention;
            }
        }
        
        return fullPath;
    }
    sf::Image* ImageCollector::LoadImage(const std::wstring& imageName, unsigned int mode)
    {
        if (images.find(imageName) != images.end())
        {
            if (threads.find(imageName) != threads.end())
            {
                if (threads[imageName].joinable()) threads[imageName].join();
                threads.erase(imageName);
            }
            ++images[imageName].usage;
            return images[imageName].image;
        }
        else
        {
            if (threads.find(imageName) != threads.end())
            {
                threads[imageName].join();
                threads.erase(imageName);
                if (images.find(imageName) != images.end() && images[imageName].image != nullptr)
                {
                    ++images[imageName].usage;
                    return images[imageName].image;
                }
                
                return nullptr;
            }
            
            std::wstring fullPath = PathWithResolutionDetermination(imageName, mode);
            
            if (base::FileExists(fullPath))
            {
                sf::Image* image = new sf::Image();
                bool imageLoaded{ false };
#ifdef _WIN32
                std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory, filesize);
                    ifStream.close();
                    
                    imageLoaded = image->loadFromMemory(fileInMemory, filesize);
                    delete[] fileInMemory;
                }
#else
#ifdef SFML_SYSTEM_ANDROID
                if (fullPath[0] == L'/')
                {
                    std::ifstream ifStream(base::utf8(fullPath), std::ios::binary | std::ios::ate);
                    if (!ifStream.is_open())
                        std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
                    else
                    {
                        auto filesize = ifStream.tellg();
                        char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                        ifStream.seekg(0, std::ios::beg);
                        ifStream.read(fileInMemory, filesize);
                        ifStream.close();
                        
                        imageLoaded = image->loadFromMemory(fileInMemory, filesize);
                        delete[] fileInMemory;
                    }
                }
                else
                {
#endif
                    if (!(imageLoaded = image->loadFromFile(base::utf8(fullPath))))
                        std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#ifdef SFML_SYSTEM_ANDROID
                }
#endif
#endif
                
                if (imageLoaded)
                {
                    images.emplace(imageName, ImageCollectorObject(image, nullptr, 1));
                    return image;
                }
            }
            return nullptr;
        }
    }
    void ImageCollector::ThreadImage(std::wstring imageName, unsigned int mode, bool destroyable, bool loadTexture)
    {
        std::wstring fullPath = PathWithResolutionDetermination(imageName, mode);
        
        if (base::FileExists(fullPath))
        {
            sf::Image* image = new sf::Image();
            bool imageLoaded{ false };
#ifdef _WIN32
            std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory, filesize);
                ifStream.close();
                
                imageLoaded = image->loadFromMemory(fileInMemory, filesize);
                delete[] fileInMemory;
            }
#else
#ifdef SFML_SYSTEM_ANDROID
            if (fullPath[0] == L'/')
            {
                std::ifstream ifStream(base::utf8(fullPath), std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory, filesize);
                    ifStream.close();
                    
                    imageLoaded = image->loadFromMemory(fileInMemory, filesize);
                    delete[] fileInMemory;
                }
            }
            else
            {
#endif
                if (!(imageLoaded = image->loadFromFile(base::utf8(fullPath))))
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#ifdef SFML_SYSTEM_ANDROID
            }
#endif
#endif
            
            if (imageLoaded)
            {
                images.emplace(imageName, ImageCollectorObject(image, nullptr, 0, destroyable));
                if (loadTexture) { LoadTexture(imageName); images[imageName].usage = 0; }
                //threads.erase(imageName);
            }
        }
    }
    void ImageCollector::PreloadImage(const std::wstring& imageName, unsigned int mode, bool destroyable)
    {
        //cout << "[][][][][][] PRELOADING: " << base::utf8(imageName) << " [][][][][][]" << endl;
        if (images.find(imageName) == images.end())
            if (threads.find(imageName) == threads.end())
                threads.emplace(imageName, std::thread(&ThreadImage, imageName, mode, destroyable, false));
    }
    sf::Texture* ImageCollector::LoadTexture(const std::wstring& imageName, unsigned int mode)
    {
        bool usagePlusPlus{ true };
        if (images.find(imageName) == images.end()) { ic::LoadImage(imageName, mode); usagePlusPlus = false; }
        if (images.find(imageName) != images.end())
        {
            if (images[imageName].image != nullptr)
            {
                if (usagePlusPlus) ++images[imageName].usage;
                if (images[imageName].texture != nullptr) return images[imageName].texture;
                
                images[imageName].texture = new sf::Texture();
                
                sf::Image* imagePtr = images[imageName].image;
                if (imagePtr->getSize().x > sf::Texture::getMaximumSize() || imagePtr->getSize().y > sf::Texture::getMaximumSize())
                    images[imageName].texture->loadFromImage(*imagePtr, sf::IntRect(0, 0, imagePtr->getSize().x > sf::Texture::getMaximumSize() ? sf::Texture::getMaximumSize() : imagePtr->getSize().x, imagePtr->getSize().y > sf::Texture::getMaximumSize() ? sf::Texture::getMaximumSize() : imagePtr->getSize().y));
                else images[imageName].texture->loadFromImage(*images[imageName].image);
                if (images[imageName].texture) images[imageName].texture->setSmooth(true);
                return images[imageName].texture;
            }
        }
        return nullptr;
    }
    void ImageCollector::PreloadTexture(const std::wstring& imageName, unsigned int mode, bool destroyable)
    {
        if (images.find(imageName) == images.end())
            if (threads.find(imageName) == threads.end())
                threads.emplace(imageName, std::thread(&ThreadImage, imageName, mode, destroyable, true));
    }
    sf::Texture* ImageCollector::RequestHigherTexture(const std::wstring& imageName, unsigned int mode)
    {
        if (images.find(imageName) == images.end())
        {
            if (threads.find(imageName) == threads.end())
            {
                if (gs::isResolutionClassEnabled && gs::resolutionClass == 0) return LoadTexture(imageName, mode);
                
                std::wstring fullPath = PathWithFolderDetermination(imageName, mode);
                std::wstring getExtention = base::GetExtentionFromString(fullPath);
                std::wstring woExtention = base::GetStringWithNoExtention(fullPath);
                if (base::FileExists(woExtention + L"@0x" + getExtention))
                {
                    std::wstring imageExtention = base::GetExtentionFromString(imageName);
                    std::wstring imageWoExtention = base::GetStringWithNoExtention(imageName);
                    sf::Texture* texture = LoadTexture(imageWoExtention + L"@0x" + imageExtention, mode);
                    if (texture)
                    {
                        // Start loading with the help of thread
                        
                        return texture;
                    }
                    else return LoadTexture(imageName, mode);
                }
                else return LoadTexture(imageName, mode);
            }
            else
            {
                threads[imageName].join();
                threads.erase(imageName);
                return LoadTexture(imageName, mode);
            }
        }
        return LoadTexture(imageName, mode);
    }
    sf::Image* ImageCollector::FindImage(const std::wstring& imageName)
    {
        if (images.find(imageName) != images.end()) return images[imageName].image;
        return nullptr;
    }
    sf::Texture* ImageCollector::FindTexture(const std::wstring& imageName)
    {
        if (images.find(imageName) != images.end()) return images[imageName].texture;
        return nullptr;
    }
    void ImageCollector::SetDestroyable(std::wstring imageName, bool destroyable)
    {
        if (images.find(imageName) != images.end())
        {
            images[imageName].destroyable = destroyable;
            if (destroyable) if (images[imageName].usage <= 0) ic::EraseImage(imageName);
        }
    }
    void ImageCollector::DeleteImage(const std::wstring& imageName)
    {
        if (images.find(imageName) != images.end())
        {
            --images[imageName].usage; //cout << "[][][][][][] images[" << base::utf8(imageName) << "].usage: " << images[imageName].usage << " [][][][][][]" << endl;
            if (images[imageName].usage <= 0 && images[imageName].destroyable)
                ic::EraseImage(imageName);
        }
    }
    void ImageCollector::EraseImage(const std::wstring& imageName)
    {
        if (images.find(imageName) != images.end())
        {
            if (threads.find(imageName) != threads.end())
            {
                if (threads[imageName].joinable()) threads[imageName].join();
                threads.erase(imageName);
            }
            
            sf::Image* image = images[imageName].image;
            if (image != nullptr) delete image;
            sf::Texture* texture = images[imageName].texture;
            if (texture != nullptr) delete texture;
            
            images.erase(imageName);
        }
    }
    void ImageCollector::FreeImages()
    {
        for (const auto& key : images)
        {
            if (key.second.image != nullptr) delete key.second.image;
            if (key.second.texture != nullptr) delete key.second.texture;
        }
        images.clear();
    }
    std::unordered_map<std::wstring, ImageCollectorObject> ic::images;
    std::unordered_map<std::wstring, std::thread> ic::threads;
    icThreadsJoiner ic::threadsJoiner;
    
    
    
    
    sf::SoundBuffer* SoundCollector::LoadSound(const std::wstring& soundName, unsigned int mode)
    {
        if (sounds.find(soundName) != sounds.end())
        {
            if (threads.find(soundName) != threads.end())
            {
                if (threads[soundName].joinable()) threads[soundName].join();
                threads.erase(soundName);
            }
            ++sounds[soundName].usage;
            return sounds[soundName].sound;
        }
        else
        {
            if (threads.find(soundName) != threads.end())
            {
                threads[soundName].join();
                threads.erase(soundName);
                if (sounds.find(soundName) != sounds.end() && sounds[soundName].sound != nullptr)
                {
                    ++sounds[soundName].usage;
                    return sounds[soundName].sound;
                }
                return nullptr;
            }
            
            std::wstring fullPath = PathWithResolutionDetermination(soundName, mode);
            if (base::FileExists(fullPath))
            {
                sf::SoundBuffer* sound = new sf::SoundBuffer();
                bool soundLoaded{ false };
#ifdef _WIN32
                std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
                if (!ifStream.is_open())
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
                else
                {
                    auto filesize = ifStream.tellg();
                    char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                    ifStream.seekg(0, std::ios::beg);
                    ifStream.read(fileInMemory, filesize);
                    ifStream.close();
                    
                    soundLoaded = sound->loadFromMemory(fileInMemory, filesize);
                    delete[] fileInMemory;
                }
#else
                if (!(soundLoaded = sound->loadFromFile(base::utf8(fullPath))))
                    std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#endif
                
                if (soundLoaded)
                {
                    sounds.emplace(soundName, SoundCollectorObject(sound, 1));
                    return sound;
                }
            }
            return nullptr;
        }
    }
    bool SoundCollector::LoadMusic(sf::Music &music, std::wstring fullPath, std::unique_ptr<char[]>& fileInMemory)
    {
        bool found{ false };
        if (!base::FileExists(fullPath))
        {
            for (int j = 0; j <= 1 && !found; ++j)
            {
                if (j == 1) fullPath = base::utf16(resourcePath()) + fullPath;
                
                std::wstring onlyFolder = base::GetFolderPath(fullPath);
                std::wstring onlyFileName = L"";
                for (int i = onlyFolder.length(); i < fullPath.length(); i++)
                    onlyFileName += fullPath[i];
                
                if (base::FileExists(onlyFolder + L"music/" + onlyFileName)) fullPath = onlyFolder + L"music/" + onlyFileName;
                else if (base::FileExists(onlyFolder + L"ambient/" + onlyFileName)) fullPath = onlyFolder + L"ambient/" + onlyFileName;
                else if (base::FileExists(onlyFolder + L"audio/" + onlyFileName)) fullPath = onlyFolder + L"audio/" + onlyFileName;
                else if (base::FileExists(onlyFolder + L"Music/" + onlyFileName)) fullPath = onlyFolder + L"Music/" + onlyFileName;
                else if (base::FileExists(onlyFolder + L"Ambient/" + onlyFileName)) fullPath = onlyFolder + L"Ambient/" + onlyFileName;
                else if (base::FileExists(onlyFolder + L"Audio/" + onlyFileName)) fullPath = onlyFolder + L"Audio/" + onlyFileName;
                found = base::FileExists(fullPath);
            }
        }
        
        if (!found) found = base::FileExists(fullPath);
        if (found)
        {
            bool audioLoaded{ false };
#ifdef _WIN32
            std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
            if (!ifStream.is_open()) std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                fileInMemory.reset(new char[static_cast<unsigned int>(filesize)]);
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory.get(), filesize);
                ifStream.close();
                
                audioLoaded = music.openFromMemory(fileInMemory.get(), filesize);
            }
#else
            if (!(audioLoaded = music.openFromFile(base::utf8(fullPath))))
                std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
#endif
            return audioLoaded;
        }
        return false;
    }
    void SoundCollector::ThreadSound(std::wstring soundName, unsigned int mode, bool destroyable)
    {
        sf::SoundBuffer* sound = new sf::SoundBuffer();
        std::wstring fullPath = PathWithResolutionDetermination(soundName, mode);
        
        if (base::FileExists(fullPath))
        {
            bool soundLoaded{ false };
    #ifdef _WIN32
            std::ifstream ifStream(fullPath, std::ios::binary | std::ios::ate);
            if (!ifStream.is_open())
                std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
            else
            {
                auto filesize = ifStream.tellg();
                char* fileInMemory = new char[static_cast<unsigned int>(filesize)];
                ifStream.seekg(0, std::ios::beg);
                ifStream.read(fileInMemory, filesize);
                ifStream.close();
                
                soundLoaded = sound->loadFromMemory(fileInMemory, filesize);
                delete[] fileInMemory;
            }
    #else
            if (!(soundLoaded = sound->loadFromFile(base::utf8(fullPath))))
                std::cerr << "Unable to open file: " << base::utf8(fullPath) << std::endl;
    #endif
            
            if (soundLoaded)
            {
                sounds.emplace(soundName, SoundCollectorObject(sound, 0, destroyable));
                //threads.erase(imageName);
            }
        }
    }
    void SoundCollector::PreloadSound(const std::wstring& soundName, unsigned int mode, bool destroyable)
    {
        if (sounds.find(soundName) == sounds.end())
            if (threads.find(soundName) == threads.end())
            {
                std::wstring fullPath = PathWithResolutionDetermination(soundName, mode);
                if (base::FileExists(fullPath))
                    threads.emplace(soundName, std::thread(&ThreadSound, soundName, mode, destroyable));
            }
    }
    void SoundCollector::SetDestroyable(std::wstring soundName, bool destroyable)
    {
        if (sounds.find(soundName) != sounds.end())
        {
            sounds[soundName].destroyable = destroyable;
            if (destroyable) if (sounds[soundName].usage <= 0) sc::EraseSound(soundName);
        }
    }
    void SoundCollector::DeleteSound(const std::wstring& soundName)
    {
        if (sounds.find(soundName) != sounds.end())
        {
            --sounds[soundName].usage;
            if (sounds[soundName].usage <= 0 && sounds[soundName].destroyable)
                sc::EraseSound(soundName);
        }
    }
    void SoundCollector::EraseSound(const std::wstring& soundName)
    {
        if (sounds.find(soundName) != sounds.end())
        {
            if (threads.find(soundName) != threads.end())
            {
                if (threads[soundName].joinable()) threads[soundName].join();
                threads.erase(soundName);
            }
            
            sf::SoundBuffer* sound = sounds[soundName].sound;
            if (sound != nullptr) delete sound;
            sounds.erase(soundName);
        }
    }
    void SoundCollector::FreeSounds()
    {
        for (const auto& key : sounds)
            if (key.second.sound != nullptr) delete key.second.sound;
        sounds.clear();
    }
    std::unordered_map<std::wstring, SoundCollectorObject> sc::sounds;
    std::unordered_map<std::wstring, std::thread> sc::threads;
    scThreadsJoiner sc::threadsJoiner;
    
    
    
    
    
    
    void GlobalSettings::Load(const std::wstring& path)
    {
        std::wifstream wif;
#ifdef _WIN32
        wif.open(base::utf16(documentsPath()) + path);
#else
        wif.open(documentsPath() + base::utf8(path));
#endif
        wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        
        if (wif.is_open())
        {
            std::wstring line;
            nss::CommandSettings command;
            command.lowercaseCommand = false;
            
            while (!wif.eof())
            {
                std::getline(wif, line);
                command.Command(line);
                
                if (nss::Command(command, L"framerateLimit:"))
                {
                    int integer = nss::ParseAsInt(command);
                    if (integer >= 0) gs::framerateLimit = integer;
                }
                else if (nss::Command(command, L"framerateNoFocus:"))
                {
                    int integer = nss::ParseAsInt(command);
                    if (integer >= 0) gs::framerateNoFocus = integer;
                }
                else if (nss::Command(command, L"resolutionClassSetting:") || nss::Command(command, L"resolutionClass:"))
                {
                    int integer = nss::ParseAsInt(command);
                    if (integer >= -1) gs::resolutionClass = gs::resolutionClassSetting = integer;
                }
                else if (nss::Command(command, L"isParallaxEnabled:"))
                    gs::isParallaxEnabled = nss::ParseAsBool(command);
                else if (nss::Command(command, L"isVerticalSyncEnabled:"))
                    gs::isVerticalSyncEnabled = nss::ParseAsBool(command);
                else if (nss::Command(command, L"maxVolumeGlobal:"))
                {
                    float integer = nss::ParseAsFloat(command);
                    if (integer >= 0) gs::maxVolumeGlobal = integer;
                }
                else if (nss::Command(command, L"maxVolumeMusic:"))
                {
                    float integer = nss::ParseAsFloat(command);
                    if (integer >= 0) gs::maxVolumeMusic = integer;
                }
                else if (nss::Command(command, L"maxVolumeAmbeint:"))
                {
                    float integer = nss::ParseAsFloat(command);
                    if (integer >= 0) gs::maxVolumeAmbeint = integer;
                }
                else if (nss::Command(command, L"maxVolumeSound:"))
                {
                    float integer = nss::ParseAsFloat(command);
                    if (integer >= 0) gs::maxVolumeSound = integer;
                }
            }
            
            wif.close();
        }
    }
    void GlobalSettings::Save(const std::wstring& path)
    {
        if (!base::FileExists(path)) base::CreateDirectory(base::utf16(documentsPath()));
        
        std::wofstream wof;
#ifdef _WIN32
        wof.open(base::utf16(documentsPath()) + path);
#else
        wof.open(documentsPath() + base::utf8(path));
#endif
        wof.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
        if (wof.is_open())
        {
            wof << " " << endl;
        }
    }
    
    
    
    
    
    
    sf::RenderWindow* gs::window = nullptr;
    
    unsigned int gs::width = 0;
    unsigned int gs::height = 0;
    
    unsigned int gs::resizeToWidth = 0;
    unsigned int gs::resizeToHeight = 0;
    
    unsigned int gs::relativeWidth = 0;
    unsigned int gs::relativeHeight = 0;
    float gs::scalex = 1.f, gs::scaley = 1.f;
    float gs::scale = 1.f; float gs::scScale = 1.f;
    
#ifdef _WIN32
    int gs::windowPositionOffset = IsWindows8OrGreater() ? GetSystemMetrics(SM_CXSIZEFRAME) : 0;
#else
    int gs::windowPositionOffset = 0;
#endif
    
    bool gs::isVerticalSyncEnabled = true;
    int gs::framerateLimit = 120;
    int gs::framerateNoFocus = 30;
    
    bool gs::isResolutionClassEnabled = true;
    int gs::resolutionClassSetting = -1;
    int gs::resolutionClass = 1;
    
    bool gs::isPauseEnabled = true;
    bool gs::isPause = false;
    bool gs::requestWindowRefresh = true;
    
    bool gs::isParallaxEnabled = true;
    float gs::defaultParallaxBackground = 0.018;
    float gs::defaultParallaxFar = 0.032;
    float gs::defaultParallaxNormal = 0.062;
    float gs::defaultParallaxClose = 0.105;
    float gs::defaultParallaxFrontground = 0.13;
    
    bool gs::forcePressInsideDialogue = true;
    
    float gs::maxVolumeGlobal = 1.f;
    float gs::maxVolumeMusic = 1.f;
    float gs::maxVolumeAmbeint = 1.f;
    float gs::maxVolumeSound = 1.f;
}
