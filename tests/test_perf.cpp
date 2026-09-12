// How much does a fixed step cost as the building gets very tall?
#include "Tower.h"
#include "PhysicsWorld.h"
#include "Constants.h"
#include <chrono>
#include <cstdio>

int main() {
    sf::Texture tex;
    PhysicsWorld physics;
    physics.createPlatform(WINDOW_WIDTH * 0.5f, PLATFORM_Y,
                           PLATFORM_WIDTH, PLATFORM_HEIGHT);
    Tower tower;

    const float blockW = BLOCK_SIZE / PPM;
    float baseY = (PLATFORM_Y - PLATFORM_HEIGHT * 0.5f) / PPM - blockW * 0.5f;

    printf("%8s %14s %14s\n", "floors", "us/step", "steps/sec");
    for (int floors = 1; floors <= 1000; ++floors) {
        Block& b = tower.spawn(physics.getWorld(),
                               WINDOW_WIDTH * 0.5f, baseY * PPM, BLOCK_SIZE, tex);
        (void)b;
        tower.settleTop(b2Vec2(WINDOW_WIDTH * 0.5f / PPM, baseY));
        tower.registerPlacement(0.15f);
        baseY -= blockW;

        if (floors % 250 != 0) continue;

        // Camera sits at the top of the tower, as it would in play.
        const float viewTop = baseY * PPM - WINDOW_HEIGHT * 0.4f;
        tower.updateDormancy(physics.getWorld(), viewTop);

        const int N = 4000;
        auto t0 = std::chrono::steady_clock::now();
        for (int i = 0; i < N; ++i) {
            tower.savePrevious();
            tower.updateSway(FIXED_DT);
            tower.applySwayVelocities();
            physics.step(FIXED_DT);
            tower.applySwayTransforms();
            tower.syncSprites(0.5f);
        }
        auto t1 = std::chrono::steady_clock::now();
        const double us = std::chrono::duration<double, std::micro>(t1 - t0).count() / N;
        printf("%8d %14.2f %14.0f\n", floors, us, 1e6 / us);
    }
    printf("\n(a 120 Hz sim has 8333 us of budget per second of play)\n");
}
