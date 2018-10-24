#include <poincare/matrix_trace.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

int MatrixTraceNode::numberOfChildren() const { return MatrixTrace::FunctionHelper()->numberOfChildren(); }

Expression MatrixTraceNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return MatrixTrace(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Layout MatrixTraceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixTrace(this), floatDisplayMode, numberOfSignificantDigits, MatrixTrace::FunctionHelper()->name());
}

int MatrixTraceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTrace::FunctionHelper()->name());
}

template<typename T>
Evaluation<T> MatrixTraceNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, angleUnit);
  Complex<T> result = Complex<T>(input.trace());
  return result;
}

Expression MatrixTrace::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix&>(c);
    if (m.numberOfRows() != m.numberOfColumns()) {
      return Undefined();
    }
    int n = m.numberOfRows();
    Addition a = Addition();
    for (int i = 0; i < n; i++) {
      a.addChildAtIndexInPlace(m.childAtIndex(i+n*i), i, a.numberOfChildren());
    }
    return a.shallowReduce(context, angleUnit);
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return c;
  }
  return *this;
#endif
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
#endif
}

constexpr Expression::FunctionHelper MatrixTrace::m_functionHelper = Expression::FunctionHelper("trace", 1, &MatrixTrace::UntypedBuilder);

}
