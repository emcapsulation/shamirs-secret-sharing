#include <iostream>
#include <cassert>

#include "shamir.h"


void RecoverSecret_IsSuccessful_WhenKShares(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret recovers the secret when k shares are combined.\n";
    
    unsigned long long secret = 1029384756, n = 6, k = 4;   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n'; 

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

    unsigned long long recoveredSecret = sss.recoverSecret({
        shares[0], shares[1], shares[2], shares[3]
    });
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    assert(recoveredSecret == secret);

    std::cout << "Passed ✅\n";
}


void RecoverSecret_IsUnsuccessful_WhenFewerThanKShares(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret creates an incorrect secret when <k shares are combined.\n";

    unsigned long long secret = 123456789, n = 6, k = 4;   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n';  

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

    unsigned long long recoveredSecret = sss.recoverSecret({
        shares[0], shares[1], shares[2]
    });
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    assert(recoveredSecret != secret);

    std::cout << "Passed ✅\n";
}


void RecoverSecret_IsSuccessful_WhenLargeK(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret recovers the secret when k is large.\n";
    
    unsigned long long secret = 957263748103, n = (1ULL << 14), k = (1ULL << 14);   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n'; 

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

    unsigned long long recoveredSecret = sss.recoverSecret(shares);
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    assert(recoveredSecret == secret);

    std::cout << "Passed ✅\n";
}


void GenerateShares_GeneratesMoreShares_WhenCalledMultipleTimes(int i) {
    std::cout << "\nTEST #" << i << ": generateShares will generate additional shares when called multiple times.\n";
    
    unsigned long long secret = 123456789, k = 4;
    unsigned long long n1 = 6, n2 = 5, n3 = 7;
    std::cout << "n1 = " << n1 << " | n2 = " << n2 << " | n3 = " << n3 << '\n';

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n1);

    std::cout << "Number of shares: " << shares.size() << " | n1: " << n1 << '\n';
    assert(shares.size() == 6);

    shares = sss.generateShares(n2);
    std::cout << "Number of shares: " << shares.size() << " | n1+n2: " << n1+n2 << '\n';
    assert(shares.size() == 6+5);

    shares = sss.generateShares(n3);
    std::cout << "Number of shares: " << shares.size() << " | n1+n2+n3: " << n1+n2+n3 << '\n';
    assert(shares.size() == 6+5+7);

    std::cout << "Passed ✅\n";
}


void GenerateShares_XValues_AreUnique1ToN(int i) {
    std::cout << "\nTEST #" << i << ": generateShares will create unique points.\n";
    
    unsigned long long secret = 123456789, k = 4;
    unsigned long long n1 = 2, n2 = 1, n3 = 3;
    std::cout << "n1 = " << n1 << " | n2 = " << n2 << " | n3 = " << n3 << '\n';

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n1);

    for (unsigned long long i = 0; i < shares.size(); i++) {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        assert(shares[i].first == i+1);
    }
    std::cout << '\n';

    shares = sss.generateShares(n2);
    for (unsigned long long i = 0; i < shares.size(); i++) {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        assert(shares[i].first == i+1);
    }
    std::cout << '\n';

    shares = sss.generateShares(n3);
    for (unsigned long long i = 0; i < shares.size(); i++) {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        assert(shares[i].first == i+1);
    }
    std::cout << '\n';

    std::cout << "Passed ✅\n";
}


int main() {
    std::vector<void (*)(int)> tests = {
        RecoverSecret_IsSuccessful_WhenKShares,
        RecoverSecret_IsUnsuccessful_WhenFewerThanKShares,
        RecoverSecret_IsSuccessful_WhenLargeK,
        GenerateShares_GeneratesMoreShares_WhenCalledMultipleTimes,
        GenerateShares_XValues_AreUnique1ToN
    };

    for (size_t i = 0; i < tests.size(); i++) {
        tests[i](i+1);
    }
    

    return 0;
}