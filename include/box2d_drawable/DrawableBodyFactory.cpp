#include "DrawableBodyFactory.hpp"
#include "CreationHelpers.hpp"
#include "box2d/b2_body.h"


DrawableBodyFactory::DrawableBodyFactory(DrawableWorld& world): m_world(world) {

}

DrawableRectangle* DrawableBodyFactory::createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_dynamicBody);
    b2Body* body = m_world.CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size, body);
    createShape(body, .7f, 1.0f, drawableRectangle);
    return dynamic_cast<DrawableRectangle*>(body->GetFixtureList()->GetShape());
}

DrawableRectangle* DrawableBodyFactory::createStaticBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_staticBody);
    b2Body* body = m_world.CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size, body);
    createShape(body, .7f, 1.0f, drawableRectangle);
    return dynamic_cast<DrawableRectangle*>(body->GetFixtureList()->GetShape());
}

KinematicRectangle* DrawableBodyFactory::createKinematicBox(
    sf::Vector2f& pos,
    sf::Vector2f& size,
    sf::Vector2f& maxPos,
    sf::Vector2f& minPos
) {
    b2BodyDef def = CreateBodyDef(pos, b2_kinematicBody);
    b2Body* body = m_world.CreateBody(&def);

    KinematicRectangle kinematicRectangle = CreateKinematicRectangle(pos, size, body, maxPos, minPos);
    createShape(body, .7f, 1.0f, kinematicRectangle);
    return dynamic_cast<KinematicRectangle*>(body->GetFixtureList()->GetShape());
}

