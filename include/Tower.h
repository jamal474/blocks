#pragma once
#include "Block.h"
#include <deque>

/// The stacked tower, modelled as a *driven* swaying column.
///
/// Settled blocks are kinematic. Every fixed step they are written to
///
///     x(h) = baseX(h) + A * h^k * sin(phase)
///
/// where `h` is the block's normalised height in the tower, `k` bends the
/// profile like a cantilever (the top moves far, the base barely at all), and
/// `A` - the amplitude - is driven by how well the player stacks:
///
///   * a near-perfect drop *reduces* instability, calming the building down;
///   * a sloppy drop *increases* it;
///   * at instability 1.0 the building goes over and the game ends.
///
/// Doing the sway this way (rather than hoping an emergent Box2D stack wobbles
/// nicely) makes it smooth, tunable, and impossible for the solver to explode.
/// Real physics is still used where it matters: the falling block, the landing
/// impact, and the final collapse.
class Tower {
public:
    Tower() = default;

    Block& spawn(b2World& world, float xPx, float yPx, float sizePx,
                 sf::Texture& texture);

    /// Promote the block that just landed into the swaying column.
    void settleTop(const b2Vec2& baseMeters);

    /// Feed the stacking accuracy into the sway. `offset` is |dx| / blockWidth.
    void registerPlacement(float offset);

    /// Advance the sway clock. Call once per fixed step.
    void updateSway(float dt);

    /// Write sway velocities onto the kinematic bodies (before the solver step)
    /// so a landing block is carried along by friction.
    void applySwayVelocities();

    /// Write the exact sway transforms (after the solver step).
    void applySwayTransforms();

    /// Convert the whole building to dynamic bodies and shove it over.
    void collapse();

    void savePrevious();
    void syncSprites(float alpha);

    /// Put blocks far below the camera to sleep (transform writes skipped).
    void updateDormancy(b2World& world, float viewTopPx);

    // ── Queries ─────────────────────────────────────────────────────────────
    std::deque<Block>&       getBlocks()       { return m_blocks; }
    const std::deque<Block>& getBlocks() const { return m_blocks; }

    int  getBlockCount() const   { return static_cast<int>(m_blocks.size()); }
    int  getSettledCount() const { return m_settledCount; }

    Block*       getTopSettled();
    const Block* getTopSettled() const;
    Block*       getActive();          // the block on the hook / in flight

    /// Y of the top of the settled column in pixels (for the camera).
    float getTopYPx() const;

    float getInstability() const    { return m_instability; }
    float getSwayAmplitude() const  { return m_swayAmpPx; }
    float getLeanRatio() const;      // |drift| in block widths

    /// Horizontal sway displacement (pixels) at a normalised height.
    float swayOffsetPx(float h) const;

    void clear();

private:
    float normalisedHeight(int heightIndex) const;
    float towerHeightPx() const;

    std::deque<Block> m_blocks;
    int   m_settledCount = 0;
    // Dormant blocks always form a prefix (the camera only ever climbs), so
    // every per-block loop can start here instead of at zero. Keeps the cost
    // of a step flat no matter how tall the building gets.
    int   m_firstAwake   = 0;

    float m_phase        = 0.0f;
    float m_instability  = 0.0f;
    float m_kick         = 0.0f;
    float m_swayAmpPx    = 0.0f;
    float m_swaySin      = 0.0f;
    float m_swayCos      = 1.0f;
    float m_omega        = 0.0f;

    float m_spanSmoothed = 1.0f;   // eased (settledCount-1): no pop on growth

    float m_baseBottomY  = 0.0f;   // metres, bottom block's neutral Y
    float m_baseBottomX  = 0.0f;
};
