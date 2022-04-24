/*!
 * @file person_log.cpp
 * @author Luminolt
 * @brief person_log class
 */

#include "person_log.h"

std::istream &operator>>(std::istream &is, PERSON_STATUS &status) {
    std::string s;
    is >> s;
    if (s == "negative") {
        status = negative;
    } else if (s == "positive") {
        status = positive;
    } else if (s == "suspicious") {
        status = suspicious;
    } else if (s == "close_cont") {
        status = close_contact;
    } else if (s == "sec_close") {
        status = secondary_close_contact;
    } else if (s == "wait_upload") {
        status = waiting_for_uploading;
    } else if (s == "queueing") {
        status = queueing;
    } else if (s == "n_examined") {
        status = not_examined;
    } else {
        throw std::runtime_error("invalid status");
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const PERSON_STATUS &status) {
    switch (status) {
    case negative: os << "negative"; break;
    case positive: os << "positive"; break;
    case suspicious: os << "suspicious"; break;
    case close_contact: os << "close_cont"; break;
    case secondary_close_contact: os << "sec_close"; break;
    case waiting_for_uploading: os << "wait_upload"; break;
    case queueing: os << "queueing"; break;
    case not_examined: os << "n_examined"; break;
    default: throw std::runtime_error("invalid status");
    }
    return os;
}

std::istream &person_log::input(std::istream &is) {
    is >> id >> name >> status >> update_time;
    return is;
}
std::ostream &person_log::output(std::ostream &os) {
    os << id << " " << name << " " << status << " " << update_time;
    return os;
}