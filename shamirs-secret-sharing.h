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
	std::vector<unsigned long long> coefficients;
	std::vector<std::pair<unsigned long long, unsigned long long>> shares;
	static constexpr unsigned long long p = (1ULL << 61) - 1;

	std::vector<unsigned long long> generateCoefficients();
	unsigned long long getShareYValue(unsigned long long) const;
	static unsigned long long getRandomIntegerInRange(unsigned long long, unsigned long long);
	static unsigned long long getMultiplicativeInverse(unsigned long long);
	static unsigned long long moduloPower(unsigned long long, unsigned long long);
	static inline unsigned long long moduloSubtract(unsigned long long, unsigned long long);
	static inline unsigned long long moduloMultiply(unsigned long long, unsigned long long);
};

#endif