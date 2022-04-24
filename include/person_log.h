/*!
 * @file person_log.h
 * @author Luminolt
 * @brief person_log class
 */

#ifndef INCLUDE_PERSON_LOG_H_
#define INCLUDE_PERSON_LOG_H_

#include <time.h>

#include <iostream>
#include <string>

#include "id_t.h"

enum PERSON_STATUS {
    negative,
    positive,
    suspicious,
    close_contact,
    secondary_close_contact,
    waiting_for_uploading,
    queueing,
    not_examined
};

std::istream &operator>>(std::istream &is, PERSON_STATUS &status);
std::ostream &operator<<(std::ostream &os, const PERSON_STATUS &status);

/*!
 * @brief person_log class
 * @brief id = xxxyyyyz denotes the building, room and person
 */
struct person_log {
    id_t<8> id;            // xxxyyyyz denotes the buiding, room and person
    std::string name;      // name of the person
    PERSON_STATUS status;  // health status
    time_t update_time;    // updated time of the status

    // default constructor
    person_log() = default;
    // constructor
    person_log(std::string id, std::string name)
        : id(id), name(name), status(waiting_for_uploading), update_time(time(NULL)) {}

    std::istream &input(std::istream &is);
    std::ostream &output(std::ostream &os);

    friend std::istream &operator>>(std::istream &is, person_log &log) { return log.input(is); }
    friend std::ostream &operator<<(std::ostream &os, person_log &log) { return log.output(os); }
};

#endif  // INCLUDE_PERSON_LOG_H_