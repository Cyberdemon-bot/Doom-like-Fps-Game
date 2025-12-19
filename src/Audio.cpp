#include "Audio.h"

bool Audio::Init()
{
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Failed to open audio device: " << Mix_GetError() << std::endl;
        return false;
    }

    std::cout << "AudioManager Initialized Successfully!" << std::endl;
    return true;
}

void Audio::PlayMusic(const std::string& name, int loops)
{
    if (musicTracks.find(name) != musicTracks.end()) {
        Mix_PlayMusic(musicTracks[name], loops);
    }
}

void Audio::PlaySound(const std::string& name, int loops)
{
    if (soundEffects.find(name) != soundEffects.end()) {
        Mix_PlayChannel(-1, soundEffects[name], loops);
    }
}

void Audio::CleanUp()
{
    for (auto const& pair : soundEffects) Mix_FreeChunk(pair.second);
    soundEffects.clear();

    for (auto const& pair : musicTracks) Mix_FreeMusic(pair.second);
    musicTracks.clear();

    Mix_CloseAudio();
    Mix_Quit();
    std::cout << "AudioManager Cleaned Up!" << std::endl;
}

std::string Audio::GetFilenameWithoutExtension(const std::string& path)
{
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    size_t lastDot = filename.find_last_of(".");
    if (lastDot == std::string::npos) {
        return filename;
    }
    return filename.substr(0, lastDot);
}

void Audio::LoadMusic(const std::string& folderPath)
{
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "Cannot open music folder: " << folderPath << std::endl;
        return;
    }

    std::cout << "Loading music from " << folderPath << "..." << std::endl;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) continue;

        auto path = entry.path();
        auto ext  = path.extension().string();

        if (ext != ".mp3" && ext != ".ogg" && ext != ".wav")
            continue;

        std::string nameKey = path.stem().string();

        Mix_Music* music = Mix_LoadMUS(path.string().c_str());
        if (!music) {
            std::cerr << "Failed to load music: "
                      << path << " | Error: "
                      << Mix_GetError() << std::endl;
        } else {
            musicTracks[nameKey] = music;
            std::cout << "  - Loaded music: " << nameKey << std::endl;
        }
    }
}



void Audio::LoadSound(const std::string& folderPath)
{
    if (!fs::exists(folderPath) || !fs::is_directory(folderPath)) {
        std::cerr << "Cannot open sound folder: " << folderPath << std::endl;
        return;
    }

    std::cout << "Loading sounds from " << folderPath << "..." << std::endl;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (!entry.is_regular_file()) continue;

        auto path = entry.path();
        auto ext  = path.extension().string();

        if (ext != ".wav")
            continue;

        std::string nameKey = path.stem().string();

        Mix_Chunk* sound = Mix_LoadWAV(path.string().c_str());
        if (!sound) {
            std::cerr << "Failed to load sound: "
                      << path << " | Error: "
                      << Mix_GetError() << std::endl;
        } else {
            soundEffects[nameKey] = sound;
            std::cout << "  - Loaded sound: " << nameKey << std::endl;
        }
    }
}


void Audio::PlayExclusiveSound(const std::string& name, int loops)
{
    if (soundEffects.find(name) != soundEffects.end())
    {
        Mix_HaltChannel(EXCLUSIVE_CHANNEL);
        Mix_PlayChannel(EXCLUSIVE_CHANNEL, soundEffects[name], loops);
    }
}

