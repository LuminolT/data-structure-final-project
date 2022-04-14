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
    int prev_page_;
    int next_page_;
    std::string folder_name_;

    // constructor
    explicit bpnode(page_id_t, std::string);

    // destructor
    ~bpnode();

    // insert_key_value
    void insert_key_value(int, T);

    // insert_ptr
    void insert_key_ptr(int, page_id_t);

    // override iostream
    std::istream &input(std::istream &is);
    std::ostream &output(std::ostream &os);

    friend std::istream &operator>>(std::istream &is, bpnode<T, ORDER> &self) {
        return self.input(is);
    }

    friend std::ostream &operator<<(std::ostream &os, bpnode<T, ORDER> &self) {
        return self.output(os);
    }
};

template <class T, std::size_t ORDER>
bpnode<T, ORDER>::bpnode(page_id_t page_id, std::string folder_name) {
    page_id_ = page_id;
    folder_name_ = folder_name;
    is_leaf_ = false;
    key_num_ = 0;
    prev_page_ = -1;
    next_page_ = -1;
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
std::istream &bpnode<T, ORDER>::input(std::istream &is) {
    is >> is_leaf_;
    is >> key_num_;
    for (int i = 0; i < key_num_; ++i) {
        int key;
        is >> key;
        keys_.emplace_back(key);
    }
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
void bpnode<T, ORDER>::insert_key_value(int key, T value) {
    for (int i = 0; i < key_num_; ++i) {
        if (key < keys_[i]) {
            keys_.insert(keys_.begin() + i, key);
            values_.insert(tvalues_.begin() + i, value);
            key_num_++;
            break;
        }
    }
}

template <class T, std::size_t ORDER>
void bpnode<T, ORDER>::insert_key_ptr(int key, page_id_t page_id) {}

template <class T, std::size_t ORDER>
std::ostream &bpnode<T, ORDER>::output(std::ostream &os) {
    os << is_leaf_ << std::endl;
    os << key_num_ << std::endl;
    for (int i = 0; i < key_num_; ++i) {
        os << keys_[i] << std::endl;
    }
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

#endif  // INCLUDE_BPNODE_H_