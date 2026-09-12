#pragma once
#include <box2d/box2d.h>
#include <memory>

/// Contact listener that simply records whether the one dynamic block in
/// flight has touched anything. Polling `b2Body::GetContactList()` reports
/// contacts that merely overlap in the broad-phase; this reports real touches.
class LandingListener : public b2ContactListener {
public:
    void watch(b2Body* body) { m_watched = body; m_touched = false; m_other = nullptr; }
    void clear()             { m_watched = nullptr; m_touched = false; m_other = nullptr; }

    bool     hasTouched() const { return m_touched; }
    b2Body*  getOther() const   { return m_other; }

    void BeginContact(b2Contact* contact) override;

private:
    b2Body* m_watched = nullptr;
    b2Body* m_other   = nullptr;
    bool    m_touched = false;
};

class PhysicsWorld {
public:
    PhysicsWorld();

    b2Body* createPlatform(float centerXPx, float centerYPx,
                           float widthPx, float heightPx);

    void step(float dt);

    b2World& getWorld() { return *m_world; }
    b2Body*  getPlatformBody() const { return m_platformBody; }

    LandingListener& listener() { return m_listener; }

private:
    std::unique_ptr<b2World> m_world;
    b2Body*         m_platformBody = nullptr;
    LandingListener m_listener;
};
