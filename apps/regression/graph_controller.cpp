#include "graph_controller.h"
#include "../apps_container.h"
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Regression {

GraphController::GraphController(Responder * parentResponder, ButtonRowController * header, Store * store, CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex) :
  InteractiveCurveViewController(parentResponder, header, store, &m_view, cursor, modelVersion, rangeVersion),
  m_crossCursorView(),
  m_roundCursorView(),
  m_bannerView(),
  m_view(store, m_cursor, &m_bannerView, &m_crossCursorView, this),
  m_store(store),
  m_initialisationParameterController(this, m_store),
  m_graphOptionsController(this, m_store, m_cursor, this),
  m_selectedDotIndex(selectedDotIndex),
  m_selectedSeriesIndex(selectedSeriesIndex)
{
  m_store->setCursor(m_cursor);
}

ViewController * GraphController::initialisationParameterController() {
  return &m_initialisationParameterController;
}

bool GraphController::isEmpty() const {
  return m_store->isEmpty();
}

I18n::Message GraphController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

void GraphController::viewWillAppear() {
  InteractiveCurveViewController::viewWillAppear();
  if (*m_selectedSeriesIndex < 0) {
    *m_selectedSeriesIndex = m_store->indexOfKthNonEmptySeries(0);
  }
  if (*m_selectedDotIndex >= 0) {
    m_view.setCursorView(static_cast<View *>(&m_crossCursorView));
  } else {
    m_view.setCursorView(static_cast<View *>(&m_roundCursorView));
    m_roundCursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
  }
}

void GraphController::selectRegressionCurve() {
  *m_selectedDotIndex = -1;
  m_view.setCursorView(&m_roundCursorView);
  m_roundCursorView.setColor(Palette::DataColor[*m_selectedSeriesIndex]);
}

CurveView * GraphController::curveView() {
  return &m_view;
}

InteractiveCurveViewRange * GraphController::interactiveCurveViewRange() {
  return m_store;
}

bool GraphController::handleEnter() {
  stackController()->push(&m_graphOptionsController);
  return true;
}

void GraphController::reloadBannerView() {
  if (*m_selectedSeriesIndex < 0) {
    return;
  }

  // Set point equals: "P(...) ="
  char buffer[k_maxNumberOfCharacters + PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  int numberOfChar = 0;
  const char * legend = " P(";
  int legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    legend = I18n::translate(I18n::Message::MeanDot);
    legendLength = strlen(legend);
    strlcpy(buffer+numberOfChar, legend, legendLength+1);
    numberOfChar += legendLength;
  } else if (*m_selectedDotIndex < 0) {
    legend = I18n::translate(I18n::Message::Reg);
    legendLength = strlen(legend);
    strlcpy(buffer+numberOfChar, legend, legendLength+1);
    numberOfChar += legendLength;
  } else {
    numberOfChar += PrintFloat::convertFloatToText<float>(std::round((float)*m_selectedDotIndex+1.0f), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::ShortNumberOfSignificantDigits), Constant::ShortNumberOfSignificantDigits, PrintFloat::Mode::Decimal);
  }
  legend = ")  ";
  legendLength = strlen(legend);
  strlcpy(buffer+numberOfChar, legend, legendLength+1);
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 0);


  // Set "x=..." or "xmean=..."
  numberOfChar = 0;
  legend = "x=";
  double x = m_cursor->x();
  // Display a specific legend if the mean dot is selected
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    constexpr static char legX[] = {Ion::Charset::XBar, '=', 0};
    legend = legX;
    x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(x, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 1);

  // Set "y=..." or "ymean=..."
  numberOfChar = 0;
  legend = "y=";
  double y = m_cursor->y();
  if (*m_selectedDotIndex == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
    constexpr static char legY[] = {Ion::Charset::YBar, '=', 0};
    legend = legY;
    y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
  }
  legendLength = strlen(legend);
  strlcpy(buffer, legend, legendLength+1);
  numberOfChar += legendLength;
  numberOfChar += PrintFloat::convertFloatToText<double>(y, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  for (int i = numberOfChar; i < k_maxLegendLength; i++) {
    buffer[numberOfChar++] = ' ';
  }
  buffer[k_maxLegendLength] = 0;
  m_bannerView.setLegendAtIndex(buffer, 2);

  // Set formula
  Model * model = m_store->modelForSeries(selectedSeriesIndex());
  m_bannerView.setMessageAtIndex(model->formulaMessage(), 3);

  // Get the coefficients
  Poincare::Context * globContext = const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext();
  double * coefficients = m_store->coefficientsForSeries(selectedSeriesIndex(), globContext);
  bool coefficientsAreDefined = true;
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    if (std::isnan(coefficients[i])) {
      coefficientsAreDefined = false;
      break;
    }
  }
  if (!coefficientsAreDefined) {
    const char * dataNotSuitableMessage = I18n::translate(I18n::Message::DataNotSuitableForRegression);
     m_bannerView.setLegendAtIndex(const_cast<char *>(dataNotSuitableMessage), 4);
     for (int i = 5; i < m_bannerView.numberOfTextviews(); i++) {
        char empty[] = {0};
        m_bannerView.setLegendAtIndex(empty, i);
     }
     return;
  }
  char coefficientName = 'a';
  for (int i = 0; i < model->numberOfCoefficients(); i++) {
    numberOfChar = 0;
    char leg[] = {' ', coefficientName, '=', 0};
    legend = leg;
    legendLength = strlen(legend);
    strlcpy(buffer, legend, legendLength+1);
    numberOfChar += legendLength;
    numberOfChar += PrintFloat::convertFloatToText<double>(coefficients[i], buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength] = 0;
    m_bannerView.setLegendAtIndex(buffer, 4 + i);
    coefficientName++;
  }

  if (m_store->seriesRegressionType(selectedSeriesIndex()) == Model::Type::Linear) {
    // Set "r=..."
    numberOfChar = 0;
    legend = " r=";
    double r = m_store->correlationCoefficient(*m_selectedSeriesIndex);
    legendLength = strlen(legend);
    strlcpy(buffer, legend, legendLength+1);
    numberOfChar += legendLength;
    numberOfChar += PrintFloat::convertFloatToText<double>(r, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength+10] = 0;
    m_bannerView.setLegendAtIndex(buffer, 6);

    // Set "r2=..."
    numberOfChar = 0;
    legend = " r2=";
    double r2 = m_store->squaredCorrelationCoefficient(*m_selectedSeriesIndex);
    legendLength = strlen(legend);
    strlcpy(buffer, legend, legendLength+1);
    numberOfChar += legendLength;
    numberOfChar += PrintFloat::convertFloatToText<double>(r2, buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
    buffer[k_maxLegendLength] = 0;
    m_bannerView.setLegendAtIndex(buffer, 7);

    // Clean the last subview
    buffer[0] = 0;
    m_bannerView.setLegendAtIndex(buffer, 8);

  } else {
    // Empty all non used subviews
    for (int i = 4+model->numberOfCoefficients(); i < m_bannerView.numberOfTextviews(); i++) {
      buffer[0] = 0;
      m_bannerView.setLegendAtIndex(buffer, i);
    }
  }
}

