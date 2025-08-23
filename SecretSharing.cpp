#include <iostream>
#include <vector>

#include "shamirs-secret-sharing.h"


SecretSharing::SecretSharing(
	unsigned long long secret, 
	unsigned long long n, 
	unsigned long long k
) : secret(secret), n(n), k(k) {}


std::vector<unsigned long long> SecretSharing::generateCoefficients() {
	unsigned long long numCoefficients = this->k-1;
	std::vector<unsigned long long> coefficients(numCoefficients);

	for (unsigned long long i = 0; i < numCoefficients; i++) {
		coefficients[i] = getRandomIntegerInRange(0, SecretSharing::p - 1);
	}

	return coefficients;
};


unsigned long long SecretSharing::getShareYValue(
	unsigned long long x, 
	std::vector<unsigned long long> coefficients
) {
	// Secret is the constant term
	unsigned long long y = this->secret;

	unsigned long long curX = 1;
	for (unsigned long long i = 0; i < coefficients.size(); i++) {
		curX = (curX*x) % SecretSharing::p;
		unsigned long long curTerm = (coefficients[i]*curX) % SecretSharing::p;
		y = (y+curTerm) % SecretSharing::p;
	}

	return y % SecretSharing::p;
}


std::vector<std::pair<unsigned long long, unsigned long long>> SecretSharing::generateNShares() {
	unsigned long long numShares = this->n;
	std::vector<std::pair<unsigned long long, unsigned long long>> shares(numShares);

	std::vector<unsigned long long> coefficients = generateCoefficients();

	for (unsigned long long i = 0; i < numShares; i++) {
		// x values are [1, 2, ..., n]
		unsigned long long x = i+1;
		shares[i] = {x, getShareYValue(x, coefficients)};
	}

	return shares;
}