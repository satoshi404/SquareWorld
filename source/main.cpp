#include "utils/windowCreator.h"

int main() {
    WindowCreator creator;
    creator.LoadFromJSON("scene.json");
    creator.DrawAll();
    return 0;
}