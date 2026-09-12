#include "Crane.h"
#include "Constants.h"
#include "BlockLogger.h"
#include <cmath>

void Crane::create(float anchorXPx, float anchorYPx, float armLengthPx,
                   sf::Texture& hookTexture) {
    m_anchorXPx   = m_prevAnchorXPx = anchorXPx;
    m_anchorYPx   = m_prevAnchorYPx = anchorYPx;
    m_armLengthPx = armLengthPx;

    // Wire: a thin quad hanging from the pivot, with a top-centre origin so it
    // rotates about the pivot.
    m_wireSprite.setSize({ 10.0f, armLengthPx });
    m_wireSprite.setTexture(&hookTexture);
    m_wireSprite.setOrigin(5.0f, 0.0f);

    m_hookSprite.setRadius(6.0f);
    m_hookSprite.setOrigin(6.0f, 6.0f);
    m_hookSprite.setFillColor(sf::Color(70, 70, 78));
    m_hookSprite.setOutlineColor(sf::Color(20, 20, 24));
    m_hookSprite.setOutlineThickness(2.0f);

    reset(CRANE_START_AMPLITUDE);

    LOG_INFO("Crane created: anchor px(%.1f, %.1f) arm=%.1fpx amplitude=%.3frad",
             anchorXPx, anchorYPx, armLengthPx, m_amplitude);
}

void Crane::reset(float amplitudeRad) {
    m_amplitude = clampf(amplitudeRad, 0.05f, CRANE_MAX_AMPLITUDE);
    // Start at an extreme with zero speed: a well-defined, repeatable state.
    m_angle     = m_prevAngle = -m_amplitude;
    m_angVel    = 0.0f;
}

void Crane::update(float dt) {
    m_prevAngle     = m_angle;
    m_prevAnchorXPx = m_anchorXPx;
    m_prevAnchorYPx = m_anchorYPx;

    const float L = m_armLengthPx / PPM;               // rope length, metres
    if (L <= 0.0f) return;

    // ── Symplectic Euler on theta'' = -(g/L) sin(theta) ──────────────────────
    // Velocity first, then position: this is energy-stable (no secular drift),
    // unlike explicit Euler which slowly winds a pendulum up until it spins.
    m_angVel += -(CRANE_GRAVITY / L) * std::sin(m_angle) * dt;
    m_angle  += m_angVel * dt;

    // ── Amplitude lock ───────────────────────────────────────────────────────
    // Specific energy of a pendulum: e = 0.5*L*w^2 + g*(1 - cos(theta)).
    // Nudge the angular speed toward the value that reproduces exactly the
    // requested amplitude. Applied only where the kinetic term is meaningful so
    // the sign of w is never ambiguous near the turning points.
    const float targetE = CRANE_GRAVITY * (1.0f - std::cos(m_amplitude));
    const float potE    = CRANE_GRAVITY * (1.0f - std::cos(m_angle));
    const float kinE    = 0.5f * L * m_angVel * m_angVel;

    if (kinE > 1e-4f) {
        const float wantedKin = targetE - potE;
        if (wantedKin > 0.0f) {
            const float wantedSpeed = std::sqrt(2.0f * wantedKin / L);
            const float sign        = (m_angVel >= 0.0f) ? 1.0f : -1.0f;
            m_angVel += (sign * wantedSpeed - m_angVel) * CRANE_ENERGY_CORRECT;
        }
    }

    // Hard clamp: numerically the arm can never leave its arc.
    if (m_angle > m_amplitude) {
        m_angle  = m_amplitude;
        m_angVel = std::min(m_angVel, 0.0f);
    } else if (m_angle < -m_amplitude) {
        m_angle  = -m_amplitude;
        m_angVel = std::max(m_angVel, 0.0f);
    }
}

void Crane::setAnchor(float xPx, float yPx) {
    m_anchorXPx = xPx;
    m_anchorYPx = yPx;
}

sf::Vector2f Crane::pointOnArm(float extraPx) const {
    const float r = m_armLengthPx + extraPx;
    return { m_anchorXPx + r * std::sin(m_angle),
             m_anchorYPx + r * std::cos(m_angle) };
}

sf::Vector2f Crane::velocityOnArm(float extraPx) const {
    // d/dt [ r*sin(t), r*cos(t) ] = r*w*[ cos(t), -sin(t) ]
    const float r = m_armLengthPx + extraPx;
    return {  r * m_angVel * std::cos(m_angle),
             -r * m_angVel * std::sin(m_angle) };
}

float Crane::getRenderAngle(float alpha) const {
    return lerpAngle(m_prevAngle, m_angle, alpha);
}

void Crane::syncSprite(float alpha) {
    const float angle   = getRenderAngle(alpha);
    const float anchorX = m_prevAnchorXPx + (m_anchorXPx - m_prevAnchorXPx) * alpha;
    const float anchorY = m_prevAnchorYPx + (m_anchorYPx - m_prevAnchorYPx) * alpha;

    m_wireSprite.setPosition(anchorX, anchorY);
    m_wireSprite.setSize({ 10.0f, m_armLengthPx });

    // Verified against sf::Transform rather than assumed: SFML maps local
    // (0, L) to (-sin(a)*L, +cos(a)*L), so pointing the rope along +theta
    // means rotating by -theta.
    m_wireSprite.setRotation(-toDegrees(angle));

    m_hookSprite.setPosition(anchorX + m_armLengthPx * std::sin(angle),
                             anchorY + m_armLengthPx * std::cos(angle));
}
