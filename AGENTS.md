# AGENTS.md — 3D OpenGL Engine Project Guide

This file gives AI coding assistants (like opencode) the project context needed to help effectively.

---

## Project Overview

A lightweight 3D game engine built with OpenGL 4.x, C++11, GLFW, and TinyGLTF. The game ("killdeathcpp") is a first-person 3D action game with skinned glTF models, physics-based collision, melee/ranged combat, enemy AI, and a UI menu system.

**Dual goals:** both a shipping game and a learning/portfolio project.

**Priorities (in order):**
1. **Correctness** — no crashes, no undefined behavior, no silent bugs
2. **Code clarity** — readable, self-documenting, easy to navigate
3. **Speed of iteration** — quick to write and test, pragmatic where it counts
4. **Modularity** — clean interfaces, loose coupling, easy to swap subsystems

**Active entry point:** `killdeathcpp/window.cpp` (GLFW/OpenGL).  
**Legacy (ignore for new work):** `main.c` is a Win32 software-rendered 2D prototype — not the active game.

---

## Build System

### Visual Studio
- Open `killdeathcpp.sln` and build.
- The build output goes to `x64/Debug/` or `x64/Release/`.

### CMake (for IDE integration / reference)
- `CMakeLists.txt` at the repo root. Currently fetches `glm` via FetchContent.
- Note: The CMake file is minimal and may not reflect the full VS project.

---

## Project Layout

```
3d-OpenGl-engine/
  killdeathcpp/              # Main engine source
    window.cpp               # Entry point — game loop, init, input
    Renderer.h/.cpp          # Central rendering orchestrator
    Mesh.h/.cpp              # GPU mesh (VAO/VBO/IBO) wrapper
    Material.h/.cpp          # Shader uniform + texture binding
    shader.h                 # Shader compilation, uniform cache
    tinyModel.h              # glTF model: loading, animation, skinning, hitboxes
    modelHelpers.h           # TinyGLTF data conversion, texture baking
    ObjectOrientation.h/.cpp # Position/rotation/scale + model/view/proj matrices
    Geometry3D.h             # Vertex, AABB, Triangle, Texture structs
    Lights.h                 # DirLight/PointLight/Spotlight uniform binding
    openglHelpers.h/.cpp     # GL error checking, VBO/VAO/IBO creation, BMP textures
    Camera.h                 # First-person camera (yaw/pitch, movement vectors)
    Collisions.h             # Collision detection: AABB, capsule, triangle mesh, segment
    ColisionManager.h        # Orchestrates all collision checks each frame
    CollisionResponse.h      # Push-back collision resolution
    HitBox.h                 # Hitbox shapes (sphere/capsule/box), AABB computation
    Player.h                 # Player movement, gravity, health
    Enemy.h                  # Enemy AI state machine (IDLE/PATROL/CHASE/ATTACK/HIT/DEAD)
    EnemyManager.h           # Manages all enemies, delegates update/render
    Weapon.h                 # Weapon with melee+range attacks, camera-attached rendering
    WeaponManager.h          # Active weapon switching (keys 1/2/3)
    Attack.h                 # Unified melee/projectile attack abstraction
    meele.h                  # Melee sweep capsule mechanics
    Projectile.h             # Projectile types + object pool
    ProjectileManager.h      # Projectile pool management
    SceneManager.h           # Scene graph: updates animations, AABBs, renders all objects
    GameState.h              # Save/restore player + enemy state for death/reload
    UIManager.h              # Screen stack (current active screen)
    UIScreen.h               # Screen with vector of UIElement unique_ptrs
    UIElement.h              # Base class for UI widgets (Update/Render/OnMouseClick)
    Button.h                 # Clickable quad button with callback
    TextLabel.h              # Static text label
    MainMenuScreen.h         # Title screen with Start/Options/Quit
    PauseScreen.h            # Pause overlay with Resume/Options/Quit
    OptionsScreen.h          # Placeholder options with Back button
    TextRenderer.h/.cpp      # stb_truetype font atlas -> OpenGL text rendering
    soundEngine.h/.cpp       # SDL2 audio wrapper (WAV playback)
    Wav.h/.cpp               # XAudio2-based WAV loading (older alternate)
    Animation.h              # (Legacy) Assimp-based animation container — NOT actively used
    Animator.h               # (Legacy) Assimp-based animation driver — NOT actively used
    Bone.h                   # (Legacy) Assimp-based bone with keyframes — NOT actively used
    misc.h                   # Constants: screen res (1024x768), collision radius
    MathHelpers.h            # Direction vector calculation
    Entity.h                 # Empty base class (unused)
    objects.h                # MeshObject / ModelObject composition structs
    stb_image.h              # stb image loading header
    stb_truetype.h           # stb truetype font rendering header
    thirdparty_impl.cpp      # TINYGLTF_IMPLEMENTATION + STB implementations
  shaders/                   # GLSL vertex + fragment shaders (25 files)
    gltfModel.vs/.fs         # Standard model rendering
    modelAnimation.vs/.fs    # Skinned model (joint matrices uniform)
    animation.vs/.fs         # (Legacy) Assimp-style bone animation shader
    basiclighting.vs/.fs     # Phong lighting (ambient+diffuse+specular)
    lightingMap.vs/.fs       # Textured material lighting
    materialLighting.fs      # Non-textured material lighting
    multiLight.fs            # Directional + 4 point lights + spotlight
    lightcube.vs/.fs         # Light source visualizer
    quad3d.vs/.fs            # Textured 3D quad
    debugShader.vs/.fs       # Wireframe debug drawing (solid color)
    text.vs/.fs              # Font atlas text rendering
    ui.vs/.fs                # 2D UI element rendering
    model_load.vs/.fs        # Basic model shader
  models/                    # glTF model assets
    mina/                    # Skinned character
    skeleton/                # Skinned skeleton enemy
    backpack/                # Static prop
    debug_fps_gun/           # Player weapon model
    debug_sword/             # Melee weapon
    glock/                   # Ranged weapon
    simple_meele_enemy/      # Melee enemy
    chair/                   # Static furniture
    Scaniverse/              # Scanned prop
  assets/                    # Game assets
    background.bmp           # Background image
    monster1.bmp             # Monster sprite
    ahem_x.wav               # Sound effect
  libraries/                 # Third-party includes + libs
  glad.c                     # GLAD OpenGL loader
```

