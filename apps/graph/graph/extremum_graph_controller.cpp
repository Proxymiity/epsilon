#include "extremum_graph_controller.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/serialization_helper.h>

using namespace Poincare;
using namespace Escher;

namespace Graph {

MinimumGraphController::MinimumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMinimumFound)
{
}

const char * MinimumGraphController::title() {
  return I18n::translate(I18n::Message::Minimum);
}

Coordinate2D<double> MinimumGraphController::computeNewPointOfInterest(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  return functionStore()->modelForRecord(m_record)->nextMinimumFrom(start, max, context, relativePrecision, minimalStep, maximalStep);
}

MaximumGraphController::MaximumGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoMaximumFound)
{
}

const char * MaximumGraphController::title() {
  return I18n::translate(I18n::Message::Maximum);
}

Coordinate2D<double> MaximumGraphController::computeNewPointOfInterest(double start, double max, Poincare::Context * context, double relativePrecision, double minimalStep, double maximalStep) {
  return functionStore()->modelForRecord(m_record)->nextMaximumFrom(start, max, context, relativePrecision, minimalStep, maximalStep);
}

}
