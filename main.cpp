#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bpnode.h"
#include "bptree.h"

using namespace std;

int main() {
    bptree<int, 5> tree(std::string("tree1"));
    for (int i = 1; i <= 10; i++) {
        tree.insert(i, i);
    }
    tree.search(3, 4, [](auto i) { cout << i << " "; });
    int n;
    char op;
    while (cin >> op >> n) {
        if (n == -1) {
            break;
        }
        if (op == 'S') {
            try {
                tree.search(n, [](auto i) { cout << i << "\n"; });
            } catch (std::exception &e) {
                cout << e.what() << "\n";
            }
        } else {
            tree.remove(n);
        }
    }
    return 0;
}