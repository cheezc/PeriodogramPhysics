
#include "DrawableRectangle.hpp"
#include "DrawableShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "SFML/System/Vector2.hpp"
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
: sf::RectangleShape(size), b2PolygonShape(shape), m_body(body), m_size(size)
{
    sf::Vector2f origin = sf::Vector2(size.x/2, size.y/2);
    sf::RectangleShape::setOrigin(origin);
    m_text.setOrigin(origin);
    if(!m_font.loadFromFile("../fonts/quicksand/Quicksand-Bold.otf")) {
        std::cout << "Invalid font file" << std::endl;
        throw std::exception();
    }
}

DrawableRectangle::DrawableRectangle() {}

void DrawableRectangle::Update() {
    if (m_body) {
        sf::Vector2f pos = box2sf(m_body->GetPosition());
        auto rotation = sf::radians(m_body->GetAngle());
        sf::RectangleShape::setPosition(pos);
        sf::RectangleShape::setRotation(rotation);
        m_text.setPosition(pos);
        m_text.setRotation(rotation);
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

const sf::Drawable* DrawableRectangle::GetDrawableText() const {
    return static_cast<const sf::Drawable*>(&m_text);
}

sf::Transformable* DrawableRectangle::GetTransformableText() {
    return static_cast<sf::Transformable*>(&m_text);
}

bool DrawableRectangle::SetText(
    const std::string &s,
    const int characterSize,
    sf::Color color,
    sf::Vector2f pos)
{
    if (m_body) {
        sf::Vector2f origin = sf::Vector2(m_size.x/2, m_size.y/2);
        m_text.setFont(m_font);
        m_text.setCharacterSize(characterSize);
        m_text.setFillColor(color);
        m_text.setString(s);
        m_text.setOrigin(origin);
        if (pos.x < 0 || pos.y < 0) {
            pos = box2sf(m_body->GetPosition());
        }
        m_text.setPosition(pos);
        m_text.setRotation(sf::radians(m_body->GetAngle()));
    }
    return true;
}

// Return the top left corner of the box in sf::pixel coordinates
sf::Vector2f DrawableRectangle::GetPosition() {
    return box2sf(m_body->GetPosition()) - getOrigin();
}



