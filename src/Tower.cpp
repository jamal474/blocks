#include "Tower.h"
#include "Constants.h"
#include "BlockLogger.h"
#include <algorithm>
#include <cmath>

Block& Tower::spawn(b2World& world, float xPx, float yPx, float sizePx,
                    sf::Texture& texture) {
    m_blocks.emplace_back();
    Block& block = m_blocks.back();
    block.create(world, xPx, yPx, sizePx, texture);
    block.setMode(Block::Mode::Attached);
    return block;
}

Block* Tower::getActive() {
    if (m_blocks.empty()) return nullptr;
    Block& back = m_blocks.back();
    if (back.getMode() == Block::Mode::Attached ||
        back.getMode() == Block::Mode::Falling) {
        return &back;
    }
    return nullptr;
}

Block* Tower::getTopSettled() {
    if (m_settledCount == 0) return nullptr;
    return &m_blocks[m_settledCount - 1];
}

const Block* Tower::getTopSettled() const {
    if (m_settledCount == 0) return nullptr;
    return &m_blocks[m_settledCount - 1];
}

void Tower::settleTop(const b2Vec2& baseMeters) {
    if (m_blocks.empty()) return;

    Block& block = m_blocks.back();
    block.setBase(baseMeters);
    block.setHeightIndex(m_settledCount);
    block.setMode(Block::Mode::Settled);

    if (m_settledCount == 0) {
        m_baseBottomY = baseMeters.y;
        m_baseBottomX = baseMeters.x;
    }
    ++m_settledCount;
}

void Tower::registerPlacement(float offset) {
    // Perfect drops pay the building back; sloppy ones wind it up.
    //
    // The response is deliberately curved rather than linear: a slightly-off
    // drop should barely register, while a genuinely bad one should visibly
    // shake the building. A straight line made a single sloppy drop almost
    // fatal, which left the player no room to recover.
    float delta;
    if (offset <= STACK_PERFECT_OFFSET) {
        delta = -SWAY_RECOVER_SCALE * (1.0f - offset / STACK_PERFECT_OFFSET);
    } else {
        const float miss = (offset - STACK_PERFECT_OFFSET)
                         / (1.0f - STACK_PERFECT_OFFSET);
        delta = SWAY_GAIN * std::pow(clampf(miss, 0.0f, 1.0f), SWAY_CURVE);
    }

    m_instability = clampf(m_instability + delta, 0.0f, 1.0f);

    // Every landing gives the building a transient shove, bigger when clumsy.
    m_kick += SWAY_LANDING_KICK * (0.35f + offset);

    LOG_INFO("Placement offset=%.3f -> instability=%.3f (delta=%+.3f)",
             offset, m_instability, delta);
}

float Tower::towerHeightPx() const {
    if (m_settledCount <= 1) return BLOCK_SIZE;
    return (m_settledCount - 1) * BLOCK_SIZE;
}

float Tower::normalisedHeight(int heightIndex) const {
    if (m_settledCount <= 1) return 0.0f;
    // Normalising by a *smoothed* span means adding a floor slides the profile
    // instead of instantly re-scaling every block's offset (a visible pop).
    return clampf(static_cast<float>(heightIndex)
                  / std::max(m_spanSmoothed, 1.0f), 0.0f, 1.0f);
}

float Tower::swayOffsetPx(float h) const {
    return m_swayAmpPx * std::pow(clampf(h, 0.0f, 1.0f), SWAY_SHAPE_EXPONENT)
         * m_swaySin;
}

void Tower::updateSway(float dt) {
    const float span = std::max(1.0f, static_cast<float>(m_settledCount - 1));
    m_spanSmoothed += (span - m_spanSmoothed) * smoothFactor(6.0f, dt);

    // Transient landing shove decays away; the building also slowly settles.
    m_kick = std::max(0.0f, m_kick - m_kick * SWAY_KICK_DECAY * dt);
    m_instability = std::max(0.0f, m_instability - SWAY_PASSIVE_DECAY * dt);

    const float height = towerHeightPx();

    // Taller buildings sway further and slower - like the real thing.
    const float energy = clampf(m_instability + m_kick, 0.0f, 1.45f);
    const float reach  = SWAY_MAX_PIXELS + height * SWAY_HEIGHT_FACTOR;
    const float target = reach * energy;

    // Ease the amplitude instead of snapping it, so a perfect drop visibly
    // *calms* the building over half a second rather than freezing it.
    m_swayAmpPx += (target - m_swayAmpPx) * smoothFactor(3.0f, dt);

    const float period = SWAY_BASE_PERIOD + height * SWAY_PERIOD_PER_PX;
    m_omega = (2.0f * b2_pi) / std::max(period, 0.2f);

    m_phase += m_omega * dt;
    if (m_phase > 2.0f * b2_pi) m_phase -= 2.0f * b2_pi;

    m_swaySin = std::sin(m_phase);
    m_swayCos = std::cos(m_phase);
}

