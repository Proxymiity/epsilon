#include "exponential_distribution.h"
#include <cmath>
#include <float.h>

namespace Probability {

float ExponentialDistribution::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  float parameter = m_parameter1;
  return parameter * std::exp(-parameter * x);
}

bool ExponentialDistribution::authorizedValueAtIndex(double x, int index) const {
  if (!OneParameterDistribution::authorizedValueAtIndex(x, index)) {
    return false;
  }
  if (x <= 0.0 || x > 7500.0) {
    return false;
  }
  return true;
}

double ExponentialDistribution::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (x < 0.0) {
    return 0.0;
  }
  return 1.0 - std::exp((-m_parameter1 * x));
}

double ExponentialDistribution::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1.0) {
    return INFINITY;
  }
  if (*probability <= 0.0) {
    return 0.0;
  }
  return -std::log(1.0 - *probability)/m_parameter1;
}

float ExponentialDistribution::computeXMax() const {
  assert(m_parameter1 != 0.0f);
  float result = 5.0f/m_parameter1;
  if (result <= FLT_EPSILON) {
    result = 1.0f;
  }
  if (std::isinf(result)) {
    // Lower computeXMax. It is used for drawing so the value is not that important.
    return 1.0f/m_parameter1;
  }
  return result * (1.0f + k_displayRightMarginRatio);
}

float ExponentialDistribution::computeYMax() const {
  float result = m_parameter1;
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result * (1.0f + k_displayTopMarginRatio);
}

}
