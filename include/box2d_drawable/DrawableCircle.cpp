#include "DrawableCircle.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_polygon_shape.h"
#include "utils.hpp"

DrawableCircle:: DrawableCircle(
    sf::Vector2f &pos,
    float radius,
    b2CircleShape &shape,
    b2Body *body
)
: sf::CircleShape(radius), b2CircleShape(shape), m_body(body), m_radius(radius)
{
    sf::Vector2f origin = sf::Vector2(radius, radius);
    sf::CircleShape::setOrigin(origin);
}

DrawableCircle::DrawableCircle() {}

void DrawableCircle::Update() {
    if (m_body) {
        sf::Vector2f pos = box2sf(m_body->GetPosition());
        auto rotation = sf::radians(m_body->GetAngle());
        sf::CircleShape::setPosition(pos);
        sf::CircleShape::setRotation(rotation);
    }
}

const sf::Drawable* DrawableCircle::GetDrawableShape() const {
    return static_cast<const sf::Drawable*>(this);
}

sf::Transformable* DrawableCircle::GetTransformableShape() {
    return static_cast<sf::Transformable*>(this);
}

b2Shape* DrawableCircle::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(DrawableCircle));
    DrawableCircle* clone = new (mem) DrawableCircle;
    *clone = *this;
    return clone;
}

sf::Vector2f DrawableCircle::GetPosition() {
    return box2sf(m_body->GetPosition()) - getOrigin();
}

const sf::Drawable* DrawableCircle::GetDrawableText() const {
    return nullptr;
}

sf::Transformable* DrawableCircle::GetTransformableText() {
    return nullptr;
}