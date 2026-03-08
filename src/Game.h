#pragma once

#include <vector>

#include "Alien.h"
#include "Laser.h"
#include "Player.h"
#include "raylib.h"

namespace am {

struct VisualEffect {
    int spriteId{0};
    int imageId{0};
    Vector2 position{};
    float timer{0.0f};
};

class Game {
public:
    Game();

    void update(float dt);
    void draw();

private:
    void loadAssets();
    void resetWorld();
    void spawnAlien(AlienTier tier, const Vector2& center, const Vector2& velocity);
    float distanceToScreenEdge(const Vector2& origin, const Vector2& direction) const;
    void drawDebugText() const;

    Player player_{};
    Laser laser_{};
    std::vector<Alien> aliens_{};
    std::vector<VisualEffect> effects_{};
    int nextAlienSpriteId_{SPR_ALIEN_START};
    int nextEffectSpriteId_{SPR_EFFECT_START};
};

}  // namespace am
