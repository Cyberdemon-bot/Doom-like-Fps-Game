#include "Physics.h"

void Physics::ImportEntity(Map& mp, Player& py, Clock& clk, std::vector<Sprites>& psl)
{
    mainMap = &mp, mainPlayer = &py, MyClock = &clk, PhySptList = &psl;
}

void Physics::UpdateAllSpt()
{
    for(unsigned int i = 0; i < PhySptList->size(); i++)
    {
        auto& ent = (*PhySptList)[i];
        int xmap = ent.GetX(), ymap = ent.GetY();
        auto& cellList = SptManager[{xmap, ymap}];
        cellList.push_back(&ent);
        ent.ListIt = std::prev(cellList.end());
    }
}

bool Physics::Check_wall(float x, float y)
{
    int mapX, mapY;
    mapX = (int)(x + 0.35), mapY = (int)y;
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)(x - 0.35), mapY = (int)y;
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)x, mapY = (int)(y + 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)x, mapY = (int)(y - 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)(x + 0.35), mapY = (int)(y - 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)(x - 0.35), mapY = (int)(y - 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)(x + 0.35), mapY = (int)(y + 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    mapX = (int)(x - 0.35), mapY = (int)(y + 0.35);
    if(mainMap->FindPos({mapY, mapX})) return true;
    return false;
}

void Physics::UpdateSpt(Sprites& spt)
{
    int Ox = spt.GetOldX(), Oy = spt.GetOldY();
    int Nx = spt.GetX(), Ny = spt.GetY();
    if(Ox != Nx || Oy != Ny)
    {
        SptManager[{Ox, Oy}].erase(spt.ListIt);
        SptManager[{Nx, Ny}].push_back(&spt);
        spt.ListIt = std::prev(SptManager[{Nx, Ny}].end());
    }
}

void Physics::MoveSpt(int index, int type)
{
    auto& ent = (*PhySptList)[index];
    std::pair<float, float> newPos = MoveEnt(ent, type);
    float newX = newPos.first;
    float newY = newPos.second;

    if(ent.CheckRigid())
    {
        if(!Check_wall(newX, ent.GetY()) && !CheckEnt(newX, ent.GetY(), mainPlayer->GetX(), mainPlayer->GetY()) && !(CheckSptCollision(newX, ent.GetY(), &ent)).size()) ent.ChangePos({newX, ent.GetY()});
        if(!Check_wall(ent.GetX(), newY) && !CheckEnt(ent.GetX(), newY, mainPlayer->GetX(), mainPlayer->GetY()) && !(CheckSptCollision(ent.GetX(), newY, &ent)).size()) ent.ChangePos({ent.GetX(), newY});
    }
    else ent.ChangePos({newX, newY});
    UpdateSpt(ent);
}

void Physics::MovePly(int type)
{
    auto& ent = *mainPlayer;
    std::pair<float, float> newPos = MoveEnt(ent, type);
    float newX = newPos.first;
    float newY = newPos.second;
    if(!Check_wall(newX, ent.GetY()) && !(CheckSptCollision(newX, ent.GetY(), nullptr)).size()) ent.ChangePos({newX, ent.GetY()});
    if(!Check_wall(ent.GetX(), newY) && !(CheckSptCollision(ent.GetX(), newY, nullptr)).size()) ent.ChangePos({ent.GetX(), newY});
}

std::vector<Sprites*> Physics::CheckSptCollision(float newX, float newY, Sprites* spt)
{
    std::vector<Sprites*> collided;
    if (!PhySptList) return collided;
    int xmap = newX, ymap = newY;
    for(int x = xmap - 1; x <= xmap + 1; x++)
    {
        for(int y = ymap - 1; y <= ymap + 1; y++)
        {
            auto it = SptManager.find({x, y});
            if(it != SptManager.end())
            {
                std::list<Sprites*>& spritesInCell = it->second;
                for (Sprites* sprite : spritesInCell)
                {
                    if(spt == sprite) continue;
                    else if(sprite->CheckRigid() && CheckEnt(newX, newY, sprite->GetX(), sprite->GetY())) collided.push_back(sprite);
                }
            }
        }
    }
    return collided;
}

bool Physics::CheckEnt(float ax, float ay, float bx, float by)
{
    float dx = bx - ax;
    float dy = by - ay;
    float dist2 = dx * dx + dy * dy;
    float radius = 0.6f;
    if (dist2 < radius * radius) return true;
    return false;
}

