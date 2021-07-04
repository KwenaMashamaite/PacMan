#include <IME/core/scene/Scene.h>
#include <IME/core/engine/Engine.h>

#ifdef NDEBUG
    #include "windows.h"
#endif

//
class EmptyScene : public ime::Scene {
public:
    void onEnter() override {}
};

int main() {

    // Hide console window in release mode
#ifdef NDEBUG
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);
#endif

    ime::Engine engine{"Pac-Man", "assets/textFiles/settings.txt"};
    engine.initialize();
    engine.pushScene(std::make_unique<EmptyScene>());
    engine.run();

    return 0;
}
