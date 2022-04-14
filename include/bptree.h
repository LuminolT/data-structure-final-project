#ifndef INCLUDE_BPTREE_H_
#define INCLUDE_BPTREE_H_

#include <cmath>

#include "bpnode.h"

template <class T, std::size_t ORDER>
class bptree {
  public:
    // Default Constructor
    bptree() = default;

    // Destructor
    ~bptree() = default;
    int get_max_child_limit() { return ceil(ORDER / 2); }
    int get_max_leaf_node_limit() { return ORDER - 1; }

    // Insert <key,value> to the B+ tree
    void insert(int key, T value);
    void remove(int key);
    void search(int key);
    void search(int key_begin, int key_end);

  protected:
    page_id_t root_;  // Root of the B+Tree
};

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::insert(int key, T value) {
    if (root_ == 0) {
        root_ = new
    }
}

#endif  // INCLUDE_BPTREE_H_