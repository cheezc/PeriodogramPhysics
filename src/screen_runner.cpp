#include "screen_runner.hpp"
#include <thread>
#include "DrawableRectangle.hpp"
#include "KinematicRectangleArray.hpp"
#include "Menu.hpp"
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
#include "box2d_drawable/KinematicRectangle.hpp"
#include <chrono>
#include "box2d_drawable/utils.hpp"
#include "box2d_drawable/DrawableBodyFactory.hpp"
#include "gui/Label.hpp"
#include "gui/Theme.hpp"

// Ground Params
#define GroundHeight 1.f

// Ceiling Params
#define CeilingHeight 1.f
#define CeilingPosY (CeilingHeight/2)

// Wall Params
#define WallWidth 1.f

// Number of rectangles
#define NUM_KINEMATIC_RECTANGLES 128

// Physics Timestep and Collision detection params
#define TIME_STEP (1.f/120.f)
#define POSITION_ITERATIONS 8
#define VELOCITY_ITERATIONS 3

// Mic sample rate
#define MIC_SAMPLE_RATE 8000

// Test boxes
#define BOX_WIDTH 32.f
#define BOX_HEIGHT 32.f

// Some const vectors for sizing menu and test objects
const sf::Vector2f MenuPosition(100, 100);
sf::Vector2f BoxSize(BOX_WIDTH, BOX_HEIGHT);

// Some typedefs
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<float> fsec;
static auto tsDuration = fsec{TIME_STEP};

PhysicsRenderer::PhysicsRenderer(sf::Vector2u windowSize, float gravityScalar)
: m_windowSize(windowSize) {
    auto callback = std::bind(&PhysicsRenderer::sliderCallback, this);
    m_slider.setCallback(callback);

    // Setup Window and World
    sf::VideoMode mode = sf::VideoMode(m_windowSize);
    sf::String title("workbench");
    b2Vec2 gravity(0.f, gravityScalar);
    m_worldWindow = std::make_unique<DrawableWorld>(mode, title, gravity);
    m_worldWindow.get()->setFramerateLimit(60);

    // Setup factory
    m_factory = std::make_unique<DrawableBodyFactory>(*m_worldWindow.get());
}

void PhysicsRenderer::Run() {
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
    setupBoundaries(*m_worldWindow.get());

    // Create Kinematic Array
    setupSpectrumAnalyzer(*m_worldWindow.get());

    // Some pre-loop setup
    (void)m_karray->start(MIC_SAMPLE_RATE);

    doRun();
}

void PhysicsRenderer::doRun() {
    // Make sure the physics step aligns with the frame-rate
    auto frameStartTime = Clock::now();
    fsec accumulator = fsec::zero();
    sf::View view = m_worldWindow.get()->getDefaultView();

    // Create menu
    gui::Theme::loadFont("../fonts/tahoma.ttf");
    gui::Theme::loadTexture("../fonts/texture-default.png");
    gui::Menu menu(*m_worldWindow.get());
    gui::Label label("Microphone Gain");
    label.setTextSize(20);
    label.setFillColor(sf::Color::White);
    menu.add(&label);
    menu.add(&m_slider);
    menu.setPosition(MenuPosition);

    // Rendering loop
    while (m_worldWindow.get()->isOpen()) {
        sf::Event event;
        // UpdateKinematicArray(kArray, DEBUG_AUDIO);

        while (m_worldWindow.get()->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                m_karray->stop();
                m_worldWindow.get()->close();
            } else if (event.type == sf::Event::Resized) {
                // resize my view
                view.setSize({
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)
                });
                m_worldWindow.get()->setView(view);
                m_worldWindow.get()->resize();
            }
            menu.onEvent(event);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Convert pixel to world coordinates in case of resize
            sf::Vector2i pixelPos = sf::Mouse::getPosition(*m_worldWindow.get());
            sf::Vector2f worldPos = m_worldWindow.get()->mapPixelToCoords(pixelPos);
            int mouseX = worldPos.x;
            int mouseY = worldPos.y;
            if (mouseX <= m_windowSize.x && mouseY <= m_windowSize.y && mouseX >= 0 && mouseY >= 0) {
                sf::Vector2f boxPos(mouseX, mouseY);
                m_factory->createDynamicBox(boxPos, BoxSize);
            }
        }

        // Get FrameTime
        auto frameEndTime = Clock::now();
        auto frameTime = frameEndTime - frameStartTime;
        frameStartTime = frameEndTime;
        accumulator += std::chrono::duration_cast<fsec>(frameTime);

        // Advance physics world by frameTime, in case we stutter
        while(accumulator.count() >= tsDuration.count()) {
            m_worldWindow.get()->Step(TIME_STEP, POSITION_ITERATIONS, VELOCITY_ITERATIONS);
            accumulator -= tsDuration;
        }

        // Render the world
        m_worldWindow.get()->drawWorld(sf::Color::Black);
        m_worldWindow.get()->draw(menu);
        m_worldWindow.get()->display();
    }
}

void PhysicsRenderer::sliderCallback() {
    int gain = m_slider.getValue();
    std::cout << "Setting Mic Gain" << std::endl;
    SHOW(gain);
    m_karray.get()->SetMicrophoneGain(gain);
}

void PhysicsRenderer::setupSpectrumAnalyzer(DrawableWorld& worldWindow) {
    sf::Vector2f arrayDimensions(m_windowSize.x, m_windowSize.y);
    sf::Vector2f arrayOrigin(0, 0);
    m_karray = std::make_unique<DrawableKinematicRectangleArrayRecorder>(
            NUM_KINEMATIC_RECTANGLES,
            arrayDimensions,
            arrayOrigin,
            &worldWindow,
            m_factory.get()
        );
}

void PhysicsRenderer::setupBoundaries(DrawableWorld &worldWindow) {
    // Greate Ground
    uint groundX = m_windowSize.x/2;
    uint groundY = m_windowSize.y - GroundHeight/2;
    uint groundHeight = GroundHeight;
    uint groundWidth = m_windowSize.x;
    sf::Vector2f groundPos = worldWindow.mapPixelToCoords(sf::Vector2i(groundX, groundY) );
    sf::Vector2f groundSize = worldWindow.mapPixelToCoords(sf::Vector2i(groundWidth, groundHeight));
    m_factory->createStaticBox(groundPos, groundSize);

    // Create ceiling
    uint ceilingX = m_windowSize.x/2.f;
    uint ceilingY = CeilingHeight/2;
    uint ceilingWidth = m_windowSize.x;
    uint ceilingHeight = ceilingHeight;
    sf::Vector2f ceilingPos = worldWindow.mapPixelToCoords(sf::Vector2i(ceilingX, ceilingY));
    sf::Vector2f ceilingSize = worldWindow.mapPixelToCoords(sf::Vector2i(ceilingWidth, CeilingHeight));
    auto ceiling = m_factory->createStaticBox(ceilingPos, ceilingSize);

    // Create Walls
    uint wallY = m_windowSize.y/2.f;
    uint wallHeight = m_windowSize.y;
    uint wallWidth = WallWidth;

    sf::Vector2f leftWallPos = worldWindow.mapPixelToCoords(sf::Vector2i(0, wallY) );
    sf::Vector2f wallSize = worldWindow.mapPixelToCoords(sf::Vector2i(wallWidth, wallHeight));
    sf::Vector2f rightWallPos = worldWindow.mapPixelToCoords(sf::Vector2i(m_windowSize.x, wallY));
    m_factory->createStaticBox(leftWallPos, wallSize);
    m_factory->createStaticBox(rightWallPos, wallSize);

}
