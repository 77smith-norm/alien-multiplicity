#include "DarkGDK.h"

#include "Game.h"
#include "constants.h"
#include "raylib.h"

void DarkGDK() {
    dbSetDisplayMode(am::kScreenWidth, am::kScreenHeight, am::kScreenDepth);
    dbSyncOn();
    dbSyncRate(am::kTargetFps);

    am::Game game;

    while (LoopGDK()) {
        dbCLS();
        game.update(GetFrameTime());
        game.draw();
        dbSync();
    }
}
