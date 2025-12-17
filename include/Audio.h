#pragma once
#include <SDL_mixer.h>
#include <string>
#include <map>
#include <iostream>
#include <dirent.h>

class Audio
{
private:
    std::map<std::string, Mix_Music*> musicTracks;
    std::map<std::string, Mix_Chunk*> soundEffects;

    std::string GetFilenameWithoutExtension(const std::string& path);
    static const int EXCLUSIVE_CHANNEL = 0;
public:
    bool Init();

    void LoadMusic(const std::string& folderPath);

    void LoadSound(const std::string& folderPath);

    void PlayMusic(const std::string& name, int loops = -1);
    void PlaySound(const std::string& name, int loops = 0);
    void PlayExclusiveSound(const std::string& name, int loops = 0);
    void CleanUp();
};
