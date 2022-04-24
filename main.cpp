/*!
 * @file main.cpp
 * @author Luminolt
 * @brief test and debug file
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bpnode.h"
#include "bptree.h"
#include "examine_log.h"
#include "nucleic_acid_sys.h"
#include "person_log.h"

using namespace std;

int main() {
    NucleicAcidSys nasys;
    // nasys.AddPerson("00000001", "Luminolt");
    // nasys.AddPerson("00000002", "Uminoltl");

    // nasys.AddExamine("00000001", "01");

    nasys.ShowQueue();
    std::cout << std::endl;
    nasys.ShowStatus();
    return 0;
}