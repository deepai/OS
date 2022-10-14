#include <iostream>
#include <stack>
#include <cmath>

using namespace std;

void convert_double_to_binary(double data, int count) {
	double lhs = floor(data);
	double rhs = 0;

	rhs = data - lhs;

	if (lhs == 0) {
		cout << "0";
	} else {
		stack<int> S;
		while (lhs != 0) {
			double dividend = floor(lhs/2);
			double remaining = (lhs - dividend * 2);
			if (remaining == 0) {
				S.push(0);
			} else {
				S.push(1);
			}
			lhs = dividend;
		}

		while (!S.empty()) {
			cout << S.top();
			S.pop();
		}
	}

	if (rhs != 0) {
		cout << ".";
		while (count != 0) {
			rhs = rhs * 2;
			if (rhs >= 1.0) {
				cout << "1";
				rhs -= 1.0;
			} else {
				cout << "0";
			}
			count--;
		}
	}

	cout << endl;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "First argument is an input decimal number " << endl;
		return -1;
	} else {
		double num1 = atof(argv[1]);
		convert_double_to_binary(num1, 10);
	}

	return 0;
}