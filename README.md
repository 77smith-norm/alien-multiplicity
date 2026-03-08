# Alien Multiplicity

Alien Multiplicity is a single-screen retro arcade shooter built with LiteGDK. Every alien splits into smaller, tougher children until the final mini beholders can be vaporized for score.

## Controls

- `A` / `D` or arrow keys: move
- `W` / Up arrow: jump
- Mouse: aim
- `Space` or left click: fire
- `Esc`: pause

## Native Build

1. Generate the sliced sprite frames.

```bash
python3 tools/slice_sprites.py
```

2. Configure and build the debug target.

```bash
cmake --preset debug
cmake --build --preset debug
./build/alien_multiplicity
```

Release builds use the same flow with the `release` preset.

## Web Build

Emscripten is expected at `/Users/norm/.local/emsdk` locally.

```bash
python3 tools/slice_sprites.py
source /Users/norm/.local/emsdk/emsdk_env.sh
emcmake cmake --preset wasm
cmake --build --preset wasm
```

The browser artifact is generated in `build-wasm/`.

## Asset Pipeline

LiteGDK loads whole images, so the source sprite sheets are pre-sliced into `assets/frames/` with `tools/slice_sprites.py`. The script also generates mirrored soldier frames, a beam sprite, a heart icon, and the ground strip.

## GitHub Actions

- `.github/workflows/ci.yml` verifies the native debug build.
- `.github/workflows/web-deploy.yml` builds the WASM target and deploys `build-wasm/` to GitHub Pages.
