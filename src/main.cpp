#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init()) {
        std::cerr << "[ERROR] Failed to initialize game: "
        << SDL_GetError() << "\n";
        return 1;
    }

    game.run();
    return 0;
}