#ifndef DRAWABLE_WORLD_H
#define DRAWABLE_WORLD_H
#include "DrawableShape.hpp"
#include "KinematicRectangle.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_common.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include "box2d/b2_shape.h"

class DrawableWorld: public b2World, public sf::RenderWindow {
public:
    DrawableWorld(sf::VideoMode& mode, sf::String& title, const b2Vec2& gravity);

    void draw(const sf::Color &bgColor);
    DrawableRectangle* createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size);
    DrawableRectangle* createStaticBox(sf::Vector2f& pos, sf::Vector2f& size);
    KinematicRectangle* createKinematicBox(sf::Vector2f& pos,
                            sf::Vector2f& size,
                            sf::Vector2f& maxPos,
                            sf::Vector2f& minPos) ;
    void resize();
    std::mutex m_drawMutex;
    void Step(float timeStep,
				int32 velocityIterations,
				int32 positionIterations);
    const sf::Drawable* getDrawableText(b2Shape *shape);
    void UpdateDrawableShape(b2Shape* shape);

private:
    const sf::Drawable* getDrawableShape(b2Shape *shape);
    sf::Transformable* getTransformableShape(b2Shape *shape);
};
#endif