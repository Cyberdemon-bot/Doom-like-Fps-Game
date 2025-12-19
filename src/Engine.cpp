#include "Engine.h"

// ===== INITIALIZATION =====

bool Engine::InitRenderer(const char* title, int w, int h, bool fullscreen, bool resizable) {return renderer.OpenWindow(title, w, h, fullscreen, resizable);}

bool Engine::InitUI() {return ui.Init();}

bool Engine::InitAudio() {return audioManager.Init();}

bool Engine::InitMap(const std::vector<std::vector<int>>& miniMap) {return worldMap.Init(miniMap);}

bool Engine::InitPlayer(std::pair<float, float> pos, float angle, float speed, float hp) {return player.Init(pos, angle, speed, hp);}

void Engine::SetupConnections()
{
    // Connect modules that depend on each other
    physicsManager.ImportEntity(worldMap, player, clock, sprites);
    renderer.ImportMap(worldMap);
    renderer.ImportPlayer(player);
    renderer.ImportSprites(sprites);
    renderer.ImportInterface(ui);
}

// ===== RENDERING =====

void Engine::ClearScreen() {renderer.Clear();}

void Engine::RenderBackground() {renderer.RenderBackGround();}

void Engine::RenderRayCasting() {renderer.RayCasting();}

void Engine::RenderSprites() {renderer.RenderSprites();}

void Engine::Render2DMap(float scale) {renderer.Render2DMap(scale);}

void Engine::Render2DPlayer(float scale) {renderer.Render2DPlayer(scale);}

void Engine::Render2DSprites(float scale) {renderer.Render2DSprites(scale);}

void Engine::RenderWeapon() {ui.RenderWeapon();}

void Engine::RenderCrosshair() {ui.RenderCrosshair();}

void Engine::RenderHpEffect() {ui.HpEffect(player.GetHp());}

void Engine::RenderEndScreen(int round, int maxScore) {renderer.RenderEnd(round, maxScore);}

void Engine::DisplayFrame() {renderer.Display();}

// ===== AUDIO =====

void Engine::LoadSounds(const std::string& path) {audioManager.LoadSound(path);}

void Engine::LoadMusic(const std::string& path) {audioManager.LoadMusic(path);}

void Engine::PlaySound(const std::string& name, int loops) {audioManager.PlaySound(name, loops);}

void Engine::PlayExclusiveSound(const std::string& name, int loops) {audioManager.PlayExclusiveSound(name, loops);}

void Engine::PlayMusic(const std::string& name, int loops) {audioManager.PlayMusic(name, loops);}

// ===== UI/WEAPONS =====

void Engine::LoadWeapon(const std::string& name, int frames, float delay, float xOffset, float scale) {ui.LoadWeapon(name, frames, delay, xOffset, scale);}

void Engine::ChangeWeapon(int weaponIndex) {ui.ChangeWeapon(weaponIndex);}

void Engine::RunShootAnimation() {ui.RunShootAni();}

bool Engine::IsWeaponAnimationDone() {return ui.IsAniDone();}

void Engine::UpdateWeaponAnimation() {ui.UpdateAnimation();}

// ===== TEXTURES =====

void Engine::LoadBackgroundTexture() {renderer.LoadBG();}

void Engine::LoadTextures(const std::string& path) {renderer.LoadTextures(path);}

void Engine::ClearTextures() {renderer.ClearTex();}

int Engine::GetTextureSize() {return renderer.GetTexSize();}

// ===== PLAYER =====

float Engine::GetPlayerX() const {return player.GetX();}

float Engine::GetPlayerY() const {return player.GetY();}

float Engine::GetPlayerAngle() const {return player.GetA();}

float Engine::GetPlayerHp() const {return player.GetHp();}

void Engine::SetPlayerHp(float hp) {player.SetHp(hp);}

void Engine::SetPlayerPos(std::pair<float, float> pos) {player.ChangePos(pos);}

void Engine::RotatePlayer(float angle) {player.Rotate(angle);}

void Engine::PlayerLookUp() {player.LookUp();}

void Engine::PlayerLookDown() {player.LookDown();}

void Engine::PlayerTakeDamage(float damage) {player.TakeDamage(damage);}

// ===== PHYSICS/MOVEMENT =====

void Engine::MovePlayer(int dir) {physicsManager.MovePly(dir);}

void Engine::MoveSprite(int index, int dir) {physicsManager.MoveSpt(index, dir);}

Sprites* Engine::PerformPlayerRaycast() {return physicsManager.Praycast();}

bool Engine::PerformSpriteRaycast(int index, float fov, float depth) {return physicsManager.Sraycast(index, fov, depth);}

void Engine::UpdateAllSpritesPhysics() {physicsManager.UpdateAllSpt();}

// ===== SPRITES =====

void Engine::AddSprite(const std::string& name, std::pair<int, int> pos, float angle,
                      float speed, float rotSpeed, bool rigid, bool visible,
                      int defaultState, float damage, float range)
{
    sprites.push_back(Sprites(pos.first, pos.second, angle, speed, rotSpeed,
                             rigid, visible, defaultState, renderer.GetTexSize(),
                             name, 100, damage, range));
    renderer.LoadTextures("res/sprites/" + name);
}

void Engine::ClearSprites() {sprites.clear();}

int Engine::GetSpriteCount() const {return static_cast<int>(sprites.size());}

Sprites& Engine::GetSprite(int index) {return sprites[index];}

const std::vector<Sprites>& Engine::GetSprites() const {return sprites;}

void Engine::UpdateSpriteStates() {for (auto& sprite : sprites) sprite.UpdateSprite();}

void Engine::UpdateSpriteAnimations() {for (auto& sprite : sprites) sprite.UpdateAnimation();}

// ===== MAP =====

bool Engine::IsWall(std::pair<int, int> pos) const {return worldMap.FindPos(pos);}

int Engine::GetMapRows() const {return worldMap.GetRow();}

int Engine::GetMapCols() const {return worldMap.GetCol();}

// ===== CLOCK =====

void Engine::Tick(float targetFPS) {clock.tick(targetFPS);}

float Engine::GetDeltaTime() const {return clock.getDeltaTime();}

// ===== CLEANUP =====

void Engine::Cleanup()
{
    ui.CleanUp();
    audioManager.CleanUp();
    renderer.CleanUp();
}
