#ifndef APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H
#define APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H

#include "z_statistic.h"

namespace Probability {

class OneMeanZStatistic : public ZStatistic {
public:
  OneMeanZStatistic();
  void computeTest() override;
  void computeInterval() override;

  // Confidence interval
  const char * estimateSymbol() override { return "x"; };
  I18n::Message estimateDescription() override { return I18n::Message::Default; };

protected:
  // Parameters
  constexpr static int k_numberOfParams = 3;
  int numberOfStatisticParameters() const override { return k_numberOfParams; }
  enum ParamsOrder { X, N, Sigma };
  const ParameterRepr * paramReprAtIndex(int i) const override;
  float * paramArray() override { return m_params; }

private:
  float x() { return m_params[ParamsOrder::X]; }
  float n() { return m_params[ParamsOrder::N]; }
  float sigma() { return m_params[ParamsOrder::Sigma]; }

  // Computation
  float _z(float meanSample, float mean, float n, float sigma);
  float _SE(float sigma, int n);

  float m_params[k_numberOfParams];
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_STATISTIC_ONE_MEAN_Z_STATISTIC_H */