#pragma once
#include <SFML/Graphics.hpp>

/// Dynamic camera that smoothly pans upward as the tower grows.
class Camera {
public:
    Camera() = default;
    void init(float windowWidth, float windowHeight);

    /// Smoothly track the tower top.
    void update(float towerTopYPx, float dt);

    const sf::View& getView() const { return m_view; }
    sf::FloatRect   getViewBounds() const;

    /// Current Y center in pixels.
    float getCenterY() const { return m_currentCenter.y; }

private:
    sf::View       m_view;
    sf::Vector2f   m_currentCenter;
    float          m_viewWidth  = 0.0f;
    float          m_viewHeight = 0.0f;
};
