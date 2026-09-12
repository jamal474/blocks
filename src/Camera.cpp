#include "Camera.h"
#include "Constants.h"
#include "BlockLogger.h"

void Camera::init(float windowWidth, float windowHeight) {
    m_viewWidth  = windowWidth;
    m_viewHeight = windowHeight;
    m_centerX    = windowWidth * 0.5f;
    m_view.setSize(windowWidth, windowHeight);
    reset(PLATFORM_Y - PLATFORM_HEIGHT * 0.5f);
    LOG_INFO("Camera initialised (%.0fx%.0f)", windowWidth, windowHeight);
}

void Camera::reset(float towerTopYPx) {
    // Snap to the same framing the camera would settle into. Without this the
    // first block falls the height of the screen while every later block falls
    // a few dozen pixels - the opening of the game played nothing like the
    // rest of it.
    m_centerY = m_prevCenterY =
        towerTopYPx + m_viewHeight * (0.5f - CAMERA_TOP_MARGIN);
    m_view.setCenter(m_centerX, m_centerY);
}

void Camera::update(float towerTopYPx, float dt) {
    const float targetY = towerTopYPx + m_viewHeight * (0.5f - CAMERA_TOP_MARGIN);

    // Only ever climb - the tower never gets shorter.
    if (targetY < m_centerY) {
        m_centerY += (targetY - m_centerY) * smoothFactor(CAMERA_SMOOTH_RATE, dt);
    }
}

float Camera::getRenderTopY(float alpha) const {
    const float c = m_prevCenterY + (m_centerY - m_prevCenterY) * alpha;
    return c - m_viewHeight * 0.5f;
}

const sf::View& Camera::getView(float alpha) {
    const float c = m_prevCenterY + (m_centerY - m_prevCenterY) * alpha;
    m_view.setCenter(m_centerX, c);
    return m_view;
}

sf::FloatRect Camera::getViewBounds(float alpha) const {
    const float c = m_prevCenterY + (m_centerY - m_prevCenterY) * alpha;
    return sf::FloatRect(m_centerX - m_viewWidth * 0.5f - CULL_PADDING,
                         c - m_viewHeight * 0.5f - CULL_PADDING,
                         m_viewWidth  + 2.0f * CULL_PADDING,
                         m_viewHeight + 2.0f * CULL_PADDING);
}
