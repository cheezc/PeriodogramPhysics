#include "DrawableWorld.hpp"
#include "DrawableShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include "DrawableRectangle.hpp"
#include <iostream>

static b2BodyDef CreateBodyDef(sf::Vector2f& pos, b2BodyType bodyType) {
    b2BodyDef def;
    def.position = sf2box(pos);
    def.type = bodyType;
    return def;
}

static DrawableRectangle CreateDrawableRectangle(sf::Vector2f& pos, sf::Vector2f &size) {
    b2PolygonShape b2shape;
    DrawableRectangle drawableShape(pos, size, b2shape);
    b2Vec2 b2Size = sf2box(size);
    drawableShape.SetAsBox(b2Size.x/2, b2Size.y/2);
    return drawableShape;
}

static void logError(const char* function, const char* context, b2Shape::Type type) {
    std::cout << "Function: " << function << " " << "Context: " << context
    << " " << "Shape Type: " << type << std::endl;
}

DrawableWorld::DrawableWorld(sf::VideoMode& mode, sf::String& title, const b2Vec2& gravity)
:sf::RenderWindow(mode, title), b2World(gravity) {

}

void DrawableWorld::createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_dynamicBody);
    b2Body* body = b2World::CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size);
    createShape(body, .7f, 1.0f, drawableRectangle);
}

void DrawableWorld::createStaticBox(sf::Vector2f& pos, sf::Vector2f& size) {
    b2BodyDef def = CreateBodyDef(pos, b2_staticBody);
    b2Body* body = b2World::CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size);
    createShape(body, .7f, 1.0f, drawableRectangle);
}

void DrawableWorld::createShape(b2Body* body, float friction, float density, b2Shape& shape) {
    b2FixtureDef fixtureDef;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.shape = &shape;
    body->CreateFixture(&fixtureDef);
}

const sf::Drawable* DrawableWorld::getDrawableShape(b2Shape *shape, b2Body* body) const {
    if (shape) {
        IDrawableTransformableShape* drawableShape = dynamic_cast<IDrawableTransformableShape*>(shape);
        if (drawableShape) {
            drawableShape->Update(body);
            return drawableShape->GetDrawableShape();
        }
        logError(__func__, "shape not drawable", shape->GetType());
    } else {
        logError(__func__, "nil shape", b2Shape::e_typeCount);
    }

    return nullptr;
}

sf::Transformable* DrawableWorld::getTransformableShape(b2Shape *shape, b2Body* body) {
    if (shape) {
        IDrawableTransformableShape* drawableShape = dynamic_cast<IDrawableTransformableShape*>(shape);
        if (drawableShape) {
            drawableShape->Update(body);
            return drawableShape->GetTransformableShape();
        }
        logError(__func__, "shape not transformable", shape->GetType());
    } else {
        logError(__func__, "nil shape", b2Shape::e_typeCount);
    }
    return nullptr;
}

void DrawableWorld::draw(const sf::Color &bgColor) {
    clear(bgColor);
    for (b2Body* bodyIterator = GetBodyList(); bodyIterator != nullptr; bodyIterator = bodyIterator->GetNext()) {
        for (b2Fixture* fixtureIterator = bodyIterator->GetFixtureList(); fixtureIterator != nullptr; fixtureIterator = fixtureIterator->GetNext()) {
            if (const sf::Drawable* drawableSfShape = getDrawableShape(fixtureIterator->GetShape(), bodyIterator)) {
                sf::RenderWindow::draw(*drawableSfShape);
            }
        }
    }
}

void DrawableWorld::resize() {
    sf::Vector2f resizeParams = this->mapPixelToCoords(sf::Vector2i{this->getSize() / 2u});
    for (b2Body* bodyIterator = GetBodyList(); bodyIterator != nullptr; bodyIterator = bodyIterator->GetNext()) {
        for (b2Fixture* fixtureIterator = bodyIterator->GetFixtureList(); fixtureIterator != nullptr; fixtureIterator = fixtureIterator->GetNext()) {
            if (sf::Transformable* transformableShape = getTransformableShape(fixtureIterator->GetShape(), bodyIterator)) {
                transformableShape->setPosition(resizeParams);
            }
        }
    }
}