#ifndef POINCARE_GHOST_NODE_H
#define POINCARE_GHOST_NODE_H

#include "tree_node.h"

namespace Poincare {

class GhostNode : public TreeNode {
public:
  // TreeNode
  int numberOfChildren() const override { return 0; }
  size_t size() const override { return sizeof(GhostNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "Ghost";
  }
#endif

  // Ghost
  bool isGhost() const override { return true; }
  // Uninitialized
  TreeNode * uninitializedStaticNode() const override;
};

}

#endif
