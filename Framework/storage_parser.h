#ifndef STORAGE_PARSER_H
#define STORAGE_PARSER_H
#include <stdio.h>

extern void set_yyin(FILE *fd);

#define MAX_STR_LEN 256

typedef struct {
  size_t version;
  size_t result_count;
  size_t columnCount;
  char test_name[MAX_STR_LEN];
} CsvHeader;

typedef struct {
  CsvHeader header;
  char *column_labels[100];
  size_t label_count;
  float records[100][100];
  size_t record_count;
  size_t field_count[100];
} CsvData;

extern CsvData csv_data;
extern int yyparse(void);
#endif