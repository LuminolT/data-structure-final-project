
#ifndef INCLUDE_NUCLEIC_ACID_SYS_H_

#include <queue>

class NucleicAcidSys {
  public:
    // Default constructor
    NucleicAcidSys() = default;

    // Destructor
    ~NucleicAcidSys() = default;

  protected:
    //
    std::queue<int> nucleic_acid_queue;
    std::queue<int> queue2;
};

#define INCLUDE_NUCLEIC_ACID_SYS_H_

#endif  // INCLUDE_NUCLEIC_ACID_SYS_H_