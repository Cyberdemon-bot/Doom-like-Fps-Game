#include "Game.h"
#include <iostream>

bool Game::Init(const char* title, int w, int h, bool fullscreen, bool resizable, float FPS, const std::vector<std::vector<int>>& miniMap)
{
    // Initialize game state
    running = true;
    MouseFree = false;
    targetFPS = FPS;
    currentWeapon = 0;
    MonCnt = 0;
    maxScore = 0;
    Round = 0;
    MouseClick = false;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Initialize engine modules
    engine.InitUI();
    engine.InitAudio();
    engine.InitMap(miniMap);
    engine.InitPlayer(GetRandomEmptyPosF(), 0.0f, 5.0f, 100.0f);
    engine.InitRenderer(title, w, h, fullscreen, resizable);

    // Setup connections between modules
    engine.SetupConnections();

    // Load resources
    engine.LoadBackgroundTexture();
    engine.LoadTextures("res/texture-doomstyle");
    engine.LoadWeapon("shotgun", SHOTGUN_TOTAL_FRAMES, SHOTGUN_FRAME_DELAY, SHOTGUN_X_OFFSET, SHOTGUN_SCALE);
    engine.LoadWeapon("handgun", HANDGUN_TOTAL_FRAMES, HANDGUN_FRAME_DELAY, HANDGUN_X_OFFSET, HANDGUN_SCALE);
    engine.LoadSounds("res/sound");
    engine.LoadMusic("res/music");
    engine.UpdateAllSpritesPhysics();

    return true;
}

void Game::RebuildData()
{
    // Reset game state
    pausing = false;
    MonCnt = 0;

    // Reset player
    engine.SetPlayerPos(GetRandomEmptyPosF());
    engine.SetPlayerHp(100);

    // Clear and reload resources
    engine.ClearSprites();
    engine.ClearTextures();
    engine.LoadBackgroundTexture();
    engine.LoadTextures("res/texture-doomstyle");

    // Spawn enemies based on round
    for(int i = 1; i <= Round; i++)
    {
        AddSprite("cacodemon", GetRandomEmptyPosF(), 0, 1, 3, 1, 1, 1, 0.2, 1.5);
        AddSprite("cyberdemon", GetRandomEmptyPosF(), 0, 1, 3, 1, 1, 1, 0.2, 5);
    }
    engine.UpdateAllSpritesPhysics();
}

void Game::AddSprite(std::string name, std::pair<int, int> pos, float a, float s, float rs, bool rg, bool vs, int defaultState, float dm, float rag)
{
    engine.AddSprite(name, pos, a, s, rs, rg, vs, defaultState, dm, rag);
    MonCnt++;
}

void Game::HandleEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEMOTION:
                if (!MouseFree)
                {
                    int dx = event.motion.xrel;
                    int dy = event.motion.yrel;
                    float sensitivity = 0.0015f;

                    if (dx != 0)
                        engine.RotatePlayer(dx * sensitivity);

                    if (dy < 0) {
                        engine.PlayerLookUp();
                    } else if (dy > 0) {
                        engine.PlayerLookDown();
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    MouseFree = true;
                }
                if (event.key.keysym.sym == SDLK_SPACE && pausing) {
                    Round = 1;
                    RebuildData();
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (MouseFree) {
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    MouseFree = false;
                }
                else if(event.button.button == SDL_BUTTON_LEFT) {
                    MouseClick = true;
                }
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                    MouseFree = false;
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    MouseFree = true;
                }
                break;
        }
    }
}

void Game::Update()
{
    // Check player death
    if(engine.GetPlayerHp() <= 0) pausing = true;

    // Handle player input
    SDL_PumpEvents();
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    // Movement
    if(keystates[SDL_SCANCODE_W]) engine.MovePlayer(Forward);
    if(keystates[SDL_SCANCODE_S]) engine.MovePlayer(Backward);
    if(keystates[SDL_SCANCODE_A]) engine.MovePlayer(Left);
    if(keystates[SDL_SCANCODE_D]) engine.MovePlayer(Right);

    // Rotation
    if (keystates[SDL_SCANCODE_LEFT]) engine.RotatePlayer(-0.05f); // Assuming default rotation speed
    if (keystates[SDL_SCANCODE_RIGHT]) engine.RotatePlayer(0.05f);
    if (keystates[SDL_SCANCODE_UP]) engine.PlayerLookUp();
    if (keystates[SDL_SCANCODE_DOWN]) engine.PlayerLookDown();

    // Update sprites
    int deadCount = 0;
    const auto& sprites = engine.GetSprites();
    for(unsigned int i = 0; i < sprites.size(); i++)
    {
        if(sprites[i].IsDead()) deadCount++;
    }
    engine.UpdateSpriteStates();
    engine.UpdateSpriteAnimations();

    // Check for round completion
    if(deadCount == MonCnt)
    {
        Round++;
        RebuildData();
    }
    UpdateAI();

    // Weapon switching
    if (keystates[SDL_SCANCODE_1])
    {
        currentWeapon = 0;
        engine.ChangeWeapon(0);
    }
    if (keystates[SDL_SCANCODE_2])
    {
        currentWeapon = 1;
        engine.ChangeWeapon(1);
    }

    // Shooting
    if (MouseClick && engine.IsWeaponAnimationDone())
    {
        Sprites* target = engine.PerformPlayerRaycast();
        if(target != nullptr && currentWeapon == 0) target->TakeDamage(30);
        if(target != nullptr && currentWeapon == 1) target->TakeDamage(20);

        engine.RunShootAnimation();

        if(currentWeapon == 0) engine.PlayExclusiveSound("shotgun", 0);
        else if(currentWeapon == 1) engine.PlayExclusiveSound("shotgun", 0);

        MouseClick = false;
    }
    engine.UpdateWeaponAnimation();
}

