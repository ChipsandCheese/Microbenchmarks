%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <storage_parser.h>

void yyerror(const char *s);
int yylex(void);

//int yydebug = 1;

CsvData csv_data;
%}

%union {
    char* str;
    int ival;
    float fval;
}

%token NEWLINE
%token COMMA
%token <str> STRING
%token <ival> INTEGER
%token <fval> FLOAT

%%
csv_file: header label_row record_list
    ;

header: INTEGER COMMA INTEGER COMMA INTEGER COMMA STRING NEWLINE {
        csv_data.header.version = $1;
        csv_data.header.result_count = $3;
        csv_data.header.columnCount = $5;
        strncpy(csv_data.header.test_name, $7, MAX_STR_LEN - 1);
    }
    ;

label_row: label_list NEWLINE
    ;

label_list: label_list COMMA STRING {
        csv_data.column_labels[csv_data.label_count++] = strndup($3, strlen($3));
    } | STRING {
        csv_data.column_labels[csv_data.label_count++] = strndup($1, strlen($1));
    }
    ;

record_list: /* empty */
    | record_list record
    ;

record: field_list NEWLINE {
        csv_data.record_count++;
    }
    ;

field_list: FLOAT {
        csv_data.records[csv_data.record_count][csv_data.field_count[csv_data.record_count]] = $1;
        csv_data.field_count[csv_data.record_count]++;
    }
    | field_list COMMA FLOAT {
        csv_data.records[csv_data.record_count][csv_data.field_count[csv_data.record_count]] = $3;
        csv_data.field_count[csv_data.record_count]++;
    }
    ;
%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}