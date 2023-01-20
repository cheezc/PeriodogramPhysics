#ifndef KINEMATIC_ARRAY_H
#define KINEMATIC_ARRAY_H
#include "DrawableBodyFactory.hpp"
#include "KinematicRectangle.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"

// Constant that determines how fast the arrays rise
#define ARRAY_TARGET_SPEED 10

// Constant that determines how fast the arrays fall
#define ARRAY_RETURN_SPEED 3

#define MAX_COLOR_MASK 0xFFFFFF

// Colors
class ColorIterator {
public:
    typedef enum {
        RED=0,
        GREEN=1,
        BLUE=2
    } IteratorColor;

    ColorIterator(int numColors):
        m_numColors(numColors),
        m_colors(0),
        m_colorMask(0xFF00FF)
    {
        m_colorStep = 1;
        m_stepRatio = 10;
        m_currentStep = 0;
        m_colorStepSign = 1;
    }

    sf::Color getNextColor() {
        sf::Color color = sf::Color(getColor(RED), getColor(GREEN), getColor(BLUE));
        updateColors();
        return color;
    }

private:
    uint8_t getColor(IteratorColor color) {
        return ((uint8_t*)(&m_colors))[color];
    }


   void updateColors() {
        if (++m_currentStep % m_stepRatio == m_colorStep) {
            if (m_colorMask + m_colorStep >= MAX_COLOR_MASK) {
                m_colorStepSign *= -1;
            }
            m_colorMask = (m_colorMask + m_colorStep*m_colorStepSign)%MAX_COLOR_MASK;;
            m_colors = m_colorMask;
            m_currentStep = m_currentStep % m_stepRatio;
        }

    }
    int m_numColors;
    int32_t m_colors;
    int32_t m_colorMask;
    int32_t m_colorStep;
    const size_t colorBits = 24;
    int32_t m_stepRatio;
    int32_t m_currentStep;
    int32_t m_colorStepSign;
};

// Class encapsulates an array of kinematic bodies; We can set the individual velocities of each of the
// bodies; Will need some sort of kinematic array class to bounds check the rectangles, unless there is
// already a way to do that
class DrawableKinematicRectangleArray {
public:
    typedef struct {
        KinematicRectangle *box;
    } BodyData;

    // Take in dimensions of rectangle array, and dimensions of a single rectangle
    // Generate the array with a b2 world object; create corresponding shapes;
    // bind shapes to body; store the shapes in an array
    /**
     * DrawableKinematicRectangleArray
     *
     * @param  {sf::Vector2f} arraySize     :
     * @param  {sf::Vector2f} rectangleSize :
     * @param  {b2World*} world             :
     */
    DrawableKinematicRectangleArray(
        int numRectangles,
        sf::Vector2f& arrayDimensions,
        sf::Vector2f& arrayOrigin,
        DrawableBodyFactory *factory);

    // Set the array positions
    void SetArrayPositions(const std::vector<float>& positions);
protected:
    std::vector<BodyData>& GetRectangles();
    sf::Vector2f GetRectangleDimensions();

private:
    float m_arrayWidth;
    float m_arrayHeight;
    float m_rectangleWidth;
    float m_rectangleHeight;
    float m_arrayOriginY;
    float m_arrayOriginX;
    int m_numRectangles;
    ColorIterator m_cIterator;

    DrawableBodyFactory* m_factory;

    // Allocated by the b2world; destructor called when b2World is destroyed
    std::vector<BodyData> m_kinematicRectangles;
};

// Resampler - resample the elements of a vector to change the given size
std::vector<float> ResampleFloat(const std::vector<float>& input, int targetSize);

#endif