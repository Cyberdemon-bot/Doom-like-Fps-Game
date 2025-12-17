#include <iostream>
#include "Renderer.h"

// ========== INIT & CLEANUP ==========

bool Renderer::OpenWindow(const char* title, int w, int h, bool fullscreen, bool resizable)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cout << "SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "SDL_Init HAS SUCCEEDED" << std::endl;

    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))) {
        std::cout << "IMG_Init FAILED: " << IMG_GetError() << "\n";
        return false;
    }
    width = w, height = h;
    Uint32 flag = SDL_WINDOW_SHOWN;
    if(resizable && fullscreen) flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if(fullscreen) flag = SDL_WINDOW_FULLSCREEN;
    else if(resizable) flag = SDL_WINDOW_RESIZABLE;
    window = NULL, renderer = NULL;
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flag);
    if(window == NULL) std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    depthBuffer.resize(w);

    return true;
}

void Renderer::CleanUp()
{
    for (size_t i = 0; i < textures.size(); i++) SDL_DestroyTexture(textures[i]);
    textures.clear();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cout << "Renderer Cleaned Up!" << std::endl;
}

// ========== DATA IMPORT ==========

void Renderer::ImportMap(Map& mpp) {mainMap = &mpp;}

void Renderer::ImportPlayer(Player& player) {mainPlayer = &player;}

void Renderer::ImportSprites(std::vector<Sprites>& spt) {SpritesList = &spt;}

void Renderer::ImportInterface(Interface& itf) {itf.GetRenderInfo(renderer, width, height);}

// ========== BASIC RENDER CONTROL ==========

void Renderer::Clear()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Renderer::Display() {SDL_RenderPresent(renderer); }

// ========== RENDER UI ==========

