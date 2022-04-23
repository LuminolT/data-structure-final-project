
#ifndef INCLUDE_NUCLEIC_ACID_SYS_H_

#include "bptree.h"
#include "examine_log.h"
#include "person_log.h"

class NucleicAcidSys {
public:
    // Default constructor
    NucleicAcidSys();

    // Destructor
    ~NucleicAcidSys() = default;

protected:
    bptree<id_t<8>, person_log, 64> person;
    bptree<id_t<5>, examine_log, 64> examine;
};

#define INCLUDE_NUCLEIC_ACID_SYS_H_

#endif  // INCLUDE_NUCLEIC_ACID_SYS_H_