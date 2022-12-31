
#include "DrawableRectangle.hpp"
#include "DrawableShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_shape.h"

#define FRICTION .7f
#define DENSITY 1.f

DrawableRectangle::DrawableRectangle(
    sf::Vector2f &pos,
    sf::Vector2f &size,
    b2PolygonShape &shape
)
: sf::RectangleShape(size), b2PolygonShape(shape)
{
    setOrigin(sf::Vector2(size.x/2, size.y/2));
}

DrawableRectangle::DrawableRectangle() {}

void DrawableRectangle::Update(b2Body *body) {
    sf::Vector2f pos = box2sf(body->GetPosition());
    setPosition(pos);
    setRotation(sf::radians(body->GetAngle()));
}

const sf::Drawable* DrawableRectangle::GetDrawableShape() const {
    return this;
}

sf::Transformable* DrawableRectangle::GetTransformableShape() {
    return this;
}

b2Shape* DrawableRectangle::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(DrawableRectangle));
    DrawableRectangle* clone = new (mem) DrawableRectangle;
    *clone = *this;
    return clone;
}

