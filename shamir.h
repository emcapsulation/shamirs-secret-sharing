#ifndef SHAMIRS_SECRET_SHARING_H
#define SHAMIRS_SECRET_SHARING_H

#include <cstdint>
#include <random>
#include <utility>
#include <vector>

using Share = std::pair<uint64_t, uint64_t>;

class ShamirsSecretSharing {
public:
	ShamirsSecretSharing(uint64_t secret, uint64_t threshold);	

	uint64_t getNumShares() const;
	uint64_t getThreshold() const;
	void generateAdditionalShares(uint64_t numToGenerate);
	const std::vector<Share>& getShares() const;

	// Static because combining the shares is independent of state
	static uint64_t recoverSecret(const std::vector<Share> &userShares);

private:
	uint64_t secret;
	uint64_t threshold;

	// 64-bit Mersenne Twister RNG
	std::mt19937_64 rng;

	std::vector<uint64_t> coefficients;
	std::vector<Share> shares;

	// The 8th Mersenne Prime, 2^61 - 1
	// Mersenne Primes are used in cryptography because they lead to fast mod operations
	static constexpr uint64_t p = (1ULL << 61) - 1;

	std::vector<uint64_t> generateCoefficients();
	uint64_t evaluatePolynomial(uint64_t x) const;
	uint64_t getRandomIntegerInRange(uint64_t min, uint64_t max);
	static uint64_t getMultiplicativeInverse(uint64_t a);
	static uint64_t modPower(uint64_t base, uint64_t exp);
	static uint64_t modSubtract(uint64_t a, uint64_t b);
	static uint64_t modMultiply(uint64_t a, uint64_t b);
	static uint64_t mod(uint64_t a);
	static uint64_t mod(__uint128_t a);
};

#endif