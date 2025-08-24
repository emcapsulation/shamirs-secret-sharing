#include <iostream>
#include <random>
#include <vector>

#include "shamirs-secret-sharing.h"


/**
 * Outputs a polynomial given k-1 coefficients and the secret, s.
 * 
 * @param secret The secret key, embedded in the constant term.
 * @param coefficients The list of k-1 coefficients, from a_{1} to a_{k-1}.
 * @param p The prime modulo.
 * 
 * @return Void
 */
void outputPolynomial(
	unsigned long long secret, 
	const std::vector<unsigned long long> &coefficients, 
	unsigned long long p
) {
	std::cout << "\nPOLYNOMIAL\n";

	std::cout << "P(x) = ";
	std::cout << secret << " + ";
	unsigned long long xPow = 1;
	for (unsigned long long i = 0; i < coefficients.size(); i++) {
		std::cout << coefficients[i] << " x^" << xPow;
		if (i < coefficients.size()-1) {
			std::cout << " + ";
		}
		xPow += 1;
	}
	std::cout << " (mod " << p << ")\n";
}


/**
 * The constructor.
 *  Given a secret and threshold, it immediately generates the degree k-1 
 *  polynomial: P(x) = secret + a_{1}x^{1} + ... + a_{k-1}x^{k-1}
 * 
 * @return Nothing, it's a constructor
 */
ShamirsSecretSharing::ShamirsSecretSharing(
	unsigned long long secret,
	unsigned long long k
) : secret(secret), k(k) {

	// n starts at 0 because the user will ask to generate shares later.
	this->n = 0;

	// Immediately hide the secret in the degree k-1 polynomial.
	this->coefficients = generateCoefficients();
}


unsigned long long ShamirsSecretSharing::getN() const {
	return this->n;
}

unsigned long long ShamirsSecretSharing::getK() const {
	return this->k;
}


/**
 * Generates k-1 random coefficients.
 * 
 * @return The list of coefficients a_{1}, a_{2}, ..., a_{k-1}
 */
std::vector<unsigned long long> ShamirsSecretSharing::generateCoefficients() {
	unsigned long long degree = this->k-1;
	std::vector<unsigned long long> coefficients(degree, 0);
	
	for (unsigned long long i = 0; i < coefficients.size(); i++) {
		coefficients[i] = getRandomIntegerInRange(0, ShamirsSecretSharing::p - 1);
	}

	outputPolynomial(this->secret, coefficients, ShamirsSecretSharing::p);

	return coefficients;
};


/**
 * Uses the polynomial's coefficients to calculate the y value of the share.
 * 
 * @param x The x-value of the share.
 * 
 * @return The y-value of the share.
 */
unsigned long long ShamirsSecretSharing::getShareYValue(
	unsigned long long x
) const {
	// The secret is the constant term of the polynomial
	__uint128_t yValue = this->secret;

	__uint128_t curX = 1;
	for (unsigned long long i = 0; i < this->coefficients.size(); i++) {
		// Increase the x exponent by 1
		curX = ShamirsSecretSharing::moduloMultiply(curX, x);

		// Multiply by this coefficient
		__uint128_t curTerm = ShamirsSecretSharing::moduloMultiply(this->coefficients[i], curX);
		yValue = (yValue+curTerm) % ShamirsSecretSharing::p;
	}

	return static_cast<unsigned long long>(yValue);
}


/**
 * Generates n additional shares by selecting points which lie on the polynomial.
 *  x-values are [1, 2, ..., n]
 * New shares are added to the existing list of shares.
 * 
 * @param n The number of new shares to create.
 * 
 * @return The list of all shares.
 */
std::vector<std::pair<unsigned long long, unsigned long long>> ShamirsSecretSharing::generateShares(
	unsigned long long n
) {

	this->shares.resize(this->n + n);

	for (unsigned long long i = this->n; i < this->shares.size(); i++) {
		unsigned long long x = i+1;
		this->shares[i] = {x, getShareYValue(x)};
	}

	this->n = this->n + n;
	return this->shares;
}


/**
 * Uses the Lagrange Interpolation Formula to recover the secret.
 *  If incorrect shares or less than k shares are inputted it will still return
 *  a result, it just won't be the correct secret. This is by design.
 * 
 * P(x) = y_{1} * ((x-x_{2})...(x-x_{k})) / ((x_{1}-x_{2})...(x_{1}-x_{k}))
 * 
 *        + ... + y_{i} * ((x-x_{1})...(x-x_{i-1})(x-x_{i+1})...(x-x_{k})) /
 *             ((x_{i}-x_{1})...(x_{i}-x_{i-1})(x_{i}-x_{i+1})...(x_{i}-x_{k}))
 * 
 *        + ... + y_{k} * ((x-x_{1})...(x-x_{k-1})) / 
 *             ((x_{k}-x_{1})...(x_{k}-x_{k-1}))
 * 
 * @param shares The list of shares to interpolate.
 * 
 * @return The secret based on the Lagrange Interpolation Formula at P(0).
 */
