#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define ARRAY_SIZE 1000000
#define NUM_BUCKETS 100
#define NUM_THREADS 4

int array[ARRAY_SIZE];
int sortedArray[ARRAY_SIZE];

typedef struct {
    int start;
    int end;
} Bucket;

Bucket buckets[NUM_BUCKETS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* sortBucket(void* arg) {
    int bucketID = *(int*)arg;

    for (int i = buckets[bucketID].start; i < buckets[bucketID].end; i++) {
        int val = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > val) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = val;
    }

    pthread_exit(NULL);
}

void sequentialBucketSort() {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        for (int j = buckets[i].start; j < buckets[i].end; j++) {
            int val = array[j];
            int k = j - 1;
            while (k >= 0 && array[k] > val) {
                array[k + 1] = array[k];
                k--;
            }
            array[k + 1] = val;
        }
    }
}

void initializeBuckets() {
    int range = ARRAY_SIZE / NUM_BUCKETS;
    int remainder = ARRAY_SIZE % NUM_BUCKETS;

    for (int i = 0; i < NUM_BUCKETS; i++) {
        buckets[i].start = i * range;
        buckets[i].end = (i + 1) * range;
    }

    buckets[NUM_BUCKETS - 1].end += remainder;
}

int main() {
    srand(time(NULL));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % ARRAY_SIZE;
    }

    initializeBuckets();

    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    sequentialBucketSort();

    gettimeofday(&endTime, NULL);
    double sequentialTime = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;

    printf("Sequential bucket sort took %f seconds\n", sequentialTime);

    gettimeofday(&startTime, NULL);

    pthread_t threads[NUM_THREADS];
    int threadIDs[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, sortBucket, (void*)&threadIDs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&endTime, NULL);
    double parallelTime = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec) / 1000000.0;

    printf("Parallel bucket sort took %f seconds\n", parallelTime);

   double speedup = sequentialTime / parallelTime;
    double efficiency = speedup / num_threads;
    printf("Speedup factor: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}
