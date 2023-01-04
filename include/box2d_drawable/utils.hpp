#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_common.h"
#include "box2d/b2_math.h"
#include "box2d/b2_polygon_shape.h"
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>
#include <iostream>

#define SCALING_FACTOR 30

static inline sf::Vector2f box2sf(const b2Vec2& vec) {
    return sf::Vector2(vec.x*SCALING_FACTOR, vec.y*SCALING_FACTOR);
}

static inline b2Vec2 sf2box(const sf::Vector2f& vec) {
    return b2Vec2(vec.x/SCALING_FACTOR, vec.y/SCALING_FACTOR);
}

static void logError(const char* function, const char* context, b2Shape::Type type) {
    std::cout << "Function: " << function << " " << "Context: " << context
    << " " << "Shape Type: " << type << std::endl;
}
