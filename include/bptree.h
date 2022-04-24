/*!
 * @file bptree.h
 * @author Luminolt
 * @brief B+tree tree class
 */

#ifndef INCLUDE_BPTREE_H_
#define INCLUDE_BPTREE_H_

#include <sys/stat.h>

#include <algorithm>
#include <cmath>
#include <functional>

#include "bpnode.h"

/*!
 * @brief template clss for bp tree
 * @tparam KT key type
 * @tparam VT value type
 * @tparam ORDER order of b+tree
 * @brief B+Tree Node
 *      - an on-file b+tree
 *      - methods including insert, remove and search(with edit)
 */
template <class KT, class VT, std::size_t ORDER>
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
    void insert(KT key, VT value);

    // Remove <key> in the B+ tree
    void remove(KT key);

    // Search <key> in the B+ tree and call the function
    void search(KT key, std::function<void(VT &)> func, int mode = 0) {
        range_search(key, key, func, mode);
    }

    // Search <st~ed> in the B+ tree and call the function
    void search(int st, int ed, std::function<void(VT &)> func, int mode = 0) {
        range_search(st, ed, func, mode);
    }

protected:
    page_id_t root_;           // Root of the B+VTree
    std::string folder_name_;  // Folder name of the B+VTree
    int page_id_counter_;      // Counter of the page id.

    // Update the parent node after insert
    void insert_update_parent(page_id_t par, page_id_t cur, KT key);

    // Update the parent node after remove
    void remove_update_parent(bpnode<KT, VT, ORDER> &node, page_id_t child_page);

    // Range search (mode 0 denotes repeartedly search)
    void range_search(KT key_start, KT key_end, std::function<void(VT &)>, int mode);
};

