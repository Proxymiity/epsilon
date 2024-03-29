#ifndef POINCARE_SECANT_H
#define POINCARE_SECANT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class SecantNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(SecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Secant";
  }
#endif

  // Properties
  Type type() const override { return Type::Secant; }

  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit = Preferences::AngleUnit::Radian);

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplication
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::Map<float>(this, approximationContext, computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return ApproximationHelper::Map<double>(this, approximationContext, computeOnComplex<double>);
  }
};

class Secant final : public Expression {
public:
  Secant(const SecantNode * n) : Expression(n) {}
  static Secant Builder(Expression child) { return TreeHandle::FixedArityBuilder<Secant, SecantNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("sec", 1, &UntypedBuilderOneChild<Secant>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
