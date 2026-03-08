#pragma once

#include <vector>

#include "Alien.h"

namespace am {

struct WaveUpdate {
    bool waveStarted{false};
    bool waveCleared{false};
    std::vector<AlienSpawnRequest> spawns{};
};

class Wave {
public:
    void reset();
    WaveUpdate start();
    WaveUpdate update(float dt, bool arenaEmpty);

    int currentWave() const;
    bool bannerVisible() const;

private:
    WaveUpdate launchNextWave();
    std::vector<AlienSpawnRequest> buildSpawns(int waveNumber) const;

    int currentWave_{0};
    float bannerTimer_{0.0f};
    float intermissionTimer_{0.0f};
    bool betweenWaves_{false};
};

}  // namespace am
