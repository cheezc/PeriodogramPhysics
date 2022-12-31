#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_common.h"
#include "box2d/b2_polygon_shape.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include "box2d_drawable/DrawableWorld.hpp"
#include <chrono>

#define WindowWidth 800
#define WindowHeight 600
#define GroundWidth 800.f
#define GroundHeight 16.f
#define GroundPosX 400.f
#define GroundPosY 500.f
#define GroundX

#define BoxWidth 32.f
#define BoxHeight 32.f
#define TimeStep (1.f/120.f)
#define PositionIterations 8
#define VelocityIterations 3

void LogMousePress(int x, int y) {
    std::cout << "Mouse X: " << x << " Mouse Y: " << y << std::endl;
}

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<float> fsec;
static auto tsDuration = fsec{TimeStep};

int main() {

    // Setup Window and World
    sf::Vector2u windowSize(WindowWidth, WindowHeight);
    sf::VideoMode mode = sf::VideoMode(windowSize);
    sf::String title("workbench");
    b2Vec2 gravity(0.f, 9.81f);
    DrawableWorld worldWindow(mode, title, gravity);
    worldWindow.setFramerateLimit(60);

    // Create Ground
    sf::Vector2f groundPos(GroundPosX, GroundPosY);
    sf::Vector2f groundSize(GroundWidth, GroundHeight);
    worldWindow.createStaticBox(groundPos, groundSize);

    auto frameStartTime = Clock::now();
    fsec accumulator = fsec::zero();
    sf::View view = worldWindow.getDefaultView();

    // Rendering loop
    while (worldWindow.isOpen()) {
        sf::Event event;

        while (worldWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                worldWindow.close();
            } else if (event.type == sf::Event::Resized) {
                // resize my view
                view.setSize({
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)
                });
                worldWindow.setView(view);
                worldWindow.resize();
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Convert pixel to world coordinates in case of resize
            sf::Vector2i pixelPos = sf::Mouse::getPosition(worldWindow);
            sf::Vector2f worldPos = worldWindow.mapPixelToCoords(pixelPos);
            int mouseX = worldPos.x;
            int mouseY = worldPos.y;
            if (mouseX <= WindowWidth && mouseY <= WindowHeight && mouseX >= 0 && mouseY >= 0) {
                LogMousePress(mouseX, mouseY);
                sf::Vector2f boxPos(mouseX, mouseY);
                sf::Vector2f boxSize(BoxWidth, BoxHeight);
                worldWindow.createDynamicBox(boxPos, boxSize);
            }
        }

        // Get FrameTime
        auto frameEndTime = Clock::now();
        auto frameTime = frameEndTime - frameStartTime;
        frameStartTime = frameEndTime;
        accumulator += std::chrono::duration_cast<fsec>(frameTime);

        // Advance physics world by frameTime
        while(accumulator.count() >= tsDuration.count()) {
            worldWindow.Step(TimeStep, PositionIterations, VelocityIterations);
            accumulator -= tsDuration;
        }
        // std::cout << "Done Stepping" << std::endl;
        // Need to sync with frame time
        worldWindow.draw(sf::Color::Black);
        worldWindow.display();
    }

    return 0;
}