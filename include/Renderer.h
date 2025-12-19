#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <string.h>
#include <memory>
#include "Map.h"
#include "Player.h"
#include "Sprites.h"
#include "Interface.h"
#define INF 10000000.0f
namespace fs = std::filesystem;
class Renderer
{
public:
    bool OpenWindow(const char* title, int w, int h, bool fullscreen, bool resizable);
    void ImportMap(Map& mpp);
    void ImportPlayer(Player& player);
    void ImportSprites(std::vector<Sprites>& spt);
    void ImportInterface(Interface& temp);
    void CleanUp();
    void Clear();
    void RenderEnd(int Score, int maxScore);
    void Render2DPlayer(float scale);
    void Render2DMap(float scale);
    void Render2DSprites(float scale);
    void RenderBackGround();
    void RenderSprites();
    void RayCasting();
    void Display();
    void LoadTextures(const std::string& folder);
    SDL_Texture* GetTextureByIndex(int index);
    int GetTexSize();
    void ClearTex();
    void LoadBG();
    void SortSprites(std::vector<Sprites>* list);
private:
    int width, height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    Player* mainPlayer;
    Map* mainMap;
    std::vector<Sprites>* SpritesList;
    std::vector<float> depthBuffer;
    std::vector<SDL_Texture*> textures;
    float RayVert(float angle, float px, float py, float& xvert, float& yvert);
    float RayHor(float angle, float px, float py, float& xhor, float& yhor);
    void DrawColByColor(int i, int height, float distanceCorrected);
    void DrawColByTex(int i, int height, float distanceCorrected, float xwall, float ywall, bool walltype);
};
