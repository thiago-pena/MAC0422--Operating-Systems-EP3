#include <iostream>
using namespace std;

int main() {
    long long max;
    cin >> max;
    for (long long i = 0; i < max; i++)
        cout << i%9 << " ";
}
