#ifndef STICK_FIGURE_H
#define STICK_FIGURE_H

#include "DrawableBodyFactory.hpp"
#include "DrawableCircle.hpp"
#include "DrawableRectangle.hpp"
#include "DrawableWorld.hpp"

#define DEFAULT_ARM_LENGTH 15
#define DEFAULT_LEG_LENGTH 20
#define DEFAULT_TORSO_LENGTH 15
#define DEFAULT_HEAD_RADIUS 3
#define DEFAULT_STICK_WIDTH 1

class StickFigure {
public:
    StickFigure(
        sf::Vector2f& bodyCentroid,
        float scalingFactor,
        DrawableBodyFactory* factory
    );

private:
    DrawableBodyFactory *m_factory;
    DrawableCircle *m_head;
    DrawableRectangle *m_torso;
    DrawableRectangle *m_leftArm;
    DrawableRectangle *m_rightArm;
    DrawableRectangle *m_leftLeg;
    DrawableRectangle *m_rightLeg;
};

#endif