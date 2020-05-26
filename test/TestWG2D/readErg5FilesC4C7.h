#ifndef READERG5FILESC4C7_H
#define READERG5FILESC4C7_H

int readERG5CellListNumber(FILE *fp);
void readTheCellNumber(FILE *fp, char* numCell);
bool readEarliestLatestDateC4C7(FILE *fp,int* firstDate, int* lastDate);

#endif // READERG5FILESC4C7_H
