#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define                 NUMTHREADS   5
#define checkResults(string, val) {             \
 if (val) {                                     \
   printf("Failed with %d at %s", val, string); \
   exit(1);                                     \
 }                                              \
}
 

typedef struct {
  int          t1;
  int          t2;
} threadSpecific_data_t;

pthread_key_t           threadSpecificKey;

static void bar() {
   threadSpecific_data_t *gData = pthread_getspecific(threadSpecificKey);
   printf("Thread %.8lu: bar(), threadSpecific data= %d %d\n",
          pthread_self(), gData->t1, gData->t2);
   return;
}

static void foo() {
   threadSpecific_data_t *gData = pthread_getspecific(threadSpecificKey);
   printf("Thread %.8lu: foo(), threadSpecific data= %d %d\n",
          pthread_self(), gData->t1, gData->t2);
   bar();
}

static void *threadfn(void *data) {
   int               rc;
   threadSpecific_data_t    *gData;
   printf("Thread %.8lu: Entered\n", pthread_self());
   gData = (threadSpecific_data_t *)data;
   rc = pthread_setspecific(threadSpecificKey, gData);
   checkResults("pthread_setspecific()", rc);
   foo();
   return NULL;
}

void dataDestructor(void *data) {
   printf("Thread %.8lu: Free data\n", pthread_self());
   pthread_setspecific(threadSpecificKey, NULL);
   free(data);
}

int main(int argc, char *argv[]) {
    pthread_t             thread[NUMTHREADS] = {0};
    int                   rc=0;
    int                   i;
    threadSpecific_data_t        *gData;

    printf("Enter Testcase - %s\n", argv[0]);
    rc = pthread_key_create(&threadSpecificKey, dataDestructor);
    checkResults("pthread_key_create()", rc);

    printf("Create/start threads\n");
    for (i=0; i < NUMTHREADS; ++i) {
        /* Create per-thread threadSpecific data and pass it to the thread */
        gData = (threadSpecific_data_t *)malloc(sizeof(threadSpecific_data_t));
        gData->t1 = i;
        gData->t2 = (i+1)*2;
        rc = pthread_create(&thread[i], NULL, threadfn, gData);
        checkResults("pthread_create()", rc);
    }
    
    printf("Wait for the threads to complete, and release their resources\n");
    for (i=0; i <NUMTHREADS; ++i) {
        rc = pthread_join(thread[i], NULL);
        checkResults("pthread_join()", rc);
    }
    
    pthread_key_delete(threadSpecificKey);
    printf("Main completed\n");

    return 0;
}