#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "timerlib.h"

#define MSEC_TO_NSEC 1000000
#define SEC_TO_MSEC 1000

unsigned long timespec_to_millisec(struct timespec *time)
{
	unsigned long ret = 0;

	ret = time->tv_sec * SEC_TO_MSEC;
	ret += time->tv_nsec / MSEC_TO_NSEC;

	return ret;
}

static void timer_print_timesec(struct timespec *ts)
{
	printf("%ld %ld\n", ts->tv_sec, ts->tv_nsec);
}

void timer_fill_itimerspec(struct timespec *ts, unsigned long msec)
{
	unsigned long remaining_msec = 0;
	memset(ts, 0, sizeof(*ts));

	if (msec != 0) {
		ts->tv_sec = msec/SEC_TO_MSEC;
		remaining_msec = msec % SEC_TO_MSEC;
		ts->tv_nsec = remaining_msec * MSEC_TO_NSEC;
		//timer_print_timesec(ts);
	}
}

static int disarm_timer(Timer_t *timer)
{
	timer_fill_itimerspec(&timer->ts.it_value, 0);
	timer_fill_itimerspec(&timer->ts.it_interval, 0);

	int ret = resurrect_timer(timer);

	return ret;
}

static void TIMER_CALLBACK(union sigval arg) {
	Timer_t *timer = (Timer_t *)arg.sival_ptr;
	timer->cb(timer, timer->user_arg);
	timer->invocation_counter++;
	if (timer->invocation_counter == timer->threshold) {

		if (disarm_timer(timer) != 0) {
			printf("Failed to disable timer\n");
		} else {
			printf("Disabled Timer \n");
		}
	}
}

Timer_t *setup_timer(
		void (*timer_cb)(Timer_t *, void *),
		unsigned long exp_timer,
		unsigned long sec_exp_timer,
		uint32_t threshold,
		void *user_arg,
		bool exponential_backoff)
{
	Timer_t *timer = (Timer_t *)calloc(1, sizeof(Timer_t));
	if (timer == NULL) {
		printf("%s\n", strerror(errno));
		return NULL;
	}

	timer->cb = timer_cb;
	timer->exp_timer = exp_timer;
	timer->second_exp_timer = sec_exp_timer;
	timer->threshold = threshold;
	timer->user_arg = user_arg;
	timer->exponential_backoff = exponential_backoff;
	timer_set_state(timer, TIMER_INIT);

	assert(timer->cb != NULL);

	struct sigevent evp = {0};

	evp.sigev_value.sival_ptr = (void *)timer;
	evp.sigev_notify_function = TIMER_CALLBACK;
	evp.sigev_notify = SIGEV_THREAD;

	int rc = timer_create(CLOCK_REALTIME, &evp, &timer->posix_timer);
	if (rc != 0) {
		printf("error creating the timer %s\n", strerror(errno));
		free(timer);
		return NULL;
	}

	timer_fill_itimerspec(&timer->ts.it_value, timer->exp_timer);
	timer_fill_itimerspec(&timer->ts.it_interval, timer->second_exp_timer);

	return timer;
}

int resurrect_timer(Timer_t *timer)
{
	int rc = timer_settime(timer->posix_timer, 0, &timer->ts, NULL);
	//timer_print_timesec(&timer->ts.it_value);
	//timer_print_timesec(&timer->ts.it_interval);
	return rc;
}
int start_timer(Timer_t *timer)
{
	int rc = resurrect_timer(timer);
	if (rc != 0) {
		printf("Failed to start timer: %s\n", strerror(errno));
		return rc;
	}

	timer_set_state(timer, TIMER_RUNNING);
	return rc;
}