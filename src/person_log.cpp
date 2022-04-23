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
    } else if (s == "close_contact") {
        status = close_contact;
    } else if (s == "secondary_close_contact") {
        status = secondary_close_contact;
    } else if (s == "waiting_for_uploading") {
        status = waiting_for_uploading;
    } else if (s == "queueing") {
        status = queueing;
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
    case close_contact: os << "close_contact"; break;
    case secondary_close_contact: os << "secondary_close_contact"; break;
    case waiting_for_uploading: os << "waiting_for_uploading"; break;
    case queueing: os << "queueing"; break;
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