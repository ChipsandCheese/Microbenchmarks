/*
 * Program Name: CoherencyLatencyTest
 * File Name: main.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Test Core-to-Core Latency of Multi-Core CPU's using Coherency checks.
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include <timing.h>
#include <platformCode.h>

typedef union Result {
    _Atomic uint64_t raw;
    double data;
} Result;

typedef struct LatencyThreadData {
    uint64_t start;
    uint64_t iters;
    // This is atomic to ease the transfer across thread boundaries
    volatile uint64_t *target;
    uint32_t processor_index;
    _Atomic uint64_t result;
} LatencyThreadData;

typedef struct LatencyPairRunData {
    uint32_t proc1;
    uint32_t proc2;
    uint64_t iter;
    Result *result;
    uint64_t *target;
    void (*thread_func)(LatencyThreadData *);
} LatencyPairRunData;

/*
 * Tests the latency using a locking algorithm.
 * @Param latencyData: Pointer to the LatencyThreadData structure to operate on.
 */
void latencyTestThread(LatencyThreadData *latencyData) {
    uint64_t current = latencyData->start;

    while (current <= 2 * latencyData->iters) {
        if (__sync_bool_compare_and_swap(latencyData->target, current - 1, current)) current += 2;
    }
}

/*
 * Starting function for newly spawned benchmark threads.
 * @Param data: Pointer to array of pointer-sized arguments
 * @Subparam data[0] func: Function to run in the test
 * @Subparam data[1] latencyData: LatencyThreadData, passed to test function
 * @Return: Returns a pointer to where the result was stored.
*/
void *timeThread(void *raw) {
    uint64_t *data = (uint64_t *)raw;
    void (*func)(void *) = (void (*)(void *))data[0];
    LatencyThreadData *latency_data = (LatencyThreadData *)data[1];

    setAffinity(pthread_self(), latency_data->processor_index);
    uint64_t ns = timeExecution(func, latency_data, latency_data->iters);
    atomic_store(&latency_data->result, ns);

    return (void *)&latency_data->result;
}

/*
 * Spawn the actual test thread and pass arguments to it.
 * @Param proc1: Processor index for first thread.
 * @Param proc2: Processor index for second thread.
 * @Param iter: How many iterations to perform.
 * @Param lat1: LatencyThreadData passed to threadFunc on first thread.
 * @Param lat1: LatencyThreadData passed to threadFunc on second thread.
 * @Param threadFunc: Function to benchmark.
*/
double spawnTest(
    uint64_t iter,
    LatencyThreadData *lat1,
    LatencyThreadData *lat2,
    void (*thread_func)(LatencyThreadData *)
) {
    pthread_t test_threads[2];
    int t1rc, t2rc;
    _Atomic uint64_t *res1 = NULL, *res2 = NULL;

    // Passed as params to timeThread
    uint64_t args0[2] = {(uint64_t)thread_func, (uint64_t)lat1};
    uint64_t args1[2] = {(uint64_t)thread_func, (uint64_t)lat2};
    
    t1rc = pthread_create(&test_threads[0], NULL, timeThread, &args0);
    t2rc = pthread_create(&test_threads[1], NULL, timeThread, &args1);
    if (t1rc != 0 || t2rc != 0) {
      fprintf(stderr, "Could not create threads\n");
      return 0;
    }

    pthread_join(test_threads[0], (void **)&res1);
    pthread_join(test_threads[1], (void **)&res2);

    double average_ns = (double)(atomic_load(res1) + atomic_load(res2)) / 2.0;
    double latency = average_ns / (double)iter;
    return latency;
}


/*
 * Starting function for starting test threads in parallel.
 * @Param raw: Void pointer to LatencyPairRunData structure.
 * @Return: Always returns NULL.
*/
void *runTest(void *raw) {
    LatencyPairRunData *data = (LatencyPairRunData *)raw;
    LatencyThreadData lat1, lat2;
    Result latency = {.data = 0.0};

    *data->target = 0;
    lat1.iters = data->iter;
    lat2.iters = data->iter;
    lat1.processor_index = data->proc1;
    lat2.processor_index = data->proc2;
    // This is where each thread will start accessing the target data
    lat1.start = 1;
    lat2.start = 2;
    lat1.target = data->target;
    lat2.target = data->target;
    lat1.result = 0.0;
    lat2.result = 0.0;
    latency.data = spawnTest(data->iter, &lat1, &lat2, data->thread_func);
    fprintf(stderr, "%d to %d: %lf ns\n", lat1.processor_index, lat2.processor_index, latency.data);
    latency.data /= 2.0;
    atomic_store(&data->result->raw, latency.raw);
    return NULL;
}

#define ITERS 10000000

