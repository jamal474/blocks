#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/// One building block.
///
/// A block moves through four modes and its Box2D body type changes with them:
///
///   Attached  - kinematic, riding the crane hook, collides with nothing.
///   Falling   - dynamic + bullet CCD, the only genuinely simulated body.
///   Settled   - kinematic, part of the driven swaying column.
///   Collapsing- dynamic again, so the game-over topple is real physics.
class Block {
public:
    enum class Mode { Attached, Falling, Settled, Collapsing };

    Block() = default;

    void create(b2World& world, float xPx, float yPx, float sizePx,
                sf::Texture& texture);

    void setMode(Mode mode);
    Mode getMode() const { return m_mode; }

    /// Teleport a kinematic block (used by the crane and the sway driver).
    void driveTo(const b2Vec2& posMeters, float angleRad);

    /// Remember the current transform so rendering can interpolate.
    void savePrevious();

    /// Push the (interpolated) physics transform into the sprite.
    void syncSprite(float alpha);

    /// Neutral slot in the tower column, i.e. the pose before sway is added.
    void  setBase(const b2Vec2& base) { m_base = base; }
    const b2Vec2& getBase() const     { return m_base; }

    void  setHeightIndex(int i) { m_heightIndex = i; }
    int   getHeightIndex() const { return m_heightIndex; }

    b2Body*             getBody() const { return m_body; }
    sf::RectangleShape& getSprite()     { return m_sprite; }
    const sf::RectangleShape& getSprite() const { return m_sprite; }

    float getSize() const      { return m_sizePx; }
    float getSizeMeters() const { return m_sizePx / PPM_LOCAL; }

    b2Vec2 getPosition() const { return m_body ? m_body->GetPosition() : b2Vec2(0, 0); }
    float  getAngle() const    { return m_body ? m_body->GetAngle() : 0.0f; }

    /// Retire a block that has scrolled permanently out of the world: its body
    /// is destroyed outright so it stops costing anything in the solver's body
    /// walk, while the sprite keeps its last pose. Safe because the camera only
    /// ever climbs, so nothing can reach it again.
    void retire(b2World& world);
    bool isDormant() const  { return m_dormant; }

private:
    static constexpr float PPM_LOCAL = 30.0f;

    b2Body*            m_body    = nullptr;
    b2Fixture*         m_fixture = nullptr;
    sf::RectangleShape m_sprite;

    float  m_sizePx      = 0.0f;
    Mode   m_mode        = Mode::Attached;
    b2Vec2 m_base        { 0.0f, 0.0f };
    int    m_heightIndex = 0;
    bool   m_dormant     = false;

    b2Vec2 m_prevPos   { 0.0f, 0.0f };
    float  m_prevAngle = 0.0f;
};
