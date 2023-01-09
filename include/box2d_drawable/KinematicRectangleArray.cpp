#include "KinematicRectangleArray.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_math.h"
#include "CreationHelpers.hpp"

#define RECT_SEPARATION 1.f
#define RECT_FRICTION .7f
#define RECT_DENSITY 10.f
#define ORIGIN_OFFSET_FACTOR 1.4f

// The rate at which the platform goes to target
#define TARGET_SPEED 5

// The rate at which the platform returns
#define RETURN_TO_ORIGIN_SPEED 3

#define NUM_COLORS 10
#define COLOR_BITS 24

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
        m_colorMask(0)
    {
        // 16bits x 3
        m_colorStep = ((1 << 24) - 1)/numColors;
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
        m_colorMask = (m_colorMask + m_colorStep)%maxMask;
        m_colors = m_colorMask;
        // std::cout << "mask: " << m_colorMask << std::endl;
    }
    int m_numColors;
    uint32_t m_colors;
    uint32_t m_colorMask;
    uint32_t m_colorStep;
    const size_t colorBits = 24;
    const uint32_t maxMask = 1 << colorBits;

};


std::vector<float> ResampleFloat(const std::vector<float>& input, int targetSize) {
    std::vector<float> ret(targetSize);
    if (targetSize == 0) {
        std::cout << "ResampleTargetSize 0\n";
        return ret;
    }
    float inputIndex = 0;
    float scalingFactor = static_cast<float>(input.size())/static_cast<float>(targetSize);
    for (int i = 0; i < targetSize; i++) {
        int inputTarget = static_cast<int>(scalingFactor*i);
        if (inputTarget > input.size() - 1) inputTarget = input.size() - 1;

        // Get the mean of samples from input index begin to input target
        float inputAccum = 0;
        float accumSize = inputTarget - inputIndex;
        if (accumSize) {
            while (inputIndex <= inputTarget) {
                inputAccum += input[inputIndex];
                inputIndex++;
            }
            ret[i] = inputAccum/accumSize;
        } else {
            ret[i] = input[inputTarget];
        }
    }

    return ret;
}

DrawableKinematicRectangleArray::DrawableKinematicRectangleArray(
    int numRectangles,
    sf::Vector2f& arrayDimensions,
    sf::Vector2f& arrayOrigin,
    b2World *world): m_world(world) {

    if (numRectangles <= 0) {
        throw std::invalid_argument("numRectangles must be positive in DrawableKinematicRectangleArray constructor");
    }

    if (arrayDimensions.x <= 0 || arrayDimensions.y <= 0) {
        throw std::invalid_argument("array dimensions must be non-zero");
    }

    ColorIterator cIterator(numRectangles);

    m_numRectangles = numRectangles;
    m_arrayWidth = arrayDimensions.x;
    m_arrayHeight = arrayDimensions.y;
    m_rectangleWidth = (m_arrayWidth - (numRectangles-1)*RECT_SEPARATION)/numRectangles;
    m_rectangleHeight = m_arrayHeight;
    m_arrayOriginY = arrayOrigin.y + ORIGIN_OFFSET_FACTOR*m_rectangleHeight;

    // Iterator params
    float currXPos = arrayOrigin.x;
    float currYPos = arrayOrigin.y;
    uint8_t redStart = 0;
    uint8_t greenStart = 0;
    uint8_t blueStart = 0;

    // Create array and set objects
    for (int i = 0; i < numRectangles; i++) {
        sf::Vector2f originPos(currXPos, currYPos);
        sf::Vector2f rectSize(m_rectangleWidth, m_rectangleHeight);
        b2BodyDef def = CreateBodyDef(originPos, b2_kinematicBody);
        b2Body* body = m_world->CreateBody(&def);
        if (body) {
            sf::Vector2f maxPos(originPos.x, m_arrayOriginY);
            sf::Vector2f minPos(originPos.x, 0);
            KinematicRectangle kinematicRectangle = CreateKinematicRectangle(maxPos, rectSize, body, maxPos, minPos);
            createShape(body, RECT_FRICTION, RECT_DENSITY, kinematicRectangle);
            auto allocatedBox = dynamic_cast<KinematicRectangle*>(body->GetFixtureList()->GetShape());
            auto color = cIterator.getNextColor();
            allocatedBox->setFillColor(color);
            allocatedBox->setOutlineColor(color);
            if (allocatedBox) {
                BodyData bData = BodyData{.box = allocatedBox};
                m_kinematicRectangles.push_back(bData);
            }
        }

        // Advance position pointers
        currXPos += m_rectangleWidth + RECT_SEPARATION;
    }
}

void TargetCb(KinematicRectangle* box) {
    b2Vec2 targetPos = box->GetBodyInitialPosition();
    box->SetTargetPosition(targetPos, RETURN_TO_ORIGIN_SPEED, nullptr);
}

sf::Vector2f DrawableKinematicRectangleArray::GetRectangleDimensions() {
    return sf::Vector2f(m_rectangleWidth, m_rectangleHeight);
}

void DrawableKinematicRectangleArray::SetArrayPositions(const std::vector<float>& positions) {
    // std::vector<float> resampledPositions = ResampleFloat(positions, m_kinematicRectangles.size());
    for (int i = 0; i < m_kinematicRectangles.size(); i++) {
        auto bData = m_kinematicRectangles[i];
        if (bData.box && i < positions.size()) {
            sf::Vector2f sfPos(0, m_arrayOriginY-positions[i]);
            b2Vec2 boxPos = sf2box(sfPos);
            b2Vec2 targetPos(bData.box->GetBodyPosition().x, boxPos.y);
            bData.box->SetTargetPosition(targetPos, TARGET_SPEED, TargetCb);
        }
    }
}

std::vector<DrawableKinematicRectangleArray::BodyData>& DrawableKinematicRectangleArray::GetRectangles() {
    return m_kinematicRectangles;
}

