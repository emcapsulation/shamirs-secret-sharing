#include <vector>

#ifndef SHAMIRS_SECRET_SHARING_H
#define SHAMIRS_SECRET_SHARING_H

class ShamirsSecretSharing {
public:
	ShamirsSecretSharing(
		unsigned long long secret, 
		unsigned long long n, 
		unsigned long long k
	);	

	unsigned long long getK();
	std::vector<std::pair<unsigned long long, unsigned long long>> generateNShares();
	static unsigned long long recoverSecret(
		std::vector<std::pair<unsigned long long, unsigned long long>>
	);

private:
	unsigned long long secret;
	unsigned long long n;
	unsigned long long k;
	// static constexpr unsigned long long p = (1ULL << 61) - 1;
	static constexpr unsigned long long p = 11;

	std::vector<unsigned long long> generateCoefficients();
	unsigned long long getShareYValue(unsigned long long, std::vector<unsigned long long>);
	static unsigned long long getRandomIntegerInRange(unsigned long long, unsigned long long);
	static unsigned long long getMultiplicativeInverse(unsigned long long);
	static unsigned long long moduloPower(unsigned long long, unsigned long long);
	static unsigned long long moduloSubtract(unsigned long long, unsigned long long);
};

#endif