#pragma once

#include <array>
#include <cstdint>

namespace am {

enum class AlienTier {
    t1,
    t2,
    t3,
};

struct AlienTierConfig {
    AlienTier tier;
    int width;
    int height;
    int maxHp;
    float speed;
    int killScore;
};

constexpr int kScreenWidth = 800;
constexpr int kScreenHeight = 600;
constexpr int kScreenDepth = 32;
constexpr int kTargetFps = 60;
constexpr int kTextSize = 20;

constexpr float kGroundY = 540.0f;
constexpr float kGroundHeight = 60.0f;
constexpr float kGroundTop = kGroundY;

constexpr float kPlayerWidth = 48.0f;
constexpr float kPlayerHeight = 64.0f;
constexpr float kPlayerMoveSpeed = 200.0f;
constexpr float kPlayerJumpVelocity = 550.0f;
constexpr float kGravity = 1200.0f;
constexpr int kPlayerStartingLives = 3;
constexpr float kPlayerRespawnInvincibilitySeconds = 1.5f;
constexpr float kPlayerSpawnX = (kScreenWidth - kPlayerWidth) * 0.5f;
constexpr float kPlayerSpawnY = kGroundTop - kPlayerHeight;

constexpr float kLaserCooldownSeconds = 0.15f;
constexpr float kLaserVisualSeconds = 0.08f;
constexpr float kLaserRange = static_cast<float>(kScreenWidth);
constexpr int kLaserDamage = 1;
constexpr float kLaserPushbackSpeed = 120.0f;
constexpr int kLaserHitScore = 10;
constexpr float kBeamBaseLength = 256.0f;

constexpr float kAlienBobAmplitude = 40.0f;
constexpr float kAlienBobPeriodSeconds = 2.0f;
constexpr float kAlienDirectionChangeMinSeconds = 1.0f;
constexpr float kAlienDirectionChangeMaxSeconds = 3.0f;
constexpr float kAlienSplitOutwardSpeed = 140.0f;
constexpr float kAlienHitFlashSeconds = 0.1f;
constexpr float kSplitEffectSeconds = 0.18f;

constexpr float kWaveAdvanceDelaySeconds = 2.0f;
constexpr float kWaveBannerSeconds = 1.5f;
constexpr float kComboWindowSeconds = 2.0f;

constexpr std::uint32_t kColorWhite = 0xFFFFFF;
constexpr std::uint32_t kColorYellow = 0xFFE260;
constexpr std::uint32_t kColorGreen = 0x77FFAA;
constexpr std::uint32_t kColorRed = 0xFF5A5A;
constexpr std::uint32_t kColorBlue = 0x7EC8FF;

constexpr std::array<AlienTierConfig, 3> kAlienTierConfigs{{
    {AlienTier::t1, 64, 64, 3, 80.0f, 50},
    {AlienTier::t2, 40, 40, 5, 55.0f, 25},
    {AlienTier::t3, 24, 24, 8, 35.0f, 100},
}};

inline constexpr const AlienTierConfig& alienConfig(AlienTier tier) {
    switch (tier) {
    case AlienTier::t1:
        return kAlienTierConfigs[0];
    case AlienTier::t2:
        return kAlienTierConfigs[1];
    case AlienTier::t3:
    default:
        return kAlienTierConfigs[2];
    }
}

inline constexpr AlienTier nextAlienTier(AlienTier tier) {
    switch (tier) {
    case AlienTier::t1:
        return AlienTier::t2;
    case AlienTier::t2:
    default:
        return AlienTier::t3;
    }
}

enum ImageId {
    IMG_GROUND = 1,
    IMG_BEAM,
    IMG_HUD_HEART,

    IMG_SOLDIER_IDLE_RIGHT,
    IMG_SOLDIER_IDLE_LEFT,
    IMG_SOLDIER_RUN0_RIGHT,
    IMG_SOLDIER_RUN0_LEFT,
    IMG_SOLDIER_RUN1_RIGHT,
    IMG_SOLDIER_RUN1_LEFT,
    IMG_SOLDIER_RUN2_RIGHT,
    IMG_SOLDIER_RUN2_LEFT,
    IMG_SOLDIER_RUN3_RIGHT,
    IMG_SOLDIER_RUN3_LEFT,
    IMG_SOLDIER_JUMP_RIGHT,
    IMG_SOLDIER_JUMP_LEFT,
    IMG_SOLDIER_SHOOT_RIGHT,
    IMG_SOLDIER_SHOOT_LEFT,
    IMG_SOLDIER_HIT_RIGHT,
    IMG_SOLDIER_HIT_LEFT,
    IMG_SOLDIER_DEATH_RIGHT,
    IMG_SOLDIER_DEATH_LEFT,

    IMG_ALIEN_T1_FLOAT0,
    IMG_ALIEN_T1_FLOAT1,
    IMG_ALIEN_T1_HIT0,
    IMG_ALIEN_T1_HIT1,
    IMG_ALIEN_T1_SPLIT,

    IMG_ALIEN_T2_FLOAT0,
    IMG_ALIEN_T2_FLOAT1,
    IMG_ALIEN_T2_HIT0,
    IMG_ALIEN_T2_HIT1,
    IMG_ALIEN_T2_SPLIT,

    IMG_ALIEN_T3_FLOAT0,
    IMG_ALIEN_T3_FLOAT1,
    IMG_ALIEN_T3_FLOAT2,
    IMG_ALIEN_T3_DEATH,
    IMG_ALIEN_T3_VAPORIZE,
};

enum SpriteId {
    SPR_GROUND = 1,
    SPR_PLAYER = 100,
    SPR_PLAYER_BEAM = 101,
    SPR_HUD_HEART0 = 110,
    SPR_HUD_HEART1 = 111,
    SPR_HUD_HEART2 = 112,
    SPR_EFFECT_START = 1000,
    SPR_ALIEN_START = 2000,
};

}  // namespace am
