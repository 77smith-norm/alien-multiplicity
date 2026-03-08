# Alien Multiplicity — Game Design Specification

**Version:** 0.1  
**Author:** Russell Dillin / Norm  
**Date:** 2026-03-08  
**Engine:** LiteGDK (C++20 / raylib / WASM-deployable)  
**Repo:** https://github.com/77smith-norm/alien-multiplicity

---

## 1. Concept

**Alien Multiplicity** is a single-screen arcade game in the spirit of _Mario Bros._ (1983), _Pac-Man_ (1980), and _Super Crate Box_ (2010). The player is a soldier who must survive wave after wave of alien eyeball creatures that multiply when shot.

The core twist: shooting an alien doesn't kill it outright — it splits into smaller, more numerous versions of itself. Only the smallest fragments can be fully vaporized. Every split makes the threat worse before it gets better. Manage the screen, manage the crowd.

---

## 2. Inspirations

| Game | Influence |
|------|-----------|
| **Mario Bros.** (1983) | Single screen, enemies bounce around |
| **Pac-Man** (1980) | Pattern recognition, escalating pressure |
| **Super Crate Box** (2010) | Arcade intensity, score-via-crates loop |
| **D&D Beholder** | The alien's floating eyeball aesthetic |
| **Contra / Halo** | The soldier's visual design — armor, visor, rifle |

---

## 3. Game Overview

- **Genre:** Single-screen arcade shooter
- **Players:** 1
- **Camera:** Fixed, orthographic 2D (no scrolling)
- **Win condition:** None — score attack / survival
- **Lose condition:** Any alien touches the player → game over
- **Platform targets:** Web (WASM / GitHub Pages), macOS, Windows, Linux

---

## 4. Player Character — "The Soldier"

### Visuals
Armored Contra/Halo-style soldier. Helmet with visor. Compact sci-fi frame. Carries a **laserbeam rifle** that fires a thin, continuous pulse.

### Controls

| Input | Action |
|-------|--------|
| Arrow Left / A | Move left |
| Arrow Right / D | Move right |
| Arrow Up / W | Jump |
| Space / Left Mouse | Fire laser |
| Aim | Mouse position (laser follows cursor) or 8-directional with keyboard |

### Physics
- Runs on a flat ground plane (no platform layers in v1)
- Single jump (no double jump in v1)
- Laser fires in the aimed direction; thin beam, instant hit, short cooldown (~0.15s)

### Stats
| Stat | Value |
|------|-------|
| Move speed | 200 px/s |
| Jump velocity | 550 px/s up |
| Gravity | 1200 px/s² |
| Lives | 3 |
| Laser cooldown | 0.15s |
| Laser damage | 1 HP per hit |

---

## 5. The Alien — "The Beholder"

### Concept
A floating eyeball creature inspired by the D&D Beholder. Single large central eye, fringe of smaller eyes or tendrils. Glows with bioluminescence. Moves erratically, floats at varying heights, drifts toward the player.

### Split Mechanic

When an alien's HP reaches zero, instead of dying it **splits into two identical copies** at the next smaller tier. Only Tier 3 aliens vaporize on death.

| Tier | Size (px) | HP | Move Speed | Count on Arena Entry |
|------|-----------|----|------------|----------------------|
| 1 — Mega | 64×64 | 3 | 80 px/s | 1 |
| 2 — Mid | 40×40 | 5 | 55 px/s | spawned by T1 split |
| 3 — Mini | 24×24 | 8 | 35 px/s | spawned by T2 split |

**Note:** Smaller aliens have *more* HP per individual, but there are exponentially more of them and they are slower. The pressure curve escalates: 1 → 2 → 4 per lineage per wave.

### Physics Push-Back
Each laser hit applies a **velocity impulse** away from the shot direction (magnitude: 120 px/s). This makes hit-staggering and crowd control feel punchy and kinetic.

### Movement Pattern
- Drifts toward player at base speed ± sinusoidal vertical oscillation (amplitude 40px, period ~2s)
- Occasional random direction change every 1–3s
- Bounces off screen edges (full velocity reflection)
- Cannot move below the ground plane

### Visuals Per Tier
- **T1 Mega:** Full beholder — large iris, ring of small eye tendrils, slow menacing drift
- **T2 Mid:** Smaller version, fewer tendrils, slightly more frantic
- **T3 Mini:** Just the core eyeball + two stubby tendrils, fast twitch movement

---

## 6. Wave Structure

| Wave | Aliens Spawned | Notes |
|------|----------------|-------|
| 1 | 1× T1 | Tutorial feel |
| 2 | 2× T1 | Doubles the chaos |
| 3 | 1× T1 + 1× T2 | Mixed threat |
| 4+ | n+1× T1 | Escalating |

A new wave begins when all aliens on screen are vaporized (no T3 or lower remain). A brief 2-second pause before next wave. Wave number shown in HUD.

---

## 7. Scoring

| Action | Points |
|--------|--------|
| Laser hit on alien | +10 |
| T1 split | +50 |
| T2 split | +25 |
| T3 vaporized | +100 |
| Wave cleared | +500 × wave number |

**Score multiplier:** Consecutive kills within 2s grant 2×, 3×, etc. Resets on hit or timeout.

---

## 8. HUD

