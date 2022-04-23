#ifndef INCLUDE_BPNODE_H_
#define INCLUDE_BPNODE_H_

#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

typedef int page_id_t;

template <class T, std::size_t ORDER>
class bpnode {
    /*
     *  B+Tree Node
     * - A node can be either an internal node or a leaf node
     * - Internal nodes have keys and pointers to child nodes
     * - Leaf nodes have keys and values
     */
  public:
    page_id_t page_id_;
    bool is_leaf_;
    int key_num_;
    std::vector<int> keys_;
    std::vector<T> values_;  // In case of same type, we don't use variant
    std::vector<page_id_t> sub_ptrs_;
    int parent_page_;
    int prev_page_;  // for leafs
    int next_page_;  // for leafs
    std::string folder_name_;

    // constructor
    explicit bpnode(page_id_t, std::string);

    // destructor
    ~bpnode();

    // override [] operator
    T &operator[](int);

    // override iostream
    std::istream &input(std::istream &is);
    std::ostream &output(std::ostream &os);

    // for my debug
    std::istream &debug_input(std::istream &is);
    std::ostream &debug_output(std::ostream &os);

    friend std::istream &operator>>(std::istream &is, bpnode<T, ORDER> &self) {
        return self.debug_input(is);
    }

    friend std::ostream &operator<<(std::ostream &os, bpnode<T, ORDER> &self) {
        return self.debug_output(os);
    }
};

template <class T, std::size_t ORDER>
bpnode<T, ORDER>::bpnode(page_id_t page_id, std::string folder_name) {
    page_id_ = page_id;  // don't save in file
    folder_name_ = folder_name;
    is_leaf_ = false;
    key_num_ = 0;
    prev_page_ = -1;
    next_page_ = -1;
    parent_page_ = -1;
    auto file_name = folder_name_ + "/" + std::to_string(page_id) + ".txt";
    std::ifstream file(file_name);
    if (!file.is_open()) {
        // throw std::runtime_error("file is not open!");
        // init an empty node
        return;
    }
    file >> *this;
    file.close();
}

template <class T, std::size_t ORDER>
bpnode<T, ORDER>::~bpnode() {
    auto file_name = folder_name_ + "/" + std::to_string(page_id_) + ".txt";
    std::ofstream file(file_name);
    //    if (!file.is_open()) {
    //         throw std::runtime_error("file is not open!");
    //    }
    file << *this;
    file.close();
}

template <class T, std::size_t ORDER>
T &bpnode<T, ORDER>::operator[](int key) {
    if (!is_leaf_) {
        throw std::runtime_error("op[]: this is not a leaf node!");
    }
    auto real_idx = std::lower_bound(keys_.begin(), keys_.end(), key);
    if (real_idx == keys_.end()) {
        throw std::runtime_error("op[]: key not found!");
        // only for debug use, this should never happen!
    }
    return values_[real_idx - keys_.begin()];
}

template <class T, std::size_t ORDER>
std::istream &bpnode<T, ORDER>::input(std::istream &is) {
    is >> is_leaf_;
    is >> key_num_;
    for (int i = 0; i < key_num_; ++i) {
        int key;
        is >> key;
        keys_.emplace_back(key);
    }
    is >> parent_page_;
    is >> prev_page_;
    is >> next_page_;
    if (is_leaf_) {
        for (int i = 0; i < key_num_; ++i) {
            T sub_ptr;
            is >> sub_ptr;
            values_.emplace_back(sub_ptr);
        }
    } else {
        for (int i = 0; i < key_num_ + 1; ++i) {
            page_id_t sub_ptr;
            is >> sub_ptr;
            sub_ptrs_.emplace_back(sub_ptr);
        }
    }
    return is;
}

template <class T, std::size_t ORDER>
std::istream &bpnode<T, ORDER>::debug_input(std::istream &is) {
    std::string tmp;
    is >> tmp >> is_leaf_;
    is >> tmp >> key_num_;
    for (int i = 0; i < key_num_; ++i) {
        int key;
        is >> key;
        keys_.emplace_back(key);
    }
    is >> tmp >> parent_page_;
    is >> tmp >> prev_page_;
    is >> tmp >> next_page_;
    is >> tmp;
    if (is_leaf_) {
        for (int i = 0; i < key_num_; ++i) {
            T sub_ptr;
            is >> sub_ptr;
            values_.emplace_back(sub_ptr);
        }
    } else {
        for (int i = 0; i < key_num_ + 1; ++i) {
            page_id_t sub_ptr;
            is >> sub_ptr;
            sub_ptrs_.emplace_back(sub_ptr);
        }
    }
    return is;
}

template <class T, std::size_t ORDER>
std::ostream &bpnode<T, ORDER>::output(std::ostream &os) {
    os << is_leaf_ << std::endl;
    os << key_num_ << std::endl;
    for (int i = 0; i < key_num_; ++i) {
        os << keys_[i] << std::endl;
    }
    os << parent_page_ << std::endl;
    os << prev_page_ << std::endl;
    os << next_page_ << std::endl;
    if (is_leaf_) {
        for (int i = 0; i < key_num_; ++i) {
            os << values_[i] << std::endl;
        }
    } else {
        for (int i = 0; i < key_num_ + 1; ++i) {
            os << sub_ptrs_[i] << std::endl;
        }
    }
    return os;
}

template <class T, std::size_t ORDER>
std::ostream &bpnode<T, ORDER>::debug_output(std::ostream &os) {
    os << "is_leaf_: " << is_leaf_ << std::endl;
    os << "key_num_: " << key_num_ << std::endl;
    for (int i = 0; i < key_num_; ++i) {
        os << keys_[i] << std::endl;
    }
    os << "parent_page_: " << parent_page_ << std::endl;
    os << "prev_page_: " << prev_page_ << std::endl;
    os << "next_page_: " << next_page_ << std::endl;
    os << "values_or_sub_ptrs: " << std::endl;
    if (is_leaf_) {
        for (int i = 0; i < key_num_; ++i) {
            os << values_[i] << std::endl;
        }
    } else {
        for (int i = 0; i < key_num_ + 1; ++i) {
            os << sub_ptrs_[i] << std::endl;
        }
    }
    return os;
}

#endif  // INCLUDE_BPNODE_H_