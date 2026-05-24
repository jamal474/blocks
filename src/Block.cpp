#include "Block.h"
#include "Constants.h"

void Block::create(b2World& world, float x, float y,
                   float sizePx, sf::Texture& texture) {
    m_sizePx = sizePx;

    // ── Physics Body ────────────────────────────────────
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x / PPM, y / PPM);
    bodyDef.fixedRotation = false;  // allow toppling
    bodyDef.allowSleep    = true;
    m_body = world.CreateBody(&bodyDef);

    b2PolygonShape shape;
    float halfSize = (sizePx / PPM) / 2.0f;
    shape.SetAsBox(halfSize, halfSize);

    b2FixtureDef fixtureDef;
    fixtureDef.shape       = &shape;
    fixtureDef.density     = BLOCK_DENSITY;
    fixtureDef.friction    = BLOCK_FRICTION;
    fixtureDef.restitution = BLOCK_RESTITUTION;
    m_body->CreateFixture(&fixtureDef);

    // ── SFML Sprite ─────────────────────────────────────
    m_sprite.setSize({ sizePx, sizePx });
    m_sprite.setTexture(&texture);
    m_sprite.setOrigin(sizePx / 2.0f, sizePx / 2.0f);
    syncSprite();
}

void Block::syncSprite() {
    if (!m_body) return;
    b2Vec2 pos = m_body->GetPosition();
    m_sprite.setPosition(toPixels(pos));
    m_sprite.setRotation(-toDegrees(m_body->GetAngle()));
}

void Block::freeze() {
    if (m_body && !m_frozen) {
        m_body->SetEnabled(false);
        m_frozen = true;
    }
}
