#ifndef _PTABLE_
#define _PTABLE_
#include "../userprog/bitmap.h"
#include "PCB.h"
#include "synch.h"

#define MAX_PROCESS 10
class PTable{
private:
	BitMap* bm;
	PCB *pcb[MAX_PROCESS];
	int pSize;
	Semaphore* bmsem;
public:
	PTable();
	PTable(int size);
	~PTable();

	int ExecUpdate(char* name);
	int ExitUpdate(int ec);
	int JoinUpdate(int id);

	int GetFreeSlot();
	bool IsExist(int pid);
	void Remove(int pid);
	
	char* GetFileName(int id);
	PCB* GetProcess(int pid);

};

#endif