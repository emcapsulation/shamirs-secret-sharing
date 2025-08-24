#include <iostream>
#include <limits>
#include <unordered_set>

#include "shamir.h"


/**
 * Gets unsigned long long input from the user and validates it.
 * 
 * @param prompt The message to prompt the user for input.
 * 
 * @return The user's ULL input.
 */
unsigned long long readUInt64(const std::string &prompt) {
	unsigned long long input;

	bool valid = false;
	while (!valid) {
		if (prompt != "") {
			std::cout << prompt;
		}
		
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
 * Prompts the user for a secret and threshold k to initialise the SSS instance.
 * 
 * @param secret The secret to hide in the constant term of the polynomial.
 * @param k The number of shares required to recover the secret. Leads to a 
 *        degree k-1 polynomial being generated.
 * 
 * @return The ShamirsSecretSharing instance.
 */
ShamirsSecretSharing hideSecret() {	
	std::cout << "\nSHAMIR'S SECRET SHARING\n";
	unsigned long long secret = readUInt64("Enter the secret (S): ");
	unsigned long long k = readUInt64("Enter the number of shares which need to be combined to recover the secret (k): ");

	ShamirsSecretSharing sssInstance(secret, k);
	return sssInstance;
}


/**
 * Prompts the user for a number of shares to be generated, and then outputs 
 * the full list of shares.
 * 
 * @param sssInstance The ShamirsSecretSharing instance to generate the shares.
 * 
 * @return Void
 */
void generateShares(ShamirsSecretSharing &sssInstance) {
	std::cout << "\nCREATE SHARES\n";
	unsigned long long n = readUInt64("Enter the number of shares to generate (n): ");

	std::vector<std::pair<unsigned long long, unsigned long long>> shares = sssInstance.generateShares(n);

	std::cout << "\nALL SHARES\n";
	for (unsigned long long i = 0; i < shares.size(); i++) {
		std::cout << shares[i].first << ' ' << shares[i].second << '\n';
	}
}


/**
 * Displays the list of shares.
 * 
 * @param sssInstance The ShamirsSecretSharing instance.
 * 
 * @return Void
 */
void viewShares(ShamirsSecretSharing &sssInstance) {
	std::cout << "\nVIEW SHARES\n";

	std::vector<std::pair<unsigned long long, unsigned long long>> shares = sssInstance.getShares();

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
		std::cout << "Enter share #" << i << "/" << k << " in the format x y: ";
		unsigned long long x = readUInt64("");
		unsigned long long y = readUInt64("");

		shares[i-1] = {x, y};
	}

	unsigned long long secret = ShamirsSecretSharing::recoverSecret(shares);
	std::cout << "Secret: " << secret << '\n';
}


int main() {
	ShamirsSecretSharing sssInstance = hideSecret();

	std::unordered_set<std::string> validChoices = {"1", "2", "3", "4", "q"};
	while (true) {
		std::cout << "\nMAIN MENU\n";
		std::cout << "[1] Generate shares\n";
		std::cout << "[2] View shares\n";
		std::cout << "[3] Recover secret\n";
		std::cout << "[4] Hide new secret\n";
		std::cout << "[q] Quit application\n";

		std::string choice = "";
		while (validChoices.find(choice) == validChoices.end()) {
			std::cout << "Choice: ";
			std::cin >> choice;
		}

		if (choice == "1") {
			generateShares(sssInstance);
		} else if (choice == "2") {
			viewShares(sssInstance);
		} else if (choice == "3") {
			recoverSecret(sssInstance.getK());	
		} else if (choice == "4") {
			sssInstance = hideSecret();		
		} else if (choice == "q") {
			std::cout << "\nGoodbye :)\n";
			break;
		}
	}

	return 0;
}