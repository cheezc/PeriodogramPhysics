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
    virtual const sf::Drawable* GetDrawableText() const = 0;
    virtual sf::Transformable* GetTransformableText() = 0;
    virtual b2Body* GetBody() = 0;
    virtual sf::Vector2f GetPosition() = 0;
    virtual void Update() = 0;
    virtual b2Shape* Clone(b2BlockAllocator* allocator) const = 0;
};
#endif