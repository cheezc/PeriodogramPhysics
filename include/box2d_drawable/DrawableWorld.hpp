#ifndef DRAWABLE_WORLD_H
#define DRAWABLE_WORLD_H
#include "DrawableShape.hpp"
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
#include "utils.hpp"

class DrawableWorld: public b2World, public sf::RenderWindow {
public:
    DrawableWorld(sf::VideoMode& mode, sf::String& title, const b2Vec2& gravity);

    void draw(const sf::Color &bgColor);
    void createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size);
    void createStaticBox(sf::Vector2f& pos, sf::Vector2f& size);
    void resize();

private:
    void createShape(b2Body* body, float friction, float density, b2Shape& shape);
    const sf::Drawable* getDrawableShape(b2Shape *shape, b2Body* body) const;
    sf::Transformable* getTransformableShape(b2Shape *shape, b2Body* body);
};
#endif