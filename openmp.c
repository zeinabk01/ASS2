#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define ARRAY_SIZE 1000000
#define NUM_BUCKETS 100

int array[ARRAY_SIZE];
int sortedArray[ARRAY_SIZE];

typedef struct {
    int start;
    int end;
} Bucket;

Bucket buckets[NUM_BUCKETS];

void initializeArray() {
    #pragma omp parallel for
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = rand() % ARRAY_SIZE;
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

void sortBucket(int bucketID) {
    for (int i = buckets[bucketID].start; i < buckets[bucketID].end; i++) {
        int val = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > val) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = val;
    }
}

void parallelBucketSort() {
    #pragma omp parallel for
    for (int i = 0; i < NUM_BUCKETS; i++) {
        sortBucket(i);
    }
}

int main() {
    srand(time(NULL));

    initializeArray();
    initializeBuckets();

    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    parallelBucketSort();

    gettimeofday(&end_time, NULL);
    double par_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Parallel bucket sort took %f seconds\n", par_time);

  double speedup = seq_time / par_time;
    double efficiency = speedup / omp_get_max_threads();
    printf("Speedup factor: %f\n", speedup);
    printf("Efficiency: %f\n", efficiency);

    return 0;
}
