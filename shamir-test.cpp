#include "shamir.h"

#include <iostream>
#include <cassert>
#include <stdexcept>


static constexpr uint64_t sssPrime = (1ULL << 61) - 1;


void RecoverSecret_IsSuccessful_WhenKShares(int i) 
{
    std::cout << "\nTEST #" << i << ": recoverSecret recovers the secret when k shares are combined.\n";
    
    uint64_t secret = 1029384756, n = 6, k = 4;   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n'; 

    ShamirsSecretSharing sss(secret, k);
    sss.generateAdditionalShares(n);
    std::vector<Share> shares = sss.getShares();

    uint64_t recoveredSecret = sss.recoverSecret( {shares[0], shares[1], shares[2], shares[3]} );
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    
    if (recoveredSecret != secret)
        throw std::logic_error("Failed: Expected recoveredSecret == secret.");
}


void RecoverSecret_IsUnsuccessful_WhenFewerThanKShares(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret creates an incorrect secret when <k shares are combined.\n";

    uint64_t secret = 123456789, n = 6, k = 4;   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n';  

    ShamirsSecretSharing sss(secret, k);
    sss.generateAdditionalShares(n);
    std::vector<Share> shares = sss.getShares();

    uint64_t recoveredSecret = sss.recoverSecret( {shares[0], shares[1], shares[2]} );
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    
    if (recoveredSecret == secret)
        throw std::logic_error("Failed: Expected recoveredSecret != secret.");
}


void RecoverSecret_IsSuccessful_WhenLargeK(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret recovers the secret when k is large.\n";
    
    uint64_t secret = 957263748103, n = (1ULL << 14), k = (1ULL << 14);   
    std::cout << "secret = " << secret << " | n = " << n << " | k = " << k << '\n'; 

    ShamirsSecretSharing sss(secret, k);
    sss.generateAdditionalShares(n);
    std::vector<Share> shares = sss.getShares();

    uint64_t recoveredSecret = sss.recoverSecret(shares);
    std::cout << "recoveredSecret: " << recoveredSecret << " | secret: " << secret << '\n';
    
    if (recoveredSecret != secret)
        throw std::logic_error("Failed: Exepcted recoveredSecret == secret.");
}


void GenerateAdditionalShares_GeneratesMoreShares_WhenCalledMultipleTimes(int i) {
    std::cout << "\nTEST #" << i << ": generateAdditionalShares will generate additional shares when called multiple times.\n";
    
    uint64_t secret = 123456789, k = 4;
    uint64_t n1 = 6, n2 = 5, n3 = 7;
    std::cout << "n1 = " << n1 << " | n2 = " << n2 << " | n3 = " << n3 << '\n';

    ShamirsSecretSharing sss(secret, k);
    sss.generateAdditionalShares(n1);
    std::vector<Share> shares = sss.getShares();

    std::cout << "Number of shares: " << shares.size() << " | n1: " << n1 << '\n';
    if (shares.size() != 6)
        throw std::logic_error("Failed: Expected number of shares == 6.");

    sss.generateAdditionalShares(n2);
    shares = sss.getShares();
    std::cout << "Number of shares: " << shares.size() << " | n1+n2: " << n1+n2 << '\n';
    if (shares.size() != 6+5)
        throw std::logic_error("Failed: Expected number of shares == 6+5.");

    sss.generateAdditionalShares(n3);
    shares = sss.getShares();
    std::cout << "Number of shares: " << shares.size() << " | n1+n2+n3: " << n1+n2+n3 << '\n';
    if (shares.size() != 6+5+7)
        throw std::logic_error("Failed: Expected number of shares == 6+5+7.");
}


void testRecoverSecretThrowsError(uint64_t x, uint64_t y) {
    uint64_t secret = 123456789, k = 2;
    ShamirsSecretSharing sss(secret, k);

    try 
    {
        sss.recoverSecret({{x, y}, {2, 2}});
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } 
    catch (const std::domain_error &e) { /* Do nothing, test passed */ }
}


