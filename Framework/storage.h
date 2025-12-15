#ifndef STORAGE_H
#define STORAGE_H
/*
 * Program Name: CnC Common Headers
 * File Name: storage.h
 * Date Created: February 4, 2024
 * Date Updated: November 11, 2024
 * Version: 0.5
 * Purpose: Provides a struct for storing results and functions for file logging
 */
#include <stdint.h>

/* Currently the ".cnc" file format is a modified csv with a header row, followed by a
 * single row of strings representing the column names, and then all subsequent rows are the data entry points.
 *
 * Header Row contains 3 metadata entries corresponding to the version number, the result count, and the column count.
 *
 * LIMITATIONS: Currently column names are limited to 255 characters (no commas allowed), and all results are formatted as double precision FP.
 * Differing text and line encoding are also currently untested and potentially unsupported.
 */

#define BUFFERLIMIT 100000 //The max value for input and other read operations

typedef struct __CnCData
{
    uint8_t isMalformed;
    char testName[256];
    uint32_t resultCount;
    uint32_t columnCount;
    char (*columnNames)[256];
    double *resultList;
} CnCData; 

/*
 * Reads/parses the specified file and produces a CnCData struct from the data entries.
 * @Param fileName: A char array representing the name of the file to be ingested.
 * @return: a single CnCData struct.  testName = FAILED if the test does not parse successfully.
 */
CnCData read_CNC(char fileName[]);

/*
 * Writes the stored list of results to a csv file with the .cnc extension
 * @Param TestName: a char array representing the name of the test.
 * @Param resultList: an array of results.
 * @Param resultCount: The length of the resultList array passed in.
 * @Param columnCount: The number of columns per line
 * @Return: 0 if successful, -1 for IO error, -2 for filename error
 */

int write_CNC(char testName[], double resultList[], uint32_t resultCount, uint32_t columnCount, char (*columnNames)[256]);
#endif // STORAGE_H
