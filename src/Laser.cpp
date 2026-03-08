#include "Laser.h"

#include <cmath>

#include "DarkGDK.h"
#include "constants.h"
#include "runtime/AppRuntime.h"

namespace am {
void Laser::fire(const Vector2& origin, const Vector2& direction, float length) {
    active_ = true;
    timer_ = kLaserVisualSeconds;
    origin_ = origin;
    direction_ = direction;
    length_ = length;
}

void Laser::update(float dt) {
    if (!active_) {
        return;
    }

    timer_ -= dt;
    if (timer_ <= 0.0f) {
        reset();
    }
}

void Laser::draw() const {
    if (!active_) {
        return;
    }

    dbSprite(SPR_PLAYER_BEAM,
             static_cast<int>(std::round(origin_.x)),
             static_cast<int>(std::round(origin_.y - 1.0f)),
             IMG_BEAM);

    if (auto* sprite = litegdk::runtime().sprites().find(SPR_PLAYER_BEAM)) {
        sprite->rotationDegrees = std::atan2(direction_.y, direction_.x) * RAD2DEG;
        sprite->scale = length_ / kBeamBaseLength;
    }
}

void Laser::reset() {
    active_ = false;
    timer_ = 0.0f;
    length_ = 0.0f;
    if (dbSpriteExist(SPR_PLAYER_BEAM)) {
        dbDeleteSprite(SPR_PLAYER_BEAM);
    }
}

bool Laser::active() const {
    return active_;
}

}  // namespace am
