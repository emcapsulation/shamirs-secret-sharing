#include <iostream>
#include <unordered_set>

#include "shamirs-secret-sharing.h"


/**
 * Gets unsigned long long input from the user and validates it.
 * 
 * @param prompt The message to prompt the user for input.
 * 
 * @return The user's ULL input.
 */
unsigned long long getULLInput(std::string prompt) {
	unsigned long long input;

	bool valid = false;
	while (!valid) {
		std::cout << prompt;
		std::cin >> input;

		if (std::cin.fail()) {
			std::cout << "Invalid input.\n";

			// Reset the buffer
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		} else {
			valid = true;
		}
	}

	return input;
}


/**
 * Prompts the user for a secret and threshold k to initialise the SSS object.
 * 
 * @param secret The secret to hide in the constant term of the polynomial.
 * @param k The number of shares required to recover the secret. Leads to a 
 *        degree k-1 polynomial being generated.
 * 
 * @return The ShamirsSecretSharing instance.
 */
ShamirsSecretSharing hideSecret() {	
	unsigned long long secret = getULLInput("Enter the secret (S): ");
	unsigned long long k = getULLInput("Enter the number of shares which need to be combined to recover the secret (k): ");

	ShamirsSecretSharing sss(secret, k);
	return sss;
}


/**
 * Prompts the user for a number of shares to be generated, and then outputs 
 * the full list of shares.
 * 
 * @param sss The ShamirsSecretSharing instance to generate the shares.
 * 
 * @return Void
 */
void generateShares(ShamirsSecretSharing &sss) {
	std::cout << "\nCREATE SHARES\n";
	unsigned long long n = getULLInput("Enter the number of shares to generate (n): ");

	std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateShares(n);

	std::cout << "\nALL SHARES\n";
	for (unsigned long long i = 0; i < shares.size(); i++) {
		std::cout << shares[i].first << ' ' << shares[i].second << '\n';
	}
}


/**
 * Prompts the user to input shares one at a time until k shares have been
 * inputted. The secret is recovered using the Lagrange Interpolation Formula.
 * 
 * @param k The number of shares to be combined to recover the secret.
 * 
 * @return Void
 */
void recoverSecret(unsigned long long k) {
	std::cout << "\nRECOVER SECRET BY COMBINING K SHARES\n";

	std::vector<std::pair<unsigned long long, unsigned long long>> shares(k);
	for (unsigned long long i = 1; i <= k; i++) {
		unsigned long long x, y;
		std::cout << "Enter share #" << i << " of " << k << " in the format x y: ";
		std::cin >> x >> y;

		std::pair<unsigned long long, unsigned long long> point = {x, y};
		shares[i-1] = point;
	}

	unsigned long long secret = ShamirsSecretSharing::recoverSecret(shares);
	std::cout << "Secret: " << secret << '\n';
}


int main() {
	std::cout << "\nSHAMIR'S SECRET SHARING\n";
	ShamirsSecretSharing sss = hideSecret();

	while (true) {
		std::unordered_set<std::string> validChoices = {"1", "2", "q"};

		std::cout << "\nMAIN MENU\n";
		std::cout << "[1] Generate shares\n";
		std::cout << "[2] Recover secret\n";
		std::cout << "[q] Quit application\n";

		std::string choice = "";
		while (validChoices.find(choice) == validChoices.end()) {
			std::cout << "Choice: ";
			std::cin >> choice;
		}

		if (choice == "1") {
			generateShares(sss);
		} else if (choice == "2") {
			recoverSecret(sss.getK());			
		} else if (choice == "q") {
			std::cout << "\nGoodbye :)\n";
			break;
		}
	}

	return 0;
}