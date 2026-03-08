#include "Laser.h"

#include <cmath>

#include "DarkGDK.h"
#include "constants.h"
#include "runtime/AppRuntime.h"

namespace am {
void Laser::fire(const std::vector<LaserBeam>& beams) {
    reset();
    if (beams.empty()) {
        return;
    }

    active_ = true;
    timer_ = kLaserVisualSeconds;
    beams_ = beams;
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

    for (std::size_t i = 0; i < beams_.size(); ++i) {
        const LaserBeam& beam = beams_[i];
        const int spriteId = SPR_PLAYER_BEAM_START + static_cast<int>(i);
        dbSprite(spriteId,
                 static_cast<int>(std::round(beam.origin.x)),
                 static_cast<int>(std::round(beam.origin.y - 1.0f)),
                 IMG_BEAM);

        if (auto* sprite = litegdk::runtime().sprites().find(spriteId)) {
            sprite->rotationDegrees = std::atan2(beam.direction.y, beam.direction.x) * RAD2DEG;
            sprite->scale = beam.length / kBeamBaseLength;
        }
    }
}

void Laser::reset() {
    active_ = false;
    timer_ = 0.0f;
    beams_.clear();
    for (int i = 0; i < kMaxWeaponBeams; ++i) {
        const int spriteId = SPR_PLAYER_BEAM_START + i;
        if (dbSpriteExist(spriteId)) {
            dbDeleteSprite(spriteId);
        }
    }
}

bool Laser::active() const {
    return active_;
}

}  // namespace am
