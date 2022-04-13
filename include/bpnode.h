#ifndef INCLUDE_BPNODE_H_
#define INCLUDE_BPNODE_H_

#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

template <typename T>
class bpnode {
  public:
    bool is_leaf_;
    int key_num_;
    std::vector<int> keys_;
    int next_page_;
    int sub_ptr_num_;
    std::variant<std::vector<T>, std::vector<int>> sub_ptrs_;
    static std::string folder_name_;

    // constructor
    bpnode(int page_id);

    // override istream
    friend std::istream &operator>>(std::istream &is, bpnode<T> &self);

    // override ostream
    friend std::ostream &operator<<(const std::ostream &os, bpnode<T> &self);
};

template <typename T>
bpnode<T>::bpnode(int page_id) {
    if (this->folder_name_.empty()) {
        throw std::runtime_error("folder_name is not set!");
    }
    auto file_name = this->folder_name_ + std::to_string(page_id);
    std::ifstream file(file_name);
    if (!file.is_open()) {
        throw std::runtime_error("file is not open!");
    }
    file >> *this;
    file.close();
}

template <typename T>
std::istream &operator>>(std::istream &is, bpnode<T> &self) {
    is >> self.is_leaf_;
    is >> self.key_num_;
    for (int i = 0; i < self.key_num_; ++i) {
        int key;
        is >> key;
        self.keys_.emplace_back(key);
    }
    is >> self.next_ptr_;
    is >> self.sub_ptr_num_;
    if (self.is_leaf_) {
        std::vector<T> sub_ptrs;
        for (int i = 0; i < self.sub_ptr_num_; ++i) {
            T sub_ptr;
            is >> sub_ptr;
            sub_ptrs.emplace_back(sub_ptr);
        }
        self.sub_ptrs_ = sub_ptrs;
    } else {
        std::vector<int> sub_ptrs;
        for (int i = 0; i < self.sub_ptr_num_; ++i) {
            int sub_ptr;
            is >> sub_ptr;
            sub_ptrs.emplace_back(sub_ptr);
        }
        self.sub_ptrs_ = sub_ptrs;
    }
    return is;
}

template <typename T>
std::ostream &operator<<(const std::ostream &os, bpnode<T> &self) {
    os << self.is_leaf_ << std::endl;
    os << self.key_num_ << std::endl;
    for (int i = 0; i < self.key_num_; ++i) {
        os << self.keys_[i] << std::endl;
    }
    os << os << self.next_ptr_;
    os << self.sub_ptr_num_;
    if (self.is_leaf_) {
        for (int i = 0; i < self.sub_ptr_num_; ++i) {
            os << std::get<T>(self.sub_ptrs_[i]) << std::endl;
        }
    } else {
        for (int i = 0; i < self.sub_ptr_num_; ++i) {
            os << std::get<int>(self.sub_ptrs_[i]) << std::endl;
        }
    }
}

#endif  // INCLUDE_BPNODE_H_