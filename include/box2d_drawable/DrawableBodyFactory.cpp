#include "DrawableBodyFactory.hpp"
#include "CreationHelpers.hpp"
#include "DrawableCircle.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_distance_joint.h"
#include "box2d/b2_joint.h"

#define DEFAULT_FRICTION .7f
#define DEFAULT_DENSITY 1.0f


DrawableBodyFactory::DrawableBodyFactory(DrawableWorld& world): m_world(world) {

}

// Pos is the center of the box
DrawableRectangle* DrawableBodyFactory::createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_dynamicBody);
    b2Body* body = m_world.CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(size, body);
    createShape(body, DEFAULT_FRICTION, DEFAULT_DENSITY, drawableRectangle);
    return dynamic_cast<DrawableRectangle*>(body->GetFixtureList()->GetShape());
}

// Pos is the center of the box
DrawableRectangle* DrawableBodyFactory::createStaticBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_staticBody);
    b2Body* body = m_world.CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(size, body);
    createShape(body, DEFAULT_FRICTION, DEFAULT_DENSITY, drawableRectangle);
    return dynamic_cast<DrawableRectangle*>(body->GetFixtureList()->GetShape());
}

// Pos is the center of the box
KinematicRectangle* DrawableBodyFactory::createKinematicBox(
    sf::Vector2f& pos,
    sf::Vector2f& size,
    sf::Vector2f& maxPos,
    sf::Vector2f& minPos
) {
    b2BodyDef def = CreateBodyDef(pos, b2_kinematicBody);
    b2Body* body = m_world.CreateBody(&def);

    KinematicRectangle kinematicRectangle = CreateKinematicRectangle(pos, size, body, maxPos, minPos);
    createShape(body, DEFAULT_FRICTION, DEFAULT_DENSITY, kinematicRectangle);
    return dynamic_cast<KinematicRectangle*>(body->GetFixtureList()->GetShape());
}

// Pos is the center of the box
DrawableCircle* DrawableBodyFactory::createDynamicCircle(
    sf::Vector2f& pos,
    float radius
) {
    b2BodyDef def = CreateBodyDef(pos, b2_dynamicBody);
    b2Body* body = m_world.CreateBody(&def);

    DrawableCircle drawableCircle = CreateDrawableCircle(radius, body);
    createShape(body, DEFAULT_FRICTION, DEFAULT_DENSITY, drawableCircle);
    return dynamic_cast<DrawableCircle*>(body->GetFixtureList()->GetShape());
}

b2Joint* DrawableBodyFactory::createConnectedJoint(
    b2Body* bodyA,
    b2Body* bodyB,
    b2Vec2& anchorA,
    b2Vec2& anchorB
) {
    b2DistanceJointDef jointDef;
    jointDef.Initialize(bodyA, bodyB, anchorA, anchorB);
    jointDef.collideConnected = false;

    b2Joint* joint = m_world.CreateJoint(&jointDef);
    return joint;
}

b2Joint* DrawableBodyFactory::createConnectedJoint(
    b2JointType jointType,
    b2Body* bodyA,
    b2Body* bodyB,
    b2Vec2& localAnchorA,
    b2Vec2& localAnchorB,
    float maxAngle,
    float minAngle
) {
    b2RevoluteJointDef jointDef;
    jointDef.collideConnected = false;
    jointDef.enableLimit = true;
    jointDef.upperAngle = maxAngle;
    jointDef.lowerAngle = minAngle;
    jointDef.bodyA = bodyA;
    jointDef.bodyB = bodyB;
    jointDef.localAnchorA = localAnchorA;
    jointDef.localAnchorB = localAnchorB;
    b2Joint* joint = m_world.CreateJoint(&jointDef);
    return joint;
}

