#include "DrawableWorld.hpp"
#include "CreationHelpers.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "KinematicRectangleArray.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include <iostream>

#include <chrono>
#define ANTI_ALIASING_LEVEL 10

DrawableWorld::DrawableWorld(sf::VideoMode& mode, sf::String& title, const b2Vec2& gravity)
    :sf::RenderWindow(mode, title, sf::Style::Default, sf::ContextSettings(0, 0, ANTI_ALIASING_LEVEL)),
    b2World(gravity) {

}

void DrawableWorld::createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size) {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    b2BodyDef def = CreateBodyDef(pos, b2_dynamicBody);
    b2Body* body = b2World::CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size, body);
    createShape(body, .7f, 1.0f, drawableRectangle);
}

void DrawableWorld::createStaticBox(sf::Vector2f& pos, sf::Vector2f& size) {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    b2BodyDef def = CreateBodyDef(pos, b2_staticBody);
    b2Body* body = b2World::CreateBody(&def);

    DrawableRectangle drawableRectangle = CreateDrawableRectangle(pos, size, body);
    createShape(body, .7f, 1.0f, drawableRectangle);
}

// Ugly; for debug only
KinematicRectangle* DrawableWorld::createKinematicBox(
    sf::Vector2f& pos,
    sf::Vector2f& size,
    sf::Vector2f& maxPos,
    sf::Vector2f& minPos
) {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    b2BodyDef def = CreateBodyDef(pos, b2_kinematicBody);
    b2Body* body = b2World::CreateBody(&def);

    KinematicRectangle kinematicRectangle = CreateKinematicRectangle(pos, size, body, maxPos, minPos);
    createShape(body, .7f, 1.0f, kinematicRectangle);
    return dynamic_cast<KinematicRectangle*>(body->GetFixtureList()->GetShape());
}

const sf::Drawable* DrawableWorld::getDrawableShape(b2Shape *shape) {
    if (shape) {
        IDrawableTransformableShape* drawableShape = dynamic_cast<IDrawableTransformableShape*>(shape);
        if (drawableShape) {
            drawableShape->Update();
            return drawableShape->GetDrawableShape();
        }
        logError(__func__, "shape not drawable", shape->GetType());
    } else {
        logError(__func__, "nil shape", b2Shape::e_typeCount);
    }

    return nullptr;
}

sf::Transformable* DrawableWorld::getTransformableShape(b2Shape *shape) {
    if (shape) {
        IDrawableTransformableShape* drawableShape = dynamic_cast<IDrawableTransformableShape*>(shape);
        if (drawableShape) {
            drawableShape->Update();
            return drawableShape->GetTransformableShape();
        }
        logError(__func__, "shape not transformable", shape->GetType());
    } else {
        logError(__func__, "nil shape", b2Shape::e_typeCount);
    }
    return nullptr;
}

void DrawableWorld::draw(const sf::Color &bgColor) {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    clear(bgColor);
    for (b2Body* bodyIterator = GetBodyList(); bodyIterator != nullptr; bodyIterator = bodyIterator->GetNext()) {
        for (b2Fixture* fixtureIterator = bodyIterator->GetFixtureList(); fixtureIterator != nullptr; fixtureIterator = fixtureIterator->GetNext()) {
            if (const sf::Drawable* drawableSfShape = getDrawableShape(fixtureIterator->GetShape())) {
                sf::RenderWindow::draw(*drawableSfShape);
            }
        }
    }
    // std::cout << "Draw Elapsed(us)=" << duration_cast<microseconds>(end - start).count()  << std::endl;
}

void DrawableWorld::Step(float timeStep,
				int32 velocityIterations,
				int32 positionIterations) {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    b2World::Step(timeStep, velocityIterations, positionIterations);
    // std::cout << "Step Elapsed(ms)=" << duration_cast<milliseconds>(end - start).count()  << std::endl;
}

// Should we resize pos/scale if we resize?
void DrawableWorld::resize() {
    std::scoped_lock<std::mutex> lock(m_drawMutex);
    sf::Vector2f resizeParams = this->mapPixelToCoords(sf::Vector2i{this->getSize() / 2u});
    for (b2Body* bodyIterator = GetBodyList(); bodyIterator != nullptr; bodyIterator = bodyIterator->GetNext()) {
        for (b2Fixture* fixtureIterator = bodyIterator->GetFixtureList(); fixtureIterator != nullptr; fixtureIterator = fixtureIterator->GetNext()) {
            if (sf::Transformable* transformableShape = getTransformableShape(fixtureIterator->GetShape())) {
                transformableShape->setPosition(resizeParams);
            }
        }
    }
}