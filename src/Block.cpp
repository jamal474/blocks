#include "Block.h"
#include "Constants.h"
#include "BlockLogger.h"

void Block::create(b2World& world, float xPx, float yPx, float sizePx,
                   sf::Texture& texture) {
    m_sizePx = sizePx;

    b2BodyDef bodyDef;
    bodyDef.type           = b2_kinematicBody;   // spawns on the hook
    bodyDef.position.Set(xPx / PPM, yPx / PPM);
    bodyDef.fixedRotation  = false;
    bodyDef.allowSleep     = false;
    bodyDef.angularDamping = BLOCK_ANG_DAMPING;
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    const float half = (sizePx / PPM) * 0.5f;
    shape.SetAsBox(half, half);

    b2FixtureDef fixtureDef;
    fixtureDef.shape             = &shape;
    fixtureDef.density           = BLOCK_DENSITY;
    fixtureDef.friction          = BLOCK_FRICTION;
    fixtureDef.restitution       = BLOCK_RESTITUTION;
    fixtureDef.filter.categoryBits = CAT_GHOST;
    fixtureDef.filter.maskBits     = 0x0000;     // attached: no collisions
    m_fixture = m_body->CreateFixture(&fixtureDef);

    m_prevPos   = m_body->GetPosition();
    m_prevAngle = m_body->GetAngle();

    m_sprite.setSize({ sizePx, sizePx });
    m_sprite.setTexture(&texture);
    m_sprite.setOrigin(sizePx * 0.5f, sizePx * 0.5f);
    syncSprite(1.0f);

    LOG_DEBUG("Block created at px(%.1f, %.1f) mass=%.2fkg", xPx, yPx,
              m_body->GetMass());
}

void Block::setMode(Mode mode) {
    if (!m_body || m_mode == mode) return;
    m_mode = mode;

    b2Filter filter = m_fixture->GetFilterData();

    switch (mode) {
    case Mode::Attached:
        m_body->SetType(b2_kinematicBody);
        m_body->SetBullet(false);
        filter.categoryBits = CAT_GHOST;
        filter.maskBits     = 0x0000;
        break;

    case Mode::Falling:
        m_body->SetType(b2_dynamicBody);
        m_body->SetBullet(true);          // CCD: never tunnel through the tower
        m_body->SetAwake(true);
        filter.categoryBits = CAT_FALLING;
        filter.maskBits     = CAT_TOWER;
        break;

    case Mode::Settled:
        m_body->SetType(b2_kinematicBody);
        m_body->SetBullet(false);
        m_body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
        m_body->SetAngularVelocity(0.0f);
        filter.categoryBits = CAT_TOWER;
        filter.maskBits     = CAT_FALLING;
        break;

    case Mode::Collapsing:
        m_body->SetType(b2_dynamicBody);
        m_body->SetBullet(false);
        m_body->SetEnabled(true);
        m_body->SetAwake(true);
        filter.categoryBits = CAT_TOWER;
        filter.maskBits     = CAT_TOWER | CAT_FALLING;
        break;
    }

    m_fixture->SetFilterData(filter);
}

void Block::driveTo(const b2Vec2& posMeters, float angleRad) {
    if (!m_body) return;
    m_body->SetTransform(posMeters, angleRad);
}

void Block::savePrevious() {
    if (!m_body) return;
    m_prevPos   = m_body->GetPosition();
    m_prevAngle = m_body->GetAngle();
}

void Block::syncSprite(float alpha) {
    if (!m_body) return;

    const b2Vec2 pos   = m_body->GetPosition();
    const float  angle = m_body->GetAngle();

    const float x = m_prevPos.x + (pos.x - m_prevPos.x) * alpha;
    const float y = m_prevPos.y + (pos.y - m_prevPos.y) * alpha;

    m_sprite.setPosition(x * PPM, y * PPM);
    m_sprite.setRotation(toDegrees(lerpAngle(m_prevAngle, angle, alpha)));
}

void Block::retire(b2World& world) {
    if (!m_body || m_dormant) return;
    world.DestroyBody(m_body);
    m_body    = nullptr;
    m_fixture = nullptr;
    m_dormant = true;
}
