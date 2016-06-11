#ifndef _STable_
#define _STable_

#include "../userprog/bitmap.h"
#include "Sem.h"

#define MAX_SEMAPHORE 10
class STable{

private:
	BitMap* bm;
	Sem* semTab[MAX_SEMAPHORE];
public:
	STable();
	~STable();
	int Create(char* name, int init);
	int Wait(char* name);
	int Signal(char* name);
	int FindFreeSlot(int id);
};

#endif