#pragma once
#include <SFML/System/Vector2.hpp>
#include <box2d/box2d.h>
#include <cmath>

// ── Window ──────────────────────────────────────────────
constexpr float WINDOW_WIDTH  = 400.0f;
constexpr float WINDOW_HEIGHT = 600.0f;

// ── Pixels Per Meter (Box2D ↔ SFML bridge) ─────────────
constexpr float PPM = 30.0f;

// ── Physics Timestep ────────────────────────────────────
constexpr float FIXED_DT       = 1.0f / 60.0f;
constexpr int   VELOCITY_ITERS = 8;
constexpr int   POSITION_ITERS = 3;

// ── Block Dimensions (in pixels) ────────────────────────
constexpr float BLOCK_SIZE     = 50.0f;
constexpr float BLOCK_DENSITY  = 5.0f;
constexpr float BLOCK_FRICTION = 0.8f;
constexpr float BLOCK_RESTITUTION = 0.05f;

// ── Crane ───────────────────────────────────────────────
constexpr float CRANE_ARM_LENGTH = 100.0f;  // pixels
constexpr float CRANE_DAMPING    = 0.3f;
constexpr float CRANE_ANGLE_LIMIT = 0.6f;   // radians (~34°)
constexpr float CRANE_INITIAL_IMPULSE = 0.8f;

// ── Platform ────────────────────────────────────────────
constexpr float PLATFORM_WIDTH  = 150.0f;  // pixels
constexpr float PLATFORM_HEIGHT = 10.0f;

// ── Camera ──────────────────────────────────────────────
constexpr float CAMERA_LERP_SPEED = 3.0f;
constexpr float CAMERA_TOP_MARGIN = 0.30f;  // keep tower top in upper 30%

// ── Gravity ─────────────────────────────────────────────
constexpr float GRAVITY_X = 0.0f;
constexpr float GRAVITY_Y = 9.81f;

// ── Performance ─────────────────────────────────────────
constexpr float FREEZE_DISTANCE = 800.0f;   // pixels below camera top
constexpr float CULL_PADDING    = 60.0f;     // pixels outside viewport

// ── Coordinate Conversions ──────────────────────────────
inline sf::Vector2f toPixels(const b2Vec2& v) {
    return { v.x * PPM, v.y * PPM };
}

inline b2Vec2 toMeters(const sf::Vector2f& v) {
    return { v.x / PPM, v.y / PPM };
}

inline float toMeters(float px) {
    return px / PPM;
}

inline float toPixels(float m) {
    return m * PPM;
}

inline float toDegrees(float radians) {
    return radians * (180.0f / b2_pi);
}

inline float toRadians(float degrees) {
    return degrees * (b2_pi / 180.0f);
}
