#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "timerlib.h"

void timer_action(Timer_t *timer, void *arg) {
	int *counter = (int *)arg;
	printf("Timer called: %d\n", (*counter)++);
};

int main(int argc, char **argv) {
	int counter = 0;
	Timer_t *timer = setup_timer(timer_action, 5000, 1000, 0, &counter, false);
	if (timer == NULL) {
		printf("Failed to create timer %d\n", errno);
		return -1;
	} else {
		printf("Created timer successfully\n");
	}

	int rc = start_timer(timer);
	if (rc != 0) {
		printf("Failed to start timer %d\n", errno);
	} else {
		pause();
	}

	free(timer);

	return 0;
}