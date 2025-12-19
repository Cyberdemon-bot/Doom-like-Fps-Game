# DOOM-Style FPS Game

A classic first-person shooter built from scratch in C++ using SDL2, featuring raycasting rendering, AI pathfinding, and retro DOOM-inspired gameplay.

## ✨ Features

### Core Engine
- **Raycasting 3D Renderer**: Classic DOOM-style pseudo-3D rendering with texture mapping
- **Multi-Platform Support**: Runs on Windows and macOS with CMake build system
- **Modular Architecture**: Clean separation between rendering, physics, audio, and game logic

### Gameplay
- **AI Pathfinding**: A* algorithm for intelligent enemy navigation
- **Multiple Enemy Types**: Cacodemon and Cyberdemon with unique stats and animations
- **Weapon System**: Multiple weapons (Shotgun, Handgun) with frame-based sprite animations
- **Round-Based Survival**: Progressive difficulty - enemies scale with each round
- **Health System**: Visual damage feedback with screen overlay effects

### Graphics & Audio
- **Texture Mapping**: Perspective-correct wall textures with multiple texture support
- **Sprite Rendering**: 8-directional sprite rendering with proper depth sorting
- **Dynamic Lighting**: Distance-based shading for atmospheric depth
- **Pitch Control**: Look up/down functionality with horizon adjustment
- **Audio Manager**: Music and sound effects with exclusive channel control
- **2D Minimap**: Real-time overhead view for navigation

### Physics & Collision
- **Spatial Partitioning**: Grid-based collision detection for performance
- **Raycasting Physics**: Wall collision and sprite visibility checks
- **Entity Management**: Rigid body collision between player, enemies, and walls

## 📋 Requirements

### Common
- C++17 compatible compiler
- CMake 3.10 or higher
- SDL2, SDL2_image, SDL2_ttf, SDL2_mixer

### Platform Specific

**macOS:**
- Homebrew/Devlib folder (included in repository)
- clang

**Windows:**
- Visual Studio Build Tools (MSVC)
- Devlib folder (included in repository)

## 🚀 How to Compile and Run

### macOS

```bash
## Using Homebrew
# Install dependencies via Homebrew
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer cmake

# Build the project
cmake -S . -B build
cmake --build build

# Run
./build/main

## Using Framework
# Open Devlib folder and open all dmg file
# Choose .framework folder and move to ~/Library/Frameworks
# Disable macos's security mechanism to your framework
sudo xattr -r -d com.apple.quarantine /Library/Frameworks/SDL2*.framework

# Build the project
cmake -S . -B build
cmake --build build

# Run
./build/main
```

### Windows

```bash
# Prerequisites: Install Visual Studio Build Tools with C++ support

# Build the project (x64)
cmake -S . -B build -A x64
cmake --build build

# Run (DLLs are automatically copied to output directory)
build\Debug\main.exe
```

## 🎮 Controls

| Input | Action |
|-------|--------|
| **W / A / S / D** | Move forward / left / backward / right |
| **Mouse Movement** | Look around (camera rotation) |
| **Mouse Up/Down** | Look up / down (pitch control) |
| **Arrow Keys** | Alternative rotation controls |
| **Left Mouse Button** | Shoot |
| **1 / 2** | Switch weapons |
| **ESC** | Release mouse / Pause |
| **Space** | Restart game (when dead) |

## 🏗️ Project Structure

```
.
├── src/                      # Source files
│   ├── main.cpp             # Entry point with map definition
│   ├── Game.cpp             # Game loop and state management
│   ├── Engine.cpp           # Core engine facade
│   ├── Renderer.cpp         # Raycasting and rendering
│   ├── Physics.cpp          # Collision detection and raycasting
│   ├── Player.cpp           # Player entity
│   ├── Sprites.cpp          # Enemy/sprite entity
│   ├── Map.cpp              # World map management
│   ├── Audio.cpp            # Audio playback system
│   ├── Interface.cpp        # UI and weapon rendering
│   └── Clock.cpp            # Frame timing and FPS control
├── include/                 # Header files
├── res/                     # Resources (not included)
│   ├── texture-doomstyle/  # Wall textures
│   ├── sprites/            # Enemy sprite sheets
│   ├── weapon/             # Weapon animations
│   ├── sound/              # Sound effects (.wav)
│   ├── music/              # Background music (.mp3, .ogg)
│   ├── bg/                 # Sky texture
│   └── font/               # Fonts for UI
├── CMakeLists.txt          # Build configuration
└── Devlib/                 # Windows SDL2 libraries 
```

## 🎯 Architecture Overview

### Engine Module
The `Engine` class acts as a facade, coordinating between subsystems:
- **Renderer**: Handles all 3D rendering and raycasting
- **Physics Manager**: Collision detection, movement, and raycasting queries
- **Audio Manager**: Sound and music playback
- **Interface**: UI elements, weapons, and crosshair
- **World Map**: Grid-based level representation

### Game Loop Flow

```
Initialize
    ↓
┌─────────────────────┐
│   Handle Events     │ → Mouse/keyboard input
└──────────┬──────────┘
           ↓
┌─────────────────────┐
│   Update Game       │ → AI, physics, player state
│   - Update AI       │ → A* pathfinding
│   - Move Entities   │ → Physics checks
│   - Check Victory   │ → Round completion
└──────────┬──────────┘
           ↓
┌─────────────────────┐
│   Render Frame      │
│   - Raycasting      │ → Wall rendering
│   - Sprite Sort     │ → Depth sorting
│   - Sprite Render   │ → Draw enemies
│   - UI/Weapons      │ → Overlay HUD
└──────────┬──────────┘
           ↓
┌─────────────────────┐
│   Display & Sync    │ → Present frame, maintain FPS
└─────────────────────┘
```

