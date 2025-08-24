#include <iostream>
#include <cassert>

#include "shamir.h"


static constexpr unsigned long long sssPrime = (1ULL << 61) - 1;


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
    
    if (recoveredSecret != secret) {
        throw std::logic_error("Failed: Expected recoveredSecret == secret.");
    }
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
    
    if (recoveredSecret == secret) {
        throw std::logic_error("Failed: Expected recoveredSecret != secret.");
    }
}


void RecoverSecret_IsSuccessful_WhenLargeK(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret recovers the secret when k is large.\n";
    
    unsigned long long secret = 957263748103, n = (1ULL << 14), k = (1ULL << 14);   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n'; 

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

    unsigned long long recoveredSecret = sss.recoverSecret(shares);
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    
    if (recoveredSecret != secret) {
        throw std::logic_error("Failed: Exepcted recoveredSecret == secret.");
    }    
}


void GenerateShares_GeneratesMoreShares_WhenCalledMultipleTimes(int i) {
    std::cout << "\nTEST #" << i << ": generateShares will generate additional shares when called multiple times.\n";
    
    unsigned long long secret = 123456789, k = 4;
    unsigned long long n1 = 6, n2 = 5, n3 = 7;
    std::cout << "n1 = " << n1 << " | n2 = " << n2 << " | n3 = " << n3 << '\n';

    ShamirsSecretSharing sss(secret, k);
    std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n1);

    std::cout << "Number of shares: " << shares.size() << " | n1: " << n1 << '\n';
    if (shares.size() != 6) {
        throw std::logic_error("Failed: Expected number of shares == 6.");
    }

    shares = sss.generateShares(n2);
    std::cout << "Number of shares: " << shares.size() << " | n1+n2: " << n1+n2 << '\n';
    if (shares.size() != 6+5) {
        throw std::logic_error("Failed: Expected number of shares == 6+5.");
    }

    shares = sss.generateShares(n3);
    std::cout << "Number of shares: " << shares.size() << " | n1+n2+n3: " << n1+n2+n3 << '\n';
    if (shares.size() != 6+5+7) {
        throw std::logic_error("Failed: Expected number of shares == 6+5+7.");
    }
}


void testRecoverSecretThrowsError(unsigned long long x, unsigned long long y) {
    unsigned long long secret = 123456789, k = 2;
    ShamirsSecretSharing sss(secret, k);

    try {
        sss.recoverSecret({{x, y}, {2, 2}});
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } catch (const std::domain_error &e) {
        // Do nothing, test passed
    }
}


void RecoverSecret_ThrowsDomainError_WhenShareIsTooLarge(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret throws a domain error when the x or y-value is >= p.\n";

    unsigned long long x = sssPrime, y = 2;
    std::cout << "x: " << x << " y: " << y << '\n';
    testRecoverSecretThrowsError(x, y);

    x = 2;
    y = sssPrime;
    std::cout << "x: " << x << " y: " << y << '\n';
    testRecoverSecretThrowsError(x, y);    
}


void RecoverSecret_ThrowsInvalidArgument_WhenXValuesNotUnique(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret throws an invalid argument exception if two shares have the same x-value.\n";

    unsigned long long secret = 123456789, k = 3;
    ShamirsSecretSharing sss(secret, k);

    try {
        sss.recoverSecret({{1, 2}, {2, 3}, {1, 4}});
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } catch (const std::invalid_argument &e) {
        // Do nothing, test passed
    }
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
        if (shares[i].first != i+1) {
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
        }
    }
    std::cout << '\n';

    shares = sss.generateShares(n2);
    for (unsigned long long i = 0; i < shares.size(); i++) {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        if (shares[i].first != i+1) {
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
        }
    }
    std::cout << '\n';

    shares = sss.generateShares(n3);
    for (unsigned long long i = 0; i < shares.size(); i++) {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        if (shares[i].first != i+1) {
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
        }
    }
}


void GenerateShares_ThrowsDomainError_WhenNIsTooLarge(int i) {
    std::cout << "\nTEST #" << i << ": generateShares will throw an error if n >= p.\n";
    
    unsigned long long secret = 1029384756, n = sssPrime, k = 4;   
    std::cout << "n = " << n << '\n';

    ShamirsSecretSharing sss(secret, k);

    try {
        std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } catch (const std::domain_error &e) {
        // Do nothing, test passed
    }
}


void testConstructorThrowsError(unsigned long long secret, unsigned long long k) {
    try {
        ShamirsSecretSharing sss(secret, k);
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } catch (const std::domain_error &e) {
        // Do nothing, test passed
    }
}


void Constructor_ThrowsDomainError_WhenSecretIsLargerThanP(int i) {
    std::cout << "\nTEST #" << i << ": The constructor will throw an error if the secret is >= p.\n";
    
    unsigned long long secret = sssPrime, k = 4;
    std::cout << "secret = " << secret << '\n'; 
    testConstructorThrowsError(secret, k);    
}


void Constructor_ThrowsDomainError_WhenKIsOutOfDomain(int i) {
    std::cout << "\nTEST #" << i << ": The constructor will throw an error if k >= p or k <= 1.\n";
    
    unsigned long long secret = 5, k = sssPrime;
    std::cout << "k = " << k << '\n';
    testConstructorThrowsError(secret, k);


    k = 1;
    std::cout << "k = " << k << '\n';
    testConstructorThrowsError(secret, k);


    k = 0;
    std::cout << "k = " << k << '\n'; 
    testConstructorThrowsError(secret, k);
}


int main() {
    std::vector<void (*)(int)> tests = {
        RecoverSecret_IsSuccessful_WhenKShares,
        RecoverSecret_IsUnsuccessful_WhenFewerThanKShares,
        RecoverSecret_IsSuccessful_WhenLargeK,
        RecoverSecret_ThrowsDomainError_WhenShareIsTooLarge,
        RecoverSecret_ThrowsInvalidArgument_WhenXValuesNotUnique,
        GenerateShares_GeneratesMoreShares_WhenCalledMultipleTimes,
        GenerateShares_XValues_AreUnique1ToN,
        GenerateShares_ThrowsDomainError_WhenNIsTooLarge,
        Constructor_ThrowsDomainError_WhenSecretIsLargerThanP,
        Constructor_ThrowsDomainError_WhenKIsOutOfDomain
    };

    int passed = 0, failed = 0;
    for (size_t i = 0; i < tests.size(); i++) {
        try {
            tests[i](i+1);
            std::cout << "Passed ✅\n";
            passed += 1;
        } catch (const std::logic_error &e) {
            std::cerr << "Failed ❌\n" << e.what() << std::endl;
            failed += 1;
        }        
    }    

    std::cout << "\nResults: " << passed << "/" << (passed+failed) << " passed." << std::endl;
    return 0;
}