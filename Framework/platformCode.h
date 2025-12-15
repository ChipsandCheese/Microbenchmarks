#ifndef PLATFORMCODE_H
#define PLATFORMCODE_H
/*
 * Program Name: CnC Common Headers
 * File Name: platformCode.h
 * Date Created: January 21, 2024
 * Date Updated: November 12, 2024
 * Version: 0.5
 * Purpose: This file contains all functions that interact with platform-specific functionality
 */
#ifdef __MINGW32__
//MINGW (at least on debian) expects _aligned_malloc, while gcc and clang expect aligned_alloc
#define aligned_alloc(sz, aln) _aligned_malloc(sz, aln)
#include <windows.h>
#include <pthread.h>

#elif __unix__
#define _GNU_SOURCE // This is required for `CPU_ZERO`, and `CPU_SET`
#include <pthread.h>
#include <unistd.h>

#ifndef strcat_s
//Only implemented for windows targets in GCC/Clang, but its useful so we define it ourselves anyways
int strcat_s(char *dest, int destsz, const char *src);
#endif

#endif

/* Gets the current available number of logical threads.
 * Should be called first as it populates the affinity mask table.
 */
int getThreadCount();

/* Gets the core affinity for the provided pthread.
 *@Param thread: the pthread to get affinity for.
 */
int getAffinity(pthread_t thread);

/* Sets the core affinity for the provided pthread
 *@Param thread: the pthread to get affinity for.
 *@Param proc: the logical processor number to set the affinity to.
 */
int setAffinity(pthread_t thread, int proc);


#endif // PLATFORMCODE_H
