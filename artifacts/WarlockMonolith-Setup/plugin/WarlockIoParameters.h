#pragma once
namespace warlock
{
inline constexpr const char* kParamOutMono   = "OUT_MONO";
inline constexpr const char* kParamOutStereo = "OUT_STEREO";

struct IoParameterInfo
{
    const char* id;
    const char* panelName;
    const char* unit;
    float minValue;
    float maxValue;
    float defaultValue;
    const char* hint;
};

inline constexpr IoParameterInfo kIoImageParams[] = {
    { kParamOutMono,   "MONO",   "%", 0.0f, 100.0f, 100.0f,
      "Folds the dry / dirt / EQ bus to a mid sum. 100 keeps the riff mono-compatible." },
    { kParamOutStereo, "STEREO", "%", 0.0f, 100.0f,  70.0f,
      "Width of cab, modulation, delay and reverb only. 0 collapses the wet bus." },
};

inline constexpr int kIoImageParamCount = 2;
}
