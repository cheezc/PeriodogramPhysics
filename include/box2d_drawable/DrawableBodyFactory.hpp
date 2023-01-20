#ifndef DRAWABLE_BODY_FACTORY_H
#define DRAWABLE_BODY_FACTORY_H
#include "DrawableWorld.hpp"
#include "DrawableShape.hpp"
#include "KinematicRectangle.hpp"
#include "DrawableCircle.hpp"

class DrawableBodyFactory {
public:
    DrawableBodyFactory(DrawableWorld& world);

    DrawableRectangle* createDynamicBox(
        sf::Vector2f& pos,
        sf::Vector2f& size
    );

    DrawableRectangle* createStaticBox(
        sf::Vector2f& pos,
        sf::Vector2f& size
    );

    KinematicRectangle* createKinematicBox(sf::Vector2f& pos,
        sf::Vector2f& size,
        sf::Vector2f& maxPos,
        sf::Vector2f& minPos
    );

    DrawableCircle* createDynamicCircle(
        sf::Vector2f& pos,
        float radius
    );

    b2Joint* createConnectedJoint(
        b2Body* bodyA,
        b2Body* bodyB,
        b2Vec2& localAnchorA,
        b2Vec2& localAnchorB
    );

    b2Joint* createConnectedJoint(
        b2JointType jointType,
        b2Body* bodyA,
        b2Body* bodyB,
        b2Vec2& localAnchorA,
        b2Vec2& localAnchorB,
        float maxAngle,
        float minAngle
    );

private:
    DrawableWorld& m_world;
};

#endif