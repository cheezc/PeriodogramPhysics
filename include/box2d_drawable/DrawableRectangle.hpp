#ifndef DRAWABLE_RECTANGLE_H
#define DRAWABLE_RECTANGLE_H
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include"SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "DrawableShape.hpp"
#include "box2d/b2_shape.h"


class DrawableRectangle: public sf::RectangleShape, public IDrawableTransformableShape, public b2PolygonShape {
public:
    DrawableRectangle(
        sf::Vector2f &size,
        b2PolygonShape &shape,
        b2Body *body
    );

    DrawableRectangle();

    // Override IDrawableShape
    const sf::Drawable* GetDrawableShape() const override;

    // Override ITransformableShape
    sf::Transformable* GetTransformableShape() override;

    // Override Itransformable Shape
    const sf::Drawable* GetDrawableText() const override;
    sf::Transformable* GetTransformableText() override;

    // Override IDrawableShape; updates position based off of body
    void Update() override;

    // Override IDrawableShape
    b2Body* GetBody() override;

    // Override b2PolygonShape, needed as allocation of a shape to a body
    // creates a shallow copy;
    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    // Set text on top of rectangle
    bool SetText(const std::string &s,
                 const int characterSize,
                 sf::Color color=sf::Color::White,
                 sf::Vector2f pos=sf::Vector2f(-1, -1));

    sf::Vector2f GetPosition() override;

private:
    b2Body *m_body;
    sf::Text m_text;
    sf::Font m_font;
    sf::Vector2f m_size;
};
#endif
