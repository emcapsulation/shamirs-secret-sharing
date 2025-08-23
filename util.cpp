#include <iostream>
#include <limits>
#include <random>

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


unsigned long long getRandomIntegerInRange(unsigned long long min, unsigned long long max) {
	// 64-bit Mersenne Twister RNG
	std::random_device rd;
	std::mt19937_64 random_number_generator(rd());

	std::uniform_int_distribution<unsigned long long> distribution(min, max);

	return distribution(random_number_generator);
}