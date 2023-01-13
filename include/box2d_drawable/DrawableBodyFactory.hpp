#ifndef DRAWABLE_BODY_FACTORY_H
#define DRAWABLE_BODY_FACTORY_H
#include "DrawableWorld.hpp"

class DrawableBodyFactory {
public:
    DrawableBodyFactory(DrawableWorld& world);
    DrawableRectangle* createDynamicBox(sf::Vector2f& pos, sf::Vector2f& size);
    DrawableRectangle* createStaticBox(sf::Vector2f& pos, sf::Vector2f& size);
    KinematicRectangle* createKinematicBox(sf::Vector2f& pos,
        sf::Vector2f& size,
        sf::Vector2f& maxPos,
        sf::Vector2f& minPos
    );
private:
    DrawableWorld& m_world;
};

#endif