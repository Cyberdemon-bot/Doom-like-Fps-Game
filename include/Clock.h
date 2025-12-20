#pragma once
#include <SDL2/SDL.h>
#include <algorithm>
class Clock
{
private:
    Uint64 lastCounter;
    Uint64 frequency;
    float deltaTime;
    float fps;

public:
    Clock();


    void tick(int targetFPS = 0);


    float getDeltaTime() const;


    float getFPS() const;
};
