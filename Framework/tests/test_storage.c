#include "storage.h"
#include <stdio.h>
#include <unitTests.h>

double DEFAULT_RESULTS[16] = {0.0, 1.0, 2.0,  3.0,  4.0,  5.0,  6.0,  7.0,
                              8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};

char DEFAULT_NAMES[16][256] = {"COLUMN0", "COLUMN1", "COLUMN2", "COLUMN3"};

CnCData data = {
    .isMalformed = 0,
    .testName = {},
    .resultCount = 16,
    .columnCount = 4,
    .columnNames = &DEFAULT_NAMES[0],
    .resultList = &DEFAULT_RESULTS[0],
}; // Test struct needs to be populated

/*
 * Test the storage API's ability to read and write accurately
 * @Return: 0 if successful, 1 for verification failure, and 2 for IO error.
 */
int test_storage() {
  printf("Writing output to %s\n", TEST_NAME);
  if (write_CNC(TEST_NAME, data.resultList, data.resultCount, data.columnCount,
                data.columnNames) != 0)
    fatal_error(2, "IO error\n");
  CnCData data_copy = read_CNC(TEST_NAME);

  // Compare the 2 versions of the data
  if (data.columnCount != data_copy.columnCount) {
    fatal_error(1, "Incorrect column count, expected %d, got %d\n",
                data.columnCount, data_copy.columnCount);
  }

  for (size_t i = 0; i < data.columnCount; i++) {
    if (strcmp(data.columnNames[i], data_copy.columnNames[i])) {
      fatal_error(1,
                  "Incorrect column name for column %d, expected %d, got %d\n",
                  i, data.columnNames[i], data_copy.columnNames[i]);
    }
  }

  if (data.resultCount != data_copy.resultCount) {
    fatal_error(1, "Incorrect column count, expected %d, got %d\n",
                data.resultCount, data_copy.resultCount);
  }

  for (size_t i = 0; i < data.resultCount; i++) {
    if (data.resultList[i] != data_copy.resultList[i]) {
      fatal_error(
          1, "Incorrect result list item for result %d, expected %d, got %d\n",
          i, data.resultList[i], data_copy.resultList[i]);
    }
  }

  if (data.isMalformed != 0 && data_copy.isMalformed != 0) {
    fatal_error(1, "Data was malformed\n");
  }

  return 0;
}

int main(int argc, char *argv[]) {
  preamble();

  int storageResult = test_storage();
  printf("Storage Test exited with return code %i\n", storageResult);

  if (!remove(OUTPUT_FILE_NAME)) {
    printf("%s deleted successfully\n", OUTPUT_FILE_NAME);
  } else {
    printf("%s could not be deleted\n", OUTPUT_FILE_NAME);
  }
  return storageResult;
}