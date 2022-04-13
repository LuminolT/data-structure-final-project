#ifndef INCLUDE_BPTREE_H_
#define INCLUDE_BPTREE_H_

#include "bpnode.h"

template <typename T>
class bptree {
  public:
    bptree(int mcl, mll);
    ~bptree();
    int get_max_child_limit() { return MAX_CHILD_LIMIT; }
    int get_max_leaf_node_limit() { return MAX_LEAF_NODE_LIMIT; }
    void insert(int key, T value);
    void remove(int key);
    void search(int key);

  protected:
    const int MAX_CHILD_LIMIT;      // Limit for internal nodes
    const int MAX_LEAF_NODE_LIMIT;  // Limit for leaf nodes
    bpnode<T>* root;                // Root of the B+Tree
};

#endif  // INCLUDE_BPTREE_H_