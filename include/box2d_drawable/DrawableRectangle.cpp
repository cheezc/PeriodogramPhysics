
#include "DrawableRectangle.hpp"
#include "DrawableShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_shape.h"
#include "utils.hpp"

#define FRICTION .7f
#define DENSITY 1.f

DrawableRectangle::DrawableRectangle(
    sf::Vector2f &pos,
    sf::Vector2f &size,
    b2PolygonShape &shape,
    b2Body* body
)
: sf::RectangleShape(size), b2PolygonShape(shape), m_body(body)
{
    sf::RectangleShape::setOrigin(sf::Vector2(size.x/2, size.y/2));
}

DrawableRectangle::DrawableRectangle() {}

void DrawableRectangle::Update() {
    if (m_body) {
        sf::Vector2f pos = box2sf(m_body->GetPosition());
        sf::RectangleShape::setPosition(pos);
        sf::RectangleShape::setRotation(sf::radians(m_body->GetAngle()));
    }
}

const sf::Drawable* DrawableRectangle::GetDrawableShape() const {
    return static_cast<const sf::Drawable*>(this);
}

sf::Transformable* DrawableRectangle::GetTransformableShape() {
    return static_cast<sf::Transformable*>(this);
}

b2Shape* DrawableRectangle::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(DrawableRectangle));
    DrawableRectangle* clone = new (mem) DrawableRectangle;
    *clone = *this;
    return clone;
}

