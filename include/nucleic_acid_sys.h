/*!
 * @file nucleic_acid_sys.h
 * @author Luminolt
 * @brief nucleic_acid_sys class
 */
#ifndef INCLUDE_NUCLEIC_ACID_SYS_H_

#include <deque>
#include <map>
#include <vector>

#include "bptree.h"
#include "examine_log.h"
#include "person_log.h"


/*!
 * @brief NucleicAcidSys class
 * @brief Add, Enque methods are the basic methods.
 *      - Show methods are just for test use.
 */
class NucleicAcidSys {
public:
    // Default constructor
    NucleicAcidSys();

    // Destructor
    ~NucleicAcidSys();

    // Add Person info, default with status not_examined
    void AddPerson(const id_t<8> &id, const std::string &name);

    // person enqueue
    void EnquePerson(const id_t<8> &id, const id_t<2> &queue_id);

    // Add examine log, mode = 0 for single
    void AddExamine(const id_t<2> &queue_id);
    void AddExamine(const id_t<8> &person_id, const id_t<2> &queue_id, bool mode = 0);

    // Show the Queue
    void ShowQueue();

    // Test tube result log
    void AddTubeResult(id_t<5> id, RESULT_STATUS result);

    // Show status of all people.
    void ShowStatus();

    // Get Personal Info
    void ShowPersonalInfo(id_t<8> id, time_t time);

    // Get queue front
    id_t<8> GetQueueFront(id_t<2> queue_id);

protected:
    std::vector<std::pair<id_t<2>, person_log>> get_queue();
    std::map<PERSON_STATUS, std::vector<person_log>> get_status();
    person_log get_person_info(id_t<8> id);

    bptree<id_t<8>, person_log, 5> person;    // xxx_yyyy_z
    bptree<id_t<8>, examine_log, 5> examine;  // k_bbbb_cc_d

    int single_serial;
    int multiple_serial;
    int queue_num;
    std::vector<int> queue_coutner;

    std::vector<std::deque<id_t<8>>> logging_queue;
};

#define INCLUDE_NUCLEIC_ACID_SYS_H_

#endif  // INCLUDE_NUCLEIC_ACID_SYS_H_