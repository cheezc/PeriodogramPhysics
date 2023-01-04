#ifndef KINEMATIC_RECTANGLE_H
#define KINEMATIC_RECTANGLE_H
#include "DrawableRectangle.hpp"
#include "box2d/b2_math.h"
#include <mutex>


class KinematicRectangle : public DrawableRectangle {
public:
    typedef void (*targetReachedCb)(KinematicRectangle* rect);
    enum State {
        IDLE = 0,
        TRAVERSING,
    };

    /**
     * KinematicRectangle
     *
     * @param  {sf::Vector2f} pos     :
     * @param  {sf::Vector2f} size    :
     * @param  {b2PolygonShape} shape :
     * @param  {b2Body*} body         :
     * @param  {b2Vec2} maxPositions  :
     * @param  {b2Vec2} minPositions  :
     */
    KinematicRectangle(
        sf::Vector2f &pos,
        sf::Vector2f &size,
        b2PolygonShape &shape,
        b2Body *body,
        b2Vec2 &maxPositions,
        b2Vec2 &minPositions);

    //  Constructor overload
    KinematicRectangle();

    /**
     * @brief Override update method of DrawableRectangle; Updates graphics
     * and physics based off of position of body
     */
    void Update() override;

    /**
     *  Sets a target position with velocity; calls a callback when target
     *  position reached; TODO: a state machine might be better here
     * @param  {b2Vec2} targetPos      : Target position
     * @param  {b2Vec2} speed : Magnitude of the velocity vector to get to the target
     * @param  {void(*)()} foo         : callback when target is reached
     */
    void SetTargetPosition(b2Vec2 &targetPos,
                           float speed,
                           targetReachedCb cb);

    /**
     *
     * @return {b2Vec2}  : Returns a velocity vector
     */
    const b2Vec2 Velocity();

    /**
     *
     * @return {b2Vec2}  : Returns the current position of the body
     */
    const b2Vec2 GetBodyPosition();

    /**
     *
     * @param  {b2BlockAllocator*} allocator : pool to allocate mem
     * @return {b2Shape*}                    : returns a b2shape virtual class
     */
    b2Shape* Clone(b2BlockAllocator* allocator) const override;


    /**
     * Gets the initial position of the box
     * @return {b2Vec2}  : original position
     */
    const b2Vec2 GetBodyInitialPosition() const;

private:
    bool reachedYPosition();
    bool reachedXPosition();
    bool reachedTarget();
    void setVelocity(const b2Vec2 &velocity);


    b2Body *m_body;
    b2Vec2 m_maxPositions;
    b2Vec2 m_minPositions;
    b2Vec2 m_targetPosition;
    b2Vec2 m_velocity;
    b2Vec2 m_originalPos;
    targetReachedCb m_targetReachedCb;
};
#endif