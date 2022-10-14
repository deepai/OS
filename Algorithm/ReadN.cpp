int read4(char *buf);

int read(char[] buf, int n) {
	char temp_buf[4];
	int curr_offset = 0;
	int count = 0;

	curr_offset = INT_MAX;
	while (count < n && curr_offset != 0) {
		curr_offset = read4(&temp_buf);
		for (int t = 0; t < curr_offset && count < n; t++) {
			buf[count++] = temp_buf[t];
		}
	}

	return count;
}

int main(int argc, char **argv) {

	return 0;
}