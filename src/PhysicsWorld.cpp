#include "PhysicsWorld.h"
#include "Constants.h"

PhysicsWorld::PhysicsWorld() {
    b2Vec2 gravity(GRAVITY_X, GRAVITY_Y);
    m_world = std::make_unique<b2World>(gravity);
    m_world->SetAllowSleeping(true);
}

b2Body* PhysicsWorld::createPlatform(float centerX, float centerY,
                                      float widthPx, float heightPx) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(centerX / PPM, centerY / PPM);

    b2Body* body = m_world->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(widthPx / (2.0f * PPM), heightPx / (2.0f * PPM));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.friction = 0.9f;
    body->CreateFixture(&fixtureDef);

    return body;
}

void PhysicsWorld::step(float dt, int velIters, int posIters) {
    m_world->Step(dt, velIters, posIters);
}

b2World& PhysicsWorld::getWorld() {
    return *m_world;
}
