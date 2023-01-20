#include "StickFigure.hpp"
#include "DrawableRectangle.hpp"
#include "SFML/System/Angle.hpp"
#include "SFML/System/Vector2.hpp"
#include "box2d/b2_joint.h"
#include "box2d/b2_revolute_joint.h"
#include "utils.hpp"

#define LEG_MAX_ANGLE 80.f
#define LEG_MIN_ANGLE 0.f
#define ARM_MAX_ANGLE 80.f
#define ARM_MIN_ANGLE -80.f
#define HEAD_MAX_ANGLE 45.f
#define HEAD_MIN_ANGLE -45.f

static sf::Vector2f getCentroid(sf::Vector2f origin, sf::Vector2f size) {
    return origin - size/2.f;
}

StickFigure::StickFigure(
    sf::Vector2f& bodyCentroid,
    float scalingFactor,
    DrawableBodyFactory* factory
) : m_factory(factory) {
    // Start with the torso - centroid is the top left of the torso
    sf::Vector2f bodySize(DEFAULT_STICK_WIDTH*scalingFactor, DEFAULT_TORSO_LENGTH*scalingFactor);
    m_torso = m_factory->createDynamicBox(
        bodyCentroid,
        bodySize);

    // Arms start off in a T Pose
    sf::Vector2f armSize(DEFAULT_ARM_LENGTH*scalingFactor, DEFAULT_STICK_WIDTH*scalingFactor);

    // Left Arm
    sf::Vector2f leftArmOffset(-armSize.x/2, -bodySize.y/4);
    sf::Vector2f leftArmPos = bodyCentroid + leftArmOffset;
    m_leftArm = m_factory->createDynamicBox(
        leftArmPos,
        armSize
    );

    // Right Arm
    sf::Vector2f rightArmOffset(armSize.x/2, -bodySize.y/4);
    sf::Vector2f rightArmPos = bodyCentroid + rightArmOffset;
    m_rightArm = m_factory->createDynamicBox(
        rightArmPos,
        armSize
    );

    // Head
    float headRadius = DEFAULT_HEAD_RADIUS*scalingFactor;
    sf::Vector2f headOffset(0, -headRadius - bodySize.y/2);
    sf::Vector2f headPos = bodyCentroid + headOffset;
    m_head = m_factory->createDynamicCircle(headPos, headRadius);

    // Then the legs, which start off as a t pose
    sf::Vector2f legSize(DEFAULT_LEG_LENGTH*scalingFactor, DEFAULT_STICK_WIDTH*scalingFactor);

    // Left Leg
    sf::Vector2f leftLegOffset(-legSize.x/2, bodySize.y/2);
    sf::Vector2f leftLegPos = bodyCentroid + leftLegOffset;
    m_leftLeg = m_factory->createDynamicBox(
        leftLegPos,
        legSize
    );

    // Right Leg
    sf::Vector2f rightLegOffset(legSize.x/2, bodySize.y/2);
    sf::Vector2f rightLegPos = bodyCentroid + rightLegOffset;
    m_rightLeg = m_factory->createDynamicBox(
        rightLegPos,
        legSize
    );

    // bind head to body
    b2Vec2 bodyHeadAnchor = sf2box(sf::Vector2f(0, -bodySize.y/2));
    b2Vec2 headBodyAnchor = sf2box(sf::Vector2f(0, headRadius));
    b2Joint* headBodyJoint = m_factory->createConnectedJoint(
        e_revoluteJoint,
        m_torso->GetBody(),
        m_head->GetBody(),
        bodyHeadAnchor,
        headBodyAnchor,
        sf::degrees(HEAD_MAX_ANGLE).asRadians(),
        sf::degrees(HEAD_MIN_ANGLE).asRadians()
    );

    // bind the left arm to body
    b2Vec2 bodyLeftArmAnchor(sf2box(sf::Vector2f(0, leftArmOffset.y)));
    b2Vec2 leftArmBodyAnchor(sf2box(sf::Vector2f(armSize.x/2, 0)));
    b2Joint* leftArmBodyJoint = m_factory->createConnectedJoint(
        e_revoluteJoint,
        m_torso->GetBody(),
        m_leftArm->GetBody(),
        bodyLeftArmAnchor,
        leftArmBodyAnchor,
        sf::degrees(ARM_MAX_ANGLE).asRadians(),
        sf::degrees(ARM_MIN_ANGLE).asRadians()
    );

    // bind the right arm to body
    b2Vec2 bodyRightArmAnchor(sf2box(sf::Vector2f(0, rightArmOffset.y)));
    b2Vec2 rightArmBodyAnchor(sf2box(sf::Vector2f(-armSize.x/2, 0)));
    b2Joint* rightArmBodyJoint = m_factory->createConnectedJoint(
        e_revoluteJoint,
        m_torso->GetBody(),
        m_rightArm->GetBody(),
        bodyRightArmAnchor,
        rightArmBodyAnchor,
        sf::degrees(ARM_MIN_ANGLE).asRadians(),
        sf::degrees(ARM_MAX_ANGLE).asRadians()
    );

    // bind the left leg to body
    b2Vec2 bodyLeftLegAnchor(sf2box(sf::Vector2f(0, leftLegOffset.y)));
    b2Vec2 leftLegBodyAnchor(sf2box(sf::Vector2f(-legSize.x/2, 0)));
    b2Joint* leftLegBodyJoint = m_factory->createConnectedJoint(
        e_revoluteJoint,
        m_torso->GetBody(),
        m_leftLeg->GetBody(),
        bodyLeftLegAnchor,
        leftLegBodyAnchor,
        sf::degrees(LEG_MAX_ANGLE).asRadians(),
        sf::degrees(LEG_MIN_ANGLE).asRadians()
    );

    // bind the right leg to body
    b2Vec2 bodyRightLegAnchor(sf2box(sf::Vector2f(0, rightLegOffset.y)));
    b2Vec2 rightLegBodyAnchor(sf2box(sf::Vector2f(legSize.x/2, 0)));
    b2Joint* rightLegBodyJoint = m_factory->createConnectedJoint(
        e_revoluteJoint,
        m_torso->GetBody(),
        m_rightLeg->GetBody(),
        bodyRightLegAnchor,
        rightLegBodyAnchor,
        sf::degrees(-1 * LEG_MIN_ANGLE).asRadians(),
        sf::degrees(-1 * LEG_MAX_ANGLE).asRadians()
    );
}