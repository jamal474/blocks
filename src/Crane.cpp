#include "Crane.h"
#include "Block.h"
#include "Constants.h"
#include <cmath>

void Crane::create(b2World& world, float anchorXPx, float anchorYPx,
                   float armLengthPx, sf::Texture& hookTexture) {
    m_armLengthPx = armLengthPx;

    // ── Static anchor (pivot at the top of the screen) ──
    b2BodyDef anchorDef;
    anchorDef.type = b2_staticBody;
    anchorDef.position.Set(anchorXPx / PPM, anchorYPx / PPM);
    m_anchor = world.CreateBody(&anchorDef);

    // ── Dynamic arm (the pendulum bob) ──────────────────
    b2BodyDef armDef;
    armDef.type = b2_dynamicBody;
    float armYPx = anchorYPx + armLengthPx;
    armDef.position.Set(anchorXPx / PPM, armYPx / PPM);
    // Set linear and angular damping to 0 for perpetual physical swing
    armDef.linearDamping  = 0.0f;
    armDef.angularDamping = 0.0f;
    m_arm = world.CreateBody(&armDef);

    // Tiny fixture for mass — doesn't collide with anything
    b2CircleShape armShape;
    armShape.m_radius = 0.1f;
    b2FixtureDef armFixture;
    armFixture.shape    = &armShape;
    armFixture.density  = 2.0f;
    armFixture.filter.maskBits = 0x0000;  // no collisions
    m_arm->CreateFixture(&armFixture);

    // ── Revolute joint = pendulum pivot ─────────────────
    b2RevoluteJointDef jointDef;
    jointDef.Initialize(m_anchor, m_arm, m_anchor->GetPosition());
    jointDef.enableLimit = true;
    jointDef.lowerAngle  = -CRANE_ANGLE_LIMIT;
    jointDef.upperAngle  =  CRANE_ANGLE_LIMIT;
    m_joint = static_cast<b2RevoluteJoint*>(world.CreateJoint(&jointDef));

    // Give an initial angular impulse to start swinging
    m_arm->ApplyAngularImpulse(CRANE_INITIAL_IMPULSE, true);

    // ── Wire sprite ─────────────────────────────────────
    m_wireSprite.setSize({ 12.0f, armLengthPx });
    m_wireSprite.setTexture(&hookTexture);
    m_wireSprite.setOrigin(6.0f, 0.0f);  // top-center origin
}

void Crane::attachBlock(b2World& world, Block& block) {
    if (m_weld) return;  // already holding a block

    b2WeldJointDef weldDef;
    weldDef.Initialize(m_arm, block.getBody(), block.getBody()->GetPosition());
    // Default stiffness=0, damping=0 produces a rigid weld in Box2D 2.4+
    m_weld = static_cast<b2WeldJoint*>(world.CreateJoint(&weldDef));
}

void Crane::dropBlock(b2World& world) {
    if (m_weld) {
        world.DestroyJoint(m_weld);
        m_weld = nullptr;
    }
}

void Crane::syncSprite() {
    if (!m_anchor || !m_arm) return;

    // Wire goes from anchor position to arm position
    sf::Vector2f anchorPos = toPixels(m_anchor->GetPosition());
    sf::Vector2f armPos    = toPixels(m_arm->GetPosition());

    m_wireSprite.setPosition(anchorPos);

    // Calculate angle from anchor to arm
    sf::Vector2f diff = armPos - anchorPos;
    float angle = std::atan2(diff.x, diff.y);  // atan2(dx, dy) for SFML rotation
    m_wireSprite.setRotation(toDegrees(angle));

    // Adjust length to match actual distance
    float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    m_wireSprite.setSize({ 12.0f, dist });
}

void Crane::translate(float deltaYPx) {
    float deltaY = deltaYPx / PPM;
    if (m_anchor) {
        b2Vec2 pos = m_anchor->GetPosition();
        m_anchor->SetTransform({ pos.x, pos.y + deltaY }, m_anchor->GetAngle());
    }
    if (m_arm) {
        b2Vec2 pos = m_arm->GetPosition();
        m_arm->SetTransform({ pos.x, pos.y + deltaY }, m_arm->GetAngle());
    }
}
