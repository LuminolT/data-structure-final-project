/*!
 * @file examine_log.cpp
 * @author Luminolt
 * @brief examine_log class
 */

#include "examine_log.h"

std::istream &operator>>(std::istream &is, RESULT_STATUS &status) {
    std::string s;
    is >> s;
    if (s == "negative") {
        status = nega;
    } else if (s == "positive") {
        status = posi;
    } else if (s == "waiting_for_uploading") {
        status = waitfor_uploading;
    } else {
        throw std::runtime_error("invalid status");
    }
    return is;
}

std::ostream &operator<<(std::ostream &os, const RESULT_STATUS &status) {
    switch (status) {
    case nega: os << "negative"; break;
    case posi: os << "positive"; break;
    case waitfor_uploading: os << "waiting_for_uploading"; break;
    default: throw std::runtime_error("invalid status");
    }
    return os;
}

std::istream &examine_log::input(std::istream &is) {
    is >> id >> person_id >> order >> status >> update_time;
    return is;
}
std::ostream &examine_log::output(std::ostream &os) {
    os << id << " " << person_id << " " << order << " " << status << " " << update_time;
    return os;
}