void Game::RenderGame()
{
    engine.ClearScreen();
    engine.RenderBackground();
    engine.RenderRayCasting();
    engine.RenderSprites();
    engine.Render2DMap(10.0f);
    engine.Render2DPlayer(10.0f);
    engine.Render2DSprites(10.0f);
    engine.RenderWeapon();
    engine.RenderCrosshair();
    engine.RenderHpEffect();
    engine.DisplayFrame();
}

void Game::Clean()
{
    engine.Cleanup();
    IMG_Quit();
    SDL_Quit();
}

void Game::Run()
{
    while(running)
    {
        engine.Tick(targetFPS);
        HandleEvent();
        if(!pausing)
        {
            Update();
            RenderGame();
        }
        else {
            maxScore = std::max(maxScore, Round - 1);
            engine.RenderEndScreen(Round - 1, maxScore);
        }
    }
}

std::pair<float, float> Game::GetRandomEmptyPosF()
{
    int rows = engine.GetMapRows();
    int cols = engine.GetMapCols();
    if (rows <= 0 || cols <= 0) return std::make_pair(-1.f, -1.f);

    static std::mt19937 rng;
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned int>(now));
    std::uniform_int_distribution<int> distRow(0, rows - 1);
    std::uniform_int_distribution<int> distCol(0, cols - 1);

    const int dx[] = {-1, -1, -1,  0, 0, 1, 1, 1};
    const int dy[] = {-1,  0,  1, -1, 1, -1, 0, 1};

    const int maxTries = 1000;
    int tries = 0;

    while (tries < maxTries)
    {
        int i = distRow(rng);
        int j = distCol(rng);

        if (engine.IsWall(std::make_pair(i, j))) {
            tries++;
            continue;
        }

        bool hasWallNearby = false;
        for (int d = 0; d < 8; ++d)
        {
            int ni = i + dx[d];
            int nj = j + dy[d];
            if (ni < 0 || ni >= rows || nj < 0 || nj >= cols) {
                hasWallNearby = true;
                break;
            }
            if (engine.IsWall(std::make_pair(ni, nj))) {
                hasWallNearby = true;
                break;
            }
        }

        if (!hasWallNearby)
        {
            float x = static_cast<float>(j) + 0.5f;
            float y = static_cast<float>(i) + 0.5f;
            return std::make_pair(x, y);
        }

        tries++;
    }

    return std::make_pair(-1.f, -1.f);
}

int Game::CalculateHeuristic(std::pair<int, int> a, std::pair<int, int> b) {
    return std::abs(a.first - b.first) + std::abs(a.second - b.second);
}

std::vector<std::pair<int, int>> Game::FindPathAStar(std::pair<int, int> start, std::pair<int, int> end) {
    std::vector<Node*> openList;
    std::vector<Node*> closedList;
    std::vector<std::pair<int, int>> path;

    Node* startNode = new Node(start.first, start.second, 0, CalculateHeuristic(start, end), nullptr);
    openList.push_back(startNode);

    while (!openList.empty()) {
        auto it_current = openList.begin();
        Node* currentNode = *it_current;
        for (auto it = openList.begin(); it != openList.end(); ++it) {
            if ((*it)->f() < currentNode->f()) {
                currentNode = *it;
                it_current = it;
            }
        }

        if (currentNode->y == end.first && currentNode->x == end.second) {
            Node* temp = currentNode;
            while (temp != nullptr) {
                path.push_back({temp->y, temp->x});
                temp = temp->parent;
            }
            std::reverse(path.begin(), path.end());
            break;
        }

        openList.erase(it_current);
        closedList.push_back(currentNode);

        int dy[] = {-1, 1, 0, 0};
        int dx[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            std::pair<int, int> neighborPos = {currentNode->y + dy[i], currentNode->x + dx[i]};

            if (engine.IsWall(neighborPos) || neighborPos.first < 0 || neighborPos.second < 0) {
                continue;
            }

            bool inClosed = false;
            for (Node* node : closedList) {
                if (node->y == neighborPos.first && node->x == neighborPos.second) {
                    inClosed = true;
                    break;
                }
            }
            if (inClosed) continue;

            int gCost = currentNode->g + 1;
            int hCost = CalculateHeuristic(neighborPos, end);
            Node* neighborNode = new Node(neighborPos.first, neighborPos.second, gCost, hCost, currentNode);

            bool inOpen = false;
            for (Node* node : openList) {
                if (node->y == neighborPos.first && node->x == neighborPos.second) {
                    inOpen = true;
                    if (gCost < node->g) {
                        node->g = gCost;
                        node->parent = currentNode;
                    }
                    delete neighborNode;
                    break;
                }
            }
            if (!inOpen) {
                openList.push_back(neighborNode);
            }
        }
    }

    for (auto node : openList) delete node;
    for (auto node : closedList) delete node;
    return path;
}

