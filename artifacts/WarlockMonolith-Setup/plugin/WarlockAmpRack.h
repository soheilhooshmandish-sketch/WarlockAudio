#pragma once
#include <cmath>
namespace warlock {
enum class AmpModel : int { Thru = 0, Platform, Iron, Pillar, Slab, Bin, Count };
inline const char* ampModelName (AmpModel m) {
  switch (m) {
    case AmpModel::Thru: return "THRU"; case AmpModel::Platform: return "PLATFORM";
    case AmpModel::Iron: return "IRON"; case AmpModel::Pillar: return "PILLAR";
    case AmpModel::Slab: return "SLAB"; case AmpModel::Bin: return "BIN"; default: return "THRU";
  }
}
struct AmpRackKnobs {
  AmpModel model = AmpModel::Iron; float inputDb = 0; float drive = 0.45f;
  float bassDb = 1; float midDb = -1; float trebleDb = 1.5f; float presence = 0.35f;
  float sag = 0.25f; float masterDb = 0;
};
inline float ampShape (float x, AmpModel model, float drive) noexcept {
  if (model == AmpModel::Thru) return x;
  const float g = 1.0f + 11.0f * drive; float y = x * g;
  switch (model) {
    case AmpModel::Platform: y = tanhf (y * 0.7f); break;
    case AmpModel::Iron: y = tanhf (y) + 0.12f * tanhf (y * 3.0f); break;
    case AmpModel::Pillar: y = tanhf (y * 1.15f); break;
    case AmpModel::Slab: { float a = tanhf (y * 1.4f); float b = y / (1.0f + fabsf (y * 2.2f)); y = 0.65f * a + 0.35f * b; break; }
    case AmpModel::Bin: y = tanhf (y * 0.85f + 0.08f * y * y); break;
    default: break;
  }
  return y;
}
}
