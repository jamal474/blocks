#pragma once
#include <SFML/Graphics.hpp>
#include "PhysicsWorld.h"
#include "Crane.h"
#include "Tower.h"
#include "Camera.h"

/// Top-level game class. Owns the window, physics world, and all subsystems.
class Game {
public:
    Game();
    void run();

private:
    void loadResources();
    void handleInput();
    void update(float dt);
    void render();
    void spawnNextBlock();

    // ── Core ────────────────────
    sf::RenderWindow m_window;
    PhysicsWorld     m_physics;
    Crane            m_crane;
    Tower            m_tower;
    Camera           m_camera;

    // ── Resources ───────────────
    sf::Texture m_blockTexture;
    sf::Texture m_hookTexture;
    sf::Texture m_bgTexture;
    sf::Texture m_skyTexture;
    sf::Sprite  m_bgSprite;
    sf::Sprite  m_skySprite;
    sf::Font    m_font;

    // ── HUD ─────────────────────
    sf::Text    m_scoreText;
    float       m_totalScore  = 0.0f;

    // ── State ───────────────────
    bool  m_blockDropped  = false;
    bool  m_waitingToLand = false;
    float m_settleTimer   = 0.0f;
    bool  m_gameOver      = false;
};
