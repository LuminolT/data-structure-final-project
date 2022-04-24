/*!
 * @file id_t.h
 * @author Luminolt
 * @brief id_t, for annoying id compare~
 */

#ifndef INCLUDE_ID_T_H_
#define INCLUDE_ID_T_H_

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>


/*!
 * @brief id_t class
 * @tparam LENGTH the length of the id
 * @brief for annoying id compare~
 */
template <int LENGTH>
class id_t {
public:
    // default constructor
    id_t() = default;

    // argument constructor
    id_t(std::string str);
    id_t(int num);
    id_t(const char *str);

    // copy constructor
    id_t(const id_t &id) = default;

    // move constructor
    id_t(id_t &&id) = default;

    // copy assignment
    id_t &operator=(const id_t &id) = default;

    // move assignment
    id_t &operator=(id_t &&id) = default;

    // destructor
    ~id_t() = default;

    // compare operations
    bool operator>(const id_t &other) const { return value_ > other.value_; }
    bool operator<(const id_t &other) const { return value_ < other.value_; }
    bool operator==(const id_t &other) const { return value_ == other.value_; }
    bool operator!=(const id_t &other) const { return value_ != other.value_; }
    bool operator>=(const id_t &other) const { return value_ >= other.value_; }
    bool operator<=(const id_t &other) const { return value_ <= other.value_; }

    // iostream
    friend std::istream &operator>>(std::istream &is, id_t &id) { return id.input(is); }
    friend std::ostream &operator<<(std::ostream &os, id_t &id) { return id.output(os); }

    // input and output
    std::istream &input(std::istream &is);
    std::ostream &output(std::ostream &os);

    // convertor
    operator std::string() const;
    operator int() const { return value_; }

protected:
    int value_;
};

template <int LENGTH>
id_t<LENGTH>::id_t(std::string str) {
    if (str.length() != LENGTH) {
        throw std::invalid_argument("id_t: invalid string length");
    }
    value_ = std::atoi(str.c_str());
}

template <int LENGTH>
id_t<LENGTH>::id_t(int num) {
    if (num < 0 || num > std::pow(10, LENGTH)) {
        throw std::invalid_argument("id_t: invalid number");
    }
    value_ = num;
}

template <int LENGTH>
id_t<LENGTH>::id_t(const char *str) {
    if (strlen(str) != LENGTH) {
        throw std::invalid_argument("id_t: invalid string length");
    }
    value_ = std::atoi(str);
}

template <int LENGTH>
std::istream &id_t<LENGTH>::input(std::istream &is) {
    std::string str;
    is >> str;
    if (str.length() != LENGTH) {
        throw std::invalid_argument("id_t: invalid string length");
    }
    value_ = std::atoi(str.c_str());
    return is;
}

template <int LENGTH>
std::ostream &id_t<LENGTH>::output(std::ostream &os) {
    std::string str = std::to_string(value_);
    // padding
    if (str.length() < LENGTH) {
        str = std::string(LENGTH - str.length(), '0') + str;
    }
    os << str;
    return os;
}

template <int LENGTH>
id_t<LENGTH>::operator std::string() const {
    std::string str = std::to_string(value_);
    // padding
    if (str.length() < LENGTH) {
        str = std::string(LENGTH - str.length(), '0') + str;
    }
    return str;
}

#endif  // INCLUDE_ID_T_H_