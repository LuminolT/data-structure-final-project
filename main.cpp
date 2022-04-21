#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "bpnode.h"
#include "bptree.h"

using namespace std;

int main() {
    // bpnode<char, 5> root(1);
    bptree<int, 5> tree(std::string("tree1"));
    int a = 1, b;
    while (true) {
        std::cin >> a >> b;
        if (a == -1) break;
        tree.insert(a, b);
    }
    return 0;
}