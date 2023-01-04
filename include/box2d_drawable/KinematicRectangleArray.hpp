#ifndef KINEMATIC_ARRAY_H
#define KINEMATIC_ARRAY_H
#include "KinematicRectangle.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_math.h"
#include "box2d/b2_world.h"

// Constant that determines how fast the arrays rise
#define ARRAY_TARGET_SPEED 10

// Constant that determines how fast the arrays fall
#define ARRAY_RETURN_SPEED 3

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
    DrawableKinematicRectangleArray(int numRectangles, sf::Vector2f& arrayDimensions, sf::Vector2f& arrayOrigin, b2World *world);

    // Set the array positions
    void SetArrayPositions(const std::vector<float>& positions);
private:
    float m_arrayWidth;
    float m_arrayHeight;
    float m_rectangleWidth;
    float m_rectangleHeight;
    float m_arrayOriginY;
    int m_numRectangles;

    b2World* m_world;

    // Allocated by the b2world; destructor called when b2World is destroyed
    std::vector<BodyData> m_kinematicRectangles;
};

// Resampler - resample the elements of a vector to change the given size
std::vector<float> ResampleFloat(const std::vector<float>& input, int targetSize);

#endif