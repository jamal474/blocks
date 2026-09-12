#pragma once
#include <SFML/Graphics.hpp>
#include <box2d/box2d.h>

/// Scripted pendulum crane.
///
/// The crane is deliberately *not* a Box2D body. Driving it through a revolute
/// joint means the solver fights every camera scroll, every weld, and every
/// hand-set velocity - which is exactly what made the old swing erratic.
///
/// Instead the arm integrates the real pendulum equation
///
///     theta'' = -(g / L) * sin(theta)
///
/// with a symplectic (semi-implicit Euler) integrator at a fixed timestep, plus
/// a gentle energy correction that pins the amplitude so the swing never decays
/// or winds up. theta = 0 hangs straight down, +theta swings screen-right.
///
/// The hook pose and its exact analytic velocity are published so the block
/// riding the hook can be placed on the arc and released with correct momentum.
class Crane {
public:
    Crane() = default;

    void create(float anchorXPx, float anchorYPx, float armLengthPx,
                sf::Texture& hookTexture);

    /// Restart the swing from one extreme with the given amplitude (radians).
    void reset(float amplitudeRad);

    /// Advance the pendulum. Call once per fixed timestep.
    void update(float dt);

    /// Move the pivot (camera follow). Free: no bodies, no solver disturbance.
    void setAnchor(float xPx, float yPx);

    void  setAmplitude(float rad) { m_amplitude = rad; }
    float getAmplitude() const    { return m_amplitude; }

    float getAngle() const           { return m_angle; }

    /// The same swing expressed as a body/sprite rotation.
    ///
    /// `m_angle` is measured from straight-down with +ve toward screen-right.
    /// Both SFML and Box2D rotate with the matrix [c -s; s c], which under a
    /// y-down screen turns a local "down" axis toward -x for a positive angle -
    /// the opposite way. So anything drawn or simulated on the rope takes the
    /// negated angle, and that conversion lives here rather than as scattered
    /// minus signs at the call sites.
    float getBodyAngle() const       { return -m_angle; }
    float getBodyAngularVelocity() const { return -m_angVel; }
    float getAngularVelocity() const { return m_angVel; }
    float getArmLength() const       { return m_armLengthPx; }

    /// Pose of a point on the rope `extraPx` beyond the hook (pixels).
    sf::Vector2f pointOnArm(float extraPx) const;

    /// Velocity of that same point (pixels / second).
    sf::Vector2f velocityOnArm(float extraPx) const;

    sf::Vector2f getAnchor() const { return { m_anchorXPx, m_anchorYPx }; }

    /// Rebuild the wire sprite; `alpha` interpolates between the previous and
    /// current simulation step for tear-free rendering.
    void syncSprite(float alpha);

    sf::RectangleShape& getWireSprite() { return m_wireSprite; }
    sf::CircleShape&    getHookSprite() { return m_hookSprite; }

    /// Interpolated angle used by the renderer (and by the attached block).
    float getRenderAngle(float alpha) const;

private:
    float m_anchorXPx   = 0.0f;
    float m_anchorYPx   = 0.0f;
    float m_armLengthPx = 0.0f;

    float m_angle     = 0.0f;   // radians from straight-down
    float m_angVel    = 0.0f;   // rad / s
    float m_amplitude = 0.0f;   // radians

    float m_prevAngle     = 0.0f;
    float m_prevAnchorXPx = 0.0f;
    float m_prevAnchorYPx = 0.0f;

    sf::RectangleShape m_wireSprite;
    sf::CircleShape    m_hookSprite;
};
