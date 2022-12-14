int fact(int n, int acc) {
    if (n == 1) {
        return acc;
    }
    return fact(n - 1, acc * n);
}

int main() { return fact(1000000, 1); }