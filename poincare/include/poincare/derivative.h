#ifndef POINCARE_DERIVATIVE_H
#define POINCARE_DERIVATIVE_H

#include <poincare/expression.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/variable_context.h>

namespace Poincare {

class DerivativeNode : public ExpressionNode {
public:
  // Allocation Failure
  static DerivativeNode * FailedAllocationStaticNode();
  DerivativeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }

  // TreeNode
  size_t size() const override { return sizeof(DerivativeNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Derivative";
  }
#endif

  // Properties
  Type type() const override { return Type::Derivative; }
  int polynomialDegree(char symbolName) const override;

private:
  // Layout
  LayoutReference createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutHelper::Prefix(this, floatDisplayMode, numberOfSignificantDigits, name());
  }
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  const char * name() const { return "diff"; }

  // Simplification
  Expression shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const override;

  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T growthRateAroundAbscissa(T x, T h, Context & context, Preferences::AngleUnit angleUnit) const;
  template<typename T> T riddersApproximation(Context & context, Preferences::AngleUnit angleUnit, T x, T h, T * error) const;
  // TODO: Change coefficients?
  constexpr static double k_maxErrorRateOnApproximation = 0.001;
  constexpr static double k_minInitialRate = 0.01;
  constexpr static double k_rateStepSize = 1.4;
};

class Derivative : public Expression {
public:
  Derivative() : Expression(TreePool::sharedPool()->createTreeNode<DerivativeNode>()) {}
  Derivative(const DerivativeNode * n) : Expression(n) {}
  Derivative(Expression child1, Expression child2) : Derivative() {
    replaceChildAtIndexInPlace(0, child1);
    replaceChildAtIndexInPlace(1, child2);
  }

  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const;
};

}

#endif
