#ifndef IDRAWABLE_SHAPE_H
#define IDRAWABLE_SHAPE_H
#include "SFML/Graphics/Transformable.hpp"
#include "box2d/b2_fixture.h"
#include "box2d/b2_shape.h"
#include "SFML/Graphics/Shape.hpp"

class IDrawableTransformableShape {
public:
    virtual ~IDrawableTransformableShape() {}
    virtual const sf::Drawable* GetDrawableShape() const = 0;
    virtual sf::Transformable* GetTransformableShape() = 0;
    virtual void Update(b2Body *body) = 0;
};
#endif