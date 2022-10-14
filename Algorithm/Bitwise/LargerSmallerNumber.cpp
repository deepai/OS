#include <iostream>
#include <climits>

using namespace std;

uint32_t getLargerNumber(uint32_t current) {
	if (current == UINT32_MAX) {
		return current;
	} else {
		uint32_t bit = 1;
		uint32_t count = 0;

//cout << "reached here1" << endl;

		//first find ones
		while ((current & bit) == 0) {
			bit <<= 1;
		}

		//cout << "reached here2" << endl;

		//find next 0 and count 1s
		while ((current & bit) != 0) {
			count++;
			bit <<= 1;
		}

		//Or 1 to this first 0.
		current = current | bit;
		current = (current & ~(bit - 1));

		uint32_t mask = (1 << (count - 1)) - 1;

		//mask current with the final value
		current |= mask;

		return current;
	}
}

uint32_t getSmallerNumber(uint32_t current) {
	if (current == 0) {
		return 0;
	}

	uint32_t bit = 1;
	uint32_t count = 0;
	while ((current & bit) != 0) {
		bit <<= 1;
		count++;
	}

	while ((current & bit) == 0) {
		bit <<= 1;
	}

	current -= bit;
	current |= (bit >> 1);

	uint32_t mask = (1 << (count)) - 1;
	if (mask == 0) {
		return current;
	}

	while ((mask & current) == 0) {
		mask <<= 1;
	}

	current |= (mask >> 1);

	return current;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "First argument is an input unsigned number " << endl;
		return -1;
	}

	uint32_t value = (uint32_t)atoi(argv[1]);
	uint32_t largerValue = getLargerNumber(value);
	uint32_t smallerValue = getSmallerNumber(value);

	cout << "Value: " << value << ", larger: " << largerValue << ", " << smallerValue << endl;

	return 0;
}