#ifndef UNIT_TESTS_H
#define UNIT_TESTS_H

/*
 * Program Name: CnC Framework Unit Tests
 * File Name: unitTests.c
 * Date Created: October 19, 2024
 * Date Updated: November 11, 2024
 * Version: 0.4
 * Purpose: Unit Tests for the Framework
 */

#include <stdarg.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#define OUTPUT_DATA_EXTENSION ".cnc"
#define OUTPUT_FILE_NAME (TEST_NAME OUTPUT_DATA_EXTENSION)
#define MAX_FILE_NAME_LEN 50

inline static void fatal_error(int code, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  exit(code);
}

static void preamble() {
  printf("CnC Framework Unit Tests.  Return code 0 for success, 1 for "
         "verification failure, and 2 for IO error\n");
}
#endif