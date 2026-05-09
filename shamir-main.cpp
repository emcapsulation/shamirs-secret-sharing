#include "shamir.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_set>


/**
 * Displays the list of shares.
 * 
 * @param sssInstance The ShamirsSecretSharing instance.
 * 
 * @return Void
 */
void viewShares(ShamirsSecretSharing &sssInstance) 
{
	std::vector<Share> shares = sssInstance.getShares();

	std::cout << "\nALL SHARES\n";
	for (size_t i = 0; i < shares.size(); i++)
		std::cout << shares[i].first << ' ' << shares[i].second << '\n';
}


/**
 * Gets uint64_t input from the user and validates it.
 * 
 * @param prompt The message to prompt the user for input.
 * 
 * @return The user's ULL input.
 */
uint64_t readUInt64(const std::string &prompt) 
{
	uint64_t input;

	while (true) 
	{
		std::cout << prompt;

		if (std::cin >> input) 
			return input;		

		// Reset the buffer
		std::cout << "Invalid input.\n";
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
ShamirsSecretSharing hideSecret() 
{
	while (true) 
	{
		std::cout << "\nSHAMIR'S SECRET SHARING\n";
		uint64_t secret = readUInt64("Enter the secret (S): ");
		uint64_t k = readUInt64("Enter the number of shares which need to be combined to recover the secret (k): ");
		uint64_t n = readUInt64("Enter the number of shares to generate (n): ");

		try 
		{
			ShamirsSecretSharing sss(secret, k);
			if (n > 0)
			{
				sss.generateAdditionalShares(n);
				viewShares(sss);
			}
			return sss;
		} 
		catch (const std::domain_error &e) 
		{
			std::cerr << e.what() << std::endl;
		}
	}
}


/**
 * Prompts the user for a number of shares to be generated, and then outputs 
 * the full list of shares.
 * 
 * @param sssInstance The ShamirsSecretSharing instance to generate the shares.
 * 
 * @return Void
 */
void generateAdditionalShares(ShamirsSecretSharing &sssInstance) {
	std::cout << "\nCREATE SHARES\n";
	uint64_t n = readUInt64("Enter the number of shares to generate (n).\nNOTE: Existing shares won't be deleted: ");

	std::vector<Share> shares;
	try 
	{
		sssInstance.generateAdditionalShares(n);
	} 
	catch (const std::domain_error &e) 
	{
		std::cerr << e.what() << std::endl;
		return;
	}	

	viewShares(sssInstance);
}


/**
 * Prompts the user to input shares one at a time until k shares have been
 * inputted. The secret is recovered using the Lagrange Interpolation Formula.
 * 
 * @param k The number of shares to be combined to recover the secret.
 * 
 * @return Void
 */
void recoverSecret(uint64_t k) 
{
	std::cout << "\nRECOVER SECRET BY COMBINING K SHARES\n";

	std::vector<Share> shares(k);
	for (uint64_t i = 1; i <= k; i++) 
	{
		std::cout << "Enter share #" << i << "/" << k << " in the format x y: ";
		uint64_t x = readUInt64("");
		uint64_t y = readUInt64("");

		shares[i-1] = {x, y};
	}

	uint64_t secret;
	try 
	{
		secret = ShamirsSecretSharing::recoverSecret(shares);
	} 
	catch (const std::domain_error &e) 
	{
		std::cerr << e.what() << std::endl;
		return;
	} 
	catch (const std::invalid_argument &e) 
	{
		std::cerr << e.what() << std::endl;
		return;
	}

	std::cout << "Secret: " << secret << '\n';
}


int main() 
{
	ShamirsSecretSharing sssInstance = hideSecret();

	std::unordered_set<std::string> validChoices = {"1", "2", "3", "4", "q"};
	while (true) 
	{
		std::cout << "\nMAIN MENU\n";
		std::cout << "[1] Generate shares\n";
		std::cout << "[2] View shares\n";
		std::cout << "[3] Recover secret\n";
		std::cout << "[4] Hide new secret\n";
		std::cout << "[q] Quit application\n";

		std::string choice = "";
		while (validChoices.find(choice) == validChoices.end()) 
		{
			std::cout << "Choice: ";
			std::cin >> choice;
		}

		if (choice == "1")
			generateAdditionalShares(sssInstance);
		else if (choice == "2")
			viewShares(sssInstance);
		else if (choice == "3")
			recoverSecret(sssInstance.getThreshold());
		else if (choice == "4")
			sssInstance = hideSecret();
		else if (choice == "q")
			break;
	}

	return 0;
}