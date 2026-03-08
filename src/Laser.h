#pragma once

#include "raylib.h"

namespace am {

class Laser {
public:
    void fire(const Vector2& origin, const Vector2& direction, float length);
    void update(float dt);
    void draw() const;
    void reset();
    bool active() const;

private:
    bool active_{false};
    float timer_{0.0f};
    float length_{0.0f};
    Vector2 origin_{};
    Vector2 direction_{1.0f, 0.0f};
};

}  // namespace am
