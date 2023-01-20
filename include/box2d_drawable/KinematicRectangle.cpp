#include "KinematicRectangle.hpp"
#include "DrawableRectangle.hpp"
#include "box2d/b2_block_allocator.h"
#include "box2d/b2_math.h"
#include "box2d/b2_shape.h"
#include <iostream>
#include "utils.hpp"
#include <float.h>

KinematicRectangle::KinematicRectangle(
    sf::Vector2f &pos,
    sf::Vector2f &size,
    b2PolygonShape &shape,
    b2Body *body,
    b2Vec2 &maxPositions,
    b2Vec2 &minPositions
) : DrawableRectangle(size, shape, body) {
    m_body = body;
    m_maxPositions = maxPositions;
    m_minPositions = minPositions;
    m_velocity = b2Vec2_zero;
    m_targetPosition = b2Vec2_zero;
    m_targetReachedCb = nullptr;
    sf::Vector2f originPos = pos;
    m_originalPos = sf2box(originPos);
    body->SetTransform(m_originalPos, body->GetAngle());
}

KinematicRectangle::KinematicRectangle(){};

bool reachedPosition(
    const float &velocity,
    const float &currPosition,
    const float &targetPosition,
    const float &maxPosition,
    const float &minPosition) {
    if (velocity > 0) {
        if (currPosition >= targetPosition || currPosition >= maxPosition) {
            return true;
        }
    } else if (velocity < 0) {
        if (currPosition <= targetPosition || currPosition <= minPosition) {
            return true;
        }
    } else {
        return currPosition == targetPosition;
    }

    return false;
}

bool KinematicRectangle::reachedYPosition() {
    if (m_body) {
        return reachedPosition(m_velocity.y, m_body->GetPosition().y, m_targetPosition.y, m_maxPositions.y, m_minPositions.y);
    }
    return false;
}

bool KinematicRectangle::reachedXPosition() {
    if (m_body) {
        return reachedPosition(m_velocity.x, m_body->GetPosition().x, m_targetPosition.x, m_maxPositions.x, m_minPositions.x);
    }
    return false;
}

bool KinematicRectangle::reachedTarget() {
    bool reachedX = reachedXPosition();
    bool reachedY = reachedYPosition();
    auto tmp = m_velocity;

    // Zero the x velocity
    if (reachedX) {
        tmp.x = 0;
        setVelocity(tmp);
    }

    if (reachedY) {
        tmp.y = 0;
        setVelocity(tmp);
    }
    return reachedX && reachedY;
}

void KinematicRectangle::Update() {
    DrawableRectangle::Update();
    if (m_body) {
        if (reachedTarget() && m_targetReachedCb != nullptr) {
            m_targetReachedCb(this);
        }
    }
}

void KinematicRectangle::SetTargetPosition(b2Vec2 &targetPos,
                           float speed,
                           targetReachedCb cb) {
    auto currPos = m_body->GetPosition();
    auto trajectory = targetPos - currPos;

    // Scale the speed by the magnitude of the trajectory
    float speedOffset = sqrt(trajectory.x*trajectory.x + trajectory.y*trajectory.y);
    // float speedOffset = 0;
    speed = speed + speedOffset;

    // TODO: Currently only does translations along the trajectory's vertical
    // instead of along the trajectory due to some strange divide by 0
    // errors when the trajectory is tiny but non-zero
    if (trajectory.y <= 0) {
        setVelocity(b2Vec2(0, -1*speed));
    } else {
        setVelocity(b2Vec2(0, speed));
    }

    m_targetPosition = targetPos;
    m_targetReachedCb = cb;
}

void KinematicRectangle::setVelocity(const b2Vec2 &velocity) {
    m_velocity = velocity;
    m_body->SetLinearVelocity(velocity);
}

const b2Vec2 KinematicRectangle::Velocity() {
    return m_velocity;
}

const b2Vec2 KinematicRectangle::GetBodyPosition() {
    if (m_body) {
        return m_body->GetPosition();
    }
    return b2Vec2_zero;
}



b2Shape* KinematicRectangle::Clone(b2BlockAllocator* allocator) const {
    void* mem = allocator->Allocate(sizeof(KinematicRectangle));
    KinematicRectangle* clone = new (mem) KinematicRectangle;
    *clone = *this;
    return clone;
}

const b2Vec2 KinematicRectangle::GetBodyInitialPosition() const {
    return m_originalPos;
}