### Raycasting Algorithm

The renderer uses Digital Differential Analysis (DDA) raycasting:

1. **Cast rays** from player position across FOV (60°)
2. **Check intersections** with vertical and horizontal grid lines
3. **Find nearest wall** for each ray
4. **Apply fish-eye correction**: `correctedDistance = distance * cos(rayAngle - playerAngle)`
5. **Calculate wall height**: `wallHeight = screenHeight / correctedDistance`
6. **Render textured column** with perspective-correct texture mapping

### AI System

Enemies use a hybrid behavior system:

**Direct Sight Mode:**
- Raycast to detect player within FOV (90°) and range (20 units)
- Rotate toward player and move forward
- Switch to attack animation when in range

**Pathfinding Mode:**
- Use A* to calculate path to player
- Navigate through waypoints
- Recalculate path every 30 frames

**Attack Mode:**
- Trigger attack animation when close enough
- Deal damage on animation completion

## 🎨 Customization

### Adding New Maps

Edit the `miniMap` array in `main.cpp`:

```cpp
std::vector<std::vector<int>> miniMap = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,2,3,0,0,1},
    {1,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1}
};
```

- `0` = Empty space
- `1-5` = Different wall textures

### Adding Weapons

In `Game::Init()`:

```cpp
engine.LoadWeapon(
    "weapon_name",     // Weapon folder in res/weapon/
    TOTAL_FRAMES,      // Number of animation frames
    FRAME_DELAY,       // Frames to wait between animation updates
    X_OFFSET,          // Horizontal screen offset
    SCALE             // Size multiplier
);
```

### Adding Enemy Types

In `Game::RebuildData()`:

```cpp
AddSprite(
    "enemy_name",      // Sprite folder in res/sprites/
    position,          // {x, y} spawn location
    angle,            // Initial rotation (radians)
    moveSpeed,        // Movement speed
    rotSpeed,         // Rotation speed
    isRigid,          // Collision enabled
    isVisible,        // Render enabled
    defaultState,     // Idle animation state
    damage,           // Attack damage
    attackRange       // Attack distance
);
```

## ⚙️ Configuration

### Frame Rate
Modify target FPS in `main.cpp`:
```cpp
mainGame.Init("FPS_GAME", 1366, 768, 0, 1, 60, miniMap);
//                         ^^    ^^        ^^    ^^
//                        width height     FPS  target map
```

### Player Stats
Adjust in `Game::Init()`:
```cpp
engine.InitPlayer(
    GetRandomEmptyPosF(),  // Position
    0.0f,                  // Initial angle
    5.0f,                  // Movement speed
    100.0f                 // Health points
);
```

### AI Behavior
Tune AI parameters in `Game::UpdateAI()`:
```cpp
const float SIGHT_FOV = PI / 2.0f;        // 90° field of view
const float SIGHT_DEPTH = 20.0f;          // 20 units sight range
const float MIN_WAYPOINT_DIST = 0.5f;     // Path following precision
const int PATH_RECALC_COOLDOWN = 30;      // Frames between pathfinding
```

## 🔧 API Reference

### Game Class

```cpp
// Initialize game
bool Init(const char* title, int width, int height, 
          bool fullscreen, bool resizable, float FPS, 
          const std::vector<std::vector<int>>& miniMap);

// Main game loop
void Run();

// Add enemy sprite
void AddSprite(std::string name, std::pair<int, int> pos, 
               float angle, float speed, float rotSpeed,
               bool rigid, bool visible, int defaultState,
               float damage, float range);
```

### Engine Class

```cpp
// Rendering
void ClearScreen();
void RenderBackground();
void RenderRayCasting();
void RenderSprites();
void DisplayFrame();

// Player control
void MovePlayer(int direction);
void RotatePlayer(float angle);
Sprites* PerformPlayerRaycast();  // Get targeted enemy

// Sprite management
void AddSprite(...);
Sprites& GetSprite(int index);
void UpdateSpriteStates();
```

### Physics System

```cpp
// Movement with collision
void MovePly(int direction);
void MoveSpt(int index, int direction);

// Raycasting
Sprites* Praycast();  // Player to sprite
bool Sraycast(int spriteIndex, float fov, float depth);  // Sprite to player

// Collision detection
bool Check_wall(float x, float y);
std::vector<Sprites*> CheckSptCollision(float x, float y, Sprites* ignore);
```

## 📊 Performance

The game targets 60 FPS and includes:
- Frame-rate limiting with SDL_Delay
- Delta time compensation for consistent movement
- Spatial partitioning for efficient collision detection
- Depth-sorted sprite rendering

**Typical Performance:**
- 1366x768 resolution: ~60 FPS
- Raycasting: ~1300 rays per frame
- Supports multiple enemies with AI

## 🐛 Known Limitations

- No texture filtering (nearest-neighbor only)
- Limited to grid-aligned walls (no diagonal walls)
- Sprite rendering limited to 8 directions
- No jumping or vertical movement
- No multiplayer support
- Enemies can path through each other

## 🎓 Learning Resources

This project demonstrates:
- **Raycasting algorithms** (DDA grid traversal)
- **Game engine architecture** (entity-component patterns)
- **A* pathfinding** implementation
- **Spatial partitioning** for collision detection
- **Frame-based animation systems**
- **Cross-platform C++ development**

## 🤝 Contributing

This is an educational project. Feel free to fork and experiment with:
- Additional weapons and enemy types
- New map generators
- Advanced AI behaviors
- Particle effects
- Level editor