unsigned long long ShamirsSecretSharing::recoverSecret(
	const std::vector<std::pair<unsigned long long, unsigned long long>> &userShares
) {
	__uint128_t secret = 0;

	for (unsigned long long i = 0; i < userShares.size(); i++) {
		// Numerator = (x-x_{1})...(x-x_{i-1})(x-x_{i+1})...(x-x_{k})
		__uint128_t numerator = 1;

		// Denominator = (x_{i}-x_{1})...(x_{i}-x_{i-1})(x_{i}-x_{i+1})...(x_{i}-x_{k})
		__uint128_t denominator = 1;

		for (unsigned long long j = 0; j < userShares.size(); j++) {
			if (j != i) {
				// (x-x_{j}), but x=0 in this case since we want the constant term
				unsigned long long factor = ShamirsSecretSharing::moduloSubtract(0, userShares[j].first);
				numerator = ShamirsSecretSharing::moduloMultiply(numerator, factor);

				// (x_{i}-x_{j})
				unsigned long long divisor = moduloSubtract(userShares[i].first, userShares[j].first);
				denominator = ShamirsSecretSharing::moduloMultiply(denominator, divisor);
			}
		}

		// To compute a/b (mod p), we must calculate a*b^{-1} (mod p)
		__uint128_t fraction = ShamirsSecretSharing::moduloMultiply(
			numerator, 
			ShamirsSecretSharing::getMultiplicativeInverse(denominator)
		);

		// Multiply the term's fraction by the y-value of the current share, i
		__uint128_t thisTerm = ShamirsSecretSharing::moduloMultiply(fraction, userShares[i].second);

		// Add this term to the secret
		secret = (secret + thisTerm) % ShamirsSecretSharing::p;
	}

	return static_cast<unsigned long long>(secret);
}


/**
 * Gets a random integer in the range [min, max]
 * 
 * @param min The minimum value.
 * @param max The maximum value.
 * 
 * @return The random integer.
 */
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


/**
 * Calculates the multiplicative inverse of an integer in the field Fp.
 *  This uses Fermat's Little Theorem: a^{p-1} = 1 (mod p)
 *  Which implies that a^{-1} = a^{p-2} (mod p)
 * 
 * @param a The integer to get the multiplicative inverse of.
 * 
 * @return The multiplicative inverse, a^{-1} (mod p).
 */
unsigned long long ShamirsSecretSharing::getMultiplicativeInverse(
	unsigned long long a
) {
	return ShamirsSecretSharing::moduloPower(a, ShamirsSecretSharing::p-2);
}


/**
 * Computes base^exp (mod p) by successive squaring.
 *  In each iteration, if exp is odd, multiply the result by the base to make 
 *  the exponent even. Then square the base and halve the exponent.
 * 
 * @param base The base.
 * @param exp The exponent.
 * 
 * @return base^exp (mod p)
 */
unsigned long long ShamirsSecretSharing::moduloPower(
	unsigned long long base,
	unsigned long long exp
) {
	__uint128_t res = 1, b = base;

	for (; exp > 0; exp >>= 1) {
		if (exp & 1) {
			res = (res * b) % ShamirsSecretSharing::p;
		}
		b = (b * b) % ShamirsSecretSharing::p;
	}

	return static_cast<unsigned long long>(res);
}


/**
 * Computes a-b (mod p).
 *  If a-b is positive, just complete the subtraction.
 *  If a-b is negative, (a-b) = p - (b-a) (mod p)
 * 
 * @param a
 * @param b
 * 
 * @return a-b (mod p)
 */
inline unsigned long long ShamirsSecretSharing::moduloSubtract(
	unsigned long long a,
	unsigned long long b
) {
	return (a >= b) ? 
		(a - b) % ShamirsSecretSharing::p : 
		(ShamirsSecretSharing::p - (b - a) % ShamirsSecretSharing::p) % ShamirsSecretSharing::p;
}


/**
 * Computes a*b (mod p).
 * 
 * @param a
 * @param b
 * 
 * @return a*b (mod p)
 */
inline unsigned long long ShamirsSecretSharing::moduloMultiply(
	unsigned long long a,
	unsigned long long b
) {
	return static_cast<unsigned long long>((__uint128_t(a) * b) % ShamirsSecretSharing::p);
}