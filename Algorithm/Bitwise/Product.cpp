#include <iostream>

using namespace std;


uint64_t negate(uint32_t first) {
	
}

uint64_t product(uint32_t first, uint32_t second) {
	uint64_t gbl_result = 0;

	auto compute_add_ten = [&](uint64_t a) {
		uint64_t result = a;

		result = (result << 3);
		result += a;
		result += a;

		return result;
	};

	auto compute_add = [&](uint64_t a, uint64_t b) {
		if (b == 0)
			return (uint64_t)0;

		uint64_t result = 0;
		// << a << " " << b << endl;
		while (b != 0) {
			result += a;
			b--;
		}
		return result;
	};

	int num_tens = 0;

	while (second != 0) {
		uint64_t curr = compute_add(first, second%10);
		for (int i = 0; i < num_tens; i++) {
			curr = compute_add_ten(curr);
		}

		gbl_result += curr;

		num_tens++;
		second /= 10;
	}

	return gbl_result;
}

int64_t add_bitwise(int64_t a, int64_t b) {
	//cout << a << " + " << b << endl;
	int64_t result = 0;
	int64_t carry = 0;
	int64_t temp = 1;
	for (uint32_t t = 0; t < 63; t++) {
		//different bits
		if ((a&temp) != (b&temp)) {
			//both are different.
			result |= ((carry == 1) ? 0 : temp);
		} else {
			//both are same
			result |= ((carry == 1) ? temp : 0);
			if ((a&temp) == 0) {
				carry = 0;
			} else {
				carry = 1;
			}
		}
		
		temp <<= 1;
	}

	//cout << result << endl;

	return result;
}

int64_t multiply_bitwise(int64_t  a, int64_t b) {
	int64_t result = 0;
	if (a == 0 || b == 0) {
		return 0;
	}
	
	int64_t sign = (a >> 63) ^ (b >> 63);
	if (sign == 0) {
		a = abs(a);
		b = abs(b);
	} else {
		a = -(abs(a));
		b = abs(b);
	}

	for (int i = 0; i < 63; i++) {
		//cout << result << endl;
		if ((b&1) == 0) {
		} else {
			//cout << a << "- here" << endl;
			result = add_bitwise(result, (a << i));
		}
		b >>= 1;
	}

	return result;
}


int main(int argc, char **argv) {
	if (argc != 3) {
		cout << " First two arguments are unsigned numbers " << endl;
		return -1;
	}

	int a = atoi(argv[1]);
	int b = atoi(argv[2]);

	//cout << a << " " << b << endl;

	cout << product((uint32_t)a, (uint32_t)b) << endl;
	//cout << "0x" << hex << multiply_bitwise(a, b) << dec << endl;
	cout << multiply_bitwise(a, b) << endl;

	return 0;
}