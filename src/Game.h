#pragma once

#include <optional>
#include <vector>

#include "Alien.h"
#include "HUD.h"
#include "Laser.h"
#include "Player.h"
#include "Wave.h"
#include "raylib.h"

namespace am {

enum class GameState {
    title,
    playing,
    paused,
    gameOver,
};

struct VisualEffect {
    int spriteId{0};
    int imageId{0};
    Vector2 position{};
    float timer{0.0f};
};

struct WeaponCrate {
    Rectangle bounds{};
    WeaponType weapon{WeaponType::laser};
    bool active{false};
};

struct HealthCrate {
    Rectangle bounds{};
    bool active{false};
};

class Game {
public:
    Game();

    void update(float dt);
    void draw();

private:
    void loadAssets();
    void clearAliens();
    void clearEffects();
    void clearWorld();
    void startNewGame();
    void spawnAlien(AlienTier tier, const Vector2& center, const Vector2& velocity);
    void spawnWeaponCrate();
    void spawnHealthCrate();
    void updateWeaponCrate(float dt);
    void updateHealthCrate(float dt);
    float distanceToScreenEdge(const Vector2& origin, const Vector2& direction) const;
    Vector2 randomPickupPosition(float width, float height, std::optional<float> avoidX = std::nullopt) const;

    void updateTitle();
    void updatePlaying(float dt);
    void updatePaused();
    void updateGameOver();

    void drawGround() const;
    void drawWeaponCrate() const;
    void drawHealthCrate() const;
    void drawWorld() const;

    Player player_{};
    Laser laser_{};
    HUD hud_{};
    Wave wave_{};
    std::vector<Alien> aliens_{};
    std::vector<VisualEffect> effects_{};
    WeaponCrate weaponCrate_{};
    HealthCrate healthCrate_{};
    int nextAlienSpriteId_{SPR_ALIEN_START};
    int nextEffectSpriteId_{SPR_EFFECT_START};
    int score_{0};
    int bestScore_{0};
    int comboMultiplier_{1};
    float lastKillTime_{-100.0f};
    float weaponCrateTimer_{kWeaponCrateSpawnSeconds};
    float healthCrateTimer_{kHealthCrateSpawnSeconds};
    float weaponAnnouncementTimer_{0.0f};
    WeaponType announcedWeapon_{WeaponType::laser};
    GameState state_{GameState::title};
};

}  // namespace am
