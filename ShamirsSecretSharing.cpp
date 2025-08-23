#include <iostream>
#include <random>
#include <vector>

#include "shamirs-secret-sharing.h"


ShamirsSecretSharing::ShamirsSecretSharing(
	unsigned long long secret, 
	unsigned long long n, 
	unsigned long long k
) : secret(secret), n(n), k(k) {}


unsigned long long ShamirsSecretSharing::getK() {
	return this->k;
}


std::vector<unsigned long long> ShamirsSecretSharing::generateCoefficients() {
	unsigned long long numCoefficients = this->k-1;
	std::vector<unsigned long long> coefficients(numCoefficients);
	
	for (unsigned long long i = 0; i < numCoefficients; i++) {
		coefficients[i] = getRandomIntegerInRange(0, ShamirsSecretSharing::p - 1);
	}

	std::cout << "\nPOLYNOMIAL\n";
	std::cout << this->secret << " + ";
	unsigned long long xPow = 1;
	for (unsigned long long i = 0; i < numCoefficients; i++) {
		std::cout << coefficients[i] << " x^" << xPow;
		if (i < numCoefficients-1) {
			std::cout << " + ";
		}
		xPow += 1;
	}
	std::cout << '\n';

	return coefficients;
};


unsigned long long ShamirsSecretSharing::getShareYValue(
	unsigned long long x, 
	std::vector<unsigned long long> coefficients
) {
	// Constant term is the secret
	unsigned long long y = this->secret;

	unsigned long long curX = 1;
	for (unsigned long long i = 0; i < coefficients.size(); i++) {
		curX = (curX*x) % ShamirsSecretSharing::p;
		unsigned long long curTerm = (coefficients[i]*curX) % ShamirsSecretSharing::p;
		y = (y+curTerm) % ShamirsSecretSharing::p;
	}

	return y % ShamirsSecretSharing::p;
}


std::vector<std::pair<unsigned long long, unsigned long long>> ShamirsSecretSharing::generateNShares() {
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


unsigned long long ShamirsSecretSharing::recoverSecret(
	std::vector<std::pair<unsigned long long, unsigned long long>> shares
) {
	unsigned long long numShares = shares.size(), secret = 0;

	for (unsigned long long i = 0; i < numShares; i++) {
		unsigned long long curTerm = shares[i].second;

		unsigned long long numerator = 1, denominator = 1;
		for (unsigned long long j = 0; j < numShares; j++) {
			if (j != i) {
				numerator = (numerator * ShamirsSecretSharing::moduloSubtract(0, shares[j].first)) % ShamirsSecretSharing::p;
				denominator = (denominator * moduloSubtract(shares[i].first, shares[j].first)) % ShamirsSecretSharing::p;
			}
		}

		unsigned long long fraction = (
			numerator * ShamirsSecretSharing::getMultiplicativeInverse(denominator)
		) % ShamirsSecretSharing::p;
		curTerm = (curTerm*fraction) % ShamirsSecretSharing::p;
		secret = (secret + curTerm) % ShamirsSecretSharing::p;
	}

	return secret;
}


unsigned long long ShamirsSecretSharing::getRandomIntegerInRange(
	unsigned long long min, 
	unsigned long long max
) {
	// 64-bit Mersenne Twister RNG
	std::random_device rd;
	std::mt19937_64 random_number_generator(rd());

	std::uniform_int_distribution<unsigned long long> distribution(min, max);

	return distribution(random_number_generator);
}


unsigned long long ShamirsSecretSharing::getMultiplicativeInverse(
	unsigned long long divisor
) {
	return ShamirsSecretSharing::moduloPower(divisor, ShamirsSecretSharing::p-2);
}


unsigned long long ShamirsSecretSharing::moduloPower(
	unsigned long long base,
	unsigned long long exp
) {
	unsigned long long res = 1;

	while (exp > 0) {
		if (exp % 2 == 1) {
			res = (res * base) % ShamirsSecretSharing::p;
		}
		base = (base * base) % ShamirsSecretSharing::p;
		exp /= 2;
	}

	return res;
}


unsigned long long ShamirsSecretSharing::moduloSubtract(
	unsigned long long a,
	unsigned long long b
) {
	return (a >= b) ? (a - b) % ShamirsSecretSharing::p : (ShamirsSecretSharing::p - (b - a) % ShamirsSecretSharing::p) % ShamirsSecretSharing::p;
}