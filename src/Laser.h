#pragma once

#include <vector>

#include "raylib.h"

namespace am {

struct LaserBeam {
    Vector2 origin{};
    Vector2 direction{1.0f, 0.0f};
    float length{0.0f};
};

class Laser {
public:
    void fire(const std::vector<LaserBeam>& beams);
    void update(float dt);
    void draw() const;
    void reset();
    bool active() const;

private:
    bool active_{false};
    float timer_{0.0f};
    std::vector<LaserBeam> beams_{};
};

}  // namespace am
