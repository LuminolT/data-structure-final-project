#ifndef INCLUDE_EXAMINE_LOG_H_
#define INCLUDE_EXAMINE_LOG_H_

#include <time.h>

#include <iostream>
#include <string>

#include "id_t.h"


enum RESULT_STATUS { nega, posi, waitfor_uploading };

std::istream &operator>>(std::istream &is, RESULT_STATUS &status);
std::ostream &operator<<(std::ostream &os, const RESULT_STATUS &status);

/*!
 * @brief examine_log class
 * @brief id k_bbbb denotes the tube serial number
 */
struct examine_log {
    id_t<5> id;            // k_bbbb k=0 denotes mixed
    id_t<8> person_id;     // xxxyyyyz denotes the buiding, room and person
    int order;             // order of the person
    RESULT_STATUS status;  // result status
    time_t update_time;    // updated time of the status

    // default constructor
    examine_log() = default;
    // constructor
    examine_log(std::string id, std::string person_id, int order)
        : id(id),
          person_id(person_id),
          order(order),
          status(waitfor_uploading),
          update_time(time(NULL)) {}

    std::istream &input(std::istream &is);
    std::ostream &output(std::ostream &os);

    friend std::istream &operator>>(std::istream &is, examine_log &log) { return log.input(is); }
    friend std::ostream &operator<<(std::ostream &os, examine_log &log) { return log.output(os); }
};

#endif  // INCLUDE_EXAMINE_LOG_H_