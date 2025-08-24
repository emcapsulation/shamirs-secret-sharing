#include <iostream>
#include <cassert>

#include "shamir.h"


int main() {

    std::cout << "\nTEST #1: Basic secret recovery\n";
    unsigned long long secret = 123456789;
    unsigned long long k = 3;
    unsigned long long n = 6;

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

    unsigned long long recoveredSecret = sss.recoverSecret({
        shares[0], shares[1], shares[2]
    });
    assert(recoveredSecret == secret);
    std::cout << "Passed ✅\n";

    return 0;
}