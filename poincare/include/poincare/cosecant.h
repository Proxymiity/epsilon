#ifndef POINCARE_COSCANT_H
#define POINCARE_COSCANT_H

#include <poincare/approximation_helper.h>
#include <poincare/expression.h>

namespace Poincare {

class CosecantNode final : public ExpressionNode {
public:
  // TreeNode
  size_t size() const override { return sizeof(CosecantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Cosecant";
  }
#endif

  // Properties
  Type type() const override { return Type::Cosecant; }

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

class Cosecant final : public Expression {
public:
  Cosecant(const CosecantNode * n) : Expression(n) {}
  static Cosecant Builder(Expression child) { return TreeHandle::FixedArityBuilder<Cosecant, CosecantNode>({child}); }

  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("csc", 1, &UntypedBuilderOneChild<Cosecant>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