Sprites* Physics::Praycast()
{
    if (!mainMap || !mainPlayer || !PhySptList) return nullptr;

    float px = mainPlayer->GetX();
    float py = mainPlayer->GetY();
    float pa = mainPlayer->GetA();
    float FOV = PI / 3;
    float HalfFOV = FOV / 2;

    Sprites* target = nullptr;
    float closestDist = 1e9f;

    for (auto& s : *PhySptList)
    {
        if(!s.IsDead())
        {
            float dx = s.GetX() - px;
            float dy = s.GetY() - py;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 0.1f) continue;

            float dirToSpt = atan2f(dy, dx);
            float diff = fabsf(pa - dirToSpt);
            if (diff > PI) diff = 2 * PI - diff;
            if (diff > HalfFOV) continue;

            float xvert, yvert, xhor, yhor;
            float d_vert = RayVert(dirToSpt, px, py, xvert, yvert);
            float d_hor  = RayHor(dirToSpt, px, py, xhor, yhor);
            float wallDist = std::min(d_vert, d_hor);

            if (wallDist > dist && dist < closestDist)
            {
                closestDist = dist;
                target = &s;
            }
        }
    }

    return target;
}


bool Physics::Sraycast(int index, float SptFov, float maxDepth)
{
    if (!mainMap || !mainPlayer || !PhySptList) return false;

    auto& sp = (*PhySptList)[index];
    float sx = sp.GetX();
    float sy = sp.GetY();
    float sa = sp.GetA();

    float px = mainPlayer->GetX();
    float py = mainPlayer->GetY();

    float dx = px - sx;
    float dy = py - sy;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > maxDepth) return false;

    float dirToPlayer = atan2f(dy, dx);
    float diff = fabsf(sa - dirToPlayer);
    if (diff > PI) diff = 2 * PI - diff;
    if (diff > SptFov / 2.0f) return false;

    float xvert, yvert, xhor, yhor;
    float d_vert = RayVert(dirToPlayer, sx, sy, xvert, yvert);
    float d_hor  = RayHor(dirToPlayer, sx, sy, xhor, yhor);
    float wallDist = std::min(d_vert, d_hor);

    if (wallDist > dist)
        return true;

    return false;
}

float Physics::RayVert(float angle, float px, float py, float& xvert, float& yvert)
{
    int xmap = (int)px;
    int mpRows = mainMap->GetRow(), mpCols = mainMap->GetCol();

    float sin_a = std::sin(angle);
    float cos_a = std::cos(angle);
    float dx, dy, depth, Jumpdepth;

    if(cos_a > 0) { xvert = xmap + 1; dx = 1; }
    else          { xvert = xmap - 0.00001f; dx = -1; }

    depth = (xvert - px) / cos_a;
    yvert = py + sin_a * depth;
    dy = sin_a / cos_a * dx;
    Jumpdepth = dx / cos_a;

    std::pair<int,int> posvert = {(int)yvert, (int)xvert};
    int maxSteps = mpRows * mpCols * 2;
    int step = 0;

    while(!mainMap->FindPos(posvert) && step < maxSteps)
    {
        xvert += dx;
        yvert += dy;
        depth += Jumpdepth;
        posvert = {(int)yvert, (int)xvert};
        if(posvert.first < 0 || posvert.first >= mpRows || posvert.second < 0 || posvert.second >= mpCols)
            return INF;
        step++;
    }

    return depth;
}

float Physics::RayHor(float angle, float px, float py, float& xhor, float& yhor)
{
    int ymap = (int)py;
    int mpRows = mainMap->GetRow(), mpCols = mainMap->GetCol();

    float sin_a = std::sin(angle);
    float cos_a = std::cos(angle);
    float dx, dy, depth, Jumpdepth;

    if(sin_a > 0) { yhor = ymap + 1; dy = 1; }
    else           { yhor = ymap - 0.0001f; dy = -1; }

    depth = (yhor - py) / sin_a;
    xhor = px + cos_a * depth;
    dx = cos_a / sin_a * dy;
    Jumpdepth = dy / sin_a;

    std::pair<int,int> poshor = {(int)yhor, (int)xhor};
    int maxSteps = mpRows * mpCols * 2;
    int step = 0;

    while(!mainMap->FindPos(poshor) && step < maxSteps)
    {
        xhor += dx;
        yhor += dy;
        depth += Jumpdepth;
        poshor = {(int)yhor, (int)xhor};
        if(poshor.first < 0 || poshor.first >= mpRows || poshor.second < 0 || poshor.second >= mpCols)
            return INF;
        step++;
    }

    return depth;
}

