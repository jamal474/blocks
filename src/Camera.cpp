#include "Camera.h"
#include "Constants.h"
#include <algorithm>

void Camera::init(float windowWidth, float windowHeight) {
    m_viewWidth  = windowWidth;
    m_viewHeight = windowHeight;
    m_currentCenter = { windowWidth / 2.0f, windowHeight / 2.0f };
    m_view.setSize(windowWidth, windowHeight);
    m_view.setCenter(m_currentCenter);
}

void Camera::update(float towerTopYPx, float dt) {
    // Target: keep the tower top in the upper portion of the screen
    float targetY = towerTopYPx + m_viewHeight * (0.5f - CAMERA_TOP_MARGIN);

    // Only scroll up, never back down
    if (targetY < m_currentCenter.y) {
        // Smooth exponential interpolation (lerp)
        m_currentCenter.y += (targetY - m_currentCenter.y) * CAMERA_LERP_SPEED * dt;
    }

    m_view.setCenter(m_currentCenter);
}

sf::FloatRect Camera::getViewBounds() const {
    sf::Vector2f topLeft = m_currentCenter - sf::Vector2f(m_viewWidth, m_viewHeight) / 2.0f;
    sf::FloatRect bounds(topLeft, { m_viewWidth, m_viewHeight });
    // Expand for culling padding
    bounds.top    -= CULL_PADDING;
    bounds.height += 2.0f * CULL_PADDING;
    return bounds;
}
