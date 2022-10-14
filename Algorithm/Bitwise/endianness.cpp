#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "First argument is an input unsigned number " << endl;
		return -1;
	} else {
		uint32_t num = (uint32_t)atoi(argv[1]);
		uint32_t reference_num = 1;
		cout << "Number is :0x" << hex << num << dec << endl;
		uint8_t *ptr = (uint8_t *)&reference_num;

		if ((ptr[0] & 0x1) == 1) {
			ptr = (uint8_t *)&num;
			cout << "Number is little endian" << endl;
			//convert number to big endian
			uint32_t new_num = 0;
			for (int i = 0; i < 4; i++) {
				new_num = new_num * 256 + ptr[i];
			}
			cout << "converted to big endian 0x" << hex << new_num << dec << endl;;
		} else {
			cout << "Number is big endian" << endl;
		}
	}
}