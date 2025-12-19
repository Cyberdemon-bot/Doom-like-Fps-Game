#pragma once
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <utility>
#define PI 3.14159265f
class Player
{
private:
    float Px, Py;
    float Pangle;
    float Pspeed;
    float Ppitch;
    float Prot_speed = 0.02f;
    float Prot_pitch = 0.015f;
    float Php;
public:
    bool Init(std::pair<float, float> pos, float a, float s, float hp);
    void ChangePos(std::pair<float, float> pos);
    void Rotate(float deltaAngle);
    void LookUp();
    void LookDown();
    void TakeDamage(float amount);
    void SetHp(float amount);
    float GetHp() const;
    float GetX() const;
    float GetY() const;
    float GetA() const;
    float GetPitch() const;
    float GetRot() const;
    float GetS() const;
};
