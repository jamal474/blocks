#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <box2d/box2d.h>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
//  Window
// ─────────────────────────────────────────────────────────────────────────────
constexpr float WINDOW_WIDTH  = 400.0f;
constexpr float WINDOW_HEIGHT = 600.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  Pixels Per Meter (Box2D <-> SFML bridge)
// ─────────────────────────────────────────────────────────────────────────────
constexpr float PPM = 30.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  Fixed timestep
//  Every piece of game logic runs inside the fixed step so that behaviour is
//  frame-rate independent and deterministic. Rendering interpolates between
//  the previous and current step using `alpha`.
// ─────────────────────────────────────────────────────────────────────────────
constexpr float FIXED_DT        = 1.0f / 120.0f;  // 120 Hz simulation
constexpr int   VELOCITY_ITERS  = 8;
constexpr int   POSITION_ITERS  = 3;
constexpr int   MAX_STEPS_FRAME = 8;              // anti "spiral of death"
constexpr float MAX_FRAME_DT    = 0.25f;

// ─────────────────────────────────────────────────────────────────────────────
//  Block
// ─────────────────────────────────────────────────────────────────────────────
constexpr float BLOCK_SIZE        = 50.0f;   // pixels (square)
constexpr float BLOCK_DENSITY     = 4.0f;
constexpr float BLOCK_FRICTION    = 0.85f;
constexpr float BLOCK_RESTITUTION = 0.0f;    // no bounce: crisp landings
constexpr float BLOCK_ANG_DAMPING = 1.2f;    // calms free-fall tumbling

// Collision filtering: the block riding the hook must not shove the tower.
constexpr uint16 CAT_TOWER   = 0x0002;
constexpr uint16 CAT_FALLING = 0x0004;
constexpr uint16 CAT_GHOST   = 0x0008;  // attached block: collides with nothing

// ─────────────────────────────────────────────────────────────────────────────
//  Crane (scripted pendulum - not a Box2D body)
//  theta = 0 points straight down; +theta swings to the right (screen +X).
// ─────────────────────────────────────────────────────────────────────────────
constexpr float CRANE_ANCHOR_Y_OFFSET = 44.0f;   // px below the top of the view
constexpr float CRANE_ARM_LENGTH      = 100.0f;  // px, anchor -> hook
constexpr float CRANE_GRAVITY         = 30.0f;   // m/s^2 used by the pendulum
                                                 // (tuned so a full swing takes
                                                 //  ~2s - feel, not realism)
constexpr float CRANE_START_AMPLITUDE = 0.32f;   // radians (~18 deg)
constexpr float CRANE_MAX_AMPLITUDE   = 0.62f;   // radians (~36 deg)
constexpr float CRANE_AMPLITUDE_STEP  = 0.014f;  // added per successful drop
constexpr float CRANE_ENERGY_CORRECT  = 0.15f;   // per-step amplitude lock [0..1]

// ─────────────────────────────────────────────────────────────────────────────
//  Platform
// ─────────────────────────────────────────────────────────────────────────────
constexpr float PLATFORM_WIDTH  = 170.0f;
constexpr float PLATFORM_HEIGHT = 16.0f;
constexpr float PLATFORM_Y      = WINDOW_HEIGHT - 70.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  Stacking / "click upright" rules
//  `offset` below is always |dx| / blockWidth between the landing block and the
//  block it lands on (0 = perfect, 1 = a whole block out).
// ─────────────────────────────────────────────────────────────────────────────
constexpr float STACK_PERFECT_OFFSET = 0.12f;  // <= this counts as a perfect hit
constexpr float STACK_SNAP_LIMIT     = 0.55f;  // full snap at 0, none at this
constexpr float STACK_FAIL_OFFSET    = 0.85f;  // too little overlap to hold
constexpr float STACK_SETTLE_DELAY   = 0.12f;  // seconds before the next block

