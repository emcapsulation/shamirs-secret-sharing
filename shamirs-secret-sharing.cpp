#include <iostream>
#include <unordered_set>

#include "shamirs-secret-sharing.h"


void hideSecret() {
	std::cout << "\nSPLIT A SECRET INTO N SHARES\n";
	unsigned long long secret = getULLInput("Enter the secret (S): ");
	unsigned long long n = getULLInput("Enter the number of shares to generate (n): ");
	unsigned long long k = getULLInput("Enter the number of shares which need to be combined to recover the secret (k): ");

	SecretSharing secretSharing(secret, n, k);
	secretSharing.generateNShares();
}


void recoverSecret() {
	std::cout << "\nRECOVER SECRET BY COMBINING K SHARES\n";

	// TODO
}


int main() {
	while (true) {
		std::unordered_set<std::string> validChoices = {"1", "2", "q"};

		std::cout << "\nSHAMIR'S SECRET SHARING\n";
		std::cout << "[1] Hide secret and generate n shares\n";
		std::cout << "[2] Recover secret by combining k shares\n";
		std::cout << "[q] Quit application\n";

		std::string choice = "";
		while (validChoices.find(choice) == validChoices.end()) {
			std::cout << "Choice: ";
			std::cin >> choice;
		}

		if (choice == "1") {
			hideSecret();
		} else if (choice == "2") {
			recoverSecret();
		} else if (choice == "q") {
			std::cout << "\nGoodbye :)\n";
			break;
		}
	}

	return 0;
}