#include <climits>
#include <iostream>

int Sum(int a, int b) {
    if (a > 0 && b > INT_MAX - a) {
        return INT_MAX;
    } else if (a < 0 && b < INT_MIN - a) {
        return INT_MIN;
    }
    return a + b;
}
