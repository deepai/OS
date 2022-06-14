#ifndef TIMER_LIB_H
#define TIMER_LIB_H

#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum{
	TIMER_INIT,
	TIMER_RUNNING,
	TIMER_CANCELLED,
	TIMER_PAUSED,
	TIMER_DELETED,
} TIMER_STATE_T;

typedef struct Timer_ {
	timer_t posix_timer;
	void *user_arg;
	unsigned long exp_timer;
	unsigned long second_exp_timer;
	uint32_t threshold;
	void (*cb)(struct Timer_ *, void *);
	bool exponential_backoff;
	struct itimerspec ts;
	
	unsigned long time_remaining;
	uint32_t invocation_counter;
	TIMER_STATE_T state;
} Timer_t;

unsigned long timespec_to_millisec(struct timespec *time);
void timer_fill_itimerspec(struct timespec *ts, unsigned long msec);

static inline TIMER_STATE_T timer_get_current_state(Timer_t *timer){
	return timer->state;
}

static inline void timer_set_state(Timer_t *timer, TIMER_STATE_T timer_state){
	timer->state = timer_state;
}

Timer_t *setup_timer(
		void (*)(Timer_t *, void *),
		unsigned long,
		unsigned long,
		uint32_t,
		void *,
		bool);

int resurrect_timer(Timer_t *timer);
int start_timer(Timer_t *timer);
#endif