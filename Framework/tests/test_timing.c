#include <stdint.h>
#include <unistd.h>
#include <unitTests.h>
#include <timing.h>

/*
 * Just a function to occupy the cpu for a bit
 */
int* delay(int* seconds) {
  // volatile double a = 34.567876867;
  // volatile double b = 24.313214355;
  // for (int i; i < iterations; i++) {
  //   a += b;
  // }
  while (sleep(*seconds) > 0) {}
  return NULL;
}

/*
 * Test the timing function
 * @Return: time in ns for execution, or 0 for failure
 */
uint64_t testTiming() {
  int seconds = 1;
  return timeExecution((timed_execution_function_t)delay, &seconds,
                       1000000000);
}

int main(int argc, char *argv[]) {
  preamble();

  uint64_t timingResult = testTiming();
  printf("Timing Test exited with result time of %lu nanoseconds\n",
         timingResult);

  return 0;
}