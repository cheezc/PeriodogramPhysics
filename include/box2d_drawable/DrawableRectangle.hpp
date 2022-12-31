#ifndef DRAWABLE_RECTANGLE_H
#define DRAWABLE_RECTANGLE_H
#include "DrawableWorld.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Transformable.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "DrawableShape.hpp"
#include "box2d/b2_shape.h"


class DrawableRectangle: public sf::RectangleShape, public IDrawableTransformableShape, public b2PolygonShape {
public:
    DrawableRectangle(
        sf::Vector2f &pos,
        sf::Vector2f &size,
        b2PolygonShape &shape
    );

    DrawableRectangle();

    // Override IDrawableShape
    const sf::Drawable* GetDrawableShape() const override;

    // Override ITransformableShape
    sf::Transformable* GetTransformableShape() override;

    // Override IDrawableShape
    void Update(b2Body *body) override;

    // Override b2PolygonShape TODO: This is kind of hacky and easy to miss
    b2Shape* Clone(b2BlockAllocator* allocator) const override;

};
#endif
