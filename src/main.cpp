#include "DarkGDK.h"

#include "constants.h"

void DarkGDK() {
    dbSetDisplayMode(am::kScreenWidth, am::kScreenHeight, am::kScreenDepth);
    dbSyncOn();
    dbSyncRate(am::kTargetFps);

    while (LoopGDK()) {
        dbCLS();
        dbSync();
    }
}
