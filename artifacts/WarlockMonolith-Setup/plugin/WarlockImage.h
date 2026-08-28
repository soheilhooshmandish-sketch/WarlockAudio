#pragma once
/**
 * WARLOCK MONOLITH · Model 1666 — output image
 * Two published knobs, output column, always visible:
 *   MONO    0–100 %   folds the dry / dirt bus toward a mid sum
 *   STEREO  0–100 %   sets width of the wet bus (cab, mod, delay, reverb)
 */
#include <cmath>
#include <algorithm>

namespace warlock
{
struct ImageKnobs
{
    float mono   = 1.0f;
    float stereo = 0.7f;
};

inline float clamp01 (float v) noexcept
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

inline void applyMono (float& left, float& right, float amount) noexcept
{
    const float a = clamp01 (amount);
    const float mid = 0.5f * (left + right);
    left  = left  + a * (mid - left);
    right = right + a * (mid - right);
}

inline void applyStereo (float& left, float& right, float width) noexcept
{
    const float w = clamp01 (width);
    const float mid  = 0.5f * (left + right);
    const float side = 0.5f * (left - right) * w;
    left  = mid + side;
    right = mid - side;
}

inline void processImageSample (float dryL, float dryR,
                                float wetL, float wetR,
                                const ImageKnobs& knobs,
                                float& outL, float& outR) noexcept
{
    applyMono   (dryL, dryR, knobs.mono);
    applyStereo (wetL, wetR, knobs.stereo);
    outL = dryL + wetL;
    outR = dryR + wetR;
}
}
