#include "shamir.h"

#include <stdexcept>


/**
 * The constructor.
 *  Given a secret and threshold, it immediately generates the degree k-1 
 *  polynomial: P(x) = secret + a_{1}x^{1} + ... + a_{k-1}x^{k-1}
 */
ShamirsSecretSharing::ShamirsSecretSharing(uint64_t secret, uint64_t threshold) 
: secret(secret), threshold(threshold), rng(std::random_device{}()) 
{
	if (secret > p-1)
		throw std::domain_error("Error: The secret is too large.");
	if (threshold < 2 || threshold > p-1)
		throw std::domain_error("Error: The threshold (k) is outside the range.");

	// Immediately hide the secret in the degree k-1 polynomial.
	this->coefficients = generateCoefficients();
}


uint64_t ShamirsSecretSharing::getNumShares() const 
{
	return this->shares.size();
}

uint64_t ShamirsSecretSharing::getThreshold() const 
{
	return this->threshold;
}

const std::vector<Share>& ShamirsSecretSharing::getShares() const 
{
	return this->shares;
}


/**
 * Generates n additional shares by selecting points which lie on the polynomial.
 *  x-values are [1, 2, ..., n]
 * New shares are added to the existing list of shares.
 * 
 * @param numToGenerate The number of new shares to create.
 * 
 * @return The list of all shares.
 */
void ShamirsSecretSharing::generateAdditionalShares(uint64_t numToGenerate) 
{
	size_t prevSize = this->shares.size();
	if (numToGenerate > p-1-prevSize)
		throw std::domain_error("Error: The number of shares requested is outside the range.");

	this->shares.reserve(prevSize + numToGenerate);
	for (uint64_t i = 0; i < numToGenerate; i++) 
	{
		uint64_t x = prevSize + i + 1;
		this->shares.emplace_back(x, evaluatePolynomial(x));
	}
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
uint64_t ShamirsSecretSharing::recoverSecret(const std::vector<Share> &userShares) 
{
	uint64_t secret = 0;

	for (const auto &[xi, yi] : userShares) 
	{
		if (xi < 1 || xi > p-1 || yi > p-1)
			throw std::domain_error("Error: A provided share is outside the field range.");

		// Numerator = (x-x_{1})...(x-x_{i-1})(x-x_{i+1})...(x-x_{k})
		uint64_t numerator = 1;

		// Denominator = (x_{i}-x_{1})...(x_{i}-x_{i-1})(x_{i}-x_{i+1})...(x_{i}-x_{k})
		uint64_t denominator = 1;

		int equalXCount = 0;
		for (const auto &[xj, yj] : userShares) 
		{
			if (xi == xj)
			{
				if (equalXCount > 0)
					throw std::invalid_argument("Error: Two or more of the provided shares had the same x-value.");
				equalXCount++;
				continue;
			}

			// (x-x_{j}), but x=0 in this case since we want the constant term
			uint64_t factor = modSubtract(0, xj);
			numerator = modMultiply(numerator, factor);

			// (x_{i}-x_{j})
			uint64_t divisor = modSubtract(xi, xj);
			denominator = modMultiply(denominator, divisor);
		}

		// To compute a/b (mod p), we must calculate a*b^{-1} (mod p)
		uint64_t fraction = modMultiply(numerator, getMultiplicativeInverse(denominator));

		// Multiply the term's fraction by the y-value of the current share, i
		uint64_t thisTerm = modMultiply(fraction, yi);

		// Add this term to the secret
		secret = mod(secret + thisTerm);
	}

	return secret;
}


/**
 * Generates k-1 random coefficients.
 * 
 * @return The list of coefficients a_{1}, a_{2}, ..., a_{k-1}
 */
std::vector<uint64_t> ShamirsSecretSharing::generateCoefficients() 
{
	uint64_t degree = threshold-1;
	std::vector<uint64_t> coefficients(degree, 0);
	
	for (size_t i = 0; i < degree-1; i++)
		coefficients[i] = getRandomIntegerInRange(0, p-1);
	coefficients[degree-1] = getRandomIntegerInRange(1, p-1);

	return coefficients;
}


/**
 * Uses the polynomial's coefficients to calculate the y-value given an x-value.
 * 
 * @param x The x-value.
 * 
 * @return The y-value.
 */
uint64_t ShamirsSecretSharing::evaluatePolynomial(uint64_t x) const 
{
	// The secret is the constant term of the polynomial
	uint64_t yValue = secret;

	uint64_t curX = 1;
	for (size_t i = 0; i < coefficients.size(); i++) 
	{
		// Increase the x exponent by 1
		curX = modMultiply(curX, x);

		// Multiply by this coefficient
		uint64_t curTerm = modMultiply(coefficients[i], curX);
		yValue = mod(yValue+curTerm);
	}

	return yValue;
}


/**
 * Gets a random integer in the range [min, max]
 * 
 * @param min The minimum value.
 * @param max The maximum value.
 * 
 * @return The random integer.
 */
uint64_t ShamirsSecretSharing::getRandomIntegerInRange(uint64_t min, uint64_t max) 
{
	std::uniform_int_distribution<uint64_t> distribution(min, max);
	return distribution(rng);
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
uint64_t ShamirsSecretSharing::getMultiplicativeInverse(uint64_t a) 
{
	if (a == 0)
		throw std::invalid_argument("Error: 0 has no multiplicative inverse.");

	return modPower(a, p-2);
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
uint64_t ShamirsSecretSharing::modPower(uint64_t base, uint64_t exp) 
{
	uint64_t res = 1, b = base;

	for (; exp > 0; exp >>= 1) 
	{
		if (exp & 1)
			res = modMultiply(res, b);
		b = modMultiply(b, b);
	}

	return res;
}


/**
 * Computes a-b (mod p). The +p is required in case a-b is negative.
 * 
 * @param a
 * @param b
 * 
 * @return a-b (mod p)
 */
uint64_t ShamirsSecretSharing::modSubtract(uint64_t a, uint64_t b) 
{
	return mod(a + p - b);
}


/**
 * Computes a*b (mod p).
 * 
 * @param a
 * @param b
 * 
 * @return a*b (mod p)
 */
uint64_t ShamirsSecretSharing::modMultiply(uint64_t a, uint64_t b) 
{
	return static_cast<uint64_t>(mod(__uint128_t(a) * b));
}


/**
 * Computes a (mod p). Overloads for uint64_t or __uint128_t.
 * 
 * @param a
 * 
 * @return a (mod p)
 */
uint64_t ShamirsSecretSharing::mod(uint64_t a) 
{
	return a % p;
}

uint64_t ShamirsSecretSharing::mod(__uint128_t a) 
{
	return static_cast<uint64_t>(a % p);
}
