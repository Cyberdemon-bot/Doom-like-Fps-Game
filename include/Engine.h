#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <utility>
#include "Renderer.h"
#include "Clock.h"
#include "Map.h"
#include "Player.h"
#include "Sprites.h"
#include "Physics.h"
#include "Interface.h"
#include "Audio.h"
#define Forward -1
#define Backward -2
#define Right -3
#define Left -4

class Engine
{
private:
    Renderer renderer;
    Interface ui;
    Clock clock;
    Map worldMap;
    Player player;
    Physics physicsManager;
    Audio audioManager;
    std::vector<Sprites> sprites;

public:
    // Initialization
    bool InitRenderer(const char* title, int w, int h, bool fullscreen, bool resizable);
    bool InitUI();
    bool InitAudio();
    bool InitMap(const std::vector<std::vector<int>>& miniMap);
    bool InitPlayer(std::pair<float, float> pos, float angle, float speed, float hp);
    void SetupConnections();

    // Rendering
    void ClearScreen();
    void RenderBackground();
    void RenderRayCasting();
    void RenderSprites();
    void Render2DMap(float scale);
    void Render2DPlayer(float scale);
    void Render2DSprites(float scale);
    void RenderWeapon();
    void RenderCrosshair();
    void RenderHpEffect();
    void RenderEndScreen(int round, int maxScore);
    void DisplayFrame();

    // Audio
    void LoadSounds(const std::string& path);
    void LoadMusic(const std::string& path);
    void PlaySound(const std::string& name, int loops);
    void PlayExclusiveSound(const std::string& name, int loops);
    void PlayMusic(const std::string& name, int loops);

    // UI/Weapons
    void LoadWeapon(const std::string& name, int frames, float delay, float xOffset, float scale);
    void ChangeWeapon(int weaponIndex);
    void RunShootAnimation();
    bool IsWeaponAnimationDone();
    void UpdateWeaponAnimation();

    // Textures
    void LoadBackgroundTexture();
    void LoadTextures(const std::string& path);
    void ClearTextures();
    int GetTextureSize();

    // Player
    float GetPlayerX() const;
    float GetPlayerY() const;
    float GetPlayerAngle() const;
    float GetPlayerHp() const;
    void SetPlayerHp(float hp);
    void SetPlayerPos(std::pair<float, float> pos);
    void RotatePlayer(float angle);
    void PlayerLookUp();
    void PlayerLookDown();
    void PlayerTakeDamage(float damage);

    // Physics/Movement
    void MovePlayer(int dir);
    void MoveSprite(int index, int dir);
    Sprites* PerformPlayerRaycast();
    bool PerformSpriteRaycast(int index, float fov, float depth);
    void UpdateAllSpritesPhysics();

    // Sprites
    void AddSprite(const std::string& name, std::pair<int, int> pos, float angle,
                   float speed, float rotSpeed, bool rigid, bool visible,
                   int defaultState, float damage, float range);
    void ClearSprites();
    int GetSpriteCount() const;
    Sprites& GetSprite(int index);
    const std::vector<Sprites>& GetSprites() const;
    void UpdateSpriteStates();
    void UpdateSpriteAnimations();

    // Map
    bool IsWall(std::pair<int, int> pos) const;
    int GetMapRows() const;
    int GetMapCols() const;

    // Clock
    void Tick(float targetFPS);
    float GetDeltaTime() const;

    // Cleanup
    void Cleanup();
};
