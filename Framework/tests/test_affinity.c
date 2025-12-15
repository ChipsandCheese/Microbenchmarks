#include <unitTests.h>
#include <platformCode.h>


/*
 * Test the affinity getter/setter
 * @Return: 0 if successful, 1 for verification failure
 */

int testAffinity() {
  pthread_t thread = pthread_self();
  int affinity = 1;

  setAffinity(thread, affinity);
  int new_affinity = getAffinity(thread);
  if (new_affinity == -1 || new_affinity != affinity)
    return 1;
  return 0;
}

int main(int argc, char *argv[]) {
  preamble();

  int affinityResult = testAffinity();
  printf("Thread Affinity Test exited with return code %i\n", affinityResult);

  return affinityResult;
}