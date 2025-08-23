#include <vector>

#ifndef SHAMIRS_SECRET_SHARING_H
#define SHAMIRS_SECRET_SHARING_H

unsigned long long getULLInput(std::string);
unsigned long long getRandomIntegerInRange(unsigned long long, unsigned long long);

class SecretSharing {
public:
	SecretSharing(
		unsigned long long secret, 
		unsigned long long n, 
		unsigned long long k
	);	

	std::vector<std::pair<unsigned long long, unsigned long long>> generateNShares();

private:
	unsigned long long secret;
	unsigned long long n;
	unsigned long long k;
	static constexpr unsigned long long p = (1ULL << 61) - 1;

	std::vector<unsigned long long> generateCoefficients();
	unsigned long long getShareYValue(unsigned long long, std::vector<unsigned long long>);
};

#endif