template <class KT, class VT, std::size_t ORDER>
bptree<KT, VT, ORDER>::bptree(std::string folder_name) {
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

template <class KT, class VT, std::size_t ORDER>
bptree<KT, VT, ORDER>::~bptree() {
    std::ofstream root_file(folder_name_ + "/root.txt");
    root_file << root_ << std::endl;
    root_file << page_id_counter_ << std::endl;
    root_file.close();
}

template <class KT, class VT, std::size_t ORDER>
void bptree<KT, VT, ORDER>::insert(KT key, VT value) {
    // Notes:
    // Here is a little 'bug' in insert function
    // acturally in prev_ptr and next_ptr (or pages)
    // the prev ptr may be wrong.
    // It could be configured, but considering the simplicity of the code,
    // I don't think it is necessary. :-)
    // Updated: first's prev is -1, so do last's next

    if (root_ == -1) {  // case of empty tree
        // generate a new root
        bpnode<KT, VT, ORDER> tmp_node = bpnode<KT, VT, ORDER>(++page_id_counter_, folder_name_);
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
    bpnode<KT, VT, ORDER> cur_node = bpnode<KT, VT, ORDER>(cur_page_id, folder_name_);
    // get the leaf node
    while (!cur_node.is_leaf_) {
        par_page_id = cur_page_id;
        cur_page_id =
            cur_node.sub_ptrs_[std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                               cur_node.keys_.begin()];
        // std::cout << cur_page_id << std::endl;   // DEBUG
        cur_node = bpnode<KT, VT, ORDER>(cur_page_id, folder_name_);
    }

    // insert key-value
    int key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                  cur_node.keys_.begin();
    cur_node.keys_.insert(cur_node.keys_.begin() + key_pos, key);
    cur_node.values_.insert(cur_node.values_.begin() + key_pos, value);
    cur_node.key_num_++;
    if (cur_node.key_num_ >= get_max_leaf_node_limit()) {
        // NOW we have to split the nodes
        bpnode<KT, VT, ORDER> new_node = bpnode<KT, VT, ORDER>(++page_id_counter_, folder_name_);
        new_node.keys_ = std::vector<KT>(
            cur_node.keys_.begin() + ceil(get_max_leaf_node_limit() / 2), cur_node.keys_.end());
        new_node.values_ = std::vector<VT>(
            cur_node.values_.begin() + ceil(get_max_leaf_node_limit() / 2), cur_node.values_.end());
        new_node.is_leaf_ = true;
        new_node.key_num_ = new_node.keys_.size();
        new_node.parent_page_ = cur_node.parent_page_;
        new_node.next_page_ = cur_node.next_page_;
        new_node.prev_page_ = cur_node.page_id_;
        cur_node.next_page_ = new_node.page_id_;
        if (new_node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> tmp_node =
                bpnode<KT, VT, ORDER>(new_node.next_page_, folder_name_);
            tmp_node.prev_page_ = new_node.page_id_;
        }
        cur_node.keys_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.values_.resize(floor(get_max_leaf_node_limit() / 2));
        cur_node.key_num_ = cur_node.keys_.size();
        // update the parent node
        if (cur_page_id == root_) {  // cur_node is the root node
            // create a new root
            bpnode<KT, VT, ORDER> new_root =
                bpnode<KT, VT, ORDER>(++page_id_counter_, folder_name_);
            new_root.is_leaf_ = false;
            new_root.key_num_ = 1;
            new_root.keys_.emplace_back(new_node.keys_[0]);
            new_root.sub_ptrs_.emplace_back(cur_node.page_id_);
            new_root.sub_ptrs_.emplace_back(new_node.page_id_);
            new_root.parent_page_ = -1;
            new_root.next_page_ = -1;
            new_root.prev_page_ = -1;
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

template <class KT, class VT, std::size_t ORDER>
void bptree<KT, VT, ORDER>::insert_update_parent(page_id_t par_page_id, page_id_t new_page_id,
                                                 KT key) {
    // Note:
    // VThis function works when the child node is splitted,
    // par_page_id denotes the parent node of the left-splitted child,
    // new_page_id denotes the right-splitted child
    // key denotes the key value of right sib.
    // (It works when split the internal nodes)
    bpnode<KT, VT, ORDER> par_node = bpnode<KT, VT, ORDER>(par_page_id, folder_name_);
    int key_pos = std::upper_bound(par_node.keys_.begin(), par_node.keys_.end(), key) -
                  par_node.keys_.begin();
    par_node.keys_.insert(par_node.keys_.begin() + key_pos, key);
    par_node.sub_ptrs_.insert(par_node.sub_ptrs_.begin() + key_pos + 1, new_page_id);
    par_node.key_num_++;
    if (par_node.key_num_ >= get_max_internal_node_limit()) {
        // SPLIIIIIVT
        // NOVTE Behavior wrong!!!!
        // VTO[x]DO: Debug
        // 3 5 7 9
        //    7
        //  ↙  ↘
        // 3,5  9
        // 7 as a new value, add to it's parent.
        // if it doesn't have parent? it become a new parent node.

        // Firstly, SPLIVT
        bpnode<KT, VT, ORDER> right_sib_node =
            bpnode<KT, VT, ORDER>(++page_id_counter_, folder_name_);
        right_sib_node.keys_ =
            std::vector<KT>(par_node.keys_.begin() + floor(get_max_internal_node_limit() / 2 + 1),
                            par_node.keys_.end());
        right_sib_node.sub_ptrs_ = std::vector<page_id_t>(
            par_node.sub_ptrs_.begin() + floor(get_max_internal_node_limit() / 2 + 1),
            par_node.sub_ptrs_.end());
        right_sib_node.key_num_ = right_sib_node.keys_.size();
        right_sib_node.parent_page_ = par_node.parent_page_;
        right_sib_node.next_page_ = par_node.next_page_;
        right_sib_node.prev_page_ = par_node.page_id_;
        if (right_sib_node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> tmp_node =
                bpnode<KT, VT, ORDER>(right_sib_node.next_page_, folder_name_);
            tmp_node.prev_page_ = right_sib_node.page_id_;
        }
        // Get the '7' in example
        auto add_key = par_node.keys_[floor(get_max_internal_node_limit() / 2)];
        // resize the previous parent
        par_node.next_page_ = right_sib_node.page_id_;
        par_node.keys_.resize(floor(get_max_internal_node_limit() / 2));
        par_node.sub_ptrs_.resize(floor(get_max_internal_node_limit() / 2) + 1);
        par_node.key_num_ = par_node.keys_.size();

        // Secondly, UPDAVTE PARENVT!
        if (par_page_id == root_) {  // par_node is the root node
            // create a new root
            auto new_root = bpnode<KT, VT, ORDER>(++page_id_counter_, folder_name_);
            new_root.is_leaf_ = false;
            new_root.key_num_ = 1;
            new_root.keys_.emplace_back(add_key);
            new_root.sub_ptrs_.emplace_back(par_node.page_id_);
            new_root.sub_ptrs_.emplace_back(right_sib_node.page_id_);
            new_root.parent_page_ = -1;
            new_root.next_page_ = -1;
            new_root.prev_page_ = -1;
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

template <class KT, class VT, std::size_t ORDER>
void bptree<KT, VT, ORDER>::range_search(KT key_start, KT key_end, std::function<void(VT &)> func,
                                         int mode) {
    // error handling
    if (key_end < key_start) {
        throw std::invalid_argument("search: key_end < key_start");
    }
    if (root_ == -1) {
        throw std::runtime_error("search: tree is empty!");
    }

    bpnode<KT, VT, ORDER> cur_node = bpnode<KT, VT, ORDER>(root_, folder_name_);
    while (!cur_node.is_leaf_) {
        auto key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key_start) -
                       cur_node.keys_.begin();
        cur_node = bpnode<KT, VT, ORDER>(cur_node.sub_ptrs_[key_pos], folder_name_);
    }
    // Now, cur_node is the leaf node
    // Get the key position
    auto key_pos = std::lower_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key_start) -
                   cur_node.keys_.begin();
    if (key_pos >= cur_node.key_num_ || cur_node.keys_[key_pos] < key_start) {
        throw std::runtime_error("search: key not found!");
    }
    if (mode == 1) {
        func(cur_node.values_[key_pos]);
    } else {
        // Now we need a loop
        while (cur_node.keys_[key_pos] <= key_end) {
            func(cur_node.values_[key_pos]);
            key_pos++;
            // go to next leaf
            if (key_pos == cur_node.key_num_) {
                // loop til the end~~~
                if (cur_node.next_page_ == -1) {
                    break;
                }
                cur_node = bpnode<KT, VT, ORDER>(cur_node.next_page_, folder_name_);
                key_pos = 0;
            }
        }
    }
}

template <class KT, class VT, std::size_t ORDER>
void bptree<KT, VT, ORDER>::remove(KT key) {
    // Note
    // [] VTODO
    // Here is sth wrong with the delete method...
    // especially at the merge part
    // if we merge to left sibling, the parent key change may be complicated

    // error handling
    if (root_ == -1) {
        throw std::runtime_error("remove: tree is empty!");
    }

    bpnode<KT, VT, ORDER> cur_node = bpnode<KT, VT, ORDER>(root_, folder_name_);
    while (!cur_node.is_leaf_) {
        auto key_pos = std::upper_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                       cur_node.keys_.begin();
        cur_node = bpnode<KT, VT, ORDER>(cur_node.sub_ptrs_[key_pos], folder_name_);
    }
    // Now, cur_node is the leaf node
    // Get the key position
    auto key_pos = std::lower_bound(cur_node.keys_.begin(), cur_node.keys_.end(), key) -
                   cur_node.keys_.begin();
    if (key_pos >= cur_node.key_num_ || cur_node.keys_[key_pos] != key) {
        throw std::runtime_error("remove: key not found!");
    }
    // Now, we can remove the key
    auto tmp_front_key = cur_node.keys_.front();  // record the front first~
    cur_node.keys_.erase(cur_node.keys_.begin() + key_pos);
    cur_node.values_.erase(cur_node.values_.begin() + key_pos);
    cur_node.key_num_--;
    // balance! note that here set zero for balance.
    // cuz in the file system we may spend more time on data stealing
    if (cur_node.key_num_ == 0) {
        // steal from left sibling
        if (cur_node.prev_page_ != -1) {
            bpnode<KT, VT, ORDER> left_sibling =
                bpnode<KT, VT, ORDER>(cur_node.prev_page_, folder_name_);
            if (left_sibling.key_num_ >= ceil(get_max_leaf_node_limit() / 2)) {
                // transfer the maximum keys from the left sibling
                cur_node.keys_.emplace(cur_node.keys_.begin(), left_sibling.keys_.back());
                cur_node.values_.emplace(cur_node.values_.begin(), left_sibling.values_.back());
                cur_node.key_num_++;
                left_sibling.keys_.pop_back();
                left_sibling.values_.pop_back();
                left_sibling.key_num_--;
                // update parent
                if (cur_node.parent_page_ != -1) {
                    bpnode<KT, VT, ORDER> par_node =
                        bpnode<KT, VT, ORDER>(cur_node.parent_page_, folder_name_);
                    auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                                    tmp_front_key) -
                                   par_node.keys_.begin();
                    par_node.keys_[key_pos] = cur_node.keys_.front();
                }
                return;
            }
        }
        // steal from right sibling
        if (cur_node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> right_sibling =
                bpnode<KT, VT, ORDER>(cur_node.next_page_, folder_name_);
            if (right_sibling.key_num_ >= ceil(get_max_leaf_node_limit() / 2)) {
                // transfer the minimum keys from the right sibling
                tmp_front_key = right_sibling.keys_.front();
                cur_node.keys_.emplace_back(right_sibling.keys_.front());
                cur_node.values_.emplace_back(right_sibling.values_.front());
                cur_node.key_num_++;
                right_sibling.keys_.erase(right_sibling.keys_.begin());
                right_sibling.values_.erase(right_sibling.values_.begin());
                right_sibling.key_num_--;
                // update parent
                if (cur_node.parent_page_ != -1) {
                    bpnode<KT, VT, ORDER> par_node =
                        bpnode<KT, VT, ORDER>(cur_node.parent_page_, folder_name_);
                    auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                                    tmp_front_key) -
                                   par_node.keys_.begin();
                    par_node.keys_[key_pos] = right_sibling.keys_.front();
                }
                return;
            }
        }
        // merge with left sibling
        if (cur_node.prev_page_ != -1) {
            bpnode<KT, VT, ORDER> left_sibling =
                bpnode<KT, VT, ORDER>(cur_node.prev_page_, folder_name_);
            // mark
            tmp_front_key = cur_node.keys_.front();
            // merge
            left_sibling.keys_.insert(left_sibling.keys_.end(), cur_node.keys_.begin(),
                                      cur_node.keys_.end());
            left_sibling.values_.insert(left_sibling.values_.end(), cur_node.values_.begin(),
                                        cur_node.values_.end());
            left_sibling.key_num_ += cur_node.key_num_;
            left_sibling.next_page_ = cur_node.next_page_;
            if (cur_node.next_page_ != -1) {
                bpnode<KT, VT, ORDER> next_node =
                    bpnode<KT, VT, ORDER>(cur_node.next_page_, folder_name_);
                next_node.prev_page_ = cur_node.prev_page_;
            }
            cur_node.key_num_ = 0;  // clear, and destruct later
            // update parent
            bpnode<KT, VT, ORDER> par_node =
                bpnode<KT, VT, ORDER>(cur_node.parent_page_, folder_name_);
            // parent is the root
            auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(), key) -
                           par_node.keys_.begin();
            par_node.keys_.erase(par_node.keys_.begin() + key_pos);
            par_node.sub_ptrs_.erase(par_node.sub_ptrs_.begin() + key_pos + 1);
            par_node.key_num_--;
            if (par_node.key_num_ == 0) {
                remove_update_parent(par_node, left_sibling.page_id_);
            }
        }  // merge with right sibling
        else if (cur_node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> right_sibling =
                bpnode<KT, VT, ORDER>(cur_node.next_page_, folder_name_);
            // mark
            tmp_front_key = right_sibling.keys_.front();
            // merge
            cur_node.keys_.insert(cur_node.keys_.end(), right_sibling.keys_.begin(),
                                  right_sibling.keys_.end());
            cur_node.values_.insert(cur_node.values_.end(), right_sibling.values_.begin(),
                                    right_sibling.values_.end());
            cur_node.key_num_ += right_sibling.key_num_;
            cur_node.next_page_ = right_sibling.next_page_;
            if (right_sibling.next_page_ != -1) {
                bpnode<KT, VT, ORDER> next_node =
                    bpnode<KT, VT, ORDER>(right_sibling.next_page_, folder_name_);
                next_node.prev_page_ = cur_node.page_id_;
            }
            right_sibling.key_num_ = 0;  // clear, and destruct later
            // update parent
            bpnode<KT, VT, ORDER> par_node =
                bpnode<KT, VT, ORDER>(right_sibling.parent_page_, folder_name_);
            // parent is the root
            auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(), key) -
                           par_node.keys_.begin();
            par_node.keys_.erase(par_node.keys_.begin() + key_pos);
            par_node.sub_ptrs_.erase(par_node.sub_ptrs_.begin() + key_pos + 1);
            par_node.key_num_--;
            if (par_node.key_num_ == 0) {
                remove_update_parent(par_node, cur_node.page_id_);
            }
        }
    }
}

template <class KT, class VT, std::size_t ORDER>
void bptree<KT, VT, ORDER>::remove_update_parent(bpnode<KT, VT, ORDER> &node,
                                                 page_id_t child_page) {
    // Notes:
    // this works only for internal nodes
    //    5
    //   / \        kind of this situation, handled in previous part
    //  []  5*
    // cur_node denotes the empty node now, next to handle steal or merge or sth~

    // Firstly, we handle the cur_page is root situation
    if (node.page_id_ == root_) {
        root_ = child_page;
        return;
    }

    // Secondly, we handle the cur_page is not root situation
    bpnode<KT, VT, ORDER> &cur_node = node;
    bpnode<KT, VT, ORDER> par_node = bpnode<KT, VT, ORDER>(node.parent_page_, folder_name_);
    if (node.key_num_ == 0) {  // 100% VTrue, just for filed setting
        // steal from left sibling
        if (node.prev_page_ != -1) {
            bpnode<KT, VT, ORDER> left_sibling =
                bpnode<KT, VT, ORDER>(node.prev_page_, folder_name_);
            if (left_sibling.key_num_ >= ceil(get_max_leaf_node_limit() / 2)) {
                // steal
                // search place
                auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                                left_sibling.keys_.back()) -
                               par_node.keys_.begin();
                // key round
                // Note: Here maybe some problems when cur_node is not empty
                // but in this case cur_node must be empty :-)
                cur_node.keys_.emplace(cur_node.keys_.begin(), par_node.keys_[key_pos]);
                cur_node.key_num_++;
                par_node.keys_[key_pos] = left_sibling.keys_.back();
                left_sibling.keys_.pop_back();
                left_sibling.key_num_--;
                // ptr round
                cur_node.sub_ptrs_.emplace(cur_node.sub_ptrs_.begin(),
                                           left_sibling.sub_ptrs_.back());
                left_sibling.sub_ptrs_.pop_back();
                return;
            }
        }
        // steal from right sibling
        if (node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> right_sibling =
                bpnode<KT, VT, ORDER>(node.next_page_, folder_name_);
            if (right_sibling.key_num_ >= ceil(get_max_leaf_node_limit() / 2)) {
                // steal
                // search place
                auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                                right_sibling.keys_.front()) -
                               par_node.keys_.begin();
                // key round
                cur_node.keys_.emplace_back(par_node.keys_[key_pos]);
                cur_node.key_num_++;
                par_node.keys_[key_pos] = right_sibling.keys_.front();
                right_sibling.keys_.erase(right_sibling.keys_.begin());
                right_sibling.key_num_--;
                // ptr round
                cur_node.sub_ptrs_.emplace_back(right_sibling.sub_ptrs_.front());
                right_sibling.sub_ptrs_.erase(right_sibling.sub_ptrs_.begin());
                return;
            }
        }
        // MERRRRRRRGE!!!!!
        // merge with left sibling
        if (node.prev_page_ != -1) {
            bpnode<KT, VT, ORDER> left_sibling =
                bpnode<KT, VT, ORDER>(node.prev_page_, folder_name_);
            // merge
            auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                            left_sibling.keys_.front()) -
                           par_node.keys_.begin();
            left_sibling.keys_.emplace_back(par_node.keys_[key_pos]);
            left_sibling.key_num_++;
            left_sibling.sub_ptrs_.emplace_back(child_page);
            par_node.keys_.erase(par_node.keys_.begin() + key_pos);
            par_node.sub_ptrs_.erase(par_node.sub_ptrs_.begin() + key_pos + 1);
            par_node.key_num_--;
            // update parent
            if (par_node.key_num_ == 0) {
                remove_update_parent(par_node, left_sibling.page_id_);
            }
        }
        // merge with right sibling
        else if (node.next_page_ != -1) {
            bpnode<KT, VT, ORDER> right_sibling =
                bpnode<KT, VT, ORDER>(node.next_page_, folder_name_);
            // merge
            auto key_pos = std::lower_bound(par_node.keys_.begin(), par_node.keys_.end(),
                                            right_sibling.keys_.back()) -
                           par_node.keys_.begin();
            right_sibling.keys_.emplace(right_sibling.keys_.begin(), par_node.keys_[key_pos]);
            right_sibling.key_num_++;
            right_sibling.sub_ptrs_.emplace(right_sibling.sub_ptrs_.begin(), child_page);
            par_node.keys_.erase(par_node.keys_.begin() + key_pos);
            par_node.sub_ptrs_.erase(par_node.sub_ptrs_.begin() + key_pos);
            par_node.key_num_--;
            // update parent
            if (par_node.key_num_ == 0) {
                remove_update_parent(par_node, right_sibling.page_id_);
            }
        }
    }
}
#endif  // INCLUDE_BPTREE_H_