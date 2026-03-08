#pragma once

#include <vector>

#include "raylib.h"

#include "constants.h"

namespace am {

struct AlienSpawnRequest {
    AlienTier tier;
    Vector2 center;
    Vector2 velocity;
};

struct AlienDamageResult {
    bool hit{false};
    bool killed{false};
    int hitScore{0};
    int killScore{0};
    int effectImageId{0};
    std::vector<AlienSpawnRequest> children{};
};

class Alien {
public:
    Alien(int spriteId, AlienTier tier, const Vector2& center, const Vector2& velocity);

    void update(float dt, const Vector2& playerCenter);
    void draw() const;

    bool intersectRay(const Vector2& origin, const Vector2& direction, float maxDistance, float& distance) const;
    AlienDamageResult applyHit(const Vector2& shotDirection);
    bool collidesWith(const Rectangle& other) const;

    Rectangle bounds() const;
    Vector2 center() const;
    int spriteId() const;
    AlienTier tier() const;

private:
    const AlienTierConfig& config() const;
    float bobOffset(float now) const;
    Rectangle boundsAt(float now) const;
    int currentImageId(float now) const;

    int spriteId_{0};
    AlienTier tier_{AlienTier::t1};
    Vector2 center_{};
    Vector2 impulse_{};
    Vector2 driftBias_{1.0f, 0.0f};
    float directionChangeTimer_{0.0f};
    float bobPhase_{0.0f};
    float hitFlashTimer_{0.0f};
    int hp_{0};
};

}  // namespace am
