#ifndef SOUND_RECTANGLE_H
#define SOUND_RECTANGLE_H
#include "KinematicRectangleArray.hpp"
#include "SFML/Audio/SoundRecorder.hpp"
#include "DrawableWorld.hpp"

class DrawableKinematicRectangleArrayRecorder: public sf::SoundRecorder, public DrawableKinematicRectangleArray {
    public:
        DrawableKinematicRectangleArrayRecorder(int numRectangles, sf::Vector2f& arrayDimensions, sf::Vector2f& arrayOrigin, DrawableWorld *world);
    protected:
        bool onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) override;
    private:
        sf::Vector2f m_arrayDimensions;
        DrawableWorld* m_drawableWorld;
};
#endif