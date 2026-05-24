#include "Tower.h"
#include "Constants.h"
#include <cmath>
#include <algorithm>

Block& Tower::spawnBlock(b2World& world, float x, float y,
                          float sizePx, sf::Texture& texture) {
    m_blocks.emplace_back();
    Block& block = m_blocks.back();
    block.create(world, x, y, sizePx, texture);
    return block;
}

float Tower::calculateAlignment() const {
    if (m_blocks.size() < 2) return 100.0f;

    const Block& current  = m_blocks[m_blocks.size() - 1];
    const Block& previous = m_blocks[m_blocks.size() - 2];

    float dx = std::abs(current.getBody()->GetPosition().x
                      - previous.getBody()->GetPosition().x);
    float blockWidth = current.getSize() / PPM;

    // Normalise offset: 0 = perfect, 1 = completely off
    float offset = dx / blockWidth;

    if (offset > 1.0f) return 0.0f;  // complete miss

    // Quadratic falloff for satisfying scoring curve
    float score = (1.0f - offset * offset) * 100.0f;

    // Perfect bonus zone (< 5% offset)
    if (offset < 0.05f) score = 100.0f;

    return score;
}

float Tower::getTopY() const {
    if (m_blocks.size() < 2) return WINDOW_HEIGHT;

    float topY = WINDOW_HEIGHT;
    // Only check settled blocks (ignore the last block, which is active/swinging)
    for (size_t i = 0; i < m_blocks.size() - 1; ++i) {
        float y = m_blocks[i].getBody()->GetPosition().y * PPM;
        topY = std::min(topY, y);
    }
    return topY;
}

void Tower::freezeDistantBodies(float cameraTopYPx) {
    float freezeThreshold = cameraTopYPx + FREEZE_DISTANCE;
    for (auto& block : m_blocks) {
        float blockY = block.getBody()->GetPosition().y * PPM;
        if (blockY > freezeThreshold && !block.isFrozen()) {
            block.freeze();
        }
    }
}

void Tower::syncSprites() {
    for (auto& block : m_blocks) {
        if (!block.isFrozen()) {
            block.syncSprite();
        }
    }
}

bool Tower::isTopBlockFallen(float bottomYPx) const {
    if (m_blocks.empty()) return false;
    const Block& top = m_blocks.back();
    float y = top.getBody()->GetPosition().y * PPM;
    return y > bottomYPx + 100.0f;  // fell well below the viewport
}
