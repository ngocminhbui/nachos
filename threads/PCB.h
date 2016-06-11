#ifndef _PCB_
#define _PCB_

#include "synch.h"
#include "thread.h"

#define MAX_PROCESS_NAME 50
class PCB{
private:
	Semaphore* joinsem;
	Semaphore* exitsem;
	Semaphore* multex;
	int exitcode;
	int numwait;
	char filename[MAX_PROCESS_NAME];
	Thread* thr;
public:
	int pid;
	int parentID;
	PCB();
	PCB(int id);
	~PCB();

	int Exec(char* filename,int pid);
	int GetID();
	int GetParentID();
	int GetNumWait();

	void JoinWait();
	void ExitWait();
	void JoinRelease();
	void ExitRelease();

	void IncNumWait();
	void DecNumWait();

	void SetExitCode(int ec);
	int GetExitCode();
	
	void SetFileName(char* fn);
	char* GetFileName();
};


#endif