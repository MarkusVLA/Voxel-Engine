#include "game.h"
#include <iostream>
#include <exception>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {

    #ifdef DEBUG_MODE
        std::cout << "Running in debug mode." << std::endl;
    #endif

    unsigned int seed;

    if (argc > 1) {
        try {
            seed = static_cast<unsigned int>(std::stoul(argv[1]));
            std::cout << "Using seed: " << seed << std::endl;
        } catch (const std::exception& e) {
            seed = static_cast<unsigned int>(std::time(nullptr));
            std::cerr << "Invalid seed. Using seed: " << seed << std::endl;

        }
    } else {
        seed = static_cast<unsigned int>(std::time(nullptr));
        std::cout << "Using seed: " << seed << std::endl;
    }

    try {
        Game game(seed);
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
