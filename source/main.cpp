#include "utils/windowCreator.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>

#define Sleep(time)  std::this_thread::sleep_for(std::chrono::seconds(time));

int main() {
    WindowCreator creator;
    creator.Create();
    creator.Create();
    
    //creator.ShowAll();

    creator.ShowByIndice(12);
    Sleep(5);
    
    return 0;
}