/*
 * Runs a core coherency latency test, this determines the average latency required to perform a transfer across cores.
 * @Param parallel: Number of threads to attempt to run in parallel, the program may run fewer threads than specified by this number.
 * @Param iterations: How many times to do the test function, higher means the results will be more precise, but slower to finish.
 * @Param offsets: How many offsets into the cachelines to test.
 * @Return: Status code, zero indicates success.
*/
int main(int argc, char **argv) {
    int processors = getThreadCount();
    int parallelism = 1;
    int offsets = 1;
    int iterations = ITERS;
    void (*thread_func)(LatencyThreadData *) = latencyTestThread;

    // Collect any command-line arguments
    for (int argIdx = 1; argIdx < argc; argIdx++) {
        if (*(argv[argIdx]) == '-') {
            char* arg = argv[argIdx] + 1;
            if (strncmp(arg, "iterations", 10) == 0) {
                argIdx++;
                iterations = atoi(argv[argIdx]);
                fprintf(stderr, "%d iterations requested\n", iterations);
            }
            else if (strncmp(arg, "offset", 6) == 0) {
                argIdx++;
                offsets = atoi(argv[argIdx]);
                fprintf(stderr, "Offsets: %d\n", offsets);
            }
            else if (strncmp(arg, "parallel", 8) == 0) {
                argIdx++;
                parallelism = atoi(argv[argIdx]);
                fprintf(stderr, "Will go for %d runs in parallel\n", parallelism);
            }
        }
    }

    // Marks whether or not a thread has been tested or not
    int *test_state = malloc(sizeof(int) * processors * processors);
    // Allocate a buffer filled with pointers to the latency result tables for every offset
    Result **results = malloc(sizeof(Result *) * offsets);
    // Allocate a buffer for all the currently resident threads to operate on
    uint64_t *targets = aligned_alloc(4096 * parallelism, 4096);
    LatencyPairRunData *pair_run_data = malloc(sizeof(LatencyPairRunData) * parallelism);
    if (results == NULL || targets == NULL) {
        fprintf(stderr, "Failed to allocate buffer\n");
        return -1;
    }

    for (int offset = 0; offset < offsets; offset++) {
        // Allocate latency result table for this offset
        results[offset] = malloc(sizeof(Result) * processors * processors);
        Result *lat_ptr = results[offset];
        if (lat_ptr == NULL) {
            fprintf(stderr, "Failed to allocate latency result table\n");
            return -2;
        }

        while (1) {
            int selected_test_count = 0;
            memset(pair_run_data, 0, sizeof(LatencyPairRunData) * parallelism);
            // Schedule cores to test
            for (int src = 0; src < processors && selected_test_count < parallelism; src++) {
                for (int partner = 0; partner < processors && selected_test_count < parallelism; partner++) {
                    if (src == partner) { 
                        // We cant test the same CPU against itself, just set the latency to 0
                        lat_ptr[partner + src * processors].data = 0.0; 
                        continue; 
                    }
                    if (test_state[partner + src * processors] == 1) {
                        // We already reached this pair... how did we get here?
                        fprintf(stderr, "Found unexpected partial thread pair %i and %i\n", src, partner);
                        return -3;
                    } else if (test_state[partner + src * processors] == 0) {
                        int valid_pair = 1;
                        for (int extra = 0; extra < processors; extra++) {
                            // Ensure none of the currently selected somewhere else
                            if (
                                test_state[partner + extra * processors] == 1 || 
                                test_state[extra + src * processors] == 1 ||
                                test_state[src + extra * processors] == 1 ||
                                test_state[extra + partner * processors] == 1
                            ) {
                                valid_pair = 0;
                                break;
                            }
                        }
                        
                        if (!valid_pair) continue;

                        // for SMT enabled CPUs, check sibling threads. will do later
                        // Mark this combination as in-flight
                        test_state[partner + src * processors] = 1;
                        pair_run_data[selected_test_count].proc1 = src;
                        pair_run_data[selected_test_count].proc2 = partner;
                        pair_run_data[selected_test_count].iter = iterations;
                        pair_run_data[selected_test_count].result = &lat_ptr[partner + src * processors];
                        pair_run_data[selected_test_count].thread_func = thread_func;
                        // Set the target for this test to be at cacheline selected_test_count
                        // Offset it by the desired cacheline offsets, in 8 byte chunks
                        pair_run_data[selected_test_count].target = targets + (512 * selected_test_count + 8 * offset);
                        fprintf(stderr, "Selected %d -> %d\n", src, partner);
                        selected_test_count++;
                    }
                }
            }
            
            if (selected_test_count == 0) break;

            // Launch threads to test with
            fprintf(stderr, "Selected %d pairs for parallel testing\n", selected_test_count);
            pthread_t *test_threads = (pthread_t *)malloc(selected_test_count * sizeof(pthread_t));
            memset(test_threads, 0, selected_test_count * sizeof(pthread_t));
            for (int index = 0; index < selected_test_count; index++) {
                if (pair_run_data[index].proc1 == 0 && pair_run_data[index].proc2 == 0) break;
                pthread_create(test_threads + index, NULL, runTest, (void *)(pair_run_data + index));
            }

            // Wait for threads to end
            for (int index = 0; index < selected_test_count; index++) {
                pthread_join(test_threads[index], NULL);
                int src = pair_run_data[index].proc1;
                int partner = pair_run_data[index].proc2;
                //lat_ptr[partner + src * processors] = pair_run_data[index].result;
                // Mark this combination as completed
                test_state[partner + src * processors] = 2;
            }

            free(test_threads);
        }
    }

    for (int offset = 0; offset < offsets; offset++) {
        for (int src = 0; src < processors; src++) {
            for (int partner = 0; partner < processors; partner++) {
                if (src == partner) printf("x");
                else printf("%lf", results[offset][partner + src * processors].data);

                if (partner + 1 == processors) printf("\n");
                else printf(",");
            }
        }
    }

    return 0;
}
