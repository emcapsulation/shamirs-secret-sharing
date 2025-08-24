#include <random>
#include <vector>

#ifndef SHAMIRS_SECRET_SHARING_H
#define SHAMIRS_SECRET_SHARING_H

class ShamirsSecretSharing {
public:
	ShamirsSecretSharing(
		unsigned long long secret, 
		unsigned long long k
	);	

	unsigned long long getN() const;
	unsigned long long getK() const;
	std::vector<std::pair<unsigned long long, unsigned long long>> getShares() const;
	std::vector<std::pair<unsigned long long, unsigned long long>> generateShares(
		unsigned long long
	);
	static unsigned long long recoverSecret(
		const std::vector<std::pair<unsigned long long, unsigned long long>> &userShares
	);

private:
	unsigned long long secret;
	unsigned long long n;
	unsigned long long k;

	// 64-bit Mersenne Twister RNG
	std::mt19937_64 rng;

	std::vector<unsigned long long> coefficients;
	std::vector<std::pair<unsigned long long, unsigned long long>> shares;

	// The 8th Mersenne Prime, 2^61 - 1
	// Mersenne Primes are used in cryptography because they lead to fast mod operations
	static constexpr unsigned long long p = (1ULL << 61) - 1;

	std::vector<unsigned long long> generateCoefficients();
	unsigned long long evaluatePolynomial(unsigned long long) const;
	unsigned long long getRandomIntegerInRange(unsigned long long, unsigned long long);
	static unsigned long long getMultiplicativeInverse(unsigned long long);
	static unsigned long long modPower(unsigned long long, unsigned long long);
	static inline unsigned long long modSubtract(unsigned long long, unsigned long long);
	static inline unsigned long long modMultiply(unsigned long long, unsigned long long);
	static inline unsigned long long mod(unsigned long long);
	static inline unsigned long long mod(__uint128_t);
};

#endif