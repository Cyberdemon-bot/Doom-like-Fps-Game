#include "Player.h"


bool Player::Init(std::pair<float, float> pos, float a, float s, float hp)
{
    Px = pos.first, Py = pos.second, Pangle = a, Pspeed = s, Php = hp;
    return true;
}

void Player::ChangePos(std::pair<float, float> pos) {Px = pos.first, Py = pos.second;}

void Player::Rotate(float deltaAngle)
{
    Pangle += deltaAngle;
    Pangle = std::fmod(Pangle, 2 * PI);
    if(Pangle < 0) Pangle += 2 * PI;
}




float Player::GetX()  const{return Px;}

float Player::GetY()  const{return Py;}

float Player::GetA()  const{return Pangle;}

float Player::GetPitch()  const{return Ppitch;}

float Player::GetRot()  const{return Prot_speed;}

void Player::LookUp()
{
    float pitch = Ppitch + Prot_pitch;
    if (pitch > 0.5f) pitch = 0.5f;
    if (pitch < -0.5f) pitch = -0.5f;
    Ppitch = pitch;
}

void Player::LookDown()
{
    float pitch = Ppitch - Prot_pitch;
    if (pitch > 0.5f) pitch = 0.5f;
    if (pitch < -0.5f) pitch = -0.5f;
    Ppitch = pitch;
}

float Player::GetS()  const{return Pspeed;}

float Player::GetHp()  const{return Php;}

void Player::TakeDamage(float amount) {Php -= amount;}

void Player::SetHp(float hp) {Php = hp;}
