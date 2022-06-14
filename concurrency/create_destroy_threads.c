#include <stdio.h>
#include <pthread.h>

/* function to be executed by the new thread */
void*
do_loop(void* data)
{
    int i;			/* counter, to print numbers */
    int j;			/* counter, for delay        */
    int me = *((int*)data);     /* thread identifying number */

    for (i=0; i<10; i++) {
        printf("'%d' - Got '%d'\n", me, i);
    }

    /* terminate the thread */
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int thread_id;
    pthread_t thread;
    int a = 1;
    int b = 2;

    thread_id = pthread_create(&thread, NULL, do_loop, (void *)&a);
    do_loop((void *)&b);

    return 0;
}