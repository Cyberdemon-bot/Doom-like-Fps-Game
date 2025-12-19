#include "Sprites.h"

Sprites::Sprites(float x, float y, float a, float s, float rs, bool rg, bool vs, int defaultState, int index, std::string n, float hp, float dm, float rag)
    :posx(x), posy(y), angle(a), speed(s), rot_speed(rs), rigid(rg), visible(vs), DEFAULTSTATE(defaultState), texid(index), name(n), HP(hp), damage(dm), range(rag)
{AniDone = true, AniFrame_delay = 0, state = DEFAULTSTATE, dead = false;}

int Sprites::GetDirIndex(float playerX, float playerY, float playerAngle, int numDirections) const
{
    float dx = playerX - posx;
    float dy = playerY - posy;
    float angleToPlayer = atan2(dy, dx);
    if (angleToPlayer < 0) angleToPlayer += 2*M_PI;
    float relativeAngle = angleToPlayer - angle;  // sprite.angle
    relativeAngle = fmod(relativeAngle + 2*M_PI, 2*M_PI);

    int frame = int(relativeAngle / (2*M_PI / numDirections)) % numDirections;
    return frame;
}

bool Sprites::CheckRigid() const {return rigid;}

bool Sprites::CheckVisible() const {return visible;}

float Sprites::GetOldX() const {return oldx;}

float Sprites::GetOldY() const {return oldy;}

float Sprites::GetX() const {return posx;}

float Sprites::GetY() const {return posy;}

float Sprites::GetA() const {return angle;}

int Sprites::GetState() const {return state;}

int Sprites::GetTexID() const {return texid;}

int Sprites::GetAniCnt() const {return AniFrame_counter;}

void Sprites::SetTexID(int id) {texid = id;}

void Sprites::SetState(int s)
{
    if (s == state) return;
    if (state == 5) return;

    state = s;
    AniFrame_counter = 0;
    AniFrame_delay = 0;
    AniDone = (s < 2);
}

void Sprites::UpdateAnimation()
{
    if (AniDone) return;
    int temp[4];
    if(name == "cacodemon") temp[0] = 3, temp[1] = 5, temp[2] = 2, temp[3] = 6;
    if(name == "cyberdemon") temp[0] = 4, temp[1] = 2, temp[2] = 2, temp[3] = 9;
    AniFrame_delay++;
    if(state == 2 && AniFrame_delay >= St2Delay) { AniFrame_counter++; AniFrame_delay = 0; }
    if(state == 3 && AniFrame_delay >= St3Delay) { AniFrame_counter++; AniFrame_delay = 0; }
    if(state == 4 && AniFrame_delay >= St4Delay) { AniFrame_counter++; AniFrame_delay = 0; }
    if(state == 5 && AniFrame_delay >= St5Delay) { AniFrame_counter++; AniFrame_delay = 0; }

    bool animationJustFinished = false;
    if(state == 2 && AniFrame_counter > temp[0]) animationJustFinished = true;
    if(state == 3 && AniFrame_counter > temp[1]) animationJustFinished = true;
    if(state == 4 && AniFrame_counter > temp[2]) animationJustFinished = true;

    if(state == 5 && AniFrame_counter > temp[3])
    {
        AniFrame_counter = temp[3];
        AniDone = true;
        return;
    }

    if (animationJustFinished) {
        SetState(DEFAULTSTATE);
    }
}

void Sprites::UpdateSprite()
{
    if (IsDead() && GetState() != 5)
    {
        SetState(5);
    }
    UpdateAnimation();
}

bool Sprites::CheckAni() {return AniDone;}

void Sprites::ChangePos(std::pair<float, float> pos)
{
    oldx = posx;
    oldy = posy;
    posx = pos.first;
    posy = pos.second;
}

void Sprites::Rotate(float deltaAngle)
{
    angle += deltaAngle;
    angle = std::fmod(angle, 2 * PI);
    if(angle < 0) angle += 2 * PI;
}

float Sprites::GetS() const {return speed;}

void Sprites::TakeDamage(float amount)
{
    if(!(GetState() == 4 && !CheckAni()) && GetState() != 5) SetState(4);
    HP -= amount;
    if(HP <= 0) dead = true, rigid = false;
}

bool Sprites::IsDead() const { return dead;}

int Sprites::GetDefaultState() {return DEFAULTSTATE;}

int Sprites::GetR() const {return rot_speed;}
