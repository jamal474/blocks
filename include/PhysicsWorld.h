#pragma once
#include <box2d/box2d.h>
#include <memory>

class PhysicsWorld {
public:
    PhysicsWorld();

    /// Create a static ground/platform body at the given pixel position.
    b2Body* createPlatform(float centerX, float centerY,
                           float widthPx, float heightPx);

    void step(float dt, int velIters, int posIters);

    b2World& getWorld();

private:
    std::unique_ptr<b2World> m_world;
};
