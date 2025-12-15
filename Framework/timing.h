#ifndef TIMING_H
#define TIMING_H
/*
 * Program Name: CnC Common Headers
 * File Name: timing.h
 * Date Created: January 24, 2024
 * Date Updated: November 11, 2024
 * Version: 0.6
 * Purpose: Provides header functionality to timing.c
 */

#include <stdint.h>

//DO NOT DECLARE GLOBALS


typedef void (*timed_execution_function_t)(void *);

/* 
 * @Param func: The pointer of the function being timed.
 * @Param data: Anonymous data that will be forwarded to func.
 * @Param iterations: How many iterations to perform
 * @Return: Time in nanoseconds (up to nanosecond precision).
 */
uint64_t timeExecution(void (*func)(void *), void *data, int iterations);

#endif // TIMING_H
