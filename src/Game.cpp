#include "Game.h"
#include "Constants.h"
#include "EmbeddedAssets.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// ── Constructor ─────────────────────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode(static_cast<unsigned>(WINDOW_WIDTH),
                              static_cast<unsigned>(WINDOW_HEIGHT)),
               "City Bloxx", sf::Style::Close | sf::Style::Default)
{
    m_window.setFramerateLimit(60);
    loadResources();

    // ── Camera ──────────────────────────────────────────
    m_camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);

    // ── Platform (static ground for the first block) ────
    float platformY = WINDOW_HEIGHT - 70.0f;
    m_physics.createPlatform(WINDOW_WIDTH / 2.0f, platformY,
                              PLATFORM_WIDTH, PLATFORM_HEIGHT);

    // ── Crane ───────────────────────────────────────────
    m_crane.create(m_physics.getWorld(),
                   WINDOW_WIDTH / 2.0f, 0.0f,
                   CRANE_ARM_LENGTH, m_hookTexture);

    // ── Spawn the first block on the crane ──────────────
    spawnNextBlock();
}

// ── Resource Loading ────────────────────────────────────────────────────────
void Game::loadResources() {
    if (!m_blockTexture.loadFromMemory(ASSET_BLOCK_PNG, ASSET_BLOCK_PNG_SIZE))
        std::cerr << "Failed to load block texture from embedded data\n";

    if (!m_hookTexture.loadFromMemory(ASSET_HOOK_PNG, ASSET_HOOK_PNG_SIZE))
        std::cerr << "Failed to load hook texture from embedded data\n";
    m_hookTexture.setSmooth(true);

    if (!m_bgTexture.loadFromMemory(ASSET_BACKGROUND_PNG, ASSET_BACKGROUND_PNG_SIZE))
        std::cerr << "Failed to load background texture from embedded data\n";
    m_bgSprite.setTexture(m_bgTexture);
    m_bgSprite.setScale(0.6f, 0.6f);
    m_bgSprite.setPosition(-20.0f, 200.0f);

    if (!m_skyTexture.loadFromMemory(ASSET_SKY_JPG, ASSET_SKY_JPG_SIZE))
        std::cerr << "Failed to load sky texture from embedded data\n";
    m_skySprite.setTexture(m_skyTexture);
    m_skySprite.setScale(0.4f, 0.4f);

    if (!m_font.loadFromMemory(ASSET_DOODLEJUMP_TTF, ASSET_DOODLEJUMP_TTF_SIZE))
        std::cerr << "Failed to load font from embedded data\n";

    m_scoreText.setFont(m_font);
    m_scoreText.setCharacterSize(24);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color::Black);
    m_scoreText.setOutlineThickness(2.0f);

    // ── Set window icon from the embedded background image (cropped to building) ──
    sf::Image iconImage;
    if (iconImage.loadFromMemory(ASSET_BACKGROUND_PNG, ASSET_BACKGROUND_PNG_SIZE)) {
        auto imgSize = iconImage.getSize();
        // Crop to a square centered on the building (zoom in)
        unsigned cropSize = static_cast<unsigned>(imgSize.x * 0.7f);
        unsigned startX   = (imgSize.x - cropSize) / 2;
        unsigned startY   = (imgSize.y - cropSize) / 3; // bias upward toward the building

        sf::Image croppedIcon;
        croppedIcon.create(cropSize, cropSize, sf::Color::Transparent);
        croppedIcon.copy(iconImage, 0, 0,
                         sf::IntRect(startX, startY, cropSize, cropSize));

        m_window.setIcon(croppedIcon.getSize().x, croppedIcon.getSize().y,
                         croppedIcon.getPixelsPtr());
    }
}

// ── Spawn a new block attached to the crane ─────────────────────────────────
void Game::spawnNextBlock() {
    // Position at the crane arm tip
    b2Vec2 armPos = m_crane.getArm()->GetPosition();
    float x = armPos.x * PPM;
    float y = armPos.y * PPM + BLOCK_SIZE / 2.0f;

    Block& block = m_tower.spawnBlock(m_physics.getWorld(),
                                       x, y, BLOCK_SIZE, m_blockTexture);

    // Weld the block to the crane arm
    m_crane.attachBlock(m_physics.getWorld(), block);

    m_blockDropped  = false;
    m_waitingToLand = false;
    m_settleTimer   = 0.0f;
}