---

## Architecture & Data Flow

### Entity Model: Composition (not inheritance)
Game objects are **structs** that compose `MeshObject` or `ModelObject`, which themselves compose:
- `Mesh` (GPU vertex data) or `tinyModel` (glTF scene)
- `ObjectOrientation` (position/rotation/scale + model/view/proj matrices)
- `ObjectCollision` (AABBs, capsule hitboxes)

Key composite types in `objects.h`:
- `MeshObject` — `Mesh + ObjectOrientation + ObjectCollision`
- `ModelObject` — `tinyModel + ObjectOrientation + ObjectCollision`

### Game Loop (window.cpp)

```
Per frame:
  1. Input processing (keyboard/mouse -> camera, player, weapons, UI)
  2. SceneManager::Update()
     - model.updateAnimation(dt)      # Advance glTF animation samplers
     - model.updateNodeTransforms()   # Recursive TRS -> local/global matrices
     - model.updateSkins()            # Joint matrices = globalMatrix * inverseBindMatrix
     - Update world AABBs, capsule locations
  3. EnemyManager::Update()
     - AI state machine (IDLE->PATROL->CHASE->ATTACK)
     - Fires projectiles, switches animations per state
  4. ProjectileManager::Update()
     - Object pool: advance position, decrement lifetime, deactivate
  5. WeaponManager::Update()
     - Position weapon relative to camera
     - Update melee sweep capsule, check collisions
  6. ColissionManager::Check*(...)
     - Player vs scene geometry (AABB vs triangle mesh)
     - Player vs enemy capsules
     - Projectile vs player/enemy
     - Melee sweep vs enemies
  7. Render
     - Scene (static meshes + animated models)
     - Enemies (animated models)
     - Projectiles
     - Weapon (drawn on top of other geometry)
     - UI / HUD text
```

### Animation System (TinyGLTF — active, NOT Assimp)

Inside `tinyModel.h`:
- **AnimationSampler** — stores input keyframe times, output transform values, interpolation type
- **AnimationChannel** — binds a sampler to a node index + path (translation/rotation/scale)
- `setAnimation(index)` — switches active animation
- `updateAnimation(deltaTime, loop)` — interpolates TRS per channel (lerp for pos/scale, slerp for rotation), writes into node transforms
- `updateNodeTransforms()` — recursively builds local then global matrices from node TRS
- `updateSkins()` — jointMatrices[i] = nodes[skin.joints[i]].globalMatrix * inverseBindMatrices[i]

**Shader** (`modelAnimation.vs`): expects `jointMatrices[128]` uniform array and per-vertex `joints` (uvec4) + `weights` (vec4).

The legacy Assimp animation system (`Bone.h`, `Animation.h`, `Animator.h`) is present in the repo but **not used** by the active game loop.

### Collision System

- **Per-object:** `ObjectCollision` holds model-space AABB, world AABB, optional triangle-mesh hitbox (`VerticeHitBox`) and capsule hitboxes
- **Hitbox types:** sphere, capsule, box — defined per-model in glTF metadata
- **Detection functions** (in `Colissions.h`):
  - `AABBvsAABB` / `AABBvsAABBContact` — broad phase
  - `pointInCapsule` / `capsuleVsAABB` — narrow phase
  - `AABBvsVertBox` — mesh triangle soup vs AABB
  - `segmentVsAABB` — raycast / slab method
- **Resolution** (`CollisionResponse.h`): simple push-back along contact normal

### Weapon System

