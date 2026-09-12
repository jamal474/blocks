#pragma once
#include <SFML/Graphics.hpp>

/// Camera that pans up as the tower grows.
///
/// Updated inside the fixed step with frame-rate independent exponential
/// smoothing, and interpolated at render time so scrolling never stutters.
class Camera {
public:
    Camera() = default;

    void init(float windowWidth, float windowHeight);

    /// Advance toward the target. Call once per fixed step.
    void update(float towerTopYPx, float dt);

    void savePrevious() { m_prevCenterY = m_centerY; }

    /// View for rendering, interpolated between the last two steps.
    const sf::View& getView(float alpha);

    sf::FloatRect getViewBounds(float alpha) const;

    float getCenterY() const { return m_centerY; }
    float getTopY() const    { return m_centerY - m_viewHeight * 0.5f; }
    float getBottomY() const { return m_centerY + m_viewHeight * 0.5f; }

    float getRenderTopY(float alpha) const;

    /// Snap straight to the framing for a given tower top (no glide).
    void reset(float towerTopYPx);

private:
    sf::View m_view;
    float    m_centerX     = 0.0f;
    float    m_centerY     = 0.0f;
    float    m_prevCenterY = 0.0f;
    float    m_viewWidth   = 0.0f;
    float    m_viewHeight  = 0.0f;
};