void GraphController::initRangeParameters() {
  m_store->setDefault();
}

void GraphController::initCursorParameters() {
  *m_selectedSeriesIndex = m_store->indexOfKthNonEmptySeries(0);
  double x = m_store->meanOfColumn(*m_selectedSeriesIndex, 0);
  double y = m_store->meanOfColumn(*m_selectedSeriesIndex, 1);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  *m_selectedDotIndex = m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex);
}

bool GraphController::moveCursorHorizontally(int direction) {
  if (*m_selectedDotIndex >= 0) {
    int dotSelected = m_store->nextDot(*m_selectedSeriesIndex, direction, *m_selectedDotIndex);
    if (dotSelected >= 0 && dotSelected < m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      *m_selectedDotIndex = dotSelected;
      m_cursor->moveTo(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    return false;
  }
  double x = direction > 0 ? m_cursor->x() + m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit :
  m_cursor->x() - m_store->xGridUnit()/k_numberOfCursorStepsInGradUnit;
  Poincare::Context * globContext = const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext();
  double y = m_store->yValueForXValue(*m_selectedSeriesIndex, x, globContext);
  m_cursor->moveTo(x, y);
  m_store->panToMakePointVisible(x, y, k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

bool GraphController::moveCursorVertically(int direction) {
  int closestRegressionSeries = -1;
  int closestDotSeries = -1;
  int dotSelected = -1;
  Poincare::Context * globContext = const_cast<AppsContainer *>(static_cast<const AppsContainer *>(app()->container()))->globalContext();

  if (*m_selectedDotIndex == -1) {
    // The current cursor is on a regression
    // Check the closest regression
    closestRegressionSeries = m_store->closestVerticalRegression(direction, m_cursor->x(), m_cursor->y(), *m_selectedSeriesIndex, globContext);
    // Check the closest dot
    dotSelected = m_store->closestVerticalDot(direction, m_cursor->x(), direction > 0 ? -FLT_MAX : FLT_MAX, *m_selectedSeriesIndex, *m_selectedDotIndex, &closestDotSeries, globContext);
  } else {
    // The current cursor is on a dot
    // Check the closest regression
    closestRegressionSeries = m_store->closestVerticalRegression(direction, m_cursor->x(), m_cursor->y(), -1, globContext);
    // Check the closest dot
    dotSelected = m_store->closestVerticalDot(direction, m_cursor->x(), m_cursor->y(), *m_selectedSeriesIndex, *m_selectedDotIndex, &closestDotSeries, globContext);
  }

  bool validRegression = closestRegressionSeries > -1;
  bool validDot = dotSelected >= 0 && dotSelected <= m_store->numberOfPairsOfSeries(closestDotSeries);

  if (validRegression && validDot) {
    /* Compare the abscissa distances to select either the dot or the
     * regression. If they are equal, compare the ordinate distances. */
    double dotDistanceX = -1;
    if (dotSelected == m_store->numberOfPairsOfSeries(closestDotSeries)) {
      dotDistanceX = std::fabs(m_store->meanOfColumn(closestDotSeries, 0) - m_cursor->x());
    } else {
      dotDistanceX = std::fabs(m_store->get(closestDotSeries, 0, dotSelected) - m_cursor->x());
    }
    if (dotDistanceX != 0) {
      /* The regression X distance to the point is 0, so it is closer than the
       * dot. */
      validDot = false;
    } else {
      // Compare the y distances
      double regressionDistanceY = std::fabs(m_store->yValueForXValue(closestRegressionSeries, m_cursor->x(), globContext) - m_cursor->y());
      double dotDistanceY = -1;
      if (dotSelected == m_store->numberOfPairsOfSeries(closestDotSeries)) {
        dotDistanceY = std::fabs(m_store->meanOfColumn(closestDotSeries, 1) - m_cursor->y());
      } else {
        dotDistanceY = std::fabs(m_store->get(closestDotSeries, 1, dotSelected) - m_cursor->y());
      }
      if (regressionDistanceY <= dotDistanceY) {
        validDot = false;
      } else {
        validRegression = false;
      }
    }
  }
  if (!validDot && validRegression) {
    // Select the regression
    *m_selectedSeriesIndex = closestRegressionSeries;
    selectRegressionCurve();
    m_cursor->moveTo(m_cursor->x(), m_store->yValueForXValue(*m_selectedSeriesIndex, m_cursor->x(), globContext));
    m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
    return true;
  }

  if (validDot && !validRegression) {
    m_view.setCursorView(&m_crossCursorView);
    *m_selectedSeriesIndex = closestDotSeries;
    *m_selectedDotIndex = dotSelected;
    if (dotSelected == m_store->numberOfPairsOfSeries(*m_selectedSeriesIndex)) {
      m_cursor->moveTo(m_store->meanOfColumn(*m_selectedSeriesIndex, 0), m_store->meanOfColumn(*m_selectedSeriesIndex, 1));
      m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
      return true;
    }
    m_cursor->moveTo(m_store->get(*m_selectedSeriesIndex, 0, *m_selectedDotIndex), m_store->get(*m_selectedSeriesIndex, 1, *m_selectedDotIndex));
    m_store->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
    return true;
  }
  return false;
}

uint32_t GraphController::modelVersion() {
  return m_store->storeChecksum();
}

uint32_t GraphController::rangeVersion() {
  return m_store->rangeChecksum();
}

bool GraphController::isCursorVisible() {
  return interactiveCurveViewRange()->isCursorVisible(k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
}

}