```
┌─────────────────────────────────────────────────────┐
│  SCORE: 000000   WAVE: 01   ❤❤❤                    │
│                                                     │
│          [game area]                                │
│                                                     │
└─────────────────────────────────────────────────────┘
```

- Score: top-left
- Wave number: top-center
- Lives (heart icons): top-right
- Brief "WAVE 2" flash on wave start

---

## 9. Game States

```
TITLE → PLAYING → GAME_OVER → TITLE
                ↓
            PAUSED (Esc)
```

- **TITLE:** Logo, "Press Space to Start", hi-score display
- **PLAYING:** Active gameplay
- **PAUSED:** Overlay, "PAUSED — Esc to resume"
- **GAME_OVER:** Score, "Press Space to Restart"

---

## 10. Art Style

- **Resolution:** 800×600 (4:3, classic arcade aspect ratio)
- **Palette:** Dark space background (#0A0A1A), neon greens and purples for aliens, warm orange/amber for soldier
- **Sprites:** Pixel art style, clean black outlines
- **Animations:**
  - Soldier: idle (2f), run (4f), jump (1f), shoot (2f), death (4f)
  - Alien T1: float (4f), hit (2f), split (3f)
  - Alien T2: float (4f), hit (2f), split (3f)
  - Alien T3: float (4f), hit (2f), death/vaporize (4f)

---

## 11. Audio

| Sound | Trigger |
|-------|---------|
| Laser fire | Player shoots |
| Alien hit | Any alien takes damage |
| Split | Alien splits into two |
| Vaporize | T3 alien dies |
| Player hurt | Alien touches player |
| Player death | Player loses last life |
| Wave clear | All aliens gone |
| Background music | Looping synthwave track |

---

## 12. Sprite Sheet Specification

All sprites use **transparent PNG** format, pixel art style, designed for `dbSprite()` / `dbLoadImage()` via LiteGDK.

### Alien Sprite Sheet (`assets/sprites/alien_sheet.png`)

- Sheet layout: horizontal strip
- Each frame: **64×64 px** (T1 frames), scaled programmatically for T2/T3
- Rows:
  - Row 0 (y=0): T1 Mega — float frames (×4), hit frames (×2), split frames (×3) — 9 frames total
  - Row 1 (y=64): T2 Mid — float (×4), hit (×2), split (×3) — 9 frames total
  - Row 2 (y=128): T3 Mini — float (×4), hit (×2), vaporize (×4) — 10 frames total
- Total sheet size: **640×192 px** (10 frames wide × 3 rows)

### Soldier Sprite Sheet (`assets/sprites/soldier_sheet.png`)

- Sheet layout: horizontal strip
- Each frame: **48×64 px**
- Single row:
  - Idle: ×2 frames
  - Run right: ×4 frames
  - Jump: ×1 frame
  - Shoot (standing): ×2 frames
  - Death: ×4 frames
- Total: 13 frames → **624×64 px**

---

## 13. Project Structure

```
alien-multiplicity/
  src/
    main.cpp              ← DarkGDK() entry point
    Game.h / Game.cpp     ← top-level game state machine
    Player.h / Player.cpp ← player entity, movement, shooting
    Alien.h / Alien.cpp   ← alien entity, AI, split logic
    Laser.h / Laser.cpp   ← laser projectile
    HUD.h / HUD.cpp       ← score, lives, wave display
    Wave.h / Wave.cpp     ← wave manager, spawn logic
    constants.h           ← screen size, physics, tuning knobs
  assets/
    sprites/
      alien_sheet.png
      soldier_sheet.png
    sounds/               ← (future)
    music/                ← (future)
  docs/
    design-spec.md        ← this file
    getting-started.md    ← build instructions
  .github/
    workflows/
      ci.yml              ← native build + tests
      web-deploy.yml      ← WASM → GitHub Pages
  CMakeLists.txt
  CMakePresets.json
  README.md
```

---

## 14. Build Targets

| Target | Command |
|--------|---------|
| Native debug | `cmake --preset debug && cmake --build --preset debug` |
| Native release | `cmake --preset release && cmake --build --preset release` |
| WASM (web) | `emcmake cmake --preset wasm && cmake --build build-wasm` |
| Preview web | `emrun build-wasm/alien_multiplicity.html` |

LiteGDK is pulled as a `FetchContent` dependency from `https://github.com/77smith-norm/LiteGDK.git`.

---

## 15. Milestone Plan

| Milestone | Goal |
|-----------|------|
| M0 — Scaffolding | Repo, CMakeLists.txt, LiteGDK dependency, empty `DarkGDK()`, native build green |
| M1 — Player | Soldier on screen, left/right movement, jump, laser fire (no enemies) |
| M2 — Alien | Single T1 alien on screen, movement, hit push-back |
| M3 — Split mechanic | T1 → T2 × 2 → T3 × 4 → vaporize chain |
| M4 — Waves | Wave manager, HUD, score, lives |
| M5 — Game states | Title, playing, paused, game over |
| M6 — Polish | Animations, audio stubs, web deploy green |

---

## 16. Open Questions / Future Work

- Double-jump or wall-jump for later versions?
- Power-ups (inspired by Super Crate Box) — freeze ray, shotgun blast?
- Local hi-score persistence (LocalStorage on web, file on native)?
- Multiplayer (co-op 2-player)?
- Boss alien variant beyond T3?
