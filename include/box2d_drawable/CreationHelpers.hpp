#ifndef CREATION_HELPERS_H
#define CREATION_HELPERS_H
#include "KinematicRectangle.hpp"
#include "DrawableCircle.hpp"
#include "box2d/b2_circle_shape.h"
#include "utils.hpp"

static inline b2BodyDef CreateBodyDef(sf::Vector2f& pos, b2BodyType bodyType) {
    b2BodyDef def;
    def.position = sf2box(pos);
    def.type = bodyType;
    def.enabled = true;
    return def;
}

static inline DrawableRectangle CreateDrawableRectangle(sf::Vector2f &size, b2Body* body) {
    b2PolygonShape b2shape;
    DrawableRectangle drawableShape(size, b2shape, body);
    b2Vec2 b2Size = sf2box(size);
    drawableShape.SetAsBox(b2Size.x/2, b2Size.y/2);
    return drawableShape;
}

static inline DrawableCircle CreateDrawableCircle(float sfRadius, b2Body* body) {
    b2CircleShape b2shape;
    DrawableCircle drawableShape(sfRadius, b2shape, body);
    return drawableShape;
}

// Position is the center of the rectangle
static inline KinematicRectangle CreateKinematicRectangle(sf::Vector2f& pos, sf::Vector2f &size, b2Body* body, sf::Vector2f& maxPos, sf::Vector2f& minPos) {
    b2PolygonShape b2shape;
    b2Vec2 b2MaxPos = sf2box(maxPos);
    b2Vec2 b2MinPos = sf2box(minPos);
    KinematicRectangle drawableShape(pos, size, b2shape, body, b2MaxPos, b2MinPos);
    b2Vec2 b2Size = sf2box(size);
    drawableShape.SetAsBox(b2Size.x/2, b2Size.y/2);
    return drawableShape;
}

static inline void createShape(b2Body* body, float friction, float density, b2Shape& shape) {
    b2FixtureDef fixtureDef;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
}
#endif