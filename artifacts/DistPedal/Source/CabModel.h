#pragma once
#include <array>
#include <cmath>

// Real-time filter-bank of the CAB_RES table in src/lib/audio/dsp.ts.
// Each mode {f, d, a} is the analog resonator
//     h(t) = a * exp(-t/d) * sin(2 π f t)
// implemented as a 2-pole digital resonator (no heap in process).

enum class CabType : int
{
    MODERN_412 = 0,
    VINTAGE_412,
    COLD_412,
    ATMOS_212,
    RAW_412,
    WALL_412,
    kNumCabs
};

inline constexpr const char* kCabNames[] = {
    "MODERN_412", "VINTAGE_412", "COLD_412", "ATMOS_212", "RAW_412", "WALL_412"};

struct Resonance
{
    float f, d, a;
};

class CabModel
{
public:
    void prepare (double sampleRate) noexcept;
    void setType (CabType type) noexcept;
    float process (float x) noexcept;
    void reset() noexcept;

    static CabType typeFromIndex (int i) noexcept;

private:
    struct Resonator
    {
        float a1 = 0, a2 = 0, g = 0;
        float z1 = 0, z2 = 0;
        float tick (float x) noexcept
        {
            const float y = g * x + a1 * z1 + a2 * z2;
            z2 = z1;
            z1 = y;
            return y;
        }
    };

    double fs = 44100.0;
    CabType type = CabType::COLD_412;
    int nRes = 0;
    std::array<Resonator, 6> res {};
    float dryMix = 0.35f;
    float wetMix = 0.65f;

    void rebuild() noexcept;
};
