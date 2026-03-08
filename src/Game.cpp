#include "Game.h"

#include <algorithm>
#include <cmath>

#include "DarkGDK.h"
#include "constants.h"
#include "runtime/AppRuntime.h"

namespace am {
namespace {
void loadFrame(const char* path, int imageId) {
    dbLoadImage(path, imageId);
}

Color colorFromRgb(std::uint32_t value) {
    return Color{
        static_cast<unsigned char>((value >> 16) & 0xFF),
        static_cast<unsigned char>((value >> 8) & 0xFF),
        static_cast<unsigned char>(value & 0xFF),
        255,
    };
}
}

Game::Game() {
    SetWindowTitle("Alien Multiplicity");
    litegdk::runtime().frameState().setClearColor({10, 10, 26, 255});
    loadAssets();
    clearWorld();
    player_.resetForNewGame();
}

void Game::update(float dt) {
    switch (state_) {
    case GameState::title:
        updateTitle();
        break;
    case GameState::playing:
        updatePlaying(dt);
        break;
    case GameState::paused:
        updatePaused();
        break;
    case GameState::gameOver:
        updateGameOver();
        break;
    }
}

void Game::draw() {
    drawGround();

    switch (state_) {
    case GameState::title:
        player_.draw();
        hud_.drawTitle(bestScore_);
        break;
    case GameState::playing:
        drawWorld();
        hud_.drawPlaying(score_, wave_.currentWave(), player_.lives(), comboMultiplier_);
        if (wave_.bannerVisible()) {
            hud_.drawWaveBanner(wave_.currentWave());
        }
        break;
    case GameState::paused:
        drawWorld();
        hud_.drawPlaying(score_, wave_.currentWave(), player_.lives(), comboMultiplier_);
        hud_.drawPaused();
        break;
    case GameState::gameOver:
        player_.draw();
        hud_.drawGameOver(score_, bestScore_);
        break;
    }
}

void Game::loadAssets() {
    loadFrame("assets/frames/ground.png", IMG_GROUND);
    loadFrame("assets/frames/beam.png", IMG_BEAM);
    loadFrame("assets/frames/hud_heart.png", IMG_HUD_HEART);

    loadFrame("assets/frames/soldier_idle_right.png", IMG_SOLDIER_IDLE_RIGHT);
    loadFrame("assets/frames/soldier_idle_left.png", IMG_SOLDIER_IDLE_LEFT);
    loadFrame("assets/frames/soldier_run0_right.png", IMG_SOLDIER_RUN0_RIGHT);
    loadFrame("assets/frames/soldier_run0_left.png", IMG_SOLDIER_RUN0_LEFT);
    loadFrame("assets/frames/soldier_run1_right.png", IMG_SOLDIER_RUN1_RIGHT);
    loadFrame("assets/frames/soldier_run1_left.png", IMG_SOLDIER_RUN1_LEFT);
    loadFrame("assets/frames/soldier_run2_right.png", IMG_SOLDIER_RUN2_RIGHT);
    loadFrame("assets/frames/soldier_run2_left.png", IMG_SOLDIER_RUN2_LEFT);
    loadFrame("assets/frames/soldier_run3_right.png", IMG_SOLDIER_RUN3_RIGHT);
    loadFrame("assets/frames/soldier_run3_left.png", IMG_SOLDIER_RUN3_LEFT);
    loadFrame("assets/frames/soldier_jump_right.png", IMG_SOLDIER_JUMP_RIGHT);
    loadFrame("assets/frames/soldier_jump_left.png", IMG_SOLDIER_JUMP_LEFT);
    loadFrame("assets/frames/soldier_shoot_right.png", IMG_SOLDIER_SHOOT_RIGHT);
    loadFrame("assets/frames/soldier_shoot_left.png", IMG_SOLDIER_SHOOT_LEFT);
    loadFrame("assets/frames/soldier_hit_right.png", IMG_SOLDIER_HIT_RIGHT);
    loadFrame("assets/frames/soldier_hit_left.png", IMG_SOLDIER_HIT_LEFT);
    loadFrame("assets/frames/soldier_death_right.png", IMG_SOLDIER_DEATH_RIGHT);
    loadFrame("assets/frames/soldier_death_left.png", IMG_SOLDIER_DEATH_LEFT);

    loadFrame("assets/frames/alien_t1_float0.png", IMG_ALIEN_T1_FLOAT0);
    loadFrame("assets/frames/alien_t1_float1.png", IMG_ALIEN_T1_FLOAT1);
    loadFrame("assets/frames/alien_t1_hit0.png", IMG_ALIEN_T1_HIT0);
    loadFrame("assets/frames/alien_t1_hit1.png", IMG_ALIEN_T1_HIT1);
    loadFrame("assets/frames/alien_t1_split.png", IMG_ALIEN_T1_SPLIT);
    loadFrame("assets/frames/alien_t2_float0.png", IMG_ALIEN_T2_FLOAT0);
    loadFrame("assets/frames/alien_t2_float1.png", IMG_ALIEN_T2_FLOAT1);
    loadFrame("assets/frames/alien_t2_hit0.png", IMG_ALIEN_T2_HIT0);
    loadFrame("assets/frames/alien_t2_hit1.png", IMG_ALIEN_T2_HIT1);
    loadFrame("assets/frames/alien_t2_split.png", IMG_ALIEN_T2_SPLIT);
    loadFrame("assets/frames/alien_t3_float0.png", IMG_ALIEN_T3_FLOAT0);
    loadFrame("assets/frames/alien_t3_float1.png", IMG_ALIEN_T3_FLOAT1);
    loadFrame("assets/frames/alien_t3_float2.png", IMG_ALIEN_T3_FLOAT2);
    loadFrame("assets/frames/alien_t3_death.png", IMG_ALIEN_T3_DEATH);
    loadFrame("assets/frames/alien_t3_vaporize.png", IMG_ALIEN_T3_VAPORIZE);
}

void Game::clearAliens() {
    for (const auto& alien : aliens_) {
        dbDeleteSprite(alien.spriteId());
    }
    aliens_.clear();
}

void Game::clearEffects() {
    for (const auto& effect : effects_) {
        if (dbSpriteExist(effect.spriteId)) {
            dbDeleteSprite(effect.spriteId);
        }
    }
    effects_.clear();
}

void Game::clearWorld() {
    clearAliens();
    clearEffects();
    laser_.reset();
    hud_.hideHearts();
    nextAlienSpriteId_ = SPR_ALIEN_START;
    nextEffectSpriteId_ = SPR_EFFECT_START;
}

void Game::startNewGame() {
    clearWorld();
    player_.resetForNewGame();
    score_ = 0;
    comboMultiplier_ = 1;
    lastKillTime_ = -100.0f;
    wave_.reset();

    const WaveUpdate startWave = wave_.start();
    for (const AlienSpawnRequest& spawn : startWave.spawns) {
        spawnAlien(spawn.tier, spawn.center, spawn.velocity);
    }

    state_ = GameState::playing;
}

void Game::spawnAlien(AlienTier tier, const Vector2& center, const Vector2& velocity) {
    aliens_.emplace_back(nextAlienSpriteId_++, tier, center, velocity);
}

float Game::distanceToScreenEdge(const Vector2& origin, const Vector2& direction) const {
    float nearest = kLaserRange;

    auto consider = [&](float t) {
        if (t > 0.0f) {
            nearest = std::min(nearest, t);
        }
    };

    if (std::fabs(direction.x) > 0.0001f) {
        consider((0.0f - origin.x) / direction.x);
        consider((static_cast<float>(kScreenWidth) - origin.x) / direction.x);
    }
    if (std::fabs(direction.y) > 0.0001f) {
        consider((0.0f - origin.y) / direction.y);
        consider((static_cast<float>(kScreenHeight) - origin.y) / direction.y);
    }

    return std::clamp(nearest, 4.0f, kLaserRange);
}

void Game::updateTitle() {
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        startNewGame();
    }
}

