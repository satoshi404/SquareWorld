#include "ventor/x11.hpp"

#include <stdexcept>
#include <thread>
#include <chrono>



#define Sleep(time)  std::this_thread::sleep_for(std::chrono::seconds(time));

int main() {
    XDisplay *display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Não foi possível abrir o display\n");
        return 1;
    }

    XWindow window = XCreateSimpleWindow(
        display, XDefaultRootWindow(display), 
        10, 10, 200, 200, 1, 
        0x0, 0xffffff
    );
    
    printf("Janela criada com XID: %lu\n", window);

    XMapWindow(display, window);
    XFlush(display);
    
    Sleep(5);
    
    XCloseDisplay(display);
    return 0;
}