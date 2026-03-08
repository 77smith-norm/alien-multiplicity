#include "HUD.h"

#include <iomanip>
#include <sstream>

#include "DarkGDK.h"
#include "constants.h"
#include "raylib.h"

namespace am {
namespace {
std::string scoreLabel(int score) {
    std::ostringstream stream;
    stream << "SCORE: " << std::setw(6) << std::setfill('0') << score;
    return stream.str();
}

std::string waveLabel(int wave) {
    std::ostringstream stream;
    stream << "WAVE: " << std::setw(2) << std::setfill('0') << wave;
    return stream.str();
}
}

void HUD::drawPlaying(int score, int wave, int lives, int comboMultiplier) const {
    const std::string scoreText = scoreLabel(score);
    const std::string waveText = waveLabel(wave);
    const int waveX = (kScreenWidth - MeasureText(waveText.c_str(), kTextSize)) / 2;

    dbInk(kColorWhite, 0);
    dbText(16, 16, scoreText.c_str());
    dbText(waveX, 16, waveText.c_str());

    const int heartY = 18;
    const int heartSpacing = 18;
    const int heartStartX = kScreenWidth - 72;
    for (int i = 0; i < kPlayerStartingLives; ++i) {
        const int spriteId = SPR_HUD_HEART0 + i;
        if (i < lives) {
            dbSprite(spriteId, heartStartX + (i * heartSpacing), heartY, IMG_HUD_HEART);
        } else if (dbSpriteExist(spriteId)) {
            dbHideSprite(spriteId);
        }
    }

    if (comboMultiplier > 1) {
        std::ostringstream combo;
        combo << "COMBO x" << comboMultiplier;
        drawCenteredText(44, combo.str(), kColorYellow);
    }
}

void HUD::drawWaveBanner(int wave) const {
    std::ostringstream stream;
    stream << "WAVE " << wave;
    drawCenteredText(96, stream.str(), kColorGreen);
}

void HUD::drawTitle(int bestScore) const {
    hideHearts();
    drawCenteredText(150, "ALIEN MULTIPLICITY", kColorYellow);
    drawCenteredText(210, "Shoot them down. Watch them multiply.", kColorWhite);
    drawCenteredText(270, "Press Space or Enter to Start", kColorGreen);

    std::ostringstream best;
    best << "BEST SCORE: " << bestScore;
    drawCenteredText(330, best.str(), kColorWhite);
    drawCenteredText(390, "A/D or arrows move, W or Up jumps, mouse aims.", kColorWhite);
}

void HUD::drawPaused() const {
    drawCenteredText(220, "PAUSED", kColorYellow);
    drawCenteredText(260, "Press Esc to Resume", kColorWhite);
}

void HUD::drawGameOver(int score, int bestScore) const {
    hideHearts();
    drawCenteredText(180, "GAME OVER", kColorRed);

    std::ostringstream scoreStream;
    scoreStream << "SCORE: " << score;
    drawCenteredText(240, scoreStream.str(), kColorWhite);

    std::ostringstream bestStream;
    bestStream << "BEST: " << bestScore;
    drawCenteredText(280, bestStream.str(), kColorWhite);
    drawCenteredText(340, "Press Space or Enter to Restart", kColorGreen);
}

void HUD::hideHearts() const {
    for (int i = 0; i < kPlayerStartingLives; ++i) {
        const int spriteId = SPR_HUD_HEART0 + i;
        if (dbSpriteExist(spriteId)) {
            dbHideSprite(spriteId);
        }
    }
}

void HUD::drawCenteredText(int y, const std::string& text, std::uint32_t color) const {
    const int x = (kScreenWidth - MeasureText(text.c_str(), kTextSize)) / 2;
    dbInk(color, 0);
    dbText(x, y, text.c_str());
}

}  // namespace am
