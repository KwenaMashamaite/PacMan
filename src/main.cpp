#include "src/models/game/Game.h"

#ifdef NDEBUG
    #include "windows.h"
#endif

int main() {

    // Hide console window in release mode
#ifdef NDEBUG
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
#endif

    pm::Game pacmanGame{};
    pacmanGame.initialize();
    pacmanGame.start();

    return 0;
}
