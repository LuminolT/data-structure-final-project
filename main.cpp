/*!
 * @file main.cpp
 * @author Luminolt
 * @brief test and debug file
 */

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bpnode.h"
#include "bptree.h"
#include "examine_log.h"
#include "nucleic_acid_sys.h"
#include "person_log.h"

int main() {
    NucleicAcidSys nasys;
    // nasys.EnquePerson("00101011", "00");
    // nasys.AddExamine("00");
    // nasys.AddTubeResult("10001", posi);
    // nasys.ShowStatus();
    int choice = 0;
    while (true) {
        // system("cls");
        std::cout << "===================================" << std::endl;
        std::cout << "Nucleic Acid System                " << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << "1) Person Enqueue                  " << std::endl;
        std::cout << "2) Examine Queue                   " << std::endl;
        std::cout << "3) Show Queue                      " << std::endl;
        std::cout << "4) Add Tube Result                 " << std::endl;
        std::cout << "5) Search all people               " << std::endl;
        std::cout << "6) Personal Search                 " << std::endl;
        std::cout << "7) Initialtion                     " << std::endl;
        std::cout << "->Added Methods<-------------------" << std::endl;
        std::cout << "8) Add Person                      " << std::endl;
        std::cout << "0) Quit                            " << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << "Please input your choice: ";
        std::cin >> choice;
        switch (choice) {
        case 1: {
            try {
                std::cout << "Please input the person id (8 digits): ";
                id_t<8> id;
                std::cin >> id;
                std::cout << "Please input the queue id (2 digits, 00 for single test): ";
                id_t<2> queue_id;
                std::cin >> queue_id;
                nasys.EnquePerson(id, queue_id);
                std::cout << "Enque Success!" << std::endl;
            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 2: {
            try {
                std::cout << "Please input the queue id (2 digits, 00 for single test): ";
                id_t<2> queue_id;
                std::cin >> queue_id;
                std::cout << nasys.GetQueueFront(queue_id) << " is tested." << std::endl;
                nasys.AddExamine(queue_id);
            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 3: {
            try {
                nasys.ShowQueue();

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 4: {
            try {
                std::cout << "Please input the tube id (5 digits): ";
                id_t<5> tube_id;
                std::cin >> tube_id;
                std::cout << "Please input the result (positive or negative): ";
                RESULT_STATUS result;
                std::cin >> result;
                nasys.AddTubeResult(tube_id, result);
                std::cout << "Result Update Succeed" << std::endl;

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 5: {
            try {
                nasys.ShowStatus();

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 6: {
            try {
                std::cout << "Please input the person id (8 digits): ";
                id_t<8> id;
                std::cin >> id;
                time_t t;  // the current time - 10000s
                t = time(NULL);
                t -= 10000;
                nasys.ShowPersonalInfo(id, t);

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 7: {
            try {
                std::ifstream ifs("line_up.in");
                int n, m;
                ifs >> n >> m;
                id_t<8> tmp;
                for (int i = 0; i < n; ++i) {
                    ifs >> tmp;
                    nasys.EnquePerson(tmp, "01");
                }
                for (int i = 0; i < m; ++i) {
                    ifs >> tmp;
                    nasys.EnquePerson(tmp, "00");
                }
                ifs.close();
                std::ifstream ifss("nucleic_acid_test.in");
                int x, y;
                ifss >> x >> y;
                for (int i = 0; i < x; ++i) {
                    nasys.AddExamine("01");
                }
                for (int i = 0; i < y; ++i) {
                    nasys.AddExamine("00");
                }

            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 8: {
            try {
                std::cout << "Please input the person id (8 digits): ";
                id_t<8> id;
                std::cin >> id;
                std::cout << "Please input the name: ";
                std::string name;
                std::cin >> name;
                nasys.AddPerson(id, name);
            } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
            }
            std::cout << "Press any key to continue..." << std::endl;
            std::cin.clear();
            std::cin.sync();
            getchar();
            break;
        }
        case 0: return 0;
        }
    }
    return 0;
}