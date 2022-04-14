#ifndef INCLUDE_BPTREE_H_
#define INCLUDE_BPTREE_H_

#include <sys/stat.h>

#include <cmath>

#include "bpnode.h"

template <class T, std::size_t ORDER>
class bptree {
  public:
    // Default Constructor
    bptree(std::string);

    // Destructor
    ~bptree();
    int get_max_child_limit() { return ceil(ORDER / 2); }
    int get_max_leaf_node_limit() { return ORDER - 1; }

    // Insert <key,value> to the B+ tree
    void insert(int key, T value);
    void remove(int key);
    void search(int key);
    void search(int key_begin, int key_end);

  protected:
    page_id_t root_;           // Root of the B+Tree
    std::string folder_name_;  // Folder name of the B+Tree
    int page_id_counter        // Counter of the page id.
};

template <class T, std::size_t ORDER>
bptree<T, ORDER>::bptree(std::string folder_name) {
    folder_name_ = folder_name;
    // get the root_ page_id
    struct stat buf;
    errno_t err = 0;
    if (stat(std::string(folder_name_ + "/root.txt").c_str(), &buf) != 0) {
        root_ = -1;
        page_id_counter = 0;
    } else {
        std::ifstream root_file(folder_name_ + "/root.txt");
        root_file >> root_;
        root_file >> page_id_counter;
        root_file.close();
    }
}

template <class T, std::size_t ORDER>
bptree<T, ORDER>::~bptree() {
    std::ofstream root_file(folder_name_ + "/root.txt");
    root_file << root_;
    root_file.close();
}

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::insert(int key, T value) {
    if (root_ == -1) {  // case of empty tree
        auto tmp_node = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
        tmp_node.is_leaf_ = true;
        tmp_node.key_num_ = 1;
        tmp_node.next_page_ = tmp_node.page_id_;
        tmp_node.prev_page_ = tmp_node.page_id_;
        tmp_node.keys_.push_back(key);
        tmp_node.values_.push_back(value);
        root_ = tmp_node.page_id_;
        return;
    }
    page_id_t cur_page_id = root_;
    page_id_t par_page_id = 0;
    auto cur_node = bpnode<T, ORDER>(cur_page_id, folder_name_);
    // get the leaf node
    while (!cur_node.is_leaf_) {
        par_page_id = cur_page_id;
        cur_page_id = std::upper_bound(cur_node.keys_.begin(),
                                       cur_node.keys_.end(), key) -
                      cur_node.keys_.begin();
        cur_node = bpnode<T, ORDER>(cur_page_id, folder_name_);
    }

    // insert key-value
    if (cur_node.key_num_ < get_max_leaf_node_limit()) {
        int key_pos = std::upper_bound(cur_node.keys_.begin(),
                                       cur_node.keys_.end(), key) -
                      cur_node.keys_.begin();
        cur_node.keys_.insert(cur_node.keys_.begin() + key_pos, key);
        cur_node.values_.insert(cur_node.values_.begin() + key_pos, value);
    } else {
        // NOW we have to split the nodes
        // insert first
        int key_pos = std::upper_bound(cur_node.keys_.begin(),
                                       cur_node.keys_.end(), key) -
                      cur_node.keys_.begin();
        cur_node.keys_.insert(cur_node.keys_.begin() + key_pos, key);
        cur_node.values_.insert(cur_node.values_.begin() + key_pos, value);
        // BOOOM, now the time to SPLIT.
        // create a new node (right sibling)
        auto new_node = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
        new_node.keys_ = std::vector<int>(
            cur_node.keys_.begin() + ceil(get_max_leaf_node_limit() / 2),
            cur_node.keys_.end());
        new_node.values_ = std::vector<T>(
            cur_node.values_.begin() + ceil(get_max_leaf_node_limit() / 2),
            cur_node.values_.end());
        new_node.is_leaf_ = true;
        new_node.key_num_ = new_node.keys_.size();
        new_node.next_page_ = cur_node.next_page_;
        new_node.prev_page_ = cur_node.page_id_;
        cur_node.next_page_ = new_node.page_id_;
        cur_node.keys_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.values_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.key_num_ = cur_node.keys_.size();
        // update the parent node
        if (par_page_id == 1) {  // cur_node is the root node
            // create a new root
            auto new_root = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
            new_root.is_leaf_ = false;
            new_root.key_num_ = 1;
            new_root.keys_.emplace_back(new_node.keys_[0]);
            new_root.sub_ptrs_.emplace_back(cur_node.page_id_);
            new_root.sub_ptrs_.emplace_back(new_node.page_id_);
            root_ = new_root.page_id_;
        } else {  // cur_node is the internal node
            // insert new key in parent node
            // TODO TODO
        }
    }
}

#endif  // INCLUDE_BPTREE_H_