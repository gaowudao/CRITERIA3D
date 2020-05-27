#ifndef READERG5FILESC4C7_H
#define READERG5FILESC4C7_H

int readERG5CellListNumber(FILE *fp);
void readTheCellNumber(FILE *fp, char* numCell);
void readEarliestLatestDateC4C7(FILE *fp,int* firstDate, int* lastDate);
void getTheNewDateShiftingDays(int dayOffset, int day0, int month0, int year0, int* dayFinal, int* monthFinal, int* yearFinal);
#endif // READERG5FILESC4C7_H