void Renderer::RenderEnd(int Score, int maxScore)
{
    if (!renderer) return;

    TTF_Init();
    TTF_Font* font = TTF_OpenFont("res/font/arial.ttf", 32);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};

    std::string message = "Game Over";
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, message.c_str(), yellow);
    SDL_Texture* textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect = { width / 2 - surfaceMessage->w / 2, height / 2 - 100, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, textureMessage, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(textureMessage);

    std::string scoreText = "Score: " + std::to_string(Score);
    surfaceMessage = TTF_RenderText_Solid(font, scoreText.c_str(), white);
    textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect scoreRect = { width / 2 - surfaceMessage->w / 2, height / 2, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, textureMessage, NULL, &scoreRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(textureMessage);

    std::string maxScoreText = "Max Score: " + std::to_string(maxScore);
    surfaceMessage = TTF_RenderText_Solid(font, maxScoreText.c_str(), white);
    textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect maxScoreRect = { width / 2 - surfaceMessage->w / 2, height / 2 + 50, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, textureMessage, NULL, &maxScoreRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(textureMessage);

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_RenderPresent(renderer);
}

void Renderer::Render2DSprites(float scale)
{
    if (!renderer || !SpritesList) return;
    if (SpritesList->empty()) return;


    for (const auto& s : *SpritesList)
    {
        int sx = static_cast<int>(s.GetX() * scale);
        int sy = static_cast<int>(s.GetY() * scale);

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        if(s.IsDead()) SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
        SDL_Rect spriteRect;
        spriteRect.x = sx - 2;
        spriteRect.y = sy - 2;
        spriteRect.w = 4;
        spriteRect.h = 4;
        SDL_RenderFillRect(renderer, &spriteRect);


        float a = s.GetA();
        int len = 6;
        int lx = static_cast<int>(sx + len * std::cos(a));
        int ly = static_cast<int>(sy + len * std::sin(a));

        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderDrawLine(renderer, sx, sy, lx, ly);
    }
}


void Renderer::Render2DMap(float scale)
{
    SDL_Rect bg;
    bg.x = 0, bg.y = 0, bg.w = mainMap->GetCol()*scale, bg.h = mainMap->GetRow()*scale;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &bg);
    std::map<std::pair<int, int>, int>* MapData = mainMap->GetMap();
    for (auto it = MapData->begin(); it != MapData->end(); ++it)
    {
        int r = it->first.first;
        int c = it->first.second;
        int val = it->second;

        if (val)
        {
            SDL_Rect rect;
            rect.x = c * scale;
            rect.y = r * scale;
            rect.w = scale;
            rect.h = scale;
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}

void Renderer::Render2DPlayer(float scale)
{
    if(!renderer) return;

    float x = mainPlayer->GetX();
    float y = mainPlayer->GetY();
    float angle = mainPlayer->GetA();
    int px = static_cast<int>(x * scale);
    int py = static_cast<int>(y * scale);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect srcRect;
    srcRect.x = px - 2;
    srcRect.y = py - 2;
    srcRect.w = 5;
    srcRect.h = 5;
    SDL_RenderFillRect(renderer, &srcRect);

    int lineLength = 10;
    int lx = static_cast<int>(px + lineLength * cos(angle));
    int ly = static_cast<int>(py + lineLength * sin(angle));
    SDL_RenderDrawLine(renderer, px, py, lx, ly);
}

void Renderer::RenderBackGround()
{
    float pitch = mainPlayer->GetPitch();


    int horizon = height / 2 + static_cast<int>(pitch * height);

    if (horizon < 0) horizon = 0;
    if (horizon > height) horizon = height;


    SDL_Rect skyRect;
    skyRect.x = 0;
    skyRect.y = 0;
    skyRect.w = width;
    skyRect.h = height;

    SDL_RenderCopy(renderer, textures[0], NULL, &skyRect);


    SDL_Rect groundRect;
    groundRect.x = 0;
    groundRect.y = horizon;
    groundRect.w = width;
    groundRect.h = height - horizon;

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &groundRect);
}



// ========== RAYCASTING CORE ==========

float Renderer::RayVert(float angle, float px, float py, float& xvert, float& yvert)
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

float Renderer::RayHor(float angle, float px, float py, float& xhor, float& yhor)
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

void Renderer::DrawColByColor(int i, int height, float distanceCorrected)
{
    float playerPitch = mainPlayer->GetPitch();
    int lineHeight = (int)(height / distanceCorrected);
    if(lineHeight > height) lineHeight = height;

    int pitchOffset = (int)(playerPitch * height);
    int drawStart = height / 2 - lineHeight / 2 + pitchOffset;
    int drawEnd   = height / 2 + lineHeight / 2 + pitchOffset;

    if(drawStart < 0) drawStart = 0;
    if(drawEnd >= height) drawEnd = height - 1;

    int color = std::min(255, (int)(255 / (1 + distanceCorrected * 0.2f)));
    SDL_SetRenderDrawColor(renderer, color, color, color, 255);
    SDL_RenderDrawLine(renderer, i, drawStart, i, drawEnd);
}

void Renderer::DrawColByTex(int i, int height, float distanceCorrected, float xwall, float ywall, bool walltype)
{
    float playerPitch = mainPlayer->GetPitch();
    int lineHeight = (int)((height) / distanceCorrected);
    if (lineHeight < 1) lineHeight = 1;

    int pitchOffset = (int)(playerPitch * height);
    int drawStart = height / 2 - lineHeight / 2 + pitchOffset;
    int drawEnd   = height / 2 + lineHeight / 2 + pitchOffset;

    int mx, my;
    if (walltype) {
        mx = (int)xwall;
        if (xwall - mx < 1e-4f) mx--;
        my = (int)ywall;
    } else {
        my = (int)ywall;
        if (ywall - my < 1e-4f) my--;
        mx = (int)xwall;
    }

    int texIndex = -1;
    if (mainMap->FindPos({my, mx})) texIndex = mainMap->GetVal({my, mx});
    else
    {
        int tryOffsets[4][2] = {
            {0, 1},
            {0, -1},
            {1, 0},
            {-1, 0}
        };
        for (int k = 0; k < 4; k++)
        {
            int nx = mx + tryOffsets[k][1];
            int ny = my + tryOffsets[k][0];
            if (mainMap->FindPos({ny, nx}))
            {
                texIndex = mainMap->GetVal({ny, nx});
                break;
            }
        }
        if (texIndex == -1) return;
    }

    if (texIndex < 0 || texIndex >= (int)textures.size()) return;

    SDL_Texture* tex = textures[texIndex];
    int texW, texH;
    SDL_QueryTexture(tex, NULL, NULL, &texW, &texH);
    float wallX = walltype ? ywall - floor(ywall) : xwall - floor(xwall);
    if (wallX < 0) wallX += 1.0f;
    if (wallX >= 1.0f) wallX = 0.999f;

    int texX = (int)(wallX * texW);
    if (texX < 0) texX = 0;
    if (texX >= texW) texX = texW - 1;

    int srcY = 0;
    int srcH = texH;
    int dstY = drawStart;
    int dstH = drawEnd - drawStart + 1;

    if (drawStart < 0) {
        srcY = (-drawStart * texH) / lineHeight;
        srcH -= srcY;
        dstY = 0;
        dstH = drawEnd + 1;
    }
    if (drawEnd >= height) {
        int cut = (drawEnd - height + 1) * texH / lineHeight;
        srcH -= cut;
        dstH = height - dstY;
    }

    if (srcH <= 0 || dstH <= 0) return;

    SDL_Rect src = { texX, srcY, 1, srcH };
    SDL_Rect dst = { i, dstY, 1, dstH };

    SDL_RenderCopy(renderer, tex, &src, &dst);
}

void Renderer::RayCasting()
{
    float FOV = PI/3;
    float Half_FOV = FOV/2;
    float RayAngle = mainPlayer->GetA() - Half_FOV;
    float px = mainPlayer->GetX(), py = mainPlayer->GetY();
    int numRays = width;
    float DeltaAngle = FOV / numRays;

    for(int i = 0; i < numRays; i++)
    {
        float xvert, yvert, xhor, yhor, xwall, ywall; bool walltype;
        float d_vert = RayVert(RayAngle, px, py, xvert, yvert);
        float d_hor  = RayHor(RayAngle, px, py, xhor, yhor);
        float distance = std::min(d_vert, d_hor);
        //depthBuffer[i] = distance;
        if(distance == INF) continue;
        if(d_vert == distance) xwall = xvert, ywall = yvert, walltype = true;
        else xwall = xhor, ywall = yhor, walltype = false;
        float distanceCorrected = distance * cos(RayAngle - mainPlayer->GetA());
        depthBuffer[i] = distanceCorrected;
        if(distanceCorrected < 0.001f) distanceCorrected = 0.001f;
        //DrawColByColor(i, height, distanceCorrected);
        DrawColByTex(i, height, distanceCorrected, xwall, ywall, walltype);
        RayAngle += DeltaAngle;
        if(RayAngle < 0) RayAngle += 2*PI;
        if(RayAngle > 2*PI) RayAngle -= 2*PI;
    }
}

// ========== RENDER SPRITES =========

void Renderer::SortSprites(std::vector<Sprites>* list) {
    std::sort(list->begin(), list->end(),
        [this](const Sprites& a, const Sprites& b) {
            float ax = a.GetX() - mainPlayer->GetX();
            float ay = a.GetY() - mainPlayer->GetY();
            float bx = b.GetX() - mainPlayer->GetX();
            float by = b.GetY() - mainPlayer->GetY();

            float distA = ax * ax + ay * ay;
            float distB = bx * bx + by * by;
            return distA > distB;
        });
}

void Renderer::RenderSprites()
{
    if (!SpritesList || !mainPlayer || depthBuffer.empty() || textures.empty()) return;
    if (depthBuffer.size() < (size_t)width) return;
    SortSprites(SpritesList);
    int temp[4];
    for (size_t i = 0; i < SpritesList->size(); i++)
    {
        const auto& sp = (*SpritesList)[i];
        if(!sp.CheckVisible()) continue;
        if(sp.name == "cacodemon") temp[0] = 3, temp[1] = 5, temp[2] = 2, temp[3] = 6;
        if(sp.name == "cyberdemon") temp[0] = 4, temp[1] = 2, temp[2] = 2, temp[3] = 9;
        int texDir = sp.GetDirIndex(mainPlayer->GetX(), mainPlayer->GetY(), mainPlayer->GetA(), 8);

        int texIndex = sp.GetTexID() + sp.GetState();
        if (texIndex < 0 || texIndex >= (int)textures.size()) continue;

        SDL_Texture* tex = textures[texIndex];
        int texW, texH;
        SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);

        float dx = sp.GetX() - mainPlayer->GetX();
        float dy = sp.GetY() - mainPlayer->GetY();
        float playerAngle = mainPlayer->GetA();

        float dist = sqrt(dx*dx + dy*dy);
        float transformY = dist * cos(atan2(dy, dx) - playerAngle);
        if (transformY <= 0.6) continue;

        float FOV = PI / 3.0f;

        float angleToSprite = atan2(dy, dx);
        float angleDiff = playerAngle - angleToSprite;

        if (angleDiff > PI) angleDiff -= 2 * PI;
        if (angleDiff < -PI) angleDiff += 2 * PI;

        int spriteScreenX = static_cast<int>((width / 2.0f) - (angleDiff * (width / FOV)));

        float playerPitch = mainPlayer->GetPitch();
        int pitchOffset = (int)(playerPitch * height);

        int frameW = texW / 8;
        if(sp.GetState() == 0) frameW = texW, texDir = 0;
        if(sp.GetState() == 2) frameW = texW/temp[0];
        if(sp.GetState() == 3) frameW = texW/temp[1];
        if(sp.GetState() == 4) frameW = texW/temp[2];
        if(sp.GetState() == 5) frameW = texW/temp[3];

        int spriteHeight = abs(int(height / transformY));
        int spriteWidth  = int(spriteHeight * (float(frameW) / float(texH)));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        int drawEndX   =  spriteWidth / 2 + spriteScreenX;
        int drawStartY = -spriteHeight / 2 + height / 2 + pitchOffset;
        int drawEndY = drawStartY + spriteHeight;
        for (int stripe = drawStartX; stripe < drawEndX; stripe++)
        {
            if (stripe < 0 || stripe >= width) continue;
            //if (dist >= depthBuffer[stripe]) continue;
            if (transformY >= depthBuffer[stripe]) continue;
            int texX = int((stripe - (-spriteWidth / 2 + spriteScreenX)) * frameW / spriteWidth);
            SDL_Rect src, dest;
            if(sp.GetState() < 2)
            {
                src  = { texDir * frameW + texX, 0, 1, texH };
                dest = { stripe, drawStartY, 1, drawEndY - drawStartY };
            }
            else
            {
                src  = { (sp.GetAniCnt() - 1) * frameW + texX, 0, 1, texH };
                dest = { stripe, drawStartY, 1, drawEndY - drawStartY };
            }
            SDL_RenderCopy(renderer, tex, &src, &dest);
        }
    }
}


// ========== TEXTURE UTILS ==========

void Renderer::LoadBG() {textures.push_back(IMG_LoadTexture(renderer, "res/bg/sky.png"));}

void Renderer::LoadTextures(const std::string& folder)
{
    int cnt = 0;
    std::cout << "Renderer Loading... ";
    std::vector<std::string> files;
    DIR* dir = opendir(folder.c_str());
    if (!dir) {
        std::cerr << "Cannot open folder: " << folder << "\n";
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        std::string filename = entry->d_name;
        files.push_back(folder + "/" + filename);
    }
    closedir(dir);

    std::sort(files.begin(), files.end());
    for (const auto& path : files)
    {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (!tex) {
            std::cerr << "Failed to load " << path << ": " << IMG_GetError() << "\n";
            continue;
        }
        textures.push_back(tex);
        cnt++;
    }

    std::cout << "Loaded " << cnt << " textures from " << folder << ".\n";
}


SDL_Texture* Renderer::GetTextureByIndex(int index)
{
    if (index < 0 || index >= (int)textures.size()) {
        std::cerr << "Texture index out of range: " << index << "\n";
        return NULL;
    }
    return textures[index];
}

int Renderer::GetTexSize() {return textures.size();}

void Renderer::ClearTex() {textures.clear();}
