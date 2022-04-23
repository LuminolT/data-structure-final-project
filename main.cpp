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
#include "person_log.h"

using namespace std;

int main() {
    person_log p1("00101011", "Luminolt");
    examine_log e1("11111", "11111111", 1);
    cout << p1;
    cout << endl << e1;
    return 0;
}