#ifndef SHARED_RANGE_1D_H
#define SHARED_RANGE_1D_H

#include <cmath>
#include <float.h>
#include <poincare/zoom.h>

#if __EMSCRIPTEN__
#include <emscripten.h>
#include <stdint.h>
#endif

namespace Shared {

/* This class is used in a DataBuffer of a Storage::Record. See comment in
 * Shared::Function::RecordDataBuffer about packing. */

class __attribute__((packed)) Range1D final {
public:
  /* If m_min and m_max are too close, we cannot divide properly the range by
   * the number of pixels, which creates a drawing problem. */
  constexpr static float k_minFloat = Poincare::Zoom::k_minimalRangeLength;
  constexpr static float k_default = Poincare::Zoom::k_defaultHalfRange;
  Range1D(float min = -k_default, float max = k_default) :
    m_min(min),
    m_max(max)
  {}
  float min() const { return m_min; }
  float max() const { return m_max; }
  void setMin(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);
  void setMax(float f, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY);

  static float checkedValue(float value, float * otherValue, float lowerMaxFloat, float upperMaxFloat, bool isMax);
  static float checkedMin(float min, float * max = nullptr, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY) { return checkedValue(min, max, lowerMaxFloat, upperMaxFloat, false); }
  static float checkedMax(float max, float * min = nullptr, float lowerMaxFloat = INFINITY, float upperMaxFloat = INFINITY) { return checkedValue(max, min, lowerMaxFloat, upperMaxFloat, true); }
  static float clipped(float x, bool isMax, float lowerMaxFloat, float upperMaxFloat);
  static float defaultRangeLengthFor(float position);

  /* We use these values to bound the graph ranges to avoid infinite ranges.
   * The values are derived from FLT_MAX with an arbitrary margin. */
  constexpr static float k_upperMaxFloat = 1E+35f;
  constexpr static float k_lowerMaxFloat = 9E+34f;
private:
#if __EMSCRIPTEN__
    // See comment about emscripten alignment in Shared::Function::RecordDataBuffer
    static_assert(sizeof(emscripten_align1_short) == sizeof(uint16_t), "emscripten_align1_short should have the same size as uint16_t");
    emscripten_align1_float m_min;
    emscripten_align1_float m_max;
#else
  float m_min;
  float m_max;
#endif
};

static_assert(Range1D::k_minFloat >= FLT_EPSILON, "InteractiveCurveViewRange's minimal float range is lower than float precision, it might draw uglily curves such as cos(x)^2+sin(x)^2");

}

#endif
