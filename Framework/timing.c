#include <time.h>
#include <stdint.h>
#include <stdio.h>

#include "timing.h"

/*
 * Program Name: CnC Common Headers
 * File Name: timing.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Provides a function to time the execution of the passed in function.
 */
uint64_t timeExecution(timed_execution_function_t func, void *data, int iterations) {
    if (iterations > 0) {
        struct timespec start = {.tv_sec = 0, .tv_nsec = 0};
        struct timespec end = {.tv_sec = 0, .tv_nsec = 0};
        if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) //Start timing
            fprintf(stderr, "Warning: Failed attempting to retrieve start time\n");
        func(data);
        if (clock_gettime(CLOCK_MONOTONIC, &end) != 0)//End timing
            fprintf(stderr, "Warning: Failed attempting to retrieve end time\n");
        uint64_t seconds = 1000000000 * (end.tv_sec - start.tv_sec);//calculate seconds diff
        uint64_t nanoseconds = end.tv_nsec - start.tv_nsec;//calculate nanoseconds diff
        return seconds + nanoseconds;
    } else
        return iterations;
}
