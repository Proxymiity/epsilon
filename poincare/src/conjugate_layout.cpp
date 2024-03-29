#include <poincare/conjugate.h>
#include <poincare/conjugate_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <escher/metric.h>
#include <assert.h>

namespace Poincare {

void ConjugateLayoutNode::moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  if (cursor->layoutNode() == childLayout()
      && cursor->position() == LayoutCursor::Position::Left)
  {
    // Case: Left of the operand. Move Left.
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  if (cursor->position() == LayoutCursor::Position::Right) {
    // Case: Right. Go to the operand.
    cursor->setLayoutNode(childLayout());
    return;
  }
  assert(cursor->position() == LayoutCursor::Position::Left);
  // Case: Left. Ask the parent.
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorLeft(cursor, shouldRecomputeLayout);
  }
}

void ConjugateLayoutNode::moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) {
  // Case: Right of the operand. Move Right.
  if (cursor->layoutNode() == childLayout()
      && cursor->position() == LayoutCursor::Position::Right)
  {
    cursor->setLayoutNode(this);
    return;
  }
  assert(cursor->layoutNode() == this);
  // Case: Left. Go to the operand.
  if (cursor->position() == LayoutCursor::Position::Left) {
    cursor->setLayoutNode(childLayout());
    return;
  }
  // Case: Right. Ask the parent.
  assert(cursor->position() == LayoutCursor::Position::Right);
  LayoutNode * parentNode = parent();
  if (parentNode != nullptr) {
    parentNode->moveCursorRight(cursor, shouldRecomputeLayout);
  }
}

int ConjugateLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Conjugate::s_functionHelper.name(), true);
}

KDSize ConjugateLayoutNode::computeSize() {
  KDSize childSize = childLayout()->layoutSize();
  KDCoordinate newWidth =
    Escher::Metric::FractionAndConjugateHorizontalMargin +
    Escher::Metric::FractionAndConjugateHorizontalOverflow +
    childSize.width() +
    Escher::Metric::FractionAndConjugateHorizontalOverflow +
    Escher::Metric::FractionAndConjugateHorizontalMargin;
  KDCoordinate newHeight =
    childSize.height() +
    k_overlineWidth +
    k_overlineVerticalMargin;
  return KDSize(newWidth, newHeight);
}

KDCoordinate ConjugateLayoutNode::computeBaseline() {
  return childLayout()->baseline() + k_overlineWidth + k_overlineVerticalMargin;
}

KDPoint ConjugateLayoutNode::positionOfChild(LayoutNode * child) {
  assert(child == childLayout());
  return KDPoint(
      Escher::Metric::FractionAndConjugateHorizontalMargin + Escher::Metric::FractionAndConjugateHorizontalOverflow,
      k_overlineWidth + k_overlineVerticalMargin);
}

void ConjugateLayoutNode::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  ctx->fillRect(
      KDRect(
        p.x() + Escher::Metric::FractionAndConjugateHorizontalMargin,
        p.y(),
        childLayout()->layoutSize().width() + 2 * Escher::Metric::FractionAndConjugateHorizontalOverflow,
        k_overlineWidth),
      expressionColor);
}

bool ConjugateLayoutNode::willReplaceChild(LayoutNode * oldChild, LayoutNode * newChild, LayoutCursor * cursor, bool force) {
  if (oldChild == newChild) {
    return false;
  }
  assert(oldChild == childLayout());
  if (!force && newChild->isEmpty()) {
    Layout(this).replaceWith(Layout(newChild), cursor);
    // WARNING: do not call "this" afterwards
    return false;
  }
  return true;
}

}
