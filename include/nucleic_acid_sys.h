
#ifndef INCLUDE_NUCLEIC_ACID_SYS_H_

#include <map>
#include <vector>

#include "bptree.h"
#include "examine_log.h"
#include "person_log.h"

class NucleicAcidSys {
public:
    // Default constructor
    NucleicAcidSys();

    // Destructor
    ~NucleicAcidSys() = default;

    // Inner Methods, for test use
    void AddPerson(const id_t<8> &id, const std::string &name);
    void AddExamine(const id_t<5> &id, const id_t<8> &person_id, int order);

    // For doctors, continues doing the nucleic acid exam.
    void ContinuousExamine(int booth_id);  // kxbbb we use x to denote the booth id

    // Show the Queue
    void ShowQueue();

    // Test tube storage
    void TubeStorage(id_t<5> id);

    // Test tube result log
    void TubeResultLog(id_t<5> id, int result);

    // Show status of all people.
    void ShowStatus();

    // Get Personal Info
    void ShowPersonalInfo(id_t<8> id, time_t time);

protected:
    std::vector<person_log> get_queue();
    std::map<PERSON_STATUS, std::vector<person_log>> get_status();

    bptree<id_t<8>, person_log, 64> person;
    bptree<id_t<6>, examine_log, 64> examine;

    int single_serial;
    int multiple_serial;
    int multiple_coutner;
};

#define INCLUDE_NUCLEIC_ACID_SYS_H_

#endif  // INCLUDE_NUCLEIC_ACID_SYS_H_