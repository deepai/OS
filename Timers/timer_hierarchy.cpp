#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <cassert>

#include <list>
#include <string>
#include <vector>
#include <iostream>

#include "timerlib.h"

class Timer;

using namespace std;

struct Tasks {
	int id;
	void *user_arg;
	void (*task_action)(struct Tasks *task, void *arg);
	uint32_t total_time;
	uint32_t orig_time;
};

struct TimerWheel {
	vector<list<Tasks>> hour_tasks;
	vector<list<Tasks>> minute_tasks;
	vector<list<Tasks>> second_tasks;

	uint32_t total_seconds = 0;
	uint32_t tasks_counter_done = 0;

	uint32_t seconds = 0;
	uint32_t mins = 0;
	uint32_t hours = 0;



	TimerWheel() {
		hour_tasks.resize(24);
		minute_tasks.resize(60);
		second_tasks.resize(60);
	}

	void pick_seconds() {
		list<Tasks> result_tasks;
		if (second_tasks[seconds].empty())
			return;
		result_tasks.splice(result_tasks.begin(), second_tasks[seconds]);
		for (auto &each_task : result_tasks) {
			each_task.task_action(&each_task, each_task.user_arg);
			tasks_counter_done++;
		}
	}

	void move_mins() {
		list<Tasks> curr_min_tasks;
		if (minute_tasks[(mins + 1)%60].empty())
			return;
		curr_min_tasks.splice(curr_min_tasks.begin(), minute_tasks[(mins + 1)%60]);
		for (auto &each_task : curr_min_tasks) {
			//update time
			each_task.total_time -= total_seconds;
			assert(each_task.total_time >= 0);
			second_tasks[each_task.total_time].push_back(each_task);
		}
	}

	void addTask(Tasks task) {
		if (task.total_time >= 60 * 60) {
			hour_tasks[task.total_time/(60 * 60)].push_back(task);
		} else if (task.total_time >= 60) {
			minute_tasks[task.total_time / 60].push_back(task);
		} else {
			second_tasks[task.total_time].push_back(task);
		}
	}

	void tick() {
		cout << "tick: " << total_seconds << endl;
		pick_seconds();
		seconds++;
		total_seconds++;
		if (seconds == 60) {
			seconds = 0;
			move_mins();
			mins++;
			if (mins == 60) {
				mins = 0;
			}
		}
	}
};

void signal_timer_action(Timer_t *timer, void *arg) {
	TimerWheel *timerwheel = (TimerWheel *)arg;
	timerwheel->tick();
};

void task_action(Tasks *task, void *arg) {
	printf("%*sExecuted Task [%d] with Total time[%u]\n", 27, "", task->id, task->orig_time);
}

int main(int argc, char **argv) {
	int counter = 0;
	int ret = 0;
	int num_tasks = 300;

	TimerWheel wheel;

	for (int i = 0; i < num_tasks; i++) {
		Tasks task;
		task.id = i + 1;
		task.task_action = task_action;
		task.user_arg = NULL;
		task.total_time = (rand() % 300) + 1;
		task.orig_time = task.total_time;
		wheel.addTask(task);
	}

	Timer_t *signalTimer = setup_timer(signal_timer_action, 5000, 1000, 1000, &wheel, false);
	if (signalTimer == NULL) {
		printf("Failed to create signalTimer %d\n", errno);
		return -1;
	} else {
		printf("Created signal Timer successfully\n");
	}

	int rc = start_timer(signalTimer);
	if (rc != 0) {
		printf("Failed to start signalTimer %d\n", errno);
	} else {
		pause();
	}

	fprintf(stdout, "Total work executed = [%u]\n", wheel.tasks_counter_done);

	free(signalTimer);

	return 0;
}