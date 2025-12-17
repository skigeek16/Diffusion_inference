#include "gui_app.h"
#include <iostream>

int main(int argc, char* argv[]) {
    GUIApp app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    app.run();
    app.shutdown();
    
    return 0;
}
