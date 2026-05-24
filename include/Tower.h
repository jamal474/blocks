#pragma once
#include "Block.h"
#include <vector>
#include <memory>

/// Manages the tower of stacked blocks.
class Tower {
public:
    Tower() = default;

    /// Spawn a new block at the given pixel position and attach it conceptually.
    Block& spawnBlock(b2World& world, float x, float y,
                      float sizePx, sf::Texture& texture);

    /// Calculate alignment score (0–100) between the last two blocks.
    float calculateAlignment() const;

    /// Returns the Y position (in pixels) of the topmost block.
    float getTopY() const;

    /// Freeze bodies far below the camera for performance.
    void freezeDistantBodies(float cameraTopYPx);

    /// Sync all block sprites from physics.
    void syncSprites();

    /// Get all blocks for rendering.
    std::vector<Block>& getBlocks() { return m_blocks; }
    const std::vector<Block>& getBlocks() const { return m_blocks; }

    int getBlockCount() const { return static_cast<int>(m_blocks.size()); }

    /// Check if the latest dropped block has fallen off screen.
    bool isTopBlockFallen(float bottomYPx) const;

private:
    std::vector<Block> m_blocks;
};
