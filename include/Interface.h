#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <algorithm>
#define SHOTGUN_TOTAL_FRAMES 6
#define SHOTGUN_FRAME_DELAY 7
#define SHOTGUN_X_OFFSET 0
#define SHOTGUN_SCALE 0.35f

#define HANDGUN_TOTAL_FRAMES 4
#define HANDGUN_FRAME_DELAY 5
#define HANDGUN_X_OFFSET 450
#define HANDGUN_SCALE 1.0f

class Interface;

struct WeaponInfo
{
    int startIndex;
    int totalFrames;
    int frameDelay;
    int screenXOffset;
    float scale;
};

class Interface
{
private:
    std::vector<SDL_Texture*> tex;
    std::vector<WeaponInfo> weapons;
    unsigned int CurrentWeaponIndex;

    int WEAPON_State;
    int WEAPON_AniFrame_counter;
    int WEAPON_FrameDelayCounter;
    bool WEAPON_AniDone;

    SDL_Renderer* renderer = nullptr;
    int screenWidth;
    int screenHeight;

    void LoadTex(const std::string& fullPath);

public:
    void GetRenderInfo(SDL_Renderer* rd, int w, int h);
    bool Init();
    void LoadWeapon(const std::string& weaponName, int totalFrames, int frameDelay, int xOffset, float scale);
    void ChangeWeapon(int index);
    void RunShootAni();
    void UpdateAnimation();
    void RenderWeapon();
    void RenderCrosshair();
    void CleanUp();
    bool IsAniDone() const { return WEAPON_AniDone; }
    void HpEffect(float hp);
};