- `Weapon` struct wraps a `ModelObject` reference + two `Attack` members (melee and ranged)
- `Attack` is a union-style struct — `initMelee()` sets up a `MeeleAttack`, `initProjectile()` sets up a `ProjectileType`
- Melee: `MeeleAttack` sweeps a capsule between previous/current weapon tip positions each frame, tracking `hitEnemyIds` to avoid double-hits
- Ranged: spawns a `Projectile` from the object pool via `ProjectileManager`

### Shader Pipeline

Shaders are compiled from `.vs`/`.fs` files via `shader.h` (Shader class). Uniforms are set either:
- Directly in `window.cpp` (e.g., light uniforms, view/proj)
- Through `Material` (maps of uniform name -> value, applied via `material.apply()`)
- Through `ObjectOrientation` (set model/view/proj via `shader.setObjectOrientation()`)

---

## Common Development Tasks

### Adding a new model
1. Place glTF files in `models/<name>/`
2. Load in `window.cpp`: `tinyModel model("models/<name>/scene.gltf");`
3. Add a `ModelObject` wrapper with position/scale
4. Register with `SceneManager` if it's a world object

### Adding a new shader
1. Create `.vs` and `.fs` files in `shaders/`
2. Construct a `Shader` object: `Shader myShader("shaders/my.vs", "shaders/my.fs");`
3. Create a `Material` referencing the shader and set uniforms via `material.floatUniforms["uName"] = value;`
4. Draw using `renderer.draw(mesh, orientation, material);`

### Adding a new enemy type
1. Define a struct or extend `Enemy`/`EnemyModel` in `Enemy.h` with new AI behavior
2. Implement `UpdateEnemy()` overload with custom state logic
3. Register in `EnemyManager` from `window.cpp`

### Adding a new UI screen
1. Create a screen class (e.g., `MyScreen.h`) that inherits `UIScreen`
2. In the constructor, call `AddElement<Button>(...)` / `AddElement<TextLabel>(...)`
3. Set it via `uiManager.SetScreen(make_unique<MyScreen>(...));`

---

## Key Conventions

- **C++11** standard (no C++17/20 features)
- **GLM** for math (vec3, mat4, quat, etc.)
- **TinyGLTF** for glTF loading (single translation unit via `thirdparty_impl.cpp`)
- **stb_image** + **stb_truetype** for image and font loading
- **No raw OpenGL state changes** outside of `Renderer`/`Mesh`/`Material`/`Shader`
- **Objects are composed, not inherited** — see `objects.h` for the building blocks
- **Error handling**: `checkGLError()` after GL calls, shader compile errors are printed to stdout

## Coding Style

- **PascalCase** for classes/structs (`MeshObject`, `ObjectOrientation`, `ColissionManager`)
- **camelCase** for variables, methods, and parameters (`currentFrame`, `updateAnimation()`, `load_model()` for TinyGLTF callbacks)
- **Early return pattern** — guard clauses at function top, avoid deep nesting
- **No Hungarian notation** — no `m_` prefix, no type-prefixing
- **Minimal dependencies** — prefer single-header stb-style libraries over heavy frameworks
- **Header-only when reasonable** — many engine headers contain full implementations (Collisions.h, HitBox.h, Player.h, etc.)

## Common Gotchas & Bug Patterns

### Collision / Hitbox Coordinate Bugs (biggest recurring issue)
- **Wrong coordinate space:** hitbox checks must be in world space; local/model-space values will silently give wrong results
- **Stale AABBs:** after moving/animating an object, `updateWorldAABB()` or `updateWorldAABBV()` must be called before collision checks
- **Wrong update order:** SceneManager must update before EnemyManager, AABBs must be refreshed before collision passes
- **Uncalled functions:** `updateSkins()`, `updateNodeTransforms()`, `updateCapsuleLocs()` are each required after specific mutations — forgetting any one produces hard-to-debug ghost collisions or missed hits
- **Capsule endpoints must track animation:** after animation advances, capsule world positions derived from joint transforms need recomputation via `computeCapsuleWorld()`
- **Melee sweep capsule:** `getCurrentTipWorld()` must be called once per frame to record both previous and current tip positions; if either is stale, the sweep volume will be wrong

### Animation
- **`updateAnimation(dt)`** modifies node TRS but does NOT cascade to global matrices or skins — must call `updateNodeTransforms()` then `updateSkins()` after
- **Model matrix chain:** `finalOrientation.modelMatrix = rootMatrix * node.globalMatrix` in Renderer; if rootMatrix (the ObjectOrientation) is not set, the model renders at origin
- **`setAnimation(index)`** resets sampler state; call it before the per-frame update, not during

### Rendering
- **Uniform name typos** will silently fail (glGetUniformLocation returns -1, no error)
- **Material uniforms must match shader expectations** — `ValidateUniforms()` in Material.h can help debug this
- **Shader files are reloaded each time the engine starts** — no hot-reload, must recompile and restart to see changes
- **Textures loaded via `create2DBitMapTexture()`** expect BMP format; other formats will fail

### General
- **Check all function calls** — many engine functions rely on manual sequencing (e.g., update -> render sequence must be explicit)
- **`window.cpp` is the source of truth** for initialization order, update order, and render order — when adding features, follow the patterns there
