#pragma once

#include <cstdint>
#include <string>

namespace am {

class HUD {
public:
    void drawPlaying(int score, int wave, int lives, int comboMultiplier) const;
    void drawWaveBanner(int wave) const;
    void drawTitle(int bestScore) const;
    void drawPaused() const;
    void drawGameOver(int score, int bestScore) const;
    void hideHearts() const;

private:
    void drawCenteredText(int y, const std::string& text, std::uint32_t color) const;
};

}  // namespace am
