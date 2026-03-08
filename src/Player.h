#pragma once

#include "raylib.h"

namespace am {

class Player {
public:
    Player();

    void resetForNewGame();
    void update(float dt);
    void draw() const;
    void hide() const;

    bool tryFire(const Vector2& aimPoint, Vector2& origin, Vector2& direction);
    bool takeHit();

    Rectangle bounds() const;
    Vector2 center() const;
    Vector2 gunOrigin() const;
    bool facingRight() const;
    int lives() const;
    bool isInvincible() const;
    bool isDead() const;

private:
    int currentImageId() const;
    bool shouldBlinkHide() const;
    void respawnAtCenter();

    Vector2 position_{};
    float velocityY_{0.0f};
    float fireCooldown_{0.0f};
    float fireAnimationTimer_{0.0f};
    float hitAnimationTimer_{0.0f};
    float runAnimationTimer_{0.0f};
    float invincibleTimer_{0.0f};
    int lives_{0};
    bool facingRight_{true};
    bool onGround_{true};
    bool moving_{false};
    bool dead_{false};
};

}  // namespace am
