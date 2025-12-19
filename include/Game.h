#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <queue>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <random>
#include "Engine.h"

struct Node {
    int y, x;
    int g, h;
    Node* parent;

    Node(int _y, int _x, int _g, int _h, Node* _parent)
        : y(_y), x(_x), g(_g), h(_h), parent(_parent) {}

    int f() const { return g + h; }
};

class Game
{
private:
    Engine engine;
    SDL_Event event;

    // Game state
    bool pausing = false;
    bool running;
    bool MouseFree;
    int targetFPS;
    int currentWeapon;
    int MonCnt;
    int Round;
    int maxScore;
    bool MouseClick;

    // Helper methods
    std::pair<float, float> GetRandomEmptyPosF();
    int CalculateHeuristic(std::pair<int, int> a, std::pair<int, int> b);
    std::vector<std::pair<int, int>> FindPathAStar(std::pair<int, int> start, std::pair<int, int> end);

public:
    bool Init(const char* title, int w, int h, bool fullscreen, bool resizable, float FPS, const std::vector<std::vector<int>>& miniMap);
    void HandleEvent();
    void RenderGame();
    void Update();
    void Clean();
    void Run();
    void RebuildData();
    void AddSprite(std::string name, std::pair<int, int> pos, float a, float s, float rs, bool vs, bool Rigid, int defaultState, float dm, float rag);
    void UpdateAI();
};