// ─────────────────────────────────────────────────────────────────────────────
//  Tower sway
//  The settled tower is a driven kinematic column: a bending-beam shape whose
//  amplitude is fed by how accurately the player stacks. Perfect drops calm it
//  down, sloppy drops wind it up. At instability 1.0 the building collapses.
// ─────────────────────────────────────────────────────────────────────────────
constexpr float SWAY_GAIN            = 0.55f;  // instability added by a full miss
constexpr float SWAY_CURVE           = 1.60f;  // >1: small misses cost little,
                                               // big ones cost a lot
constexpr float SWAY_RECOVER_SCALE   = 0.18f;  // instability repaid by a perfect
constexpr float SWAY_PASSIVE_DECAY   = 0.012f; // instability lost per second
constexpr float SWAY_MAX_PIXELS      = 46.0f;  // top-of-tower sway at full inst.
constexpr float SWAY_HEIGHT_FACTOR   = 0.11f;  // sway also grows with height
constexpr float SWAY_BASE_PERIOD     = 1.35f;  // seconds, short tower
constexpr float SWAY_PERIOD_PER_PX   = 0.0016f;// taller buildings sway slower
constexpr float SWAY_SHAPE_EXPONENT  = 1.7f;   // cantilever bend profile
constexpr float SWAY_LANDING_KICK    = 0.22f;  // transient shove on impact
constexpr float SWAY_KICK_DECAY      = 2.4f;   // per second
constexpr float TOWER_MAX_DRIFT      = 1.35f;  // block widths of lean allowed

// ─────────────────────────────────────────────────────────────────────────────
//  Backdrop & sky
//  The backdrop art is a mansion under a skyline; these fractions locate its
//  roof in the source image so the platform can be lined up with it.
// ─────────────────────────────────────────────────────────────────────────────
constexpr float BG_ROOF_WIDTH_FRAC = 0.313f;  // roof width / image width
constexpr float BG_ROOF_Y_FRAC     = 0.352f;  // roof top   / image height

constexpr float SKY_FADE_HEIGHT = 3000.0f;    // px climbed to reach full night
const sf::Color SKY_LOW_TOP    (108, 192, 246);
const sf::Color SKY_LOW_BOTTOM (198, 232, 250);
const sf::Color SKY_HIGH_TOP   ( 12,  16,  48);
const sf::Color SKY_HIGH_BOTTOM( 64,  86, 156);

// ─────────────────────────────────────────────────────────────────────────────
//  Camera
// ─────────────────────────────────────────────────────────────────────────────
constexpr float CAMERA_SMOOTH_RATE = 4.5f;   // exponential rate (1/s)
constexpr float CAMERA_TOP_MARGIN  = 0.40f;  // keep the tower top this far down
constexpr float CULL_PADDING       = 120.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  Physics world
// ─────────────────────────────────────────────────────────────────────────────
constexpr float GRAVITY_X = 0.0f;
constexpr float GRAVITY_Y = 26.0f;   // snappier than 9.81: arcade fall speed

// Blocks this far below the visible top stop having their transform written.
constexpr float SLEEP_DISTANCE = 900.0f;

// ─────────────────────────────────────────────────────────────────────────────
//  Conversions
// ─────────────────────────────────────────────────────────────────────────────
inline sf::Vector2f toPixels(const b2Vec2& v) { return { v.x * PPM, v.y * PPM }; }
inline b2Vec2 toMeters(const sf::Vector2f& v) { return { v.x / PPM, v.y / PPM }; }
inline float  toMeters(float px)  { return px / PPM; }
inline float  toPixels(float m)   { return m * PPM; }
inline float  toDegrees(float r)  { return r * (180.0f / b2_pi); }
inline float  toRadians(float d)  { return d * (b2_pi / 180.0f); }

/// Frame-rate independent exponential smoothing factor.
inline float smoothFactor(float rate, float dt) {
    return 1.0f - std::exp(-rate * dt);
}

inline float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

/// Shortest-arc interpolation between two angles (radians).
inline float lerpAngle(float a, float b, float t) {
    float diff = std::fmod(b - a + b2_pi * 3.0f, b2_pi * 2.0f) - b2_pi;
    return a + diff * t;
}
