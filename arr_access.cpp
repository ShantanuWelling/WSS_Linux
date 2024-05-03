#include <iostream>
#include <vector>
#include <random>

int main() {
    int sz = 1;
    std::vector<int> lst(sz);

    for (int i = 0; i < sz; i++) {
        lst[i] = i;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sz-1);

    while (true) {
        int idx = dis(gen);
        int a = lst[idx];
        // Do something with 'a'
    }

    return 0;
}
