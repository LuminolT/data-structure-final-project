#ifndef INCLUDE_BPTREE_H_
#define INCLUDE_BPTREE_H_

#include <sys/stat.h>

#include <cmath>
#include <functional>

#include "bpnode.h"

template <class T, std::size_t ORDER>
class bptree {
  public:
    // Default Constructor
    bptree(std::string);

    // Destructor
    ~bptree();

    // Order related arguments
    // constexpr int get_max_internal_node_limit() { return ceil(ORDER / 2); }
    constexpr int get_max_internal_node_limit() { return ORDER - 1; }
    constexpr int get_max_leaf_node_limit() { return ORDER - 1; }

    // Insert <key,value> to the B+ tree
    void insert(int key, T value);

    // Remove <key> in the B+ tree
    void remove(int key);

    // Search <key> in the B+ tree and call the function
    void search(int key, std::function<void(T&)> func, int mode = 0) {
        search(key, key, func, mode);
    }

    // Search <st~ed> in the B+ tree and call the function
    void search(int st, int ed, std::function<void(T&)> func, int mode = 0) {
        range_search(st, ed, func, mode);
    }

  protected:
    page_id_t root_;           // Root of the B+Tree
    std::string folder_name_;  // Folder name of the B+Tree
    int page_id_counter_;      // Counter of the page id.

    // Update the parent node after insert
    void insert_update_parent(page_id_t par, page_id_t cur, int key);

    // Update the parent node after remove
    void remove_update_parent(page_id_t par, page_id_t cur, int key);

    // Range search (mode 0 denotes repeartedly search)
    void range_search(int key_start, int key_end, std::function<void(T&)>, int mode);
};

template <class T, std::size_t ORDER>
bptree<T, ORDER>::bptree(std::string folder_name) {
    folder_name_ = folder_name;
    // get the root_ page_id
    struct stat buf;
    errno_t err = 0;
    if (stat(std::string(folder_name_ + "/root.txt").c_str(), &buf) != 0) {
        root_ = -1;
        page_id_counter_ = 0;
    } else {
        std::ifstream root_file(folder_name_ + "/root.txt");
        root_file >> root_;
        root_file >> page_id_counter_;
        root_file.close();
    }
}

template <class T, std::size_t ORDER>
bptree<T, ORDER>::~bptree() {
    std::ofstream root_file(folder_name_ + "/root.txt");
    root_file << root_ << std::endl;
    root_file << page_id_counter_ << std::endl;
    root_file.close();
}

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::insert(int key, T value) {
    // Notes:
    // Here is a little 'bug' in insert function
    // acturally in prev_ptr and next_ptr (or pages)
    // the prev ptr may be wrong.
    // It could be configured, but considering the simplicity of the code,
    // I don't think it is necessary. :-)
    // Updated: first's prev is -1, so do last's next

    if (root_ == -1) {  // case of empty tree
        // generate a new root
        auto tmp_node = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
        tmp_node.is_leaf_ = true;
        tmp_node.key_num_ = 1;
        tmp_node.next_page_ = -1;
        tmp_node.prev_page_ = -1;
        tmp_node.parent_page_ = -1;
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
        cur_page_id =
            cur_node.sub_ptrs_[std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                               cur_node.keys_.begin()];
        // std::cout << cur_page_id << std::endl;   // DEBUG
        cur_node = bpnode<T, ORDER>(cur_page_id, folder_name_);
    }

    // insert key-value
    int key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                  cur_node.keys_.begin();
    cur_node.keys_.insert(cur_node.keys_.begin() + key_pos, key);
    cur_node.values_.insert(cur_node.values_.begin() + key_pos, value);
    cur_node.key_num_++;
    if (cur_node.key_num_ >= get_max_leaf_node_limit()) {
        // NOW we have to split the nodes
        auto new_node = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
        new_node.keys_ = std::vector<int>(
            cur_node.keys_.begin() + ceil(get_max_leaf_node_limit() / 2), cur_node.keys_.end());
        new_node.values_ = std::vector<T>(
            cur_node.values_.begin() + ceil(get_max_leaf_node_limit() / 2), cur_node.values_.end());
        new_node.is_leaf_ = true;
        new_node.key_num_ = new_node.keys_.size();
        new_node.parent_page_ = cur_node.parent_page_;
        new_node.next_page_ = cur_node.next_page_;
        new_node.prev_page_ = cur_node.page_id_;
        cur_node.next_page_ = new_node.page_id_;
        cur_node.keys_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.values_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.key_num_ = cur_node.keys_.size();
        // update the parent node
        if (cur_page_id == root_) {  // cur_node is the root node
            // create a new root
            auto new_root = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
            new_root.is_leaf_ = false;
            new_root.key_num_ = 1;
            new_root.keys_.emplace_back(new_node.keys_[0]);
            new_root.sub_ptrs_.emplace_back(cur_node.page_id_);
            new_root.sub_ptrs_.emplace_back(new_node.page_id_);
            new_root.parent_page_ = -1;
            new_root.next_page_ = new_root.page_id_;
            new_root.prev_page_ = new_root.page_id_;
            root_ = new_root.page_id_;
            // update child's parent
            cur_node.parent_page_ = new_root.page_id_;
            new_node.parent_page_ = new_root.page_id_;
        } else {  // cur_node is the internal node
            // insert new key in parent node
            insert_update_parent(par_page_id, new_node.page_id_,
                                 new_node.keys_[0]);  // recursion
        }
    }
}

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::insert_update_parent(page_id_t par_page_id, page_id_t new_page_id, int key) {
    // Note:
    // This function works when the child node is splitted,
    // par_page_id denotes the parent node of the left-splitted child,
    // new_page_id denotes the right-splitted child
    // key denotes the key value of right sib.
    // (It works when split the internal nodes)
    auto par_node = bpnode<T, ORDER>(par_page_id, folder_name_);
    int key_pos = std::upper_bound(par_node.keys_.begin(), par_node.keys_.end(), key) -
                  par_node.keys_.begin();
    par_node.keys_.insert(par_node.keys_.begin() + key_pos, key);
    par_node.sub_ptrs_.insert(par_node.sub_ptrs_.begin() + key_pos + 1, new_page_id);
    par_node.key_num_++;
    if (par_node.key_num_ >= get_max_internal_node_limit()) {
        // SPLIIIIIT
        // NOTE Behavior wrong!!!!
        // TO[x]DO: Debug
        // 3 5 7 9
        //    7
        //  ↙  ↘
        // 3,5  9
        // 7 as a new value, add to it's parent.
        // if it doesn't have parent? it become a new parent node.

        // Firstly, SPLIT
        auto right_sib_node = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
        right_sib_node.keys_ =
            std::vector<int>(par_node.keys_.begin() + floor(get_max_internal_node_limit() / 2 + 1),
                             par_node.keys_.end());
        right_sib_node.sub_ptrs_ = std::vector<page_id_t>(
            par_node.sub_ptrs_.begin() + floor(get_max_internal_node_limit() / 2 + 1),
            par_node.sub_ptrs_.end());
        right_sib_node.key_num_ = right_sib_node.keys_.size();
        right_sib_node.parent_page_ = par_node.parent_page_;
        right_sib_node.next_page_ = par_node.next_page_;
        right_sib_node.prev_page_ = par_node.page_id_;
        // Get the '7' in example
        auto add_key = par_node.keys_[floor(get_max_internal_node_limit() / 2)];
        // resize the previous parent
        par_node.next_page_ = right_sib_node.page_id_;
        par_node.keys_.resize(floor(get_max_internal_node_limit() / 2));
        par_node.sub_ptrs_.resize(floor(get_max_internal_node_limit() / 2) + 1);
        par_node.key_num_ = par_node.keys_.size();

        // Secondly, UPDATE PARENT!
        if (par_page_id == root_) {  // par_node is the root node
            // create a new root
            auto new_root = bpnode<T, ORDER>(++page_id_counter_, folder_name_);
            new_root.is_leaf_ = false;
            new_root.key_num_ = 1;
            new_root.keys_.emplace_back(add_key);
            new_root.sub_ptrs_.emplace_back(par_node.page_id_);
            new_root.sub_ptrs_.emplace_back(right_sib_node.page_id_);
            new_root.parent_page_ = -1;
            new_root.next_page_ = new_root.page_id_;
            new_root.prev_page_ = new_root.page_id_;
            root_ = new_root.page_id_;
            // update child's parent
            par_node.parent_page_ = new_root.page_id_;
            right_sib_node.parent_page_ = new_root.page_id_;
        } else {  // par_node is the internal node
            // insert new key in parent node
            insert_update_parent(par_node.parent_page_, right_sib_node.page_id_,
                                 add_key);  // recursion AGAIN!
        }
    }
}

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::range_search(int key_start, int key_end, std::function<void(T&)> func,
                                    int mode) {
    // error handling
    if (key_end < key_start) {
        throw std::invalid_argument("search: key_end < key_start");
    }
    if (root_ == -1) {
        throw std::runtime_error("search: tree is empty!");
    }

    auto cur_node = bpnode<T, ORDER>(root_, folder_name_);
    while (!cur_node.is_leaf_) {
        auto key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key_start) -
                       cur_node.keys_.begin();
        cur_node = bpnode<T, ORDER>(cur_node.sub_ptrs_[key_pos], folder_name_);
    }
    // Now, cur_node is the leaf node
    // Get the key position
    auto key_pos = std::lower_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key_start) -
                   cur_node.keys_.begin();
    if (key_pos >= cur_node.key_num_ || cur_node.keys_[key_pos] != key_start) {
        throw std::runtime_error("search: key not found!");
    }
    if (mode == 1) {
        func(cur_node.keys_[key_pos]);
    } else {
        // Now we need a loop
        while (cur_node.keys_[key_pos] <= key_end) {
            func(cur_node.keys_[key_pos]);
            key_pos++;
            if (key_pos == cur_node.key_num_) {
                // go to next leaf
                if (cur_node.next_page_ == -1) {
                    break;
                }
                cur_node = bpnode<T, ORDER>(cur_node.next_page_, folder_name_);
                key_pos = 0;
            }
        }
    }
}

template <class T, std::size_t ORDER>
void bptree<T, ORDER>::remove(int key) {
    // error handling
    if (root_ == -1) {
        throw std::runtime_error("remove: tree is empty!");
    }

    auto cur_node = bpnode<T, ORDER>(root_, folder_name_);
    while (!cur_node.is_leaf_) {
        auto key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                       cur_node.keys_.begin();
        cur_node = bpnode<T, ORDER>(cur_node.sub_ptrs_[key_pos], folder_name_);
    }
    // Now, cur_node is the leaf node
    // Get the key position
    auto key_pos = std::lower_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                   cur_node.keys_.begin();
    if (key_pos >= cur_node.key_num_ || cur_node.keys_[key_pos] != key) {
        throw std::runtime_error("remove: key not found!");
    }
    // Now, we can remove the key
    cur_node.keys_.erase(cur_node.keys_.begin() + key_pos);
    cur_node.key_num_--;
    // balance!
    if (cur_node.key_num < ceil(get_max_leaf_node_limit / 2)) {
        // steal from left sibling
        // merge with left sibling
        // steal from right sibling
        // merge with right sibling
    } else {
        // reset the parent's key-ptr
    }
}

#endif  // INCLUDE_BPTREE_H_