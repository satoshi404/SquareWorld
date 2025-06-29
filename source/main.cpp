#include "utils/windowCreator.h"

#include <stdexcept>
#include <thread>
#include <chrono>
#include <memory>

#define Sleep(time)  std::this_thread::sleep_for(std::chrono::seconds(time));

int main() {

    std::unique_ptr<WindowCreator> creator = std::make_unique<WindowCreator>();
    creator.get()->Create();
    creator.get()->Create();
    creator.get()->ShowAll();
    //creator.get()->ShowByIndice(1);  // 0 to 1
    Sleep(5);
    return 0;
}