#include "DrawableRectangle.hpp"
#include "KinematicRectangleArray.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Audio/SoundRecorder.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_common.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>
#include "box2d_drawable/DrawableWorld.hpp"
#include "box2d_drawable/KinematicRectangle.hpp"
#include <chrono>
#include "box2d_drawable/utils.hpp"
#include "box2d_drawable/SoundKinematicRectangleArray.hpp"
#include "Slider.hpp"

#define WindowWidth 1200
#define WindowHeight 1000

// Ground Params
#define GroundWidth WindowWidth
#define GroundHeight 1.f
#define GroundPosX WindowWidth/2.f
#define GroundPosY (WindowHeight-GroundHeight/2)

// Ceiling Params
#define CeilingWidth WindowWidth
#define CeilingHeight 1.f
#define CeilingPosX WindowWidth/2.f
#define CeilingPosY (CeilingHeight/2)

// Wall Params
#define WallWidth 1.f
#define WallHeight WindowHeight
#define WallPositionY WindowHeight/2

#define BoxWidth 32.f
#define BoxHeight 32.f
#define TimeStep (1.f/120.f)
#define PositionIterations 8
#define VelocityIterations 3
#define NumKinematicRectangles 129

#define PlatformSpeed 10.f
#define TestArrayLength 4000
#define DEBUG_AUDIO true
#define MIC_SAMPLE_RATE 8000

void LogMousePress(int x, int y) {
    std::cout << "Mouse X: " << x << " Mouse Y: " << y << std::endl;
}

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<float> fsec;
static auto tsDuration = fsec{TimeStep};

void LogError(const std::string & err) {
    std::cout << "Error: " << err << std::endl;
}

void setupKinematicMovement(KinematicRectangle* box) {
    if (box) {
        // Do some setup
        auto setup = [](KinematicRectangle* box, KinematicRectangle::targetReachedCb cb) {
            b2Vec2 targetPos(box->GetBodyPosition().x, 0);
            box->SetTargetPosition(targetPos, PlatformSpeed, cb);
        };

        KinematicRectangle::targetReachedCb targetCb = [](KinematicRectangle* box){
            b2Vec2 targetPos = box->GetBodyPosition() + sf2box(sf::Vector2f(0, WindowHeight-GroundHeight));
            std::cout << "targetReachedCb" << std::endl;
            box->SetTargetPosition(targetPos, PlatformSpeed, nullptr);
        };

        setup(box, targetCb);
    } else {
        LogError("nil kinematic box");
    }
}

void UpdateKinematicArray(
    std::unique_ptr<DrawableKinematicRectangleArray>& kArray,
    bool debug
) {
    if (debug) {
        std::vector<float> positions(TestArrayLength);
        for (int i = 0; i < TestArrayLength; i++) {
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/WindowHeight);
            positions[i] = r;
        }
        kArray->SetArrayPositions(positions);
    }
}

int main() {

    // Setup Window and World
    sf::Vector2u windowSize(WindowWidth, WindowHeight);
    sf::VideoMode mode = sf::VideoMode(windowSize);
    sf::String title("workbench");
    b2Vec2 gravity(0.f, 9.81f);
    DrawableWorld worldWindow(mode, title, gravity);
    worldWindow.setFramerateLimit(60);

    // Do pixel mapping
    if (!sf::SoundRecorder::isAvailable()) {
        std::cout << "Sound recorder not available" << std::endl;
    } else {
        std::cout << "Sound RecordingAvailable" << std::endl;
        auto devices = sf::SoundRecorder::getAvailableDevices();
        for (std::string device: devices) {
            std::cout << "Audio Device: " << device << "\n";
        }
    }

    // Create Boundaries

    // Greate Ground
    sf::Vector2f groundPos = worldWindow.mapPixelToCoords(sf::Vector2i(GroundPosX, GroundPosY) );
    sf::Vector2f groundSize = worldWindow.mapPixelToCoords(sf::Vector2i(GroundWidth, GroundHeight));
    worldWindow.createStaticBox(groundPos, groundSize);

    // Create ceiling
    sf::Vector2f ceilingPos = worldWindow.mapPixelToCoords(sf::Vector2i(CeilingPosX, CeilingPosY) );
    sf::Vector2f ceilingSize = worldWindow.mapPixelToCoords(sf::Vector2i(CeilingWidth, CeilingHeight));
    auto ceiling = worldWindow.createStaticBox(ceilingPos, ceilingSize);

    // Create Walls
    sf::Vector2f leftWallPos = worldWindow.mapPixelToCoords(sf::Vector2i(0, WallPositionY) );
    sf::Vector2f wallSize = worldWindow.mapPixelToCoords(sf::Vector2i(WallWidth, WallHeight));
    sf::Vector2f rightWallPos = worldWindow.mapPixelToCoords(sf::Vector2i(WindowWidth, WallPositionY));
    worldWindow.createStaticBox(leftWallPos, wallSize);
    worldWindow.createStaticBox(rightWallPos, wallSize);

    // Create Kinematic Array
    sf::Vector2f arrayDimensions(WindowWidth, WindowHeight);
    sf::Vector2f arrayOrigin(0, 0);
    std::unique_ptr<DrawableKinematicRectangleArrayRecorder> kArray =
        std::make_unique<DrawableKinematicRectangleArrayRecorder>(
            NumKinematicRectangles,
            arrayDimensions,
            arrayOrigin,
            &worldWindow
        );

    // Do some Logging
    SHOW(kArray->getChannelCount());

    // Make sure the physics step aligns with the frame-rate
    auto frameStartTime = Clock::now();
    fsec accumulator = fsec::zero();

    // Some pre-loop setup
    sf::View view = worldWindow.getDefaultView();
    (void)kArray->start(MIC_SAMPLE_RATE);
    sf::Vector2f boxSize(BoxWidth, BoxHeight);

    // kArray->DisplayNotes();
    // Display slider
    gui::Slider slider;
    slider.setPosition(sf::Vector2f(30, 30));


    // Rendering loop
    while (worldWindow.isOpen()) {
        sf::Event event;
        // UpdateKinematicArray(kArray, DEBUG_AUDIO);

        while (worldWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                kArray->stop();
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
                worldWindow.createDynamicBox(boxPos, boxSize);
            }
        }

        // Get FrameTime
        auto frameEndTime = Clock::now();
        auto frameTime = frameEndTime - frameStartTime;
        frameStartTime = frameEndTime;
        accumulator += std::chrono::duration_cast<fsec>(frameTime);

        // Advance physics world by frameTime, in case we stutter
        while(accumulator.count() >= tsDuration.count()) {
            worldWindow.Step(TimeStep, PositionIterations, VelocityIterations);
            accumulator -= tsDuration;
        }

        // Render the world
        worldWindow.draw(sf::Color::Black);
        dynamic_cast<sf::RenderWindow*>(&worldWindow)->draw(slider);
        worldWindow.display();
    }

    return 0;
}