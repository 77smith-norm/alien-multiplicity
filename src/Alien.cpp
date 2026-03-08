#include "Alien.h"

#include <algorithm>
#include <cmath>
#include <random>

#include "DarkGDK.h"
#include "raymath.h"

namespace am {
namespace {
std::mt19937& rng() {
    static std::mt19937 generator{std::random_device{}()};
    return generator;
}

float randomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> dist(minValue, maxValue);
    return dist(rng());
}

Vector2 randomUnitVector() {
    const float angle = randomFloat(0.0f, PI * 2.0f);
    return Vector2{std::cos(angle), std::sin(angle)};
}

int t1FloatFrame(float now) {
    return (static_cast<int>(now * 6.0f) % 2 == 0) ? IMG_ALIEN_T1_FLOAT0 : IMG_ALIEN_T1_FLOAT1;
}

int t2FloatFrame(float now) {
    return (static_cast<int>(now * 6.0f) % 2 == 0) ? IMG_ALIEN_T2_FLOAT0 : IMG_ALIEN_T2_FLOAT1;
}

int t3FloatFrame(float now) {
    switch (static_cast<int>(now * 9.0f) % 3) {
    case 0:
        return IMG_ALIEN_T3_FLOAT0;
    case 1:
        return IMG_ALIEN_T3_FLOAT1;
    default:
        return IMG_ALIEN_T3_FLOAT2;
    }
}
}

Alien::Alien(int spriteId, AlienTier tier, const Vector2& center, const Vector2& velocity)
    : spriteId_(spriteId),
      tier_(tier),
      center_(center),
      impulse_(velocity),
      driftBias_(Vector2Normalize(randomUnitVector())),
      directionChangeTimer_(randomFloat(kAlienDirectionChangeMinSeconds, kAlienDirectionChangeMaxSeconds)),
      bobPhase_(randomFloat(0.0f, PI * 2.0f)),
      hp_(config().maxHp) {
}

void Alien::update(float dt, const Vector2& playerCenter) {
    directionChangeTimer_ -= dt;
    if (directionChangeTimer_ <= 0.0f) {
        driftBias_ = Vector2Normalize(randomUnitVector());
        directionChangeTimer_ = randomFloat(kAlienDirectionChangeMinSeconds, kAlienDirectionChangeMaxSeconds);
    }

    hitFlashTimer_ = std::max(0.0f, hitFlashTimer_ - dt);

    Vector2 toPlayer = Vector2Subtract(playerCenter, center_);
    if (Vector2LengthSqr(toPlayer) < 0.01f) {
        toPlayer = driftBias_;
    } else {
        toPlayer = Vector2Normalize(toPlayer);
    }

    Vector2 desiredDirection = Vector2Add(Vector2Scale(toPlayer, 1.7f), driftBias_);
    if (Vector2LengthSqr(desiredDirection) < 0.01f) {
        desiredDirection = driftBias_;
    }
    desiredDirection = Vector2Normalize(desiredDirection);

    center_ = Vector2Add(center_, Vector2Scale(desiredDirection, config().speed * dt));
    center_ = Vector2Add(center_, Vector2Scale(impulse_, dt));
    impulse_ = Vector2Lerp(impulse_, Vector2{0.0f, 0.0f}, Clamp(dt * 4.0f, 0.0f, 1.0f));

    const float halfWidth = static_cast<float>(config().width) * 0.5f;
    const float halfHeight = static_cast<float>(config().height) * 0.5f;
    const float minX = halfWidth;
    const float maxX = static_cast<float>(kScreenWidth) - halfWidth;
    const float minY = halfHeight + kAlienBobAmplitude;
    const float maxY = kGroundTop - halfHeight - kAlienBobAmplitude;

    if (center_.x < minX) {
        center_.x = minX;
        driftBias_.x = std::fabs(driftBias_.x);
        impulse_.x = std::fabs(impulse_.x);
    } else if (center_.x > maxX) {
        center_.x = maxX;
        driftBias_.x = -std::fabs(driftBias_.x);
        impulse_.x = -std::fabs(impulse_.x);
    }

    if (center_.y < minY) {
        center_.y = minY;
        driftBias_.y = std::fabs(driftBias_.y);
        impulse_.y = std::fabs(impulse_.y);
    } else if (center_.y > maxY) {
        center_.y = maxY;
        driftBias_.y = -std::fabs(driftBias_.y);
        impulse_.y = -std::fabs(impulse_.y);
    }
}

void Alien::draw() const {
    const float now = static_cast<float>(GetTime());
    const Rectangle rect = boundsAt(now);
    dbSprite(spriteId_, static_cast<int>(std::round(rect.x)), static_cast<int>(std::round(rect.y)), currentImageId(now));
}

