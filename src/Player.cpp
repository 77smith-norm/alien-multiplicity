#include "Player.h"

#include <algorithm>
#include <cmath>

#include "DarkGDK.h"
#include "constants.h"
#include "raymath.h"

namespace am {
namespace {
int imageForDirection(bool facingRight, int rightImage, int leftImage) {
    return facingRight ? rightImage : leftImage;
}
}

Player::Player() {
    resetForNewGame();
}

void Player::resetForNewGame() {
    position_ = {kPlayerSpawnX, kPlayerSpawnY};
    velocityY_ = 0.0f;
    fireCooldown_ = 0.0f;
    fireAnimationTimer_ = 0.0f;
    hitAnimationTimer_ = 0.0f;
    runAnimationTimer_ = 0.0f;
    invincibleTimer_ = 0.0f;
    lives_ = kPlayerStartingLives;
    facingRight_ = true;
    onGround_ = true;
    moving_ = false;
    dead_ = false;
    dbDeleteSprite(SPR_PLAYER);
}

void Player::update(float dt) {
    fireCooldown_ = std::max(0.0f, fireCooldown_ - dt);
    fireAnimationTimer_ = std::max(0.0f, fireAnimationTimer_ - dt);
    hitAnimationTimer_ = std::max(0.0f, hitAnimationTimer_ - dt);
    invincibleTimer_ = std::max(0.0f, invincibleTimer_ - dt);

    if (dead_) {
        return;
    }

    int moveAxis = 0;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        --moveAxis;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        ++moveAxis;
    }

    moving_ = moveAxis != 0;
    if (moveAxis < 0) {
        facingRight_ = false;
    } else if (moveAxis > 0) {
        facingRight_ = true;
    }

    position_.x += static_cast<float>(moveAxis) * kPlayerMoveSpeed * dt;
    position_.x = Clamp(position_.x, 0.0f, static_cast<float>(kScreenWidth) - kPlayerWidth);

    if (onGround_ && (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))) {
        velocityY_ = -kPlayerJumpVelocity;
        onGround_ = false;
    }

    if (!onGround_ || std::fabs(velocityY_) > 0.01f) {
        velocityY_ += kGravity * dt;
        position_.y += velocityY_ * dt;
        if (position_.y >= kPlayerSpawnY) {
            position_.y = kPlayerSpawnY;
            velocityY_ = 0.0f;
            onGround_ = true;
        }
    }

    if (moving_ && onGround_) {
        runAnimationTimer_ += dt;
    }
}

bool Player::tryFire(const Vector2& aimPoint, Vector2& origin, Vector2& direction) {
    if (dead_) {
        return false;
    }

    const bool firePressed = IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if (!firePressed || fireCooldown_ > 0.0f) {
        return false;
    }

    origin = gunOrigin();
    direction = Vector2Subtract(aimPoint, origin);
    if (Vector2LengthSqr(direction) < 1.0f) {
        direction = facingRight_ ? Vector2{1.0f, 0.0f} : Vector2{-1.0f, 0.0f};
    }
    direction = Vector2Normalize(direction);

    fireCooldown_ = kLaserCooldownSeconds;
    fireAnimationTimer_ = kLaserVisualSeconds;
    return true;
}

bool Player::takeHit() {
    if (dead_ || invincibleTimer_ > 0.0f) {
        return !dead_;
    }

    --lives_;
    hitAnimationTimer_ = 0.3f;
    fireAnimationTimer_ = 0.0f;

    if (lives_ <= 0) {
        dead_ = true;
        return false;
    }

    respawnAtCenter();
    invincibleTimer_ = kPlayerRespawnInvincibilitySeconds;
    return true;
}

void Player::draw() const {
    if (shouldBlinkHide()) {
        hide();
        return;
    }

    dbSprite(SPR_PLAYER,
             static_cast<int>(std::round(position_.x)),
             static_cast<int>(std::round(position_.y)),
             currentImageId());
}

void Player::hide() const {
    if (dbSpriteExist(SPR_PLAYER)) {
        dbHideSprite(SPR_PLAYER);
    }
}

Rectangle Player::bounds() const {
    return Rectangle{position_.x, position_.y, kPlayerWidth, kPlayerHeight};
}

Vector2 Player::center() const {
    return Vector2{position_.x + (kPlayerWidth * 0.5f), position_.y + (kPlayerHeight * 0.5f)};
}

Vector2 Player::gunOrigin() const {
    return Vector2{
        facingRight_ ? position_.x + kPlayerWidth - 2.0f : position_.x + 2.0f,
        position_.y + 24.0f,
    };
}

bool Player::facingRight() const {
    return facingRight_;
}

int Player::lives() const {
    return lives_;
}

bool Player::isInvincible() const {
    return invincibleTimer_ > 0.0f;
}

bool Player::isDead() const {
    return dead_;
}

int Player::currentImageId() const {
    if (dead_) {
        return imageForDirection(facingRight_, IMG_SOLDIER_DEATH_RIGHT, IMG_SOLDIER_DEATH_LEFT);
    }
    if (hitAnimationTimer_ > 0.0f) {
        return imageForDirection(facingRight_, IMG_SOLDIER_HIT_RIGHT, IMG_SOLDIER_HIT_LEFT);
    }
    if (fireAnimationTimer_ > 0.0f) {
        return imageForDirection(facingRight_, IMG_SOLDIER_SHOOT_RIGHT, IMG_SOLDIER_SHOOT_LEFT);
    }
    if (!onGround_) {
        return imageForDirection(facingRight_, IMG_SOLDIER_JUMP_RIGHT, IMG_SOLDIER_JUMP_LEFT);
    }
    if (moving_) {
        const int frame = static_cast<int>(runAnimationTimer_ * 12.0f) % 4;
        switch (frame) {
        case 0:
            return imageForDirection(facingRight_, IMG_SOLDIER_RUN0_RIGHT, IMG_SOLDIER_RUN0_LEFT);
        case 1:
            return imageForDirection(facingRight_, IMG_SOLDIER_RUN1_RIGHT, IMG_SOLDIER_RUN1_LEFT);
        case 2:
            return imageForDirection(facingRight_, IMG_SOLDIER_RUN2_RIGHT, IMG_SOLDIER_RUN2_LEFT);
        default:
            return imageForDirection(facingRight_, IMG_SOLDIER_RUN3_RIGHT, IMG_SOLDIER_RUN3_LEFT);
        }
    }

    return imageForDirection(facingRight_, IMG_SOLDIER_IDLE_RIGHT, IMG_SOLDIER_IDLE_LEFT);
}

bool Player::shouldBlinkHide() const {
    return invincibleTimer_ > 0.0f && std::fmod(invincibleTimer_ * 20.0f, 2.0f) < 1.0f;
}

void Player::respawnAtCenter() {
    position_ = {kPlayerSpawnX, kPlayerSpawnY};
    velocityY_ = 0.0f;
    onGround_ = true;
}

}  // namespace am
