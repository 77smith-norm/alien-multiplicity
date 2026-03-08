#include "Wave.h"

#include <algorithm>

#include "constants.h"

namespace am {
namespace {
Vector2 spawnPosition(int index, int total) {
    const float spacing = static_cast<float>(kScreenWidth - 160) / static_cast<float>(std::max(total - 1, 1));
    const float x = (total == 1) ? (kScreenWidth * 0.5f) : 80.0f + (spacing * static_cast<float>(index));
    const float y = 120.0f + static_cast<float>((index % 3) * 70);
    return Vector2{x, y};
}

Vector2 spawnVelocity(int index) {
    const float x = (index % 2 == 0) ? -40.0f : 40.0f;
    const float y = static_cast<float>((index % 3) - 1) * 18.0f;
    return Vector2{x, y};
}
}

void Wave::reset() {
    currentWave_ = 0;
    bannerTimer_ = 0.0f;
    intermissionTimer_ = 0.0f;
    betweenWaves_ = false;
}

WaveUpdate Wave::start() {
    return launchNextWave();
}

WaveUpdate Wave::update(float dt, bool arenaEmpty) {
    if (bannerTimer_ > 0.0f) {
        bannerTimer_ = std::max(0.0f, bannerTimer_ - dt);
    }

    if (betweenWaves_) {
        intermissionTimer_ -= dt;
        if (intermissionTimer_ <= 0.0f) {
            betweenWaves_ = false;
            return launchNextWave();
        }
        return {};
    }

    if (arenaEmpty && currentWave_ > 0) {
        betweenWaves_ = true;
        intermissionTimer_ = kWaveAdvanceDelaySeconds;
        return WaveUpdate{false, true, {}};
    }

    return {};
}

int Wave::currentWave() const {
    return currentWave_;
}

bool Wave::bannerVisible() const {
    return bannerTimer_ > 0.0f;
}

WaveUpdate Wave::launchNextWave() {
    ++currentWave_;
    bannerTimer_ = kWaveBannerSeconds;
    return WaveUpdate{true, false, buildSpawns(currentWave_)};
}

std::vector<AlienSpawnRequest> Wave::buildSpawns(int waveNumber) const {
    std::vector<AlienSpawnRequest> spawns;

    if (waveNumber == 1) {
        spawns.push_back({AlienTier::t1, Vector2{400.0f, 140.0f}, Vector2{-40.0f, 16.0f}});
        return spawns;
    }

    if (waveNumber == 2) {
        spawns.push_back({AlienTier::t1, Vector2{260.0f, 140.0f}, Vector2{-40.0f, 18.0f}});
        spawns.push_back({AlienTier::t1, Vector2{540.0f, 220.0f}, Vector2{40.0f, -18.0f}});
        return spawns;
    }

    if (waveNumber == 3) {
        spawns.push_back({AlienTier::t1, Vector2{260.0f, 130.0f}, Vector2{-40.0f, 12.0f}});
        spawns.push_back({AlienTier::t2, Vector2{560.0f, 220.0f}, Vector2{45.0f, -12.0f}});
        return spawns;
    }

    for (int i = 0; i < waveNumber; ++i) {
        spawns.push_back({AlienTier::t1, spawnPosition(i, waveNumber), spawnVelocity(i)});
    }
    return spawns;
}

}  // namespace am