bool Alien::intersectRay(const Vector2& origin, const Vector2& direction, float maxDistance, float& distance) const {
    const Rectangle rect = bounds();
    const float minX = rect.x;
    const float minY = rect.y;
    const float maxX = rect.x + rect.width;
    const float maxY = rect.y + rect.height;

    float tMin = 0.0f;
    float tMax = maxDistance;

    auto updateAxis = [&](float originValue, float directionValue, float axisMin, float axisMax) {
        if (std::fabs(directionValue) < 0.0001f) {
            return originValue >= axisMin && originValue <= axisMax;
        }

        float inv = 1.0f / directionValue;
        float t1 = (axisMin - originValue) * inv;
        float t2 = (axisMax - originValue) * inv;
        if (t1 > t2) {
            std::swap(t1, t2);
        }
        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
        return tMin <= tMax;
    };

    if (!updateAxis(origin.x, direction.x, minX, maxX)) {
        return false;
    }
    if (!updateAxis(origin.y, direction.y, minY, maxY)) {
        return false;
    }

    distance = tMin;
    return distance >= 0.0f && distance <= maxDistance;
}

AlienDamageResult Alien::applyHit(const Vector2& shotDirection) {
    AlienDamageResult result;
    result.hit = true;
    result.hitScore = kLaserHitScore;

    hitFlashTimer_ = kAlienHitFlashSeconds;
    hp_ -= kLaserDamage;
    impulse_ = Vector2Add(impulse_, Vector2Scale(shotDirection, -kLaserPushbackSpeed));

    if (hp_ > 0) {
        return result;
    }

    result.killed = true;
    result.killScore = config().killScore;

    if (tier_ == AlienTier::t1 || tier_ == AlienTier::t2) {
        result.effectImageId = (tier_ == AlienTier::t1) ? IMG_ALIEN_T1_SPLIT : IMG_ALIEN_T2_SPLIT;
        const AlienTier childTier = nextAlienTier(tier_);
        Vector2 away = Vector2LengthSqr(shotDirection) > 0.001f ? Vector2Scale(shotDirection, -1.0f) : Vector2{1.0f, 0.0f};
        Vector2 perp = Vector2Normalize(Vector2{-away.y, away.x});
        if (Vector2LengthSqr(perp) < 0.001f) {
            perp = Vector2{0.0f, 1.0f};
        }
        const Vector2 childVelocityA = Vector2Scale(Vector2Normalize(Vector2Add(perp, Vector2Scale(away, 0.35f))), kAlienSplitOutwardSpeed);
        const Vector2 childVelocityB = Vector2Scale(Vector2Normalize(Vector2Add(Vector2Scale(perp, -1.0f), Vector2Scale(away, 0.35f))), kAlienSplitOutwardSpeed);

        result.children.push_back(AlienSpawnRequest{childTier, center_, childVelocityA});
        result.children.push_back(AlienSpawnRequest{childTier, center_, childVelocityB});
    } else {
        result.effectImageId = IMG_ALIEN_T3_VAPORIZE;
    }

    return result;
}

bool Alien::collidesWith(const Rectangle& other) const {
    return CheckCollisionRecs(bounds(), other);
}

Rectangle Alien::bounds() const {
    return boundsAt(static_cast<float>(GetTime()));
}

Vector2 Alien::center() const {
    return Vector2{center_.x, center_.y + bobOffset(static_cast<float>(GetTime()))};
}

int Alien::spriteId() const {
    return spriteId_;
}

AlienTier Alien::tier() const {
    return tier_;
}

const AlienTierConfig& Alien::config() const {
    return alienConfig(tier_);
}

float Alien::bobOffset(float now) const {
    const float bobRate = (PI * 2.0f) / kAlienBobPeriodSeconds;
    return std::sin((now * bobRate) + bobPhase_) * kAlienBobAmplitude;
}

Rectangle Alien::boundsAt(float now) const {
    const float width = static_cast<float>(config().width);
    const float height = static_cast<float>(config().height);
    return Rectangle{
        center_.x - (width * 0.5f),
        (center_.y + bobOffset(now)) - (height * 0.5f),
        width,
        height,
    };
}

int Alien::currentImageId(float now) const {
    if (hitFlashTimer_ > 0.0f) {
        switch (tier_) {
        case AlienTier::t1:
            return (static_cast<int>(now * 20.0f) % 2 == 0) ? IMG_ALIEN_T1_HIT0 : IMG_ALIEN_T1_HIT1;
        case AlienTier::t2:
            return (static_cast<int>(now * 20.0f) % 2 == 0) ? IMG_ALIEN_T2_HIT0 : IMG_ALIEN_T2_HIT1;
        case AlienTier::t3:
        default:
            return IMG_ALIEN_T3_DEATH;
        }
    }

    switch (tier_) {
    case AlienTier::t1:
        return t1FloatFrame(now);
    case AlienTier::t2:
        return t2FloatFrame(now);
    case AlienTier::t3:
    default:
        return t3FloatFrame(now);
    }
}

}  // namespace am
