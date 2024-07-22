#include "application.hpp"
#include "log.hpp"

#include <stdexcept>

int main() try {
    Application app;
    
    app.run();
} catch(std::runtime_error &e) {
    LOG_DEBUG("Runtime error: {}", e.what());
}