void Tower::applySwayVelocities() {
    if (m_settledCount == 0) return;

    const float height = towerHeightPx();
    // d/dt of the sway displacement, in metres/second.
    const float rate = m_swayAmpPx * m_omega * m_swayCos / PPM;

    for (int i = m_firstAwake; i < m_settledCount; ++i) {
        Block& b = m_blocks[i];
        if (b.getMode() != Block::Mode::Settled) continue;

        const float h     = normalisedHeight(b.getHeightIndex());
        const float shape = std::pow(h, SWAY_SHAPE_EXPONENT);
        b.getBody()->SetLinearVelocity(b2Vec2(rate * shape, 0.0f));
    }
    (void)height;
}

void Tower::applySwayTransforms() {
    if (m_settledCount == 0) return;

    const float height = std::max(towerHeightPx(), 1.0f);

    for (int i = m_firstAwake; i < m_settledCount; ++i) {
        Block& b = m_blocks[i];
        if (b.getMode() != Block::Mode::Settled) continue;

        const float h     = normalisedHeight(b.getHeightIndex());
        const float shape = std::pow(h, SWAY_SHAPE_EXPONENT);
        const float dxPx  = m_swayAmpPx * shape * m_swaySin;

        // Tilt each block along the tangent of the bent column so the tower
        // reads as one flexing structure instead of a sliding pile.
        float slope = 0.0f;
        if (h > 0.0f) {
            slope = m_swayAmpPx * SWAY_SHAPE_EXPONENT
                  * std::pow(h, SWAY_SHAPE_EXPONENT - 1.0f) * m_swaySin / height;
        }
        const float angle = std::atan(slope);

        const b2Vec2 base = b.getBase();
        b.driveTo(b2Vec2(base.x + dxPx / PPM, base.y), angle);
    }
}

void Tower::collapse() {
    LOG_INFO("Tower collapsing: %d blocks, instability=%.2f",
             m_settledCount, m_instability);

    const float dir = (m_swaySin >= 0.0f) ? 1.0f : -1.0f;

    for (int i = m_firstAwake; i < static_cast<int>(m_blocks.size()); ++i) {
        Block& b = m_blocks[i];
        b2Body* body = b.getBody();
        if (!body) continue;              // already retired, far below the view

        const float h = normalisedHeight(b.getHeightIndex());
        b.setMode(Block::Mode::Collapsing);
        body->SetLinearVelocity(b2Vec2(dir * (1.0f + 5.0f * h), 0.0f));
        body->SetAngularVelocity(dir * (0.6f + 2.2f * h));
    }
    m_settledCount = 0;
}

void Tower::savePrevious() {
    for (int i = m_firstAwake; i < static_cast<int>(m_blocks.size()); ++i) {
        m_blocks[i].savePrevious();
    }
}

void Tower::syncSprites(float alpha) {
    for (int i = m_firstAwake; i < static_cast<int>(m_blocks.size()); ++i) {
        m_blocks[i].syncSprite(alpha);
    }
}

void Tower::updateDormancy(b2World& world, float viewTopPx) {
    const float threshold = viewTopPx + WINDOW_HEIGHT + SLEEP_DISTANCE;

    // Retire blocks off the bottom of the world. They keep their last drawn
    // pose; nothing can reach them again because the camera never descends.
    while (m_firstAwake < m_settledCount) {
        Block& b = m_blocks[m_firstAwake];
        if (b.getMode() != Block::Mode::Settled) break;
        if (b.getPosition().y * PPM <= threshold) break;
        b.retire(world);
        ++m_firstAwake;
    }
}

float Tower::getTopYPx() const {
    const Block* top = getTopSettled();
    if (!top) return PLATFORM_Y - PLATFORM_HEIGHT * 0.5f;
    return top->getBase().y * PPM - BLOCK_SIZE * 0.5f;
}

float Tower::getLeanRatio() const {
    const Block* top = getTopSettled();
    if (!top || m_settledCount < 2) return 0.0f;
    const float driftM = std::abs(top->getBase().x - m_baseBottomX);
    return driftM / (BLOCK_SIZE / PPM);
}

void Tower::clear() {
    m_blocks.clear();
    m_settledCount = 0;
    m_firstAwake   = 0;
    m_phase        = 0.0f;
    m_instability  = 0.0f;
    m_kick         = 0.0f;
    m_swayAmpPx    = 0.0f;
    m_swaySin      = 0.0f;
    m_swayCos      = 1.0f;
    m_spanSmoothed = 1.0f;
}
