#include <iostream>

using namespace std;

int negateV(int a) {
	int result = ~a + 1;
	return result;
}

int sub(int a, int b) {
	int borrow = 0;
	while (b != 0) {
		borrow = ~a&b;
		a = a^b;
		b = borrow << 1;
	}

	return a;
}

int add(int a, int b) {
	int carry = 0;
	while (b != 0) {
		carry = a&b;
		a = a^b;
		b = carry << 1;
	}

	return a;
}

int prod(int a, int b) {
	int result = 0;
	bool sign = (a>>31) ^ (b>>31);

	a = abs(a);
	b = abs(b);

	while (b > 0) {
		if ((b&1) == 1) {
			result = add(result, a);
		}

		a <<= 1;
		b >>= 1;
	}

	if (sign) {
		result = negateV(result);
	}

	return result;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		cout << "First argument is an input unsigned number " << endl;
		return -1;
	} else {
		int num1 = atoi(argv[1]);
		int num2 = atoi(argv[2]);

		cout << add(num1, num2) << endl;
		cout << add(num1, negateV(num2)) << endl;
		cout << sub(num1, num2) << endl;
		cout << prod(num1, num2) << endl;
	}

	return 0;
}