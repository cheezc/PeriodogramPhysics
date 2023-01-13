#include "screen_runner.hpp"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 1000
#define GRAVITY 9.81f


int main() {
    sf::Vector2u windowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    PhysicsRenderer renderer(windowSize, GRAVITY);
    renderer.Run();
}