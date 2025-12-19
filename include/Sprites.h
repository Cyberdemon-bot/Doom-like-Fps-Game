#pragma once
#include <cmath>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <list>
#include <string>
#define PI 3.14159265f
#define St2Delay 14
#define St3Delay 8
#define St4Delay 15
#define St5Delay 9

class Sprites
{
private:
    float posx, posy;
    float oldx, oldy;
    float angle;
    float speed;
    float rot_speed;
    bool rigid;
    bool visible;
    int DEFAULTSTATE;
    int texid;
    int AniFrame_counter;
    int AniFrame_delay;
    bool AniDone;
    int state;
    bool dead;
public:
    Sprites(float x, float y, float a, float s, float rs, bool rg, bool vs, int defaultState, int index, std::string n, float hp, float dm, float rag);
    virtual ~Sprites() {}
    std::vector<std::pair<int, int>> path;
    int GetDirIndex(float playerX, float playerY, float playerAngle, int numDirections) const;
    float GetOldX() const;
    float GetOldY() const;
    float GetX() const;
    float GetY() const;
    float GetA() const;
    float GetS() const;
    int GetR() const;
    int GetDefaultState();
    int GetState() const;
    int GetTexID() const;
    int GetAniCnt() const;
    bool CheckRigid() const;
    bool CheckVisible() const;
    void SetState(int s);
    void UpdateAnimation();
    bool CheckAni();
    void ChangePos(std::pair<float, float> pos);
    void Rotate(float deltaAngle);
    void SetTexID(int id);
    std::list<Sprites*>::iterator ListIt;
    std::string name;
    float HP;
    void UpdateSprite();
    void UpdateAI();
    void TakeDamage(float amount);
    bool IsDead() const;
    float damage;
    float range;
};
