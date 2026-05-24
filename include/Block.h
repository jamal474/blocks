#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/// A single building block backed by a Box2D dynamic body.
class Block {
public:
    Block() = default;

    /// Create the physics body and configure the sprite.
    void create(b2World& world, float x, float y,
                float sizePx, sf::Texture& texture);

    /// Sync the SFML sprite position/rotation from the physics body.
    void syncSprite();

    b2Body*              getBody()   const { return m_body; }
    sf::RectangleShape&  getSprite()       { return m_sprite; }
    const sf::RectangleShape& getSprite() const { return m_sprite; }
    float                getSize()   const { return m_sizePx; }
    bool                 isFrozen()  const { return m_frozen; }

    /// Disable this body in the physics world (performance optimisation).
    void freeze();

private:
    b2Body*            m_body   = nullptr;
    sf::RectangleShape m_sprite;
    float              m_sizePx = 0.0f;
    bool               m_frozen = false;
};
