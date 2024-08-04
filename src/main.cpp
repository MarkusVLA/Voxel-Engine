#include "game.h"

int main() {
    #ifdef DEBUG_MODE
    std::cout << "Running in debug mode," << std::endl;
    #endif

    Game game;
    game.run();

    return 0;
}
