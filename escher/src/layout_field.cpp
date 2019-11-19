#include <escher/layout_field.h>
#include <escher/clipboard.h>
#include <escher/text_field.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <assert.h>
#include <string.h>

using namespace Poincare;

static inline KDCoordinate minCoordinate(KDCoordinate x, KDCoordinate y) { return x < y ? x : y; }

LayoutField::ContentView::ContentView() :
  m_cursor(),
  m_expressionView(0.0f, 0.5f, KDColorBlack, KDColorWhite, &m_selectionStart, &m_selectionEnd),
  m_cursorView(),
  m_selectionStart(),
  m_selectionEnd(),
  m_isEditing(false)
{
  clearLayout();
}

bool LayoutField::ContentView::setEditing(bool isEditing) {
  m_isEditing = isEditing;
  markRectAsDirty(bounds());
  if (isEditing) {
    /* showEmptyLayoutIfNeeded is done in LayoutField::handleEvent, so no need
     * to do it here. */
    if (m_cursor.hideEmptyLayoutIfNeeded()) {
      m_expressionView.layout().invalidAllSizesPositionsAndBaselines();
      return true;
    }
  }
  layoutSubviews();
  markRectAsDirty(bounds());
  return false;
}

void LayoutField::ContentView::clearLayout() {
  HorizontalLayout h = HorizontalLayout::Builder();
  if (m_expressionView.setLayout(h)) {
    m_cursor.setLayout(h);
  }
}

KDSize LayoutField::ContentView::minimalSizeForOptimalDisplay() const {
  KDSize evSize = m_expressionView.minimalSizeForOptimalDisplay();
  return KDSize(evSize.width() + Poincare::LayoutCursor::k_cursorWidth, evSize.height());
}

bool IsBefore(Layout& l1, Layout& l2, bool strict) {
  //TODO LEA
  char * node1 = reinterpret_cast<char *>(l1.node());
  char * node2 = reinterpret_cast<char *>(l2.node());
  return strict ? (node1 < node2) : (node1 <= node2);
}

void LayoutField::ContentView::addSelection(Layout left, Layout right) {
  if (selectionIsEmpty()) {
    /*
     *  ----------  -> +++ is the previous previous selection
     *     (   )    -> added selection
     *  ---+++++--  -> next selection
     * */
    m_selectionStart = left;
    m_selectionEnd = right;
  }
#if 0
  else if (left == m_selectionEnd) {
    /*
     *  +++-------  -> +++ is the previous previous selection
     *     (   )    -> added selection
     *  ++++++++--  -> next selection
     * */
    m_selectionEnd = right;
  } else if (right == m_selectionStart) {
    /*
     *  -------+++  -> +++ is the previous previous selection
     *     (   )    -> added selection
     *  ---+++++++  -> next selection
     * */
    m_selectionStart = left;
  }
#endif
   else if (IsBefore(m_selectionEnd, left, false) && !left.hasAncestor(m_selectionEnd, true)) {
    /*
     *  +++-------  -> +++ is the previous previous selection
     *       (   )  -> added selection
     *  ++++++++++  -> next selection
     * */
     m_selectionEnd = right;
  } else if (IsBefore(right, m_selectionStart, true)) {
    /*
     *  -------+++  -> +++ is the previous previous selection
     *  (   )       -> added selection
     *  ++++++++++  -> next selection
     * */
    m_selectionStart = left;
  } else if (m_selectionEnd == right) {
    /*
     *  ++++++++++  -> +++ is the previous previous selection
     *       (   )  -> added selection
     *  +++++-----  -> next selection
     * */
     LayoutCursor c1 = LayoutCursor(left, LayoutCursor::Position::Left);
     if (c1.layoutReference() == m_selectionStart) {
       m_selectionStart = Layout();
       m_selectionEnd = Layout();
     } else {
       LayoutCursor c2 = left.equivalentCursor(&c1);
       Layout c2Layout = c2.layoutReference();
       if (c2.position() == LayoutCursor::Position::Right) {
         assert(IsBefore(m_selectionStart, c2Layout, false));
         m_selectionEnd = c2Layout;
       } else {
        //TODO LEA
        assert(false);
       }
     }
  } else {
    assert(m_selectionStart == left);
    /*
     *  ++++++++++  -> +++ is the previous previous selection
     *  (   )       -> added selection
     *  -----+++++  -> next selection
     * */
     LayoutCursor c1 = LayoutCursor(right, LayoutCursor::Position::Right);
     if (c1.layoutReference() == m_selectionEnd) {
       m_selectionStart = Layout();
       m_selectionEnd = Layout();
     } else {
       LayoutCursor c2 = right.equivalentCursor(&c1);
       Layout c2Layout = c2.layoutReference();
       if (c2.position() == LayoutCursor::Position::Left) {
         assert(IsBefore(c2Layout, m_selectionEnd, false));
         m_selectionStart = c2Layout;
       } else {
        //TODO LEA
        assert(false);
       }
     }
  }
  //reloadRectFromAndToPositions(left, right);
  /*if (m_selectionStart == m_selectionEnd) {
    m_selectionStart = Layout();
    m_selectionEnd = Layout();
  } TODO LEA*/
}

