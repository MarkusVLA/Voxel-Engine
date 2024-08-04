#include "game.h"
#include <iostream>
#include <exception>

int main() {
    #ifdef DEBUG_MODE
    std::cout << "Running in debug mode." << std::endl;
    #endif

    try {
        Game game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    return 0;
}
