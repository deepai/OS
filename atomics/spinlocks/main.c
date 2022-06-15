#include <stdio.h>
#ifndef __STDC_NO_ATOMICS__
#include <stdatomic.h>
#endif
#include <stdbool.h>

struct spinlock {
    atomic_bool flag;
};

typedef struct spinlock spinlock_t;

void spinlock_init(spinlock_t *s_lock)
{
   atomic_store(&s_lock->flag, false);
}

void spinlock_lock(spinlock_t *s_lock)
{
    bool expected = false;
    while (!atomic_compare_exchange_strong(&s_lock->flag, &expected, true));
}

void spinlock_unlock(spinlock_t *s_lock)
{
   atomic_store(&s_lock->flag, false);
}

int main(int argc, char *argv[])
{
    spinlock_t s_lock;
    spinlock_init(&s_lock);
    spinlock_lock(&s_lock);

    printf("hello\n");

    spinlock_unlock(&s_lock);
    return 0;
}
