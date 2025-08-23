#include <iostream>
#include <unordered_set>

#include "shamirs-secret-sharing.h"


unsigned long long getULLInput(std::string prompt) {
	unsigned long long input;

	bool valid = false;
	while (!valid) {
		std::cout << prompt;
		std::cin >> input;

		if (std::cin.fail()) {
			std::cout << "Invalid input.\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		} else {
			valid = true;
		}
	}

	return input;
}


ShamirsSecretSharing hideSecret() {
	std::cout << "\nSPLIT A SECRET INTO N SHARES\n";
	unsigned long long secret = getULLInput("Enter the secret (S): ");
	unsigned long long n = getULLInput("Enter the number of shares to generate (n): ");
	unsigned long long k = getULLInput("Enter the number of shares which need to be combined to recover the secret (k): ");

	ShamirsSecretSharing sss(secret, n, k);
	std::vector<std::pair<unsigned long long, unsigned long long>> shares = sss.generateNShares();

	std::cout << "\nSHARES\n";
	for (unsigned long long i = 0; i < n; i++) {
		std::cout << shares[i].first << ' ' << shares[i].second << '\n';
	}

	return sss;
}


void recoverSecret(ShamirsSecretSharing sss) {
	std::cout << "\nRECOVER SECRET BY COMBINING K SHARES\n";

	unsigned long long k = sss.getK();
	std::vector<std::pair<unsigned long long, unsigned long long>> shares(k);
	for (unsigned long long i = 1; i <= k; i++) {
		unsigned long long x, y;
		std::cout << "Enter share #" << i << " in the format x y: ";
		std::cin >> x >> y;

		std::pair<unsigned long long, unsigned long long> point = {x, y};
		shares[i-1] = point;
	}

	unsigned long long secret = sss.recoverSecret(shares);
	std::cout << "Secret: " << secret << '\n';
}


int main() {
	ShamirsSecretSharing sss = hideSecret();

	while (true) {
		std::unordered_set<std::string> validChoices = {"1", "q"};

		std::cout << "\nSHAMIR'S SECRET SHARING\n";
		std::cout << "[1] Recover secret\n";
		std::cout << "[q] Quit application\n";

		std::string choice = "";
		while (validChoices.find(choice) == validChoices.end()) {
			std::cout << "Choice: ";
			std::cin >> choice;
		}

		if (choice == "1") {
			recoverSecret(sss);
		} else if (choice == "q") {
			std::cout << "\nGoodbye :)\n";
			break;
		}
	}

	return 0;
}