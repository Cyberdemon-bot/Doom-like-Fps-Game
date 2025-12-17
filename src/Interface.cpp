#include "Interface.h"

void Interface::LoadTex(const std::string& fullPath)
{
    int cnt = 0;
    std::cout << "Interface Loading... ";
    std::vector<std::string> files;
    DIR* dir = opendir(fullPath.c_str());
    if (!dir) {
        std::cerr << "Cannot open folder: " << fullPath << "\n";
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        std::string filename = entry->d_name;
        files.push_back(fullPath + "/" + filename);
    }
    closedir(dir);

    std::sort(files.begin(), files.end());

    int filesToLoad = std::min((int)files.size(), 2);
    for (int i = 0; i < filesToLoad; ++i)
    {
        const auto& path = files[i];
        SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
        if (!texture) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        tex.push_back(texture);
        cnt++;
    }
    std::cout << "Loaded " << cnt << " textures from " << fullPath << ".\n";
}


void Interface::GetRenderInfo(SDL_Renderer* rd, int w, int h)
{
    renderer = rd;
    screenWidth = w;
    screenHeight = h;
    if (!renderer) {
        std::cout << "Fail to load Renderer\n";
        return;
    }
}

bool Interface::Init()
{
    CurrentWeaponIndex = 0;
    WEAPON_State = 0;
    WEAPON_AniDone = true;
    WEAPON_AniFrame_counter = 0;
    WEAPON_FrameDelayCounter = 0;
    return true;
}

void Interface::LoadWeapon(const std::string& weaponName, int totalFrames, int frameDelay, int xOffset, float scale)
{
    WeaponInfo info;
    info.startIndex = tex.size();
    info.totalFrames = totalFrames;
    info.frameDelay = frameDelay;
    info.screenXOffset = xOffset;
    info.scale = scale;

    std::string fullPath = "res/weapon/" + weaponName;

    LoadTex(fullPath);

    weapons.push_back(info);
}

void Interface::RunShootAni()
{
    if (WEAPON_State == 0 || (WEAPON_State == 1 && WEAPON_AniDone))
    {
        WEAPON_State = 1;
        WEAPON_AniDone = false;
        WEAPON_AniFrame_counter = 1;
        WEAPON_FrameDelayCounter = 0;
    }
}

void Interface::UpdateAnimation()
{
    if (WEAPON_State != 1 || WEAPON_AniDone) return;

    WeaponInfo& currentWeapon = weapons[CurrentWeaponIndex];

    WEAPON_FrameDelayCounter++;

    if (WEAPON_FrameDelayCounter >= currentWeapon.frameDelay)
    {
        WEAPON_AniFrame_counter++;
        WEAPON_FrameDelayCounter = 0;

        int totalFrames = currentWeapon.totalFrames;

        if (WEAPON_AniFrame_counter >= totalFrames)
        {
            WEAPON_AniDone = true;
            WEAPON_AniFrame_counter = 0;
            WEAPON_State = 0;
        }
    }
}

void Interface::RenderWeapon()
{
    if (!renderer || CurrentWeaponIndex >= weapons.size()) return;

    WeaponInfo& currentWeapon = weapons[CurrentWeaponIndex];

    SDL_Texture* renderTex = nullptr;
    SDL_Rect srcRect = { 0, 0, 0, 0 };

    int frameW, frameH;

    if (WEAPON_State == 0)
    {
        unsigned int defaultTexIndex = currentWeapon.startIndex;
        if (defaultTexIndex < 0 || defaultTexIndex >= tex.size()) return;
        renderTex = tex[defaultTexIndex];

        if (!renderTex) return;

        SDL_QueryTexture(renderTex, NULL, NULL, &frameW, &frameH);
        srcRect = { 0, 0, frameW, frameH };
    }
    else
    {
        unsigned int sheetTexIndex = currentWeapon.startIndex + 1;
        if (sheetTexIndex < 0 || sheetTexIndex >= tex.size()) return;
        renderTex = tex[sheetTexIndex];

        if (!renderTex) return;

        int sheetW, sheetH;
        SDL_QueryTexture(renderTex, NULL, NULL, &sheetW, &sheetH);

        frameW = sheetW / currentWeapon.totalFrames;
        frameH = sheetH;

        srcRect = {WEAPON_AniFrame_counter * frameW, 0, frameW, frameH};
    }

    const float SCALE_FACTOR = currentWeapon.scale;
    int scaledW = (int)(frameW * SCALE_FACTOR);
    int scaledH = (int)(frameH * SCALE_FACTOR);

    SDL_Rect destRect = {(screenWidth - scaledW) / 2 + currentWeapon.screenXOffset, screenHeight - scaledH, scaledW, scaledH};

    SDL_RenderCopy(renderer, renderTex, &srcRect, &destRect);
}

void Interface::ChangeWeapon(int index) {CurrentWeaponIndex = index;}


void Interface::RenderCrosshair()
{
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    int crosshairSize = 5;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderDrawLine(renderer, centerX - crosshairSize, centerY, centerX + crosshairSize, centerY);
    SDL_RenderDrawLine(renderer, centerX, centerY - crosshairSize, centerX, centerY + crosshairSize);
}

void Interface::CleanUp()
{
    for (size_t i = 0; i < tex.size(); i++) SDL_DestroyTexture(tex[i]);
    tex.clear();
    std::cout << "Interface Cleaned Up!" << std::endl;
}

void Interface::HpEffect(float hp)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};

    if (hp <= 20.0f)
    {

        SDL_SetRenderDrawColor(renderer, 180, 0, 0, 180);
        SDL_RenderFillRect(renderer, &overlay);
    }
    else if (hp <= 50.0f)
    {

        SDL_SetRenderDrawColor(renderer, 255, 40, 40, 120);
        SDL_RenderFillRect(renderer, &overlay);
    }
    else if (hp <= 80.0f)
    {

        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 60);
        SDL_RenderFillRect(renderer, &overlay);
    }
}
