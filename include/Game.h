#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsWorld.h"
#include "Crane.h"
#include "Tower.h"
#include "Camera.h"

/// Top-level game: owns the window, the world, and the state machine.
///
/// Every piece of logic lives in `fixedUpdate`, which runs at a constant rate.
/// `render` interpolates, so the picture is smooth regardless of frame rate.
class Game {
public:
    Game();
    void run();

private:
    enum class State { Swinging, Falling, Settling, Collapsing, GameOver };

    void loadResources();
    void handleEvents();
    void fixedUpdate(float dt);
    void render(float alpha);

    void spawnNextBlock();
    void dropBlock();
    void resolveLanding();
    void triggerCollapse(const char* reason);
    void restart();

    void drawHud();

    // ── Core ─────────────────────────────────────────────
    sf::RenderWindow m_window;
    PhysicsWorld     m_physics;
    Crane            m_crane;
    Tower            m_tower;
    Camera           m_camera;

    // ── Resources ────────────────────────────────────────
    sf::Texture m_blockTexture;
    sf::Texture m_hookTexture;
    sf::Texture m_bgTexture;
    sf::Sprite  m_bgSprite;
    sf::VertexArray m_sky;
    sf::Font    m_font;

    // ── HUD ──────────────────────────────────────────────
    sf::Text           m_scoreText;
    sf::Text           m_floorText;
    sf::Text           m_messageText;
    sf::Text           m_hintText;
    sf::RectangleShape m_meterBack;
    sf::RectangleShape m_meterFill;
    sf::Text           m_meterLabel;

    // ── State ────────────────────────────────────────────
    State m_state        = State::Swinging;
    float m_settleTimer  = 0.0f;
    float m_collapseTimer = 0.0f;
    int   m_score        = 0;
    int   m_combo        = 0;
    int   m_floors       = 0;
    float m_lastOffset   = 0.0f;
    float m_groundViewTop = 0.0f;   // view top at ground level, for the sky
    float m_flashTimer   = 0.0f;
    bool  m_perfectFlash = false;
};