// ── Main Game Loop ──────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        dt = std::min(dt, 0.05f);  // clamp to prevent spiral of death

        handleInput();

        if (!m_gameOver) {
            // ── Fixed timestep physics ──────────────────
            accumulator += dt;
            while (accumulator >= FIXED_DT) {
                m_physics.step(FIXED_DT, VELOCITY_ITERS, POSITION_ITERS);
                accumulator -= FIXED_DT;
            }

            update(dt);
        }

        render();
    }
}

// ── Input Handling ──────────────────────────────────────────────────────────
void Game::handleInput() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            m_window.close();

        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                m_window.close();

            // Drop the block on Space
            if (event.key.code == sf::Keyboard::Space
                && !m_blockDropped && !m_gameOver) {
                m_crane.dropBlock(m_physics.getWorld());
                m_blockDropped  = true;
                m_waitingToLand = true;
                m_settleTimer   = 0.0f;
            }
        }
    }
}

// ── Per-Frame Update ────────────────────────────────────────────────────────
void Game::update(float dt) {
    // Sync all sprites from physics
    m_tower.syncSprites();
    m_crane.syncSprite();

    // ── Waiting for block to settle after dropping ──────
    if (m_waitingToLand && m_blockDropped) {
        Block& latest = m_tower.getBlocks().back();
        b2Body* body  = latest.getBody();

        // Check if the block fell off the bottom
        float cameraBottom = m_camera.getCenterY() + WINDOW_HEIGHT / 2.0f;
        if (m_tower.isTopBlockFallen(cameraBottom)) {
            m_gameOver = true;
            return;
        }

        // Check if block has come to rest (low velocity)
        b2Vec2 vel = body->GetLinearVelocity();
        float speed = vel.Length();

        if (speed < 0.3f) {
            m_settleTimer += dt;
        } else {
            m_settleTimer = 0.0f;
        }

        // Block has settled — score it and spawn the next one
        if (m_settleTimer > 0.4f) {
            float alignment = m_tower.calculateAlignment();
            m_totalScore += alignment;

            m_scoreText.setString("Score: " + std::to_string(static_cast<int>(m_totalScore)));

            // Spawn the next block
            spawnNextBlock();
        }
    }

    // ── Camera tracking ─────────────────────────────────
    float oldCenterY = m_camera.getCenterY();
    m_camera.update(m_tower.getTopY(), dt);
    float newCenterY = m_camera.getCenterY();
    float deltaY = newCenterY - oldCenterY;

    if (std::abs(deltaY) > 0.0001f) {
        // Smoothly translate the crane system
        m_crane.translate(deltaY);

        // Translate the active block as well if it's still attached to the crane
        if (!m_blockDropped && !m_tower.getBlocks().empty()) {
            Block& activeBlock = m_tower.getBlocks().back();
            b2Body* body = activeBlock.getBody();
            if (body) {
                b2Vec2 pos = body->GetPosition();
                body->SetTransform({ pos.x, pos.y + deltaY / PPM }, body->GetAngle());
            }
        }
    }

    float viewTop = newCenterY - WINDOW_HEIGHT / 2.0f;

    // Keep the sky background following the camera
    m_skySprite.setPosition(0.0f, viewTop);

    // ── Performance: freeze distant bodies ──────────────
    if (m_tower.getBlockCount() > 10) {
        m_tower.freezeDistantBodies(viewTop);
    }
}

// ── Rendering ───────────────────────────────────────────────────────────────
void Game::render() {
    m_window.clear(sf::Color(20, 20, 40));
    m_window.setView(m_camera.getView());

    // ── Background ──────────────────────────────────────
    m_window.draw(m_skySprite);
    m_window.draw(m_bgSprite);

    // ── Crane wire ──────────────────────────────────────
    m_window.draw(m_crane.getWireSprite());

    // ── Blocks (with viewport culling) ──────────────────
    sf::FloatRect viewBounds = m_camera.getViewBounds();
    for (auto& block : m_tower.getBlocks()) {
        sf::Vector2f pos = block.getSprite().getPosition();
        if (viewBounds.contains(pos)) {
            m_window.draw(block.getSprite());
        }
    }

    // ── HUD (use default view for screen-space text) ────
    sf::View hudView = m_window.getDefaultView();
    m_window.setView(hudView);

    m_scoreText.setPosition(10.0f, 10.0f);
    m_window.draw(m_scoreText);

    if (m_gameOver) {
        sf::Text gameOverText;
        gameOverText.setFont(m_font);
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setOutlineColor(sf::Color::Black);
        gameOverText.setOutlineThickness(3.0f);
        gameOverText.setString("GAME OVER");
        sf::FloatRect textBounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        gameOverText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        m_window.draw(gameOverText);
    }

    m_window.display();
}