void Game::updatePlaying(float dt) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        state_ = GameState::paused;
        return;
    }

    player_.update(dt);
    laser_.update(dt);

    for (auto& alien : aliens_) {
        alien.update(dt, player_.center());
    }

    for (auto& effect : effects_) {
        effect.timer -= dt;
    }
    effects_.erase(
        std::remove_if(effects_.begin(), effects_.end(), [](const VisualEffect& effect) {
            if (effect.timer <= 0.0f) {
                if (dbSpriteExist(effect.spriteId)) {
                    dbDeleteSprite(effect.spriteId);
                }
                return true;
            }
            return false;
        }),
        effects_.end());

    if (comboMultiplier_ > 1 && (static_cast<float>(GetTime()) - lastKillTime_) > kComboWindowSeconds) {
        comboMultiplier_ = 1;
    }

    Vector2 origin{};
    Vector2 direction{};
    const Vector2 aimPoint{static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY())};
    if (player_.tryFire(aimPoint, origin, direction)) {
        float nearestDistance = distanceToScreenEdge(origin, direction);
        int nearestIndex = -1;

        for (std::size_t i = 0; i < aliens_.size(); ++i) {
            float hitDistance = 0.0f;
            if (aliens_[i].intersectRay(origin, direction, kLaserRange, hitDistance) && hitDistance < nearestDistance) {
                nearestDistance = hitDistance;
                nearestIndex = static_cast<int>(i);
            }
        }

        laser_.fire(origin, direction, nearestDistance);

        if (nearestIndex >= 0) {
            score_ += kLaserHitScore;
            const Rectangle hitBounds = aliens_[nearestIndex].bounds();
            const int killedSpriteId = aliens_[nearestIndex].spriteId();
            AlienDamageResult damage = aliens_[nearestIndex].applyHit(direction);
            if (damage.killed) {
                const float now = static_cast<float>(GetTime());
                comboMultiplier_ = ((now - lastKillTime_) <= kComboWindowSeconds) ? (comboMultiplier_ + 1) : 1;
                lastKillTime_ = now;
                score_ += damage.killScore * comboMultiplier_;

                if (damage.effectImageId != 0) {
                    effects_.push_back(VisualEffect{
                        nextEffectSpriteId_++,
                        damage.effectImageId,
                        Vector2{hitBounds.x, hitBounds.y},
                        kSplitEffectSeconds,
                    });
                }

                for (const AlienSpawnRequest& child : damage.children) {
                    spawnAlien(child.tier, child.center, child.velocity);
                }

                dbDeleteSprite(killedSpriteId);
                aliens_.erase(aliens_.begin() + nearestIndex);
            } else {
                comboMultiplier_ = 1;
            }
        }
    }

    if (!player_.isInvincible() && !player_.isDead()) {
        const Rectangle playerBounds = player_.bounds();
        for (const auto& alien : aliens_) {
            if (alien.collidesWith(playerBounds)) {
                if (!player_.takeHit()) {
                    bestScore_ = std::max(bestScore_, score_);
                    clearAliens();
                    clearEffects();
                    laser_.reset();
                    hud_.hideHearts();
                    state_ = GameState::gameOver;
                }
                break;
            }
        }
    }

    const WaveUpdate waveUpdate = wave_.update(dt, aliens_.empty());
    if (waveUpdate.waveCleared) {
        score_ += 500 * wave_.currentWave();
    }
    if (waveUpdate.waveStarted) {
        for (const AlienSpawnRequest& spawn : waveUpdate.spawns) {
            spawnAlien(spawn.tier, spawn.center, spawn.velocity);
        }
    }
}

void Game::updatePaused() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        state_ = GameState::playing;
    }
}

void Game::updateGameOver() {
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
        startNewGame();
    }
}

void Game::drawGround() const {
    dbSprite(SPR_GROUND, 0, static_cast<int>(kGroundTop), IMG_GROUND);

    for (const Platform& platform : kPlatforms) {
        DrawRectangle(static_cast<int>(platform.x),
                      static_cast<int>(platform.y),
                      static_cast<int>(platform.width),
                      static_cast<int>(platform.height),
                      colorFromRgb(kColorPlatform));
        DrawRectangle(static_cast<int>(platform.x),
                      static_cast<int>(platform.y),
                      static_cast<int>(platform.width),
                      3,
                      colorFromRgb(kColorPlatformHighlight));
    }
}

void Game::drawWorld() const {
    for (const auto& effect : effects_) {
        dbSprite(effect.spriteId,
                 static_cast<int>(std::round(effect.position.x)),
                 static_cast<int>(std::round(effect.position.y)),
                 effect.imageId);
    }

    for (const auto& alien : aliens_) {
        alien.draw();
    }

    player_.draw();
    laser_.draw();
}

}  // namespace am