void Game::UpdateAI()
{
    const float SIGHT_FOV = PI / 2.0f;
    const float SIGHT_DEPTH = 20.0f;
    const float MIN_WAYPOINT_DIST = 0.5f;
    static int path_recalculation_timer = 0;
    const int PATH_RECALC_COOLDOWN = 30;
    float dt = engine.GetDeltaTime();
    int playerGridX = static_cast<int>(engine.GetPlayerX());
    int playerGridY = static_cast<int>(engine.GetPlayerY());

    path_recalculation_timer++;

    for (int i = 0; i < engine.GetSpriteCount(); ++i)
    {
        auto& ent = engine.GetSprite(i);
        if (ent.IsDead() || (ent.GetState() == 4 && !ent.CheckAni())) continue;
        float ATTACK_RANGE = ent.range;
        float DAMAGE = ent.damage;

        float spriteX = ent.GetX();
        float spriteY = ent.GetY();
        float playerX = engine.GetPlayerX();
        float playerY = engine.GetPlayerY();

        float dx = playerX - spriteX;
        float dy = playerY - spriteY;
        float distanceToPlayer = std::sqrt(dx * dx + dy * dy);
        bool canSeePlayer = engine.PerformSpriteRaycast(i, SIGHT_FOV, SIGHT_DEPTH);

        if (distanceToPlayer <= ATTACK_RANGE && canSeePlayer)
        {
            ent.SetState(3);
            if(!ent.CheckAni())
            {
                engine.PlayerTakeDamage(DAMAGE);
                engine.PlaySound("player_pain", 0);
            }
            continue;
        }

        if (canSeePlayer) {
            ent.path.clear();
            float angleToPlayer = atan2(dy, dx);
            float currentAngle = ent.GetA();
            float angleDiff = angleToPlayer - currentAngle;
            while (angleDiff <= -PI) angleDiff += 2 * PI;
            while (angleDiff > PI) angleDiff -= 2 * PI;

            float maxRotation = ent.GetR() * dt;
            float rotation = std::max(-maxRotation, std::min(maxRotation, angleDiff));
            ent.Rotate(rotation);

            engine.MoveSprite(i, Forward);
            ent.SetState(1);
        }
        else {
            if (path_recalculation_timer >= PATH_RECALC_COOLDOWN) {
                int spriteGridX = static_cast<int>(spriteX);
                int spriteGridY = static_cast<int>(spriteY);
                ent.path = FindPathAStar({spriteGridY, spriteGridX}, {playerGridY, playerGridX});
            }

            if (!ent.path.empty()) {
                float targetX = static_cast<float>(ent.path[0].second) + 0.5f;
                float targetY = static_cast<float>(ent.path[0].first) + 0.5f;
                float dx_path = targetX - spriteX;
                float dy_path = targetY - spriteY;
                float distanceToWaypoint = std::sqrt(dx_path * dx_path + dy_path * dy_path);

                if (distanceToWaypoint < MIN_WAYPOINT_DIST) {
                    ent.path.erase(ent.path.begin());
                    if (ent.path.empty()) continue;
                }

                float angleToTarget = atan2(dy_path, dx_path);
                float currentAngle = ent.GetA();
                float angleDiff = angleToTarget - currentAngle;
                while (angleDiff <= -PI) angleDiff += 2 * PI;
                while (angleDiff > PI) angleDiff -= 2 * PI;

                float maxRotation = ent.GetR() * dt;
                float rotation = std::max(-maxRotation, std::min(maxRotation, angleDiff));
                ent.Rotate(rotation);

                engine.MoveSprite(i, Forward);
                ent.SetState(1);
            } else {
                ent.SetState(ent.GetDefaultState());
            }
        }
    }

    if (path_recalculation_timer >= PATH_RECALC_COOLDOWN) {
        path_recalculation_timer = 0;
    }
}
