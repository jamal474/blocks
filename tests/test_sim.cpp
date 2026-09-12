// Headless numeric checks for the pendulum and the sway model.
#include "Crane.h"
#include "Constants.h"
#include <cstdio>
#include <cmath>
#include <vector>

int failures = 0;
void check(bool ok, const char* what, double v = 0.0) {
    printf("%s %-58s %g\n", ok ? "  PASS" : "> FAIL", what, v);
    if (!ok) ++failures;
}

int main() {
    sf::Texture tex;

    // ── 1. Amplitude must be rock-stable over a long run ────────────────────
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.42f);

        float maxA = 0.0f, minA = 0.0f;
        // discard the first second, then measure 120 s of swinging
        for (int i = 0; i < static_cast<int>(1.0f / FIXED_DT); ++i) c.update(FIXED_DT);
        for (int i = 0; i < static_cast<int>(120.0f / FIXED_DT); ++i) {
            c.update(FIXED_DT);
            maxA = std::max(maxA, c.getAngle());
            minA = std::min(minA, c.getAngle());
        }
        check(std::fabs(maxA - 0.42f) < 0.004f, "amplitude right extreme holds at 0.42 rad", maxA);
        check(std::fabs(minA + 0.42f) < 0.004f, "amplitude left  extreme holds at -0.42 rad", minA);
        check(std::fabs(maxA + minA) < 0.004f, "swing is symmetric about vertical", maxA + minA);
    }

    // ── 2. Period must match the physical pendulum ──────────────────────────
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.42f);

        const float L = CRANE_ARM_LENGTH / PPM;
        // Small-angle period with the first-order large-angle correction.
        const float T0 = 2.0f * b2_pi * std::sqrt(L / CRANE_GRAVITY);
        const float Texp = T0 * (1.0f + 0.42f * 0.42f / 16.0f);

        int crossings = 0; float first = -1.0f, last = -1.0f;
        float prev = c.getAngle(); float t = 0.0f;
        for (int i = 0; i < static_cast<int>(60.0f / FIXED_DT); ++i) {
            c.update(FIXED_DT); t += FIXED_DT;
            const float a = c.getAngle();
            if (prev < 0.0f && a >= 0.0f) {          // upward zero crossing
                if (first < 0.0f) first = t; else last = t;
                ++crossings;
            }
            prev = a;
        }
        const float measured = (last - first) / std::max(1, crossings - 1);
        check(std::fabs(measured - Texp) / Texp < 0.02f,
              "period within 2% of the analytic pendulum period", measured);
        printf("         expected %.4fs, measured %.4fs\n", Texp, measured);
    }

    // ── 3. Hook velocity must equal the numeric derivative of hook position ─
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.55f);
        float worst = 0.0f;
        for (int i = 0; i < 4000; ++i) {
            const sf::Vector2f p0 = c.pointOnArm(BLOCK_SIZE * 0.5f);
            const sf::Vector2f v  = c.velocityOnArm(BLOCK_SIZE * 0.5f);
            c.update(FIXED_DT);
            const sf::Vector2f p1 = c.pointOnArm(BLOCK_SIZE * 0.5f);
            const sf::Vector2f fd((p1.x - p0.x) / FIXED_DT, (p1.y - p0.y) / FIXED_DT);
            worst = std::max(worst, std::hypot(fd.x - v.x, fd.y - v.y));
        }
        check(worst < 6.0f, "released velocity matches the actual arc (px/s error)", worst);
    }

    // ── 4. The block must stay exactly on the rope, at the right radius ─────
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.6f);
        float worst = 0.0f;
        const float want = CRANE_ARM_LENGTH + BLOCK_SIZE * 0.5f;
        for (int i = 0; i < 4000; ++i) {
            c.update(FIXED_DT);
            const sf::Vector2f p = c.pointOnArm(BLOCK_SIZE * 0.5f);
            const sf::Vector2f a = c.getAnchor();
            worst = std::max(worst, std::fabs(std::hypot(p.x - a.x, p.y - a.y) - want));
        }
        check(worst < 1e-3f, "block radius from pivot is constant (px error)", worst);
    }

    // ── 5. Camera scrolling must not perturb the swing at all ───────────────
    {
        Crane a, b;
        a.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex); a.reset(0.42f);
        b.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex); b.reset(0.42f);
        for (int i = 0; i < 6000; ++i) {
            a.update(FIXED_DT);
            b.update(FIXED_DT);
            b.setAnchor(200.0f, 40.0f - i * 0.37f);   // camera climbing
        }
        check(std::fabs(a.getAngle() - b.getAngle()) < 1e-6f,
              "scrolling the camera leaves the swing bit-identical",
              std::fabs(a.getAngle() - b.getAngle()));
    }

    // ── 6. Wire sprite must point at the block, not away from it ────────────
    //  Asked of SFML itself: where does the rectangle's bottom-centre actually
    //  land? An earlier version of this check re-implemented the rotation from
    //  my own assumption about SFML's sign convention, so it happily confirmed
    //  a wire that was drawn mirrored.
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.6f);
        float worst = 0.0f;
        for (int i = 0; i < 600; ++i) {
            c.update(FIXED_DT);
            c.syncSprite(1.0f);

            const sf::RectangleShape& w = c.getWireSprite();
            const sf::Vector2f drawnTip =
                w.getTransform().transformPoint(w.getSize().x * 0.5f,
                                                w.getSize().y);
            const sf::Vector2f hook = c.pointOnArm(0.0f);
            worst = std::max(worst, std::hypot(drawnTip.x - hook.x,
                                               drawnTip.y - hook.y));
        }
        check(worst < 0.05f, "wire is drawn along the rope (px error)", worst);
    }

    // ── 6b. The block must hang square on the rope ──────────────────────────
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.6f);
        float worst = 0.0f;
        for (int i = 0; i < 600; ++i) {
            c.update(FIXED_DT);
            // A block driven with getBodyAngle(): its local "down" axis, under
            // the [c -s; s c] rotation both SFML and Box2D use.
            const float a = c.getBodyAngle();
            const sf::Vector2f down(-std::sin(a), std::cos(a));
            // ...must point from the hook toward the block's centre.
            const sf::Vector2f hook  = c.pointOnArm(0.0f);
            const sf::Vector2f block = c.pointOnArm(BLOCK_SIZE * 0.5f);
            const sf::Vector2f along(block.x - hook.x, block.y - hook.y);
            const float len = std::hypot(along.x, along.y);
            const float cosang = (down.x * along.x + down.y * along.y) / len;
            worst = std::max(worst, std::fabs(1.0f - cosang));
        }
        check(worst < 1e-5f, "block hangs square on the rope (1-cos error)", worst);
    }

    // ── 7. Amplitude changes must not kick the pendulum ─────────────────────
    {
        Crane c;
        c.create(200.0f, 40.0f, CRANE_ARM_LENGTH, tex);
        c.reset(0.42f);
        float worstJump = 0.0f, prev = c.getAngle();
        for (int i = 0; i < 6000; ++i) {
            if (i % 400 == 0) c.setAmplitude(std::min(0.42f + i * 0.00006f, CRANE_MAX_AMPLITUDE));
            c.update(FIXED_DT);
            worstJump = std::max(worstJump, std::fabs(c.getAngle() - prev));
            prev = c.getAngle();
        }
        check(worstJump < 0.05f, "raising difficulty never jolts the arm (rad/step)", worstJump);
    }

    printf("\n%s  (%d failure%s)\n", failures ? "FAILURES" : "ALL CHECKS PASSED",
           failures, failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
