#ifndef SCREEN_RUNNER_H
#define SCREEN_RUNNER_H


#include "TextBox.hpp"
#include "box2d_drawable/SoundKinematicRectangleArray.hpp"
#include "Slider.hpp"
#include "box2d_drawable/DrawableWorld.hpp"
#include "box2d_drawable/DrawableBodyFactory.hpp"
#include "box2d_drawable/StickFigure.hpp"

#include <thread>

class PhysicsRenderer {
public:
    PhysicsRenderer(sf::Vector2u windowSize, float gravityScalar);
    void Run();

private:
    void doRun();
    void sliderCallback();
    void setupSpectrumAnalyzer(DrawableWorld& worldWindow);
    void setupBoundaries(DrawableWorld &worldWindow);
    std::unique_ptr<DrawableKinematicRectangleArrayRecorder> m_karray;
    std::vector<StickFigure> m_stickFigures;
    sf::Vector2u m_windowSize;
    gui::Slider m_slider;
    float m_gravityScalar;

    std::unique_ptr<DrawableWorld> m_worldWindow;
    std::unique_ptr<DrawableBodyFactory> m_factory;
};

#endif