/*!
 * @file nucleic_acid_sys.cpp
 * @author Luminolt
 * @brief nucleic_acid_sys class
 */

#include "nucleic_acid_sys.h"

#include <iomanip>
#include <utility>

#include "examine_log.h"
#include "person_log.h"
#include "utils.h"

NucleicAcidSys::NucleicAcidSys() : person("person"), examine("examine") {
    std::string file = "data.txt";
    // load the single_serial, multiple_serial, multiple_coutner;
    struct stat buf;
    errno_t err = 0;
    if (stat(std::string(file).c_str(), &buf) != 0) {
        single_serial = 0;
        multiple_serial = 0;
        queue_num = 10;
        queue_coutner.resize(10, 0);
    } else {
        std::ifstream ifs(file);
        ifs >> single_serial >> multiple_serial >> queue_num;
        int tmp;
        for (int i = 0; i < queue_num; i++) {
            ifs >> tmp;
            queue_coutner.emplace_back(tmp);
        }
        ifs.close();
    }
}

NucleicAcidSys::~NucleicAcidSys() {
    std::string file = "data.txt";
    std::ofstream ofs(file);
    ofs << single_serial << ' ' << multiple_serial << ' ' << queue_num << ' ';
    for (int i = 0; i < queue_num; i++) {
        ofs << queue_coutner[i] << ' ';
    }
    ofs.close();
}

void NucleicAcidSys::AddPerson(const id_t<8> &id, const std::string &name) {
    person_log log;
    log.id = id;
    log.name = name;
    log.status = not_examined;
    log.update_time = time(NULL);
    person.insert(id, log);
}

void NucleicAcidSys::EnquePerson(const id_t<8> &id, const id_t<2> &queue_id) {
    person.search(id, [&](auto &log) {
        log.status = queueing;
        log.update_time = time(NULL);
    });
    // enqueue
    logging_queue[int(queue_id)].push(id);  // stupid queue
}

void NucleicAcidSys::AddExamine(const id_t<2> &queue_id, bool mode) {
    AddExamine(logging_queue[int(queue_id)].front(), queue_id, mode);
    logging_queue[int(queue_id)].pop();
}

void NucleicAcidSys::AddExamine(const id_t<8> &person_id, const id_t<2> &queue_id, bool mode) {
    examine_log log;
    if (mode == 0) {
        log.id = ++multiple_serial;
    } else {
        log.id = ++single_serial;
    }
    log.person_id = person_id;
    log.queue_id = queue_id;
    log.status = waitfor_uploading;
    log.update_time = time(NULL);
    if (mode == 0) {
        examine.insert(std::string(log.id) + std::string(queue_id) +
                           std::to_string(++queue_coutner[int(queue_id)]),
                       log);
        if (queue_coutner[int(queue_id)] == 10) {
            queue_coutner[int(queue_id)] = 0;
        }
    } else {
        examine.insert(std::string(log.id) + std::string(queue_id) + std::to_string(0), log);
    }
    // change person status to queueing
    person.search(person_id, [&](person_log &log) {
        log.status = queueing;
        log.update_time = time(NULL);
    });
}

void NucleicAcidSys::ShowQueue() {
    std::vector<std::pair<id_t<2>, person_log>> queue = get_queue();
    std::cout << std::setw(4) << "QID" << std::setw(4) << "No" << std::setw(9) << "ID"
              << std::setw(10) << "Name" << std::setw(10) << "Status" << std::setw(18)
              << "Update Time" << std::endl;
    int cnt = 0;
    int cur_q = 0;
    for (auto &item : queue) {
        if (int(item.first) != cur_q) {
            cur_q = item.first;
            cnt = 0;
        }
        auto &log = item.second;
        std::cout << std::setw(4) << item.first << std::setw(4) << ++cnt << std::setw(9) << log.id
                  << std::setw(10) << log.name << std::setw(12) << log.status << std::setw(18)
                  << DatetimeToString(log.update_time) << std::endl;
    }
}

void NucleicAcidSys::ShowStatus() {
    std::map<PERSON_STATUS, std::vector<person_log>> map = get_status();
    std::cout << std::setw(9) << "ID" << std::setw(10) << "Name" << std::setw(12) << "Status"
              << std::setw(18) << "Update Time" << std::endl;
    for (auto &item : map) {
        for (auto &log : item.second) {
            std::cout << std::setw(9) << log.id << std::setw(10) << log.name << std::setw(12)
                      << log.status << std::setw(18) << DatetimeToString(log.update_time)
                      << std::endl;
        }
    }
}

void NucleicAcidSys::ShowPersonalInfo(id_t<8> id, time_t time) {
    person_log log;
    person.search(id, [&](auto &log) {
        std::cout << "ID: " << log.id << std::endl;
        std::cout << "Name: " << log.name << std::endl;
        std::cout << "Status: " << log.status << std::endl;
        std::cout << "Update Time: " << DatetimeToString(log.update_time) << std::endl;
    });
}

std::vector<std::pair<id_t<2>, person_log>> NucleicAcidSys::get_queue() {
    std::vector<std::pair<id_t<2>, person_log>> queue;
    examine.search(id_t<8>("00000000"), id_t<8>("99999999"), [&queue, this](examine_log item) {
        if (item.status == waitfor_uploading) {
            this->person.search(item.person_id, [&queue, &item](person_log log) {
                queue.emplace_back(std::make_pair(item.queue_id, log));
            });
        }
    });
    return queue;
}

std::map<PERSON_STATUS, std::vector<person_log>> NucleicAcidSys::get_status() {
    std::map<PERSON_STATUS, std::vector<person_log>> map;
    person.search(id_t<8>("00000000"), id_t<8>("99999999"),
                  [&map](person_log item) { map[item.status].emplace_back(item); });
    return map;
}
