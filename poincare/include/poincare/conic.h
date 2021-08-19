#ifndef POINCARE_CONIC_H
#define POINCARE_CONIC_H

#include <poincare/expression.h>
#include <float.h>

namespace Poincare {

class Conic {
public:
  enum class Type : uint8_t {
    Hyperbola = 0,
    Parabola,
    Ellipse,
    Circle,
    Undefined,
    Unknown,
  };
  // Default constructor
  Conic();
  // Extract A,B,C,D,E,F parameters
  Conic(const Expression e, Context * context, const char * x = "x", const char * y = "y");
  // Return conic type from parameters
  Type getConicType();
  // Conic Eccentricity
  double getEccentricity();
  // Conic Center
  void getCenter(double * cx, double * cy);
  // Ellipse or Hyperbola's semi major axis
  double getSemiMajorAxis();
  // Ellipse or Hyperbola's linear eccentricity
  double getLinearEccentricity();
  // Ellipse or Hyperbola's semi minor axis
  double getSemiMinorAxis();
  // Parabola's parameter
  double getParameter();
  // Coordinates of Parabola's summit (relative to parameters' center)
  void getSummit(double * sx, double * sy);
  // Circle's radius
  double getRadius();

private:
  // Thereshold under which a parameter is considered null
  static constexpr double k_tolerance = DBL_EPSILON * 10.0;
  // Return 0.0 if value should be considered null
  static double ApproximateForParameter(double value) {
    return std::abs(value) < k_tolerance ? 0.0 : value;
  }
  // Remove rotation from the parameters (B = 0)
  void rotateConic();
  // Remove both rotation and off-centering from the parameters
  void centerConic();
  // Make it so that F parameter is -1 or 0
  void canonize();
  // Conic determinant, allow identification of the type
  double getDeterminant() const;
  // Ax^2+Bxy+Cy^2+Dx+Ey+F=0
  double m_a;
  double m_b;
  double m_c;
  double m_d;
  double m_e;
  double m_f;
  double m_cx;
  double m_cy;
  double m_r;
  Type m_type;
};

}  // namespace Poincare

#endif