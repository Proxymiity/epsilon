#ifndef POINCARE_REEL_PART_H
#define POINCARE_REEL_PART_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class RealPartNode final : public ExpressionNode  {
public:

  // TreeNode
  size_t size() const override { return sizeof(RealPartNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "RealPart";
  }
#endif

  // Properties
  Type type() const override { return Type::RealPart; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true) override;
  // Evaluation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
    return Complex<T>(std::real(c));
  }
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::Map<double>(this, context, angleUnit, computeOnComplex<double>);
  }
};

class RealPart final : public Expression {
public:
  RealPart(const RealPartNode * n) : Expression(n) {}
  static RealPart Builder(Expression child) { return RealPart(child); }
  static Expression UntypedBuilder(Expression children) { return Builder(children.childAtIndex(0)); }
  static const Expression::FunctionHelper * FunctionHelper() { return &m_functionHelper; }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols = true);
private:
  explicit RealPart(Expression child) : Expression(TreePool::sharedPool()->createTreeNode<RealPartNode>()) {
    replaceChildAtIndexInPlace(0, child);
  }
  static const Expression::FunctionHelper m_functionHelper;
};

}

#endif
