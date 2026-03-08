#include "Game.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

#include "DarkGDK.h"
#include "constants.h"
#include "runtime/AppRuntime.h"

namespace am {
namespace {
void loadFrame(const char* path, int imageId) {
    dbLoadImage(path, imageId);
}

std::string centeredStatus(const std::string& text) {
    return text;
}
}

Game::Game() {
    SetWindowTitle("Alien Multiplicity");
    litegdk::runtime().frameState().setClearColor({10, 10, 26, 255});
    loadAssets();
    resetWorld();
}

void Game::update(float dt) {
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
            const Rectangle hitBounds = aliens_[nearestIndex].bounds();
            const int killedSpriteId = aliens_[nearestIndex].spriteId();
            AlienDamageResult damage = aliens_[nearestIndex].applyHit(direction);
            if (damage.killed) {
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
            }
        }
    }
}

void Game::draw() {
    dbSprite(SPR_GROUND, 0, static_cast<int>(kGroundTop), IMG_GROUND);

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
    drawDebugText();
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

void Game::resetWorld() {
    for (const auto& alien : aliens_) {
        dbDeleteSprite(alien.spriteId());
    }
    aliens_.clear();

    for (const auto& effect : effects_) {
        if (dbSpriteExist(effect.spriteId)) {
            dbDeleteSprite(effect.spriteId);
        }
    }
    effects_.clear();

    nextAlienSpriteId_ = SPR_ALIEN_START;
    nextEffectSpriteId_ = SPR_EFFECT_START;
    player_.resetForNewGame();
    laser_.reset();

    spawnAlien(AlienTier::t1, Vector2{560.0f, 150.0f}, Vector2{-50.0f, 20.0f});
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

void Game::drawDebugText() const {
    dbInk(kColorWhite, 0);
    dbText(16, 16, "A/D or arrows move  W/Up jumps  Space/click fires");

    std::ostringstream status;
    status << "Aliens: " << aliens_.size() << "  Lives: " << player_.lives();
    const std::string statusText = status.str();
    dbText(16, 40, statusText.c_str());

    if (aliens_.empty()) {
        const std::string doneText = centeredStatus("All aliens cleared. Restart the app for another chain.");
        const int x = (kScreenWidth - MeasureText(doneText.c_str(), kTextSize)) / 2;
        dbInk(kColorGreen, 0);
        dbText(x, 72, doneText.c_str());
    }
}

}  // namespace am