bool LayoutField::ContentView::resetSelection() {
  Layout previousStart = m_selectionStart;
  if (selectionIsEmpty()) {
    return false;
  }
  Layout previousEnd = m_selectionEnd;
  m_selectionStart = Layout();
  m_selectionEnd = Layout();
  //TODO LEA reloadRectFromAndToPositions(previousStart, previousEnd);
  return true;
}

bool LayoutField::ContentView::selectionIsEmpty() const {
  assert(!m_selectionStart.isUninitialized() || m_selectionEnd.isUninitialized());
  assert(!m_selectionEnd.isUninitialized() || m_selectionStart.isUninitialized());
  return m_selectionStart.isUninitialized();
}

size_t LayoutField::ContentView::deleteSelection() {
  //TODO LEA
  return 1;
}

View * LayoutField::ContentView::subviewAtIndex(int index) {
  assert(0 <= index && index < numberOfSubviews());
  View * m_views[] = {&m_expressionView, &m_cursorView};
  return m_views[index];
}

void LayoutField::ContentView::layoutSubviews(bool force) {
  m_expressionView.setFrame(bounds(), force);
  layoutCursorSubview(force);
}

void LayoutField::ContentView::layoutCursorSubview(bool force) {
  if (!m_isEditing) {
    m_cursorView.setFrame(KDRectZero, force);
    return;
  }
  KDPoint expressionViewOrigin = m_expressionView.absoluteDrawingOrigin();
  Layout pointedLayoutR = m_cursor.layoutReference();
  LayoutCursor::Position cursorPosition = m_cursor.position();
  LayoutCursor eqCursor = pointedLayoutR.equivalentCursor(&m_cursor);
  if (eqCursor.isDefined() && pointedLayoutR.hasChild(eqCursor.layoutReference())) {
    pointedLayoutR = eqCursor.layoutReference();
    cursorPosition = eqCursor.position();
  }
  KDPoint cursoredExpressionViewOrigin = pointedLayoutR.absoluteOrigin();
  KDCoordinate cursorX = expressionViewOrigin.x() + cursoredExpressionViewOrigin.x();
  if (cursorPosition == LayoutCursor::Position::Right) {
    cursorX += pointedLayoutR.layoutSize().width();
  }
  KDPoint cursorTopLeftPosition(cursorX, expressionViewOrigin.y() + cursoredExpressionViewOrigin.y() + pointedLayoutR.baseline() - m_cursor.baseline());
  m_cursorView.setFrame(KDRect(cursorTopLeftPosition, LayoutCursor::k_cursorWidth, m_cursor.cursorHeight()), force);
}

void LayoutField::setEditing(bool isEditing) {
  KDSize previousLayoutSize = m_contentView.minimalSizeForOptimalDisplay();
  if (m_contentView.setEditing(isEditing)) {
    reload(previousLayoutSize);
  }
}

CodePoint LayoutField::XNTCodePoint(CodePoint defaultXNTCodePoint) {
  CodePoint xnt = m_contentView.cursor()->layoutReference().XNTCodePoint();
  if (xnt != UCodePointNull) {
    return xnt;
  }
  return defaultXNTCodePoint;
}

void LayoutField::reload(KDSize previousSize) {
  layout().invalidAllSizesPositionsAndBaselines();
  KDSize newSize = minimalSizeForOptimalDisplay();
  if (m_delegate && previousSize.height() != newSize.height()) {
    m_delegate->layoutFieldDidChangeSize(this);
  }
  markRectAsDirty(bounds());
}

bool LayoutField::handleEventWithText(const char * text, bool indentation, bool forceCursorRightOfText) {
  /* The text here can be:
   * - the result of a key pressed, such as "," or "cos(•)"
   * - the text added after a toolbox selection
   * - the result of a copy-paste. */
  if (text[0] == 0) {
    // The text is empty
    return true;
  }

  int currentNumberOfLayouts = m_contentView.expressionView()->numberOfLayouts();
  if (currentNumberOfLayouts >= k_maxNumberOfLayouts - 6) {
    /* We add -6 because in some cases (Ion::Events::Division,
     * Ion::Events::Exp...) we let the layout cursor handle the layout insertion
     * and these events may add at most 6 layouts (e.g *10^). */
    return true;
  }

  // Handle special cases
  if (strcmp(text, Ion::Events::Division.text()) == 0) {
    m_contentView.cursor()->addFractionLayoutAndCollapseSiblings();
  } else if (strcmp(text, Ion::Events::Exp.text()) == 0) {
    m_contentView.cursor()->addEmptyExponentialLayout();
  } else if (strcmp(text, Ion::Events::Power.text()) == 0) {
    m_contentView.cursor()->addEmptyPowerLayout();
  } else if (strcmp(text, Ion::Events::Sqrt.text()) == 0) {
    m_contentView.cursor()->addEmptySquareRootLayout();
  } else if (strcmp(text, Ion::Events::Square.text()) == 0) {
    m_contentView.cursor()->addEmptySquarePowerLayout();
  } else if (strcmp(text, Ion::Events::EE.text()) == 0) {
    m_contentView.cursor()->addEmptyTenPowerLayout();
  } else if ((strcmp(text, "[") == 0) || (strcmp(text, "]") == 0)) {
    m_contentView.cursor()->addEmptyMatrixLayout();
  } else {
    Expression resultExpression = Expression::Parse(text);
    if (resultExpression.isUninitialized()) {
      // The text is not parsable (for instance, ",") and is added char by char.
      KDSize previousLayoutSize = minimalSizeForOptimalDisplay();
      m_contentView.cursor()->insertText(text);
      reload(previousLayoutSize);
      return true;
    }
    // The text is parsable, we create its layout an insert it.
    Layout resultLayout = resultExpression.createLayout(Poincare::Preferences::sharedPreferences()->displayMode(), Poincare::PrintFloat::k_numberOfStoredSignificantDigits);
    if (currentNumberOfLayouts + resultLayout.numberOfDescendants(true) >= k_maxNumberOfLayouts) {
      return true;
    }
    insertLayoutAtCursor(resultLayout, resultExpression, forceCursorRightOfText);
  }
  return true;
}

bool LayoutField::handleEvent(Ion::Events::Event event) {
  bool didHandleEvent = false;
  KDSize previousSize = minimalSizeForOptimalDisplay();
  bool shouldRecomputeLayout = m_contentView.cursor()->showEmptyLayoutIfNeeded();
  bool moveEventChangedLayout = false;
  if (privateHandleMoveEvent(event, &moveEventChangedLayout)) {
    if (!isEditing()) {
      setEditing(true);
    }
    shouldRecomputeLayout = shouldRecomputeLayout || moveEventChangedLayout;
    didHandleEvent = true;
  } else if (privateHandleSelectionEvent(event, &shouldRecomputeLayout)) {
    didHandleEvent = true;
    shouldRecomputeLayout = true; //TODO LEA
  } else if (privateHandleEvent(event)) {
    shouldRecomputeLayout = true;
    didHandleEvent = true;
  }
  /* Hide empty layout only if the layout is being edited, otherwise the cursor
   * is not visible so any empty layout should be visible. */
  bool didHideLayouts = isEditing() && m_contentView.cursor()->hideEmptyLayoutIfNeeded();
  if (!didHandleEvent) {
    return false;
  }
  shouldRecomputeLayout = didHideLayouts || shouldRecomputeLayout;
  if (shouldRecomputeLayout) {
    reload(previousSize);
  }
  m_contentView.cursorPositionChanged();
  scrollToCursor();
  return true;
}

void LayoutField::deleteSelection() {
  //TODO LEA
}

bool LayoutField::privateHandleEvent(Ion::Events::Event event) {
  if (m_delegate && m_delegate->layoutFieldDidReceiveEvent(this, event)) {
    return true;
  }
  if (handleBoxEvent(event)) {
    if (!isEditing()) {
      setEditing(true);
    }
    return true;
  }
  if (isEditing() && m_delegate->layoutFieldShouldFinishEditing(this, event)) { //TODO use class method?
    setEditing(false);
    if (m_delegate->layoutFieldDidFinishEditing(this, layout(), event)) {
      // Reinit layout for next use
      clearLayout();
    } else {
      setEditing(true);
    }
    return true;
  }
  /* if move event was not caught neither by privateHandleMoveEvent nor by
   * layoutFieldShouldFinishEditing, we handle it here to avoid bubbling the
   * event up. */
  if ((event == Ion::Events::Up || event == Ion::Events::Down || event == Ion::Events::Left || event == Ion::Events::Right) && isEditing()) {
    return true;
  }
  if ((event == Ion::Events::OK || event == Ion::Events::EXE) && !isEditing()) {
    setEditing(true);
    m_contentView.cursor()->setLayout(layout());
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
    return true;
  }
  if (event == Ion::Events::Back && isEditing()) {
    clearLayout();
    setEditing(false);
    m_delegate->layoutFieldDidAbortEditing(this);
    return true;
  }
  if (event.hasText() || event == Ion::Events::Paste || event == Ion::Events::Backspace) {
    if (!isEditing()) {
      setEditing(true);
    }
    if (event.hasText()) {
      handleEventWithText(event.text());
    } else if (event == Ion::Events::Paste) {
      handleEventWithText(Clipboard::sharedClipboard()->storedText(), false, true);
    } else {
      assert(event == Ion::Events::Backspace);
      m_contentView.cursor()->performBackspace();
    }
    return true;
  }
  if (event == Ion::Events::Clear && isEditing()) {
    clearLayout();
    return true;
  }
  return false;
}

static inline bool IsSimpleMoveEvent(Ion::Events::Event event) {
  return event == Ion::Events::Left
    || event == Ion::Events::Right
    || event == Ion::Events::Up
    || event == Ion::Events::Down;
}

bool LayoutField::privateHandleMoveEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  if (!IsSimpleMoveEvent(event)) {
    return false;
  }
  if (resetSelection()) {
    *shouldRecomputeLayout = true;
    return true;
  }
  LayoutCursor result;
  if (event == Ion::Events::Left) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Left, shouldRecomputeLayout);
  } else if (event == Ion::Events::Right) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Right, shouldRecomputeLayout);
  } else if (event == Ion::Events::Up) {
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Up, shouldRecomputeLayout);
  } else {
    assert(event == Ion::Events::Down);
    result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Down, shouldRecomputeLayout);
  }
  if (result.isDefined()) {
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

bool LayoutField::privateHandleSelectionEvent(Ion::Events::Event event, bool * shouldRecomputeLayout) {
  LayoutCursor result;
  if (event == Ion::Events::ShiftLeft || event == Ion::Events::ShiftRight) {
    Layout addedSelectionLeft;
    Layout addedSelectionRight;
    result = m_contentView.cursor()->selectAtDirection(event == Ion::Events::ShiftLeft ? LayoutCursor::MoveDirection::Left : LayoutCursor::MoveDirection::Right, shouldRecomputeLayout, &addedSelectionLeft, &addedSelectionRight);
    if (!addedSelectionLeft.isUninitialized() && !addedSelectionRight.isUninitialized()) { //TODO LEA assert?
      m_contentView.addSelection(addedSelectionLeft, addedSelectionRight);
    } else {
      return false;
    }
  }/* else if (event == Ion::Events::ShiftUp) {
    //TODO LEA result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Up, shouldRecomputeLayout);
  } else if (event == Ion::Events::ShiftDown) {
    //TODO LEA result = m_contentView.cursor()->cursorAtDirection(LayoutCursor::MoveDirection::Down, shouldRecomputeLayout);
  }*/
  if (result.isDefined()) {
    m_contentView.setCursor(result);
    return true;
  }
  return false;
}

void LayoutField::scrollRightOfLayout(Layout layoutR) {
  KDRect layoutRect(layoutR.absoluteOrigin().translatedBy(m_contentView.expressionView()->drawingOrigin()), layoutR.layoutSize());
  scrollToBaselinedRect(layoutRect, layoutR.baseline());
}

void LayoutField::scrollToBaselinedRect(KDRect rect, KDCoordinate baseline) {
  scrollToContentRect(rect, true);
  // Show the rect area around its baseline
  KDCoordinate underBaseline = rect.height() - baseline;
  KDCoordinate minAroundBaseline = minCoordinate(baseline, underBaseline);
  minAroundBaseline = minCoordinate(minAroundBaseline, bounds().height() / 2);
  KDRect balancedRect(rect.x(), rect.y() + baseline - minAroundBaseline, rect.width(), 2 * minAroundBaseline);
  scrollToContentRect(balancedRect, true);
}

void LayoutField::insertLayoutAtCursor(Layout layoutR, Poincare::Expression correspondingExpression, bool forceCursorRightOfLayout) {
  if (layoutR.isUninitialized()) {
    return;
  }

  KDSize previousSize = minimalSizeForOptimalDisplay();
  Poincare::LayoutCursor * cursor = m_contentView.cursor();

  // Handle empty layouts
  cursor->showEmptyLayoutIfNeeded();

  bool layoutWillBeMerged = layoutR.type() == LayoutNode::Type::HorizontalLayout;
  Layout lastMergedLayoutChild = (layoutWillBeMerged && layoutR.numberOfChildren() > 0) ? layoutR.childAtIndex(layoutR.numberOfChildren()-1) : Layout();

  // If the layout will be merged, find now where the cursor will point
  assert(!correspondingExpression.isUninitialized());
  Layout cursorMergedLayout = Layout();
  if (layoutWillBeMerged) {
    if (forceCursorRightOfLayout) {
      cursorMergedLayout = lastMergedLayoutChild;
    } else {
      cursorMergedLayout = layoutR.layoutToPointWhenInserting(&correspondingExpression);
      if (cursorMergedLayout == layoutR) {
        /* LayoutR will not be inserted in the layout, so point to its last
         * child instead. It is visually equivalent. */
        cursorMergedLayout = lastMergedLayoutChild;
      }
    }
  }

  // Add the layout. This puts the cursor at the right of the added layout
  cursor->addLayoutAndMoveCursor(layoutR);

  /* Move the cursor if needed.
   *
   * If forceCursorRightOfLayout is true and the layout has been merged, there
   * is no need to move the cursor because it already points to the right of the
   * added layouts.
   *
   * If the layout to point to has been merged, only its children have been
   * inserted in the layout. We already computed where the cursor should point,
   * because we cannot compute this now that the children are merged in between
   * another layout's children.
   *
   * For other cases, move the cursor to the layout indicated by
   * layoutToPointWhenInserting. This pointed layout cannot be computed before
   * adding layoutR, because addLayoutAndMoveCursor might have changed layoutR's
   * children.
   * For instance, if we add an absolute value with an empty child left of a 0,
   * the empty child is deleted and the 0 is collapsed into the absolute value.
   * Sketch of the situation, ' being the cursor:
   *  Initial layout:   '0
   *  "abs(x)" pressed in the toolbox => |•| is added, • being an empty layout
   *  Final layout: |0'|
   *
   * Fortunately, merged layouts' children are not modified by the merge, so it
   * is ok to compute their pointed layout before adding them.
   * */

  if (!forceCursorRightOfLayout) {
    if (!layoutWillBeMerged) {
      assert(cursorMergedLayout.isUninitialized());
      assert(!correspondingExpression.isUninitialized());
      cursorMergedLayout = layoutR.layoutToPointWhenInserting(&correspondingExpression);
    }
    assert(!cursorMergedLayout.isUninitialized());
    m_contentView.cursor()->setLayout(cursorMergedLayout);
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
  } else if (!layoutWillBeMerged) {
    m_contentView.cursor()->setLayout(layoutR);
    m_contentView.cursor()->setPosition(LayoutCursor::Position::Right);
  }

  // Handle matrices
  cursor->layoutReference().addGreySquaresToAllMatrixAncestors();

  // Handle empty layouts
  cursor->hideEmptyLayoutIfNeeded();

  // Reload
  reload(previousSize);
  if (!layoutWillBeMerged) {
    scrollRightOfLayout(layoutR);
  } else {
    assert(!lastMergedLayoutChild.isUninitialized());
    scrollRightOfLayout(lastMergedLayoutChild);
  }
  scrollToCursor();
}
