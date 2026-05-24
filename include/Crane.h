#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

class Block;

/// Pendulum crane: static anchor + dynamic arm connected by a revolute joint.
/// The current block is welded to the arm tip until the player drops it.
class Crane {
public:
    Crane() = default;

    /// Build the crane bodies and revolute joint.
    void create(b2World& world, float anchorXPx, float anchorYPx,
                float armLengthPx, sf::Texture& hookTexture);

    /// Weld a block to the arm tip.
    void attachBlock(b2World& world, Block& block);

    /// Release the welded block (destroy the weld joint).
    void dropBlock(b2World& world);

    /// Update the SFML sprite for the wire/hook from physics state.
    void syncSprite();

    /// Shift the crane bodies when the camera scrolls.
    void translate(float deltaYPx);

    bool hasBlock() const { return m_weld != nullptr; }
    b2Body* getArm() const { return m_arm; }

    sf::RectangleShape& getWireSprite() { return m_wireSprite; }

private:
    b2Body*          m_anchor = nullptr;
    b2Body*          m_arm    = nullptr;
    b2RevoluteJoint* m_joint  = nullptr;
    b2WeldJoint*     m_weld   = nullptr;

    float            m_armLengthPx = 0.0f;
    sf::RectangleShape m_wireSprite;
};
