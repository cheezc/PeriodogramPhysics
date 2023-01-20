#ifndef DRAWABLE_CIRCLE_H
#define DRAWABLE_CIRCLE_H
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include"SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "DrawableShape.hpp"
#include "box2d/b2_shape.h"

class DrawableCircle: public sf::CircleShape, public IDrawableTransformableShape, public b2CircleShape {
public:
    DrawableCircle(
        float radius,
        b2CircleShape &shape,
        b2Body *body
    );

    DrawableCircle();

    // Override IDrawableShape
    const sf::Drawable* GetDrawableShape() const override;

    // Override ITransformableShape
    sf::Transformable* GetTransformableShape() override;

    // Override Itransformable Shape
    const sf::Drawable* GetDrawableText() const override;
    sf::Transformable* GetTransformableText() override;

    // Override IDrawableShape; updates position based off of body
    void Update() override;

    // Override b2PolygonShape, needed as allocation of a shape to a body
    // creates a shallow copy;
    b2Shape* Clone(b2BlockAllocator* allocator) const override;

    sf::Vector2f GetPosition() override;

    // Override IDrawableTransformableShape
    b2Body* GetBody() override;


private:
    b2Body *m_body;
    sf::Text m_text;
    sf::Font m_font;
    sf::Vector2f m_size;
    float m_radius;
};


#endif