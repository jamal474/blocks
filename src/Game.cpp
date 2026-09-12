#include "Game.h"
#include "Constants.h"
#include "EmbeddedAssets.h"
#include "BlockLogger.h"
#include <algorithm>
#include <cmath>
#include <iostream>

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────
Game::Game()
    : m_window(sf::VideoMode(static_cast<unsigned>(WINDOW_WIDTH),
                             static_cast<unsigned>(WINDOW_HEIGHT)),
               "Blocks", sf::Style::Close | sf::Style::Titlebar)
{
    m_window.setVerticalSyncEnabled(true);
    m_window.setFramerateLimit(120);
    m_window.setKeyRepeatEnabled(false);

    loadResources();

    m_camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_physics.createPlatform(WINDOW_WIDTH * 0.5f, PLATFORM_Y,
                             PLATFORM_WIDTH, PLATFORM_HEIGHT);

    m_crane.create(WINDOW_WIDTH * 0.5f,
                   m_camera.getTopY() + CRANE_ANCHOR_Y_OFFSET,
                   CRANE_ARM_LENGTH, m_hookTexture);

    m_groundViewTop = m_camera.getTopY();
    spawnNextBlock();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Resources
// ─────────────────────────────────────────────────────────────────────────────
void Game::loadResources() {
    if (!m_blockTexture.loadFromMemory(ASSET_BLOCK_PNG, ASSET_BLOCK_PNG_SIZE))
        std::cerr << "Failed to load block texture\n";
    m_blockTexture.setSmooth(true);

    if (!m_hookTexture.loadFromMemory(ASSET_HOOK_PNG, ASSET_HOOK_PNG_SIZE))
        std::cerr << "Failed to load hook texture\n";
    m_hookTexture.setSmooth(true);

    if (!m_bgTexture.loadFromMemory(ASSET_BACKGROUND_PNG, ASSET_BACKGROUND_PNG_SIZE))
        std::cerr << "Failed to load background texture\n";
    m_bgSprite.setTexture(m_bgTexture);
    {
        // The backdrop is the mansion the tower is built on, so scale it until
        // its roof is the width of the platform and line that roof up with the
        // platform itself. Hard-coded screen coordinates used to work only
        // because the camera happened to start unscrolled.
        const sf::Vector2u bg = m_bgTexture.getSize();
        if (bg.x > 0 && bg.y > 0) {
            const float scale = PLATFORM_WIDTH / (bg.x * BG_ROOF_WIDTH_FRAC);
            m_bgSprite.setScale(scale, scale);
            m_bgSprite.setPosition(
                WINDOW_WIDTH * 0.5f - bg.x * scale * 0.5f,
                (PLATFORM_Y - PLATFORM_HEIGHT * 0.5f) - bg.y * scale * BG_ROOF_Y_FRAC);
        }
    }

    // The sky is drawn as a two-colour gradient that deepens with altitude.
    // It replaces a 6016x4016 JPEG that cost ~96 MB of texture memory and 5 MB
    // of executable to fill a 400x600 window, and it doubles as a height cue.
    m_sky.setPrimitiveType(sf::Quads);
    m_sky.resize(4);

    if (!m_font.loadFromMemory(ASSET_DOODLEJUMP_TTF, ASSET_DOODLEJUMP_TTF_SIZE))
        std::cerr << "Failed to load font\n";

    auto styleText = [this](sf::Text& t, unsigned size, sf::Color fill) {
        t.setFont(m_font);
        t.setCharacterSize(size);
        t.setFillColor(fill);
        t.setOutlineColor(sf::Color(10, 10, 16, 220));
        t.setOutlineThickness(2.0f);
    };

    styleText(m_scoreText,   24, sf::Color::White);
    styleText(m_floorText,   18, sf::Color(190, 220, 255));
    styleText(m_messageText, 34, sf::Color::White);
    styleText(m_hintText,    16, sf::Color(220, 220, 230));
    styleText(m_meterLabel,  13, sf::Color(220, 220, 230));

    m_meterBack.setSize({ 110.0f, 12.0f });
    m_meterBack.setFillColor(sf::Color(0, 0, 0, 130));
    m_meterBack.setOutlineColor(sf::Color(240, 240, 240, 180));
    m_meterBack.setOutlineThickness(1.5f);
    m_meterFill.setSize({ 0.0f, 12.0f });

    // Window icon, cropped from the city backdrop.
    sf::Image icon;
    if (icon.loadFromMemory(ASSET_BACKGROUND_PNG, ASSET_BACKGROUND_PNG_SIZE)) {
        const auto size = icon.getSize();
        const unsigned crop = static_cast<unsigned>(size.x * 0.7f);
        sf::Image cropped;
        cropped.create(crop, crop, sf::Color::Transparent);
        cropped.copy(icon, 0, 0,
                     sf::IntRect((size.x - crop) / 2, (size.y - crop) / 3,
                                 crop, crop));
        m_window.setIcon(crop, crop, cropped.getPixelsPtr());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Block lifecycle
// ─────────────────────────────────────────────────────────────────────────────
void Game::spawnNextBlock() {
    // The block rides the rope, so it sits half a block beyond the hook and
    // shares the rope's angle - a rigid pendulum, as requested.
    const sf::Vector2f pos = m_crane.pointOnArm(BLOCK_SIZE * 0.5f);

    Block& block = m_tower.spawn(m_physics.getWorld(), pos.x, pos.y,
                                 BLOCK_SIZE, m_blockTexture);
    block.driveTo(toMeters(pos), m_crane.getBodyAngle());
    block.savePrevious();

    // Difficulty: the swing widens as the building climbs.
    m_crane.setAmplitude(std::min(CRANE_START_AMPLITUDE
                                  + m_floors * CRANE_AMPLITUDE_STEP,
                                  CRANE_MAX_AMPLITUDE));

    m_physics.listener().clear();
    m_state = State::Swinging;
}

void Game::dropBlock() {
    Block* active = m_tower.getActive();
    if (!active || active->getMode() != Block::Mode::Attached) return;

    active->setMode(Block::Mode::Falling);

    // Release with the exact tangential velocity of the point it was riding:
    // the block continues the arc it was already on, so the throw reads true.
    const sf::Vector2f vPx = m_crane.velocityOnArm(BLOCK_SIZE * 0.5f);
    active->getBody()->SetLinearVelocity(toMeters(vPx));
    active->getBody()->SetAngularVelocity(m_crane.getBodyAngularVelocity());

    m_physics.listener().watch(active->getBody());
    m_state = State::Falling;

    LOG_INFO("Drop: angle=%.3frad v=(%.2f, %.2f) m/s", m_crane.getAngle(),
             vPx.x / PPM, vPx.y / PPM);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Landing: "click upright" against the block below
// ─────────────────────────────────────────────────────────────────────────────
void Game::resolveLanding() {
    Block* active = m_tower.getActive();
    if (!active) return;

    const float blockW = BLOCK_SIZE / PPM;

    // Measure against what the player is actually aiming at: the *swayed*
    // position of the top of the building (or the platform, for floor one).
    float supportX    = WINDOW_WIDTH * 0.5f / PPM;
    float supportBaseX = supportX;
    float baseY        = (PLATFORM_Y - PLATFORM_HEIGHT * 0.5f) / PPM - blockW * 0.5f;

    if (Block* support = m_tower.getTopSettled()) {
        supportX     = support->getPosition().x;
        supportBaseX = support->getBase().x;
        baseY        = support->getBase().y - blockW;
    }

    const float dx     = active->getPosition().x - supportX;
    const float offset = std::abs(dx) / blockW;
    m_lastOffset = offset;

    // ── Too far out: the block does not take, the building goes over ────────
    if (offset > STACK_FAIL_OFFSET) {
        LOG_INFO("Missed landing (offset %.2f > %.2f)", offset, STACK_FAIL_OFFSET);
        triggerCollapse("MISSED!");
        return;
    }

    // ── Click it upright, by an amount that falls off with the miss ─────────
    // offset 0        -> snapped dead centre and perfectly level
    // offset >= LIMIT -> left exactly where it landed, leaning the column
    const float t          = clampf(1.0f - offset / STACK_SNAP_LIMIT, 0.0f, 1.0f);
    const float residualDx = dx * (1.0f - t);

    const b2Vec2 base(supportBaseX + residualDx, baseY);

    m_tower.settleTop(base);
    m_tower.registerPlacement(offset);
    m_tower.applySwayTransforms();   // place it on the column immediately
    active->savePrevious();

    m_physics.listener().clear();

    // ── Score ───────────────────────────────────────────────────────────────
    int gained;
    if (offset <= STACK_PERFECT_OFFSET) {
        ++m_combo;
        gained = 100 + 25 * std::min(m_combo - 1, 8);
        m_perfectFlash = true;
        m_flashTimer   = 0.9f;
    } else {
        m_combo = 0;
        gained  = static_cast<int>(std::max(0.0f, (1.0f - offset * offset) * 100.0f));
        m_perfectFlash = false;
    }
    m_score += gained;
    ++m_floors;

    // ── Has the building given up? ──────────────────────────────────────────
    if (m_tower.getInstability() >= 0.995f) {
        triggerCollapse("TOO WOBBLY!");
        return;
    }
    if (m_tower.getLeanRatio() > TOWER_MAX_DRIFT) {
        triggerCollapse("TOO LOPSIDED!");
        return;
    }

    m_settleTimer = STACK_SETTLE_DELAY;
    m_state       = State::Settling;
}

void Game::triggerCollapse(const char* reason) {
    LOG_INFO("Collapse: %s", reason);
    m_messageText.setString(reason);
    m_tower.collapse();
    m_physics.listener().clear();
    m_collapseTimer = 1.6f;
    m_state         = State::Collapsing;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Fixed-rate update - all game logic lives here
// ─────────────────────────────────────────────────────────────────────────────
void Game::fixedUpdate(float dt) {
    m_tower.savePrevious();
    m_camera.savePrevious();

    if (m_flashTimer > 0.0f) m_flashTimer -= dt;

    // The pivot rides the camera. Because the crane is not a physics body this
    // costs nothing and, crucially, never disturbs the solver - the old code
    // teleported three jointed bodies every single frame.
    m_crane.setAnchor(WINDOW_WIDTH * 0.5f,
                      m_camera.getTopY() + CRANE_ANCHOR_Y_OFFSET);

    if (m_state == State::Swinging || m_state == State::Falling ||
        m_state == State::Settling) {
        m_crane.update(dt);
    }
    m_tower.updateSway(dt);

    // ── Pre-step: publish kinematic velocities so contacts behave ───────────
    if (m_state == State::Swinging) {
        if (Block* active = m_tower.getActive()) {
            const sf::Vector2f vPx = m_crane.velocityOnArm(BLOCK_SIZE * 0.5f);
            active->getBody()->SetLinearVelocity(toMeters(vPx));
            active->getBody()->SetAngularVelocity(m_crane.getBodyAngularVelocity());
        }
    }
    m_tower.applySwayVelocities();

    m_physics.step(dt);

    // ── Post-step: pin the kinematic bodies to their exact scripted pose ────
    if (m_state == State::Swinging) {
        if (Block* active = m_tower.getActive()) {
            active->driveTo(toMeters(m_crane.pointOnArm(BLOCK_SIZE * 0.5f)),
                            m_crane.getBodyAngle());
        }
    }
    m_tower.applySwayTransforms();

    // ── State machine ───────────────────────────────────────────────────────
    switch (m_state) {
    case State::Falling: {
        Block* active = m_tower.getActive();
        if (!active) break;

        if (active->getPosition().y * PPM > m_camera.getBottomY() + 200.0f) {
            LOG_INFO("Block fell past the bottom of the view");
            m_messageText.setString("DROPPED IT!");
            m_physics.listener().clear();
            m_collapseTimer = 1.2f;
            m_state         = State::Collapsing;
            break;
        }
        if (m_physics.listener().hasTouched()) {
            resolveLanding();
        }
        break;
    }

    case State::Settling:
        m_settleTimer -= dt;
        if (m_settleTimer <= 0.0f) spawnNextBlock();
        break;

    case State::Collapsing:
        m_collapseTimer -= dt;
        if (m_collapseTimer <= 0.0f) m_state = State::GameOver;
        break;

    default:
        break;
    }

    // ── Camera & housekeeping ───────────────────────────────────────────────
    if (m_state != State::Collapsing && m_state != State::GameOver) {
        m_camera.update(m_tower.getTopYPx(), dt);
    }
    m_tower.updateDormancy(m_physics.getWorld(), m_camera.getTopY());
}

// ─────────────────────────────────────────────────────────────────────────────
//  Main loop - fixed simulation, interpolated presentation
// ─────────────────────────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_window.isOpen()) {
        float frameDt = clock.restart().asSeconds();
        if (frameDt > MAX_FRAME_DT) frameDt = MAX_FRAME_DT;

        handleEvents();

        accumulator += frameDt;
        int steps = 0;
        while (accumulator >= FIXED_DT && steps < MAX_STEPS_FRAME) {
            fixedUpdate(FIXED_DT);
            accumulator -= FIXED_DT;
            ++steps;
        }
        if (steps == MAX_STEPS_FRAME) accumulator = 0.0f;   // give up the debt

        render(accumulator / FIXED_DT);
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }
        if (event.type != sf::Event::KeyPressed) continue;

        switch (event.key.code) {
        case sf::Keyboard::Escape:
            m_window.close();
            break;

        case sf::Keyboard::Space:
            if (m_state == State::Swinging)       dropBlock();
            else if (m_state == State::GameOver)  restart();
            break;

        case sf::Keyboard::R:
        case sf::Keyboard::Enter:
            if (m_state == State::GameOver) restart();
            break;

        default:
            break;
        }
    }
}

void Game::restart() {
    LOG_INFO("Restarting. Final score %d over %d floors", m_score, m_floors);

    // Tear every body out of the world, then rebuild from scratch.
    b2World& world = m_physics.getWorld();
    for (auto& block : m_tower.getBlocks()) {
        if (block.getBody()) world.DestroyBody(block.getBody());
    }
    m_tower.clear();

    m_camera.reset(PLATFORM_Y - PLATFORM_HEIGHT * 0.5f);
    m_crane.setAnchor(WINDOW_WIDTH * 0.5f,
                      m_camera.getTopY() + CRANE_ANCHOR_Y_OFFSET);
    m_crane.reset(CRANE_START_AMPLITUDE);

    m_score        = 0;
    m_combo        = 0;
    m_floors       = 0;
    m_lastOffset   = 0.0f;
    m_flashTimer   = 0.0f;
    m_perfectFlash = false;
    m_messageText.setString("");

    spawnNextBlock();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Rendering
// ─────────────────────────────────────────────────────────────────────────────
void Game::render(float alpha) {
    alpha = clampf(alpha, 0.0f, 1.0f);

    m_crane.syncSprite(alpha);
    m_tower.syncSprites(alpha);

    const float viewTop = m_camera.getRenderTopY(alpha);

    // Sky first, in screen space: a gradient that deepens the higher you build.
    const float altitude = clampf((m_groundViewTop - viewTop) / SKY_FADE_HEIGHT,
                                  0.0f, 1.0f);
    auto mix = [](const sf::Color& a, const sf::Color& b, float t) {
        return sf::Color(
            static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
            static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
            static_cast<sf::Uint8>(a.b + (b.b - a.b) * t));
    };
    const sf::Color top = mix(SKY_LOW_TOP,    SKY_HIGH_TOP,    altitude);
    const sf::Color bot = mix(SKY_LOW_BOTTOM, SKY_HIGH_BOTTOM, altitude);

    m_sky[0] = sf::Vertex({ 0.0f,          0.0f          }, top);
    m_sky[1] = sf::Vertex({ WINDOW_WIDTH,  0.0f          }, top);
    m_sky[2] = sf::Vertex({ WINDOW_WIDTH,  WINDOW_HEIGHT }, bot);
    m_sky[3] = sf::Vertex({ 0.0f,          WINDOW_HEIGHT }, bot);

    m_window.clear();
    m_window.setView(m_window.getDefaultView());
    m_window.draw(m_sky);

    m_window.setView(m_camera.getView(alpha));
    m_window.draw(m_bgSprite);

    // Crane: only while it is actually holding or waiting for a block.
    if (m_state != State::Collapsing && m_state != State::GameOver) {
        m_window.draw(m_crane.getWireSprite());
        m_window.draw(m_crane.getHookSprite());
    }

    const sf::FloatRect bounds = m_camera.getViewBounds(alpha);
    for (auto& block : m_tower.getBlocks()) {
        const sf::Vector2f p = block.getSprite().getPosition();
        if (p.y > bounds.top && p.y < bounds.top + bounds.height) {
            m_window.draw(block.getSprite());
        }
    }

    m_window.setView(m_window.getDefaultView());
    drawHud();
    m_window.display();
}

void Game::drawHud() {
    m_scoreText.setString(std::to_string(m_score));
    m_scoreText.setPosition(12.0f, 8.0f);
    m_window.draw(m_scoreText);

    m_floorText.setString("Floor " + std::to_string(m_floors));
    m_floorText.setPosition(12.0f, 38.0f);
    m_window.draw(m_floorText);

    // ── Stability meter: the feedback loop the whole game hangs on ──────────
    const float inst = clampf(m_tower.getInstability(), 0.0f, 1.0f);
    const float x    = WINDOW_WIDTH - 122.0f;

    m_meterLabel.setString("SWAY");
    m_meterLabel.setPosition(x, 8.0f);
    m_window.draw(m_meterLabel);

    m_meterBack.setPosition(x, 28.0f);
    m_window.draw(m_meterBack);

    m_meterFill.setSize({ 110.0f * inst, 12.0f });
    m_meterFill.setPosition(x, 28.0f);
    m_meterFill.setFillColor(sf::Color(static_cast<sf::Uint8>(60 + 195 * inst),
                                       static_cast<sf::Uint8>(220 - 190 * inst),
                                       90));
    m_window.draw(m_meterFill);

    if (m_flashTimer > 0.0f && m_perfectFlash) {
        const sf::Uint8 a = static_cast<sf::Uint8>(255.0f * clampf(m_flashTimer / 0.9f, 0.0f, 1.0f));
        sf::Text perfect = m_floorText;
        perfect.setCharacterSize(26);
        perfect.setString(m_combo > 1 ? "PERFECT x" + std::to_string(m_combo)
                                      : "PERFECT!");
        perfect.setFillColor(sf::Color(255, 230, 120, a));
        perfect.setOutlineColor(sf::Color(10, 10, 16, a));
        const sf::FloatRect b = perfect.getLocalBounds();
        perfect.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
        perfect.setPosition(WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.30f);
        m_window.draw(perfect);
    }

    if (m_state == State::GameOver) {
        sf::RectangleShape veil({ WINDOW_WIDTH, WINDOW_HEIGHT });
        veil.setFillColor(sf::Color(0, 0, 0, 140));
        m_window.draw(veil);

        auto centreAt = [this](sf::Text& t, float y) {
            const sf::FloatRect b = t.getLocalBounds();
            t.setOrigin(b.left + b.width * 0.5f, b.top + b.height * 0.5f);
            t.setPosition(WINDOW_WIDTH * 0.5f, y);
            m_window.draw(t);
        };

        sf::Text over = m_messageText;
        over.setString("GAME OVER");
        over.setFillColor(sf::Color(255, 90, 90));
        centreAt(over, WINDOW_HEIGHT * 0.38f);

        sf::Text reason = m_hintText;
        reason.setCharacterSize(20);
        reason.setString(m_messageText.getString());
        centreAt(reason, WINDOW_HEIGHT * 0.47f);

        sf::Text summary = m_hintText;
        summary.setCharacterSize(20);
        summary.setString(std::to_string(m_floors) + " floors  -  "
                          + std::to_string(m_score) + " points");
        centreAt(summary, WINDOW_HEIGHT * 0.55f);

        sf::Text hint = m_hintText;
        hint.setString("SPACE / R to rebuild");
        centreAt(hint, WINDOW_HEIGHT * 0.63f);
    }
}
