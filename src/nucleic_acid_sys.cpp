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
        single_serial = 10000;
        multiple_serial = 0;
        queue_num = 20;
        queue_coutner.resize(queue_num, 0);
        for (int i = 0; i < queue_num; ++i) {
            std::deque<id_t<8>> q;
            logging_queue.emplace_back(q);
        }
    } else {
        std::ifstream ifs(file);
        ifs >> single_serial >> multiple_serial >> queue_num;
        int tmp;
        for (int i = 0; i < queue_num; i++) {
            ifs >> tmp;
            queue_coutner.emplace_back(tmp);
        }
        for (int i = 0; i < queue_num; ++i) {
            std::deque<id_t<8>> q;
            logging_queue.emplace_back(q);
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
    logging_queue[int(queue_id)].emplace_back(id);
}

void NucleicAcidSys::AddExamine(const id_t<2> &queue_id) {
    if (queue_id == id_t<2>(0)) {
        AddExamine(logging_queue[int(queue_id)].front(), queue_id, 1);
    } else {
        AddExamine(logging_queue[int(queue_id)].front(), queue_id, 0);
    }
    logging_queue[int(queue_id)].pop_front();
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
    // change person status to wait for upload
    person.search(person_id, [&](person_log &log) {
        log.status = waiting_for_uploading;
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
    if (queue.size() == 0) {
        std::cout << "Queue is empty." << std::endl;
        return;
    }
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

void NucleicAcidSys::AddTubeResult(id_t<5> id, RESULT_STATUS result) {
    std::vector<id_t<8>> person_ids;
    std::vector<int> orders;
    id_t<2> queue_id;
    examine.search(std::string(id) + "000", std::string(id) + "999",
                   [&result, &person_ids, &queue_id, &orders](examine_log &log) {
                       log.status = result;
                       log.update_time = time(NULL);
                       person_ids.emplace_back(log.person_id);
                       queue_id = log.queue_id;
                       orders.emplace_back(log.order);
                   });
    bool flag = false;
    for (auto &person_id : person_ids) {
        person.search(person_id, [&result, &person_ids, &flag](person_log &log) {
            if (result == posi) {
                if (person_ids.size() == 1) {
                    log.status = positive;
                    flag = true;
                } else {
                    log.status = suspicious;
                }
                log.update_time = time(NULL);
            } else if (result == nega) {
                log.status = negative;
                log.update_time = time(NULL);
            } else {
                throw std::runtime_error("AddTubeResult: invalid result");
            }
        });
    }
    std::vector<id_t<8>> close_guys;
    std::vector<id_t<8>> sec_close_guys;
    if (flag) {
        //并且对于确诊人员，其同一栋楼人员以及测试时排在他前面的10人和后面的1人设置为密接者；密接者的同一栋楼人员为次密接者。
        auto posi_guy = person_ids[0];
        auto posi_guy_queue = queue_id;
        auto order = orders[0];
        auto building_id = std::string(posi_guy).substr(0, 3);
        try {
            person.search(building_id + "00000", building_id + "99999",
                          [&posi_guy, &close_guys](person_log &log) {
                              if (log.id != posi_guy) {
                                  close_guys.emplace_back(log.id);
                              }
                          });
        } catch (std::runtime_error &e) {
            ;
        }
        std::vector<id_t<8>> close_ids;
        auto start_num = int(id) - 1 > 10000 ? int(id) - 1 : 10000;
        auto end_num = int(id) + 1 < 99999 ? int(id) + 1 : 99999;
        auto start = std::to_string(start_num) + "000";
        auto end = std::to_string(end_num) + "009";
        // queue search
        try {
            examine.search(start, end, [&close_ids](examine_log &log) {
                close_ids.emplace_back(log.person_id);
            });
        } catch (std::runtime_error &e) {
            ;
        }
        int pos_posi_guy = 0;
        while (close_ids[pos_posi_guy] != posi_guy) {
            pos_posi_guy++;
        }
        std::vector<id_t<8>> sec_close_ids;
        for (int i = (pos_posi_guy - 10 > 0 ? pos_posi_guy - 10 : 0); i < pos_posi_guy; i++) {
            sec_close_ids.emplace_back(close_ids[i]);
        }
        sec_close_ids.emplace_back(close_ids[pos_posi_guy + 1]);
        for (auto &item : sec_close_ids) {
            try {
                person.search(item, [this, &close_guys, &sec_close_guys](person_log &log) {
                    close_guys.emplace_back(log.id);
                    auto posi_guy = log.id;
                    auto building_id = std::string(posi_guy).substr(0, 3);
                    person.search(building_id + "00000", building_id + "99999",
                                  [&posi_guy, &sec_close_guys](person_log &log) {
                                      if (log.id != posi_guy) {
                                          sec_close_guys.emplace_back(log.id);
                                      }
                                  });
                });
            } catch (std::runtime_error &e) {
                ;
            }
        }
    }
    for (auto &item : close_guys) {
        try {
            person.search(item, [this, &queue_id, &orders](person_log &log) {
                log.status = close_contact;
                log.update_time = time(NULL);
            });
        } catch (std::runtime_error &e) {
            ;
        }
    }
    for (auto &item : sec_close_guys) {
        try {
            person.search(item, [this, &queue_id, &orders](person_log &log) {
                log.status = secondary_close_contact;
                log.update_time = time(NULL);
            });
        } catch (std::runtime_error &e) {
            ;
        }
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
        if (log.update_time < time) {
            std::cout << "Status: " << log.status << std::endl;
        } else {
            std::cout << "Status: "
                      << "not_examined" << std::endl;
            std::cout << "** The system shows you HAVEN'T take the lastest test!!" << std::endl;
            std::cout << "Here is your previous test status" << std::endl;
            std::cout << "Previous Status: " << log.status << std::endl;
        }
        std::cout << "Previous Update Time: " << DatetimeToString(log.update_time) << std::endl;
    });
}

std::vector<std::pair<id_t<2>, person_log>> NucleicAcidSys::get_queue() {
    std::vector<std::pair<id_t<2>, person_log>> queue;
    for (int i = 0; i < queue_num; i++) {
        for (auto &item : logging_queue[i]) {
            this->person.search(item, [&queue, &item, &i](person_log log) {
                queue.emplace_back(std::make_pair(i, log));
            });
        }
    }
    return queue;
}

std::map<PERSON_STATUS, std::vector<person_log>> NucleicAcidSys::get_status() {
    std::map<PERSON_STATUS, std::vector<person_log>> map;
    person.search(id_t<8>("00000000"), id_t<8>("99999999"),
                  [&map](person_log item) { map[item.status].emplace_back(item); });
    return map;
}

id_t<8> NucleicAcidSys::GetQueueFront(id_t<2> queue_id) {
    return logging_queue[int(queue_id)].front();
}