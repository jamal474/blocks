#include "PhysicsWorld.h"
#include "Constants.h"
#include "BlockLogger.h"

void LandingListener::BeginContact(b2Contact* contact) {
    if (!m_watched) return;

    b2Body* a = contact->GetFixtureA()->GetBody();
    b2Body* b = contact->GetFixtureB()->GetBody();

    if (a == m_watched)      { m_touched = true; m_other = b; }
    else if (b == m_watched) { m_touched = true; m_other = a; }
}

PhysicsWorld::PhysicsWorld() {
    m_world = std::make_unique<b2World>(b2Vec2(GRAVITY_X, GRAVITY_Y));
    m_world->SetAllowSleeping(true);
    m_world->SetContactListener(&m_listener);
    LOG_INFO("PhysicsWorld ready (gravity %.1f)", GRAVITY_Y);
}

b2Body* PhysicsWorld::createPlatform(float centerXPx, float centerYPx,
                                     float widthPx, float heightPx) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(centerXPx / PPM, centerYPx / PPM);
    m_platformBody = m_world->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(widthPx / (2.0f * PPM), heightPx / (2.0f * PPM));

    b2FixtureDef fixtureDef;
    fixtureDef.shape               = &shape;
    fixtureDef.friction            = 0.95f;
    fixtureDef.restitution         = 0.0f;
    fixtureDef.filter.categoryBits = CAT_TOWER;
    fixtureDef.filter.maskBits     = CAT_FALLING | CAT_TOWER;
    m_platformBody->CreateFixture(&fixtureDef);

    LOG_INFO("Platform created at px(%.1f, %.1f)", centerXPx, centerYPx);
    return m_platformBody;
}

void PhysicsWorld::step(float dt) {
    m_world->Step(dt, VELOCITY_ITERS, POSITION_ITERS);
}
