#include "Game.h"
#include "BlockLogger.h"

int main() {
    LOG_INFO("Blocks game starting...");
    Game game;
    game.run();
    LOG_INFO("Blocks game shutting down.");
    return 0;
}
