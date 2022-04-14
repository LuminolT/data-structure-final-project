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
};

template <class T, std::size_t ORDER>
bptree<T, ORDER>::bptree(std::string folder_name) {
    folder_name_ = folder_name;
    // get the root_ page_id
    struct stat buf;
    errno_t err = 0;
    if (stat(std::string(folder_name_ + "/root.txt").c_str(), &buf) != 0) {
        root_ = -1;
    } else {
        std::ifstream root_file(folder_name_ + "/root.txt");
        root_file >> root_;
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
        auto tmp_node = bpnode<T, ORDER>(1, folder_name_);
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
    // while (!tmp_node.is_leaf_) {
    //     auto tmp_node = bpnode<T, ORDER>(tmp_page_id, folder_name_);
    //     // compare key value
    //     for (int i = 0; i < tmp_node.key_num_; ++i) {
    //         if (key < tmp_node.keys_[i]) {
    //             tmp_page_id = tmp_node.sub_ptrs_[i];
    //             break;
    //         }
    //     }
    //     // if key is in the right-hand-side
    //     if (tmp_page_id == tmp_node.page_id_) {
    //         tmp_page_id = tmp_node.sub_ptrs_[tmp_node.key_num_ + 1];
    //     }
    // }

    // insert key-value
    if (cur_node.key_num_ < get_max_leaf_node_limit()) {
        int key_pos = std::upper_bound(cur_node.keys_.begin(),
                                       cur_node.keys_.end(), key) -
                      cur_node.keys_.begin();
        cur_node.keys_.insert(cur_node.keys_.begin() + key_pos, key);
        cur_node.values_.insert(cur_node.values_.begin() + key_pos, value);
    } else {
        // split the nooooode
        }
    // if (key_num < ORDER - 1) {
    //     tmp_node.insert_key_value(key, value);
    // } else {  // no room for new key-value
    //     // find left sibling
    //     auto l_sib = bpnode<T, ORDER>(tmp_node.prev_page_, folder_name_);
    //     // TODO: need split ~~~ & a global counter
    //     if (l_sib.key_num_ < ORDER - 1) {
    //         if (key < tmp_node.keys_[0]) {
    //         }
    //     }
    //     // find right sibling
    //     auto r_sib = bpnode<T, ORDER>(tmp_node.next_page_, folder_name_);
    //     //
    // }
}

#endif  // INCLUDE_BPTREE_H_