void RecoverSecret_ThrowsDomainError_WhenShareIsTooLarge(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret throws a domain error when the x or y-value is >= p.\n";

    uint64_t x = sssPrime, y = 2;
    std::cout << "x: " << x << " y: " << y << '\n';
    testRecoverSecretThrowsError(x, y);

    x = 2;
    y = sssPrime;
    std::cout << "x: " << x << " y: " << y << '\n';
    testRecoverSecretThrowsError(x, y);    
}


void RecoverSecret_ThrowsDomainError_WhenShareIsTheSecret(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret throws a domain error when the share is the secret itself.\n";

    uint64_t x = 0, y = 2;
    std::cout << "x: " << x << " y: " << y << '\n';
    testRecoverSecretThrowsError(x, y);
}


void RecoverSecret_ThrowsInvalidArgument_WhenXValuesNotUnique(int i) {
    std::cout << "\nTEST #" << i << ": recoverSecret throws an invalid argument exception if two shares have the same x-value.\n";

    uint64_t secret = 123456789, k = 3;
    ShamirsSecretSharing sss(secret, k);

    try 
    {
        sss.recoverSecret({{1, 2}, {2, 3}, {1, 4}});
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } 
    catch (const std::invalid_argument &e) { /* Do nothing, test passed */ }
}


void GenerateAdditionalShares_XValues_AreUnique1ToN(int i) {
    std::cout << "\nTEST #" << i << ": generateAdditionalShares will create unique points.\n";
    
    uint64_t secret = 123456789, k = 4;
    uint64_t n1 = 2, n2 = 1, n3 = 3;
    std::cout << "n1 = " << n1 << " | n2 = " << n2 << " | n3 = " << n3 << '\n';

    ShamirsSecretSharing sss(secret, k);
    sss.generateAdditionalShares(n1);
    std::vector<Share> shares = sss.getShares();

    for (size_t i = 0; i < shares.size(); i++) 
    {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        if (shares[i].first != i+1)
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
    }
    std::cout << '\n';

    sss.generateAdditionalShares(n2);
    for (uint64_t i = 0; i < shares.size(); i++) 
    {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        if (shares[i].first != i+1)
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
    }
    std::cout << '\n';

    sss.generateAdditionalShares(n3);
    for (uint64_t i = 0; i < shares.size(); i++) 
    {
        std::cout << "Share #" << i+1 << " x-value: " << shares[i].first << '\n';
        if (shares[i].first != i+1)
            throw std::logic_error("Failed: Expected share x-value to be i+1.");
    }
}


void GenerateAdditionalShares_ThrowsDomainError_WhenNIsTooLarge(int i) {
    std::cout << "\nTEST #" << i << ": generateAdditionalShares will throw an error if n >= p.\n";
    
    uint64_t secret = 1029384756, n = sssPrime, k = 4;   
    std::cout << "n = " << n << '\n';

    ShamirsSecretSharing sss(secret, k);    

    try 
    {
        sss.generateAdditionalShares(n);
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } 
    catch (const std::domain_error &e) { /* Do nothing, test passed */ }
}


void testConstructorThrowsError(uint64_t secret, uint64_t k) {
    try {
        ShamirsSecretSharing sss(secret, k);
        throw std::logic_error("Failed: Expected domain error to be thrown.");
    } 
    catch (const std::domain_error &e) { /* Do nothing, test passed */ }
}


void Constructor_ThrowsDomainError_WhenSecretIsLargerThanP(int i) {
    std::cout << "\nTEST #" << i << ": The constructor will throw an error if the secret is >= p.\n";
    
    uint64_t secret = sssPrime, k = 4;
    std::cout << "secret = " << secret << '\n'; 
    testConstructorThrowsError(secret, k);    
}


void Constructor_ThrowsDomainError_WhenKIsOutOfDomain(int i) {
    std::cout << "\nTEST #" << i << ": The constructor will throw an error if k >= p or k <= 1.\n";
    
    uint64_t secret = 5, k = sssPrime;
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
        RecoverSecret_ThrowsDomainError_WhenShareIsTheSecret,
        RecoverSecret_ThrowsInvalidArgument_WhenXValuesNotUnique,
        GenerateAdditionalShares_GeneratesMoreShares_WhenCalledMultipleTimes,
        GenerateAdditionalShares_XValues_AreUnique1ToN,
        GenerateAdditionalShares_ThrowsDomainError_WhenNIsTooLarge,
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