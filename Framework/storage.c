// Bring in the platformCode, just in case we're using GCC and resultingly don't
// have access to strcat_s :)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <platformCode.h>
#include <storage.h>
#include <storage_parser.tab.h>
#include <storage_parser.h>

/*
 * Program Name: CnC Common Headers
 * File Name: storage.c
 * Date Created: November 11, 2024
 * Date Updated: November 11, 2024
 * Version: 0.1
 * Purpose: Provides functions for storage aspects of the framework.
 */

const uint8_t VERSIONCODE = 1;

CnCData read_CNC(char fileName[]) {
  // Copied for parity with write_CNC
  // Append .cnc to the testName input.  File type is ALWAYS .cnc
  char AppendedName[255];
  strcpy(AppendedName, fileName);
  strcat_s(AppendedName, 255, ".cnc");

  CnCData data = {.isMalformed = 1};
  FILE *file;
  if ((file = fopen(AppendedName, "r")) == NULL)
    return data;
  set_yyin(file);
  // mutates csv_data global, it's just how YACC works
  int parse_result = yyparse();

  if (parse_result > 0) {
    return data;
  }

  if (csv_data.header.version != 1) {
    return data;
  }

  if (csv_data.header.result_count !=
      (csv_data.record_count * csv_data.label_count)) {
    return data;
  }

  data.resultCount = csv_data.header.result_count;

  data.columnCount = csv_data.header.columnCount;

  size_t num_column_labels = 0;
  for (size_t i = 0; i < 100; i++) {
    if (csv_data.column_labels[i] != NULL) {
      num_column_labels += 1;
    } else {
      break;
    }
  }

  if (data.columnCount != num_column_labels) {
    return data;
  }

  if (data.columnCount != csv_data.label_count) {
    return data;
  }

  data.columnNames = malloc(data.columnCount * sizeof(char[256]));

  for (size_t i = 0; i < data.columnCount; i++) {
    strcpy(data.columnNames[i], csv_data.column_labels[i]);
  }

  data.resultList =
      malloc(data.resultCount * data.columnCount * sizeof(double));
  if (data.resultList == NULL) {
    return data;
  }

  for (size_t i = 0; i < csv_data.record_count; i++) {
    for (size_t j = 0; j < csv_data.field_count[i]; j++) {
      data.resultList[(i * csv_data.record_count) + j] = csv_data.records[i][j];
    }
  }

  fclose(file);
  // Sets the malform check to false as the operation is complete.
  data.isMalformed = 0; 

  return data;
}

int write_CNC(char testName[], double resultList[], uint32_t resultCount,
              uint32_t columnCount, char (*columnNames)[256]) {
  FILE *file;

  // Enforce maximum file name limit of 255 or lower in accordance with most
  // restrictive OS limits
  if (strlen(testName) > 250)
    return -2;

  // Append .cnc to the testName input.  File type is ALWAYS .cnc
  char AppendedName[255];
  strcpy(AppendedName, testName);
  strcat_s(AppendedName, 255, ".cnc");

  if ((file = fopen(AppendedName, "w")) == NULL)
    return -1;

  // Writes the file metadata to the first row of the csv
  fprintf(file, "%u,", VERSIONCODE);
  fprintf(file, "%u,", resultCount);
  fprintf(file, "%u,", columnCount);
  fprintf(file, "%s\n", testName);

  // Writes the column names to the second row of the csv
  for (size_t i = 0; i < columnCount; i++) {
    fprintf(file, "%s", columnNames[i]);
    if (i + 1 != columnCount)
      fprintf(file, ",");
  }
  fwrite("\n", sizeof(char), 1, file);

  // Writes the resultList in csv format with columnCount enforcing the
  // frequency of line breaks
  uint16_t columnCursor = 0;
  for (size_t i = 0; i < resultCount; i++) {
    fprintf(file, "%lf", resultList[i]);
    if (columnCursor < columnCount) {
      columnCursor++;
      if (columnCursor == columnCount) {
        fprintf(file, "\n");
        columnCursor = 0;
      } else
        fprintf(file, ",");
    }
  }

  fclose(file);
  return 0;
}