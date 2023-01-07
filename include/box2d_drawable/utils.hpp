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

#define SHOW(a) std::cout << #a << ": " << (a) << std::endl
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

static inline float sfloat2box(float f) {
    return f/SCALING_FACTOR;
}

static float getClosest(float val1, float val2,
               float target)
{
    if (target - val1 >= val2 - target)
        return val2;
    else
        return val1;
}

// Returns element closest to target in arr[]
static float findClosest(std::vector<float>& arr, float target)
{
    int n = arr.size();
    // Corner cases
    //left-side case
    if (target <= arr[0])
        return arr[0];
    //right-side case
    if (target >= arr[n - 1])
        return arr[n - 1];

    // Doing binary search
    int i = 0, j = n, mid = 0;
    while (i < j) {
        mid = (i + j) / 2;

        if (arr[mid] == target)
            return arr[mid];

        /* If target is less than array element,
            then search in left */
        if (target < arr[mid]) {

            // If target is greater than previous
            // to mid, return closest of two
            if (mid > 0 && target > arr[mid - 1])
                return getClosest(arr[mid - 1],
                                  arr[mid], target);
            j = mid;
        }

        /* Repeat for left half */
        // If target is greater than mid
        else {
            if (mid < n - 1 && target < arr[mid + 1])
                return getClosest(arr[mid],
                                  arr[mid + 1], target);
            // update i
            i = mid + 1;
        }
    }

    // Only single element left after search
    return arr[mid];
}
