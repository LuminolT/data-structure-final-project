/*!
 * @file nucleic_acid_sys.cpp
 * @author Luminolt
 * @brief nucleic_acid_sys class
 */

#include "nucleic_acid_sys.h"

#include <iomanip>

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
        multiple_coutner = 0;
    } else {
        std::ifstream ifs(file);
        ifs >> single_serial >> multiple_serial >> multiple_coutner;
        ifs.close();
    }
}

NucleicAcidSys::~NucleicAcidSys() {
    std::string file = "data.txt";
    std::ofstream ofs(file);
    ofs << single_serial << " " << multiple_serial << " " << multiple_coutner;
    ofs.close();
}

void NucleicAcidSys::AddPerson(const id_t<8> &id, const std::string &name) {
    person_log log;
    log.id = id;
    log.name = name;
    log.status = negative;
    log.update_time = time(NULL);
    person.insert(id, log);
}

void NucleicAcidSys::AddExamine(const id_t<5> &id, const id_t<8> &person_id, int order) {
    examine_log log;
    log.id = id;
    log.person_id = person_id;
    log.order = order;
    log.status = waitfor_uploading;
    log.update_time = time(NULL);
    examine.insert(std::string(id) + std::to_string(order), log);
    // change person status to queueing
    person.search(person_id, [&](person_log &log) {
        log.status = queueing;
        log.update_time = time(NULL);
    });
}

void NucleicAcidSys::ContinuousExamine(int booth_id) {}

void NucleicAcidSys::ShowQueue() {
    std::vector<person_log> queue = get_queue();
    std::cout << std::setw(4) << "No" << std::setw(9) << "ID" << std::setw(10) << "Name"
              << std::setw(10) << "Status" << std::setw(16) << "Update Time" << std::endl;
    int cnt = 0;
    for (auto &log : queue) {
        std::cout << std::setw(4) << ++cnt << std::setw(9) << log.id << std::setw(10) << log.name
                  << std::setw(10) << log.status << std::setw(16)
                  << DatetimeToString(log.update_time) << std::endl;
    }
}

void NucleicAcidSys::TubeStorage(id_t<5> id) {}

void NucleicAcidSys::TubeResultLog(id_t<5> id, int result) {}

void NucleicAcidSys::ShowStatus() {}

void NucleicAcidSys::ShowPersonalInfo(id_t<8> id, time_t time) {}

std::vector<person_log> NucleicAcidSys::get_queue() {
    std::vector<person_log> queue;
    person.search(id_t<8>("00000000"), id_t<8>("99999999"), [&queue](person_log item) {
        if (item.status == queueing) {
            queue.emplace_back(item);
        }
    });
    return queue;
}

std::map<PERSON_STATUS, std::vector<person_log>> NucleicAcidSys::get_status() {
    return std::map<PERSON_STATUS, std::vector<person_log>>();
}
