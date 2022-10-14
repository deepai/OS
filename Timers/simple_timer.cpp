#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>

#include "timerlib.h"

#define MAX_TIME (24 * 60 * 60)

struct tasks {
	int id;
	void *user_arg;
	void (*task_action)(struct tasks *task, void *arg);
	uint32_t total_time;
};

struct ListNode {
	struct tasks task;
	struct ListNode *next;
};

struct ListNodeHead {
	struct ListNode *head;
	struct ListNode *last;
	pthread_spinlock_t lock;
};

struct timer_wheel {
	struct ListNodeHead *secondHead;

	uint32_t s_ptr;
	uint32_t m_ptr;
	uint32_t h_ptr;

	uint32_t gbl_ptr;

	uint32_t timer_tick;

	pthread_spinlock_t s_lock;

	int tasks_counter_queued;
	int tasks_counter_done;
};

static void move_elements(struct ListNodeHead *to, struct ListNodeHead *from)
{
	pthread_spin_lock(&from->lock);

	if (to->head == NULL) {
		to->head = from->head;
		to->last = from->last;
	} else {
		to->last->next = from->head;
		to->last = from->last;
	}

	from->head = NULL;
	from->last = NULL;

	pthread_spin_unlock(&from->lock);
}

int insert_task(struct ListNodeHead *head, struct tasks task)
{
	struct ListNode *tmp = (struct ListNode *)calloc(1, sizeof(struct ListNode));
	if (tmp == NULL) {
		return -1;
	}

	pthread_spin_lock(&head->lock);

	tmp->task = task;
	tmp->next = NULL;

	if (head->head == NULL) {
		head->head = tmp;
		head->last = tmp;
	} else {
		head->last->next = tmp;
		head->last = tmp;
	}

	pthread_spin_unlock(&head->lock);

	return 0;
}

int init_timer_wheel(struct timer_wheel *timer_wheel)
{
	timer_wheel->secondHead = (struct ListNodeHead *)calloc(MAX_TIME, sizeof(struct ListNodeHead));
	if (timer_wheel->secondHead == NULL) {
		return -1;
	}

	for (int i = 0; i < MAX_TIME; i++)
		pthread_spin_init(&timer_wheel->secondHead[i].lock, PTHREAD_PROCESS_PRIVATE);

	pthread_spin_init(&timer_wheel->s_lock, PTHREAD_PROCESS_PRIVATE);

	timer_wheel->tasks_counter_queued = 0;
	timer_wheel->tasks_counter_done = 0;
	timer_wheel->h_ptr = 0;
	timer_wheel->m_ptr = 0;
	timer_wheel->s_ptr = 0;
	timer_wheel->gbl_ptr = 0;
	timer_wheel->timer_tick = 0;

	return 0;
}

void queue_work(struct timer_wheel *wheel, struct tasks task) {
	uint32_t time = task.total_time;

	uint32_t hours = time/(24 * 60);
	uint32_t mins =  time / 60;
	uint32_t seconds = (time % 60);
	uint32_t next_pos;
	int ret = 0;

	fprintf(stdout, "queued work [%d] at h:[%u], m:[%u], s:[%u], time:[%u]\n", task.id, hours, mins, seconds, time);

	pthread_spin_lock(&wheel->s_lock);
	next_pos = (wheel->gbl_ptr + time) % (MAX_TIME);
	pthread_spin_unlock(&wheel->s_lock);

	ret = insert_task(&wheel->secondHead[next_pos], task);
	if (ret != 0) {
		fprintf(stderr, "Error queueing task[%d]", task.id);
		return;
	}

	wheel->tasks_counter_queued++;
}

void task_action(struct tasks *task, void *arg) {
	printf("%*sExecuted Task [%d] with Total time[%u]\n", 27, "", task->id, task->total_time);
}

void timer_wheel_tick(struct timer_wheel *wheel) {
	struct ListNodeHead Head = {0};
	struct ListNode *node, *next_node;
	uint32_t count_work = 0;
	uint32_t current_pointer;

	pthread_spin_lock(&wheel->s_lock);

	current_pointer = wheel->gbl_ptr;
	wheel->s_ptr++;
	if (wheel->s_ptr == 60) {
		wheel->s_ptr = 0;

		wheel->m_ptr++;
		if (wheel->m_ptr == 60) {
			wheel->m_ptr = 0;

			wheel->h_ptr++;
			if (wheel->h_ptr == 24) {
				wheel->h_ptr = 0;
			}
		}
	}
	wheel->gbl_ptr++;
	pthread_spin_unlock(&wheel->s_lock);

	move_elements(&Head, &wheel->secondHead[current_pointer]);

	fprintf(stdout, "Timer tick called [%u:%u:%u]\n", wheel->h_ptr, wheel->m_ptr, wheel->s_ptr);

	for (node = Head.head; node != NULL;) {
		next_node = node->next;
		node->task.task_action(&node->task, node->task.user_arg);
		count_work++;
		free(node);
		node = next_node;
	}

	pthread_spin_lock(&wheel->s_lock);
	wheel->tasks_counter_done++;
	pthread_spin_unlock(&wheel->s_lock);
}

void timer_action(Timer_t *timer, void *arg) {
	struct timer_wheel *wheel = (struct timer_wheel *)arg;
	timer_wheel_tick(wheel);
};

int main(int argc, char **argv) {
	int counter = 0;
	int ret = 0;
	int num_tasks = 200;
	struct timer_wheel wheel;
	ret = init_timer_wheel(&wheel);
	if (ret != 0) {
		fprintf(stderr, "failed to initialize timer wheels");
		return -1;
	}

	for (int i = 0; i < num_tasks; i++) {
		struct tasks task;
		task.id = i + 1;
		task.task_action = task_action;
		task.user_arg = NULL;
		task.total_time = rand() % 120;
		queue_work(&wheel, task);
	}

	Timer_t *timer = setup_timer(timer_action, 5000, 1000, 120, &wheel, false);
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

	fprintf(stdout, "Total work executed = [%u]\n", wheel.tasks_counter_done);

	free(timer);

	return 0;
}