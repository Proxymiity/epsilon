#ifndef POINCARE_GRID_LAYOUT_NODE_H
#define POINCARE_GRID_LAYOUT_NODE_H

#include <poincare/layout_reference.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/empty_layout_node.h>

namespace Poincare {

class GridLayoutRef;
class MatrixLayoutNode;

class GridLayoutNode : public LayoutNode {
  friend class MatrixLayoutNode;
  friend class BinomialCoefficientLayoutNode;
  friend class BinomialCoefficientLayoutRef;
  friend class GridLayoutRef;
public:
  GridLayoutNode() :
    LayoutNode(),
    m_numberOfRows(0),
    m_numberOfColumns(0)
  {}

  virtual void setNumberOfRows(int numberOfRows) { m_numberOfRows = numberOfRows; }
  virtual void setNumberOfColumns(int numberOfColumns) { m_numberOfColumns = numberOfColumns; }
  KDSize gridSize() const { return KDSize(width(), height()); }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
  }

  // TreeNode
  size_t size() const override { return sizeof(GridLayoutNode); }
  void didAddChildAtIndex(int newNumberOfChildren) override;
  int numberOfChildren() const override { return m_numberOfRows * m_numberOfColumns; }
  void eraseNumberOfChildren() override {
    m_numberOfRows = 0;
    m_numberOfColumns = 0;
  }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "GridLayout";
  }
#endif

protected:
  // GridLayoutNode
  virtual void addEmptyRow(EmptyLayoutNode::Color color);
  virtual void addEmptyColumn(EmptyLayoutNode::Color color);
  virtual void deleteRowAtIndex(int index);
  virtual void deleteColumnAtIndex(int index);
  bool childIsRightOfGrid(int index) const;
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  int rowAtChildIndex(int index) const;
  int columnAtChildIndex(int index) const;
  int indexAtRowColumn(int rowIndex, int columnIndex) const;
  int m_numberOfRows;
  int m_numberOfColumns;

  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  // GridLayoutNode
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i) const;
  KDCoordinate height() const;
  KDCoordinate columnWidth(int j) const;
  KDCoordinate width() const;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class GridLayoutRef : public LayoutReference {
public:
  GridLayoutRef(const GridLayoutNode * n) : LayoutReference(n) {}
  GridLayoutRef() : LayoutReference(TreePool::sharedPool()->createTreeNode<GridLayoutNode>()) {}
  void setDimensions(int rows, int columns);
  void addChildAtIndex(LayoutReference l, int index, int currentNumberOfChildren, LayoutCursor * cursor) override {
    LayoutReference::addChildAtIndex(l, index, currentNumberOfChildren, cursor);
  }
private:
  virtual GridLayoutNode * node() const { return static_cast<GridLayoutNode *>(LayoutReference::node()); }
  void setNumberOfRows(int rows) {
    assert(rows >= 0);
    node()->setNumberOfRows(rows);
  }
  void setNumberOfColumns(int columns) {
    assert(columns >= 0);
    node()->setNumberOfColumns(columns);
  }
};

}

#endif
