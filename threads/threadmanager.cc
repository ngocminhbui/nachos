/*************************
    Created by quocbao
*************************/

#include "threadmanager.h"
#include <string.h>
#include "synch.h"
#include "thread.h"

PCB::PCB()
{
    joinsem = new Semaphore("[PCB] Join semaphore", 0);
    exitsem = new Semaphore("[PCB] Exit semaphore", 0);
    mutex = new Semaphore("[PCB] Mutex ", 1);

    exitcode = 0;
    numwait = 0;
    parentID = -1;
    isForeground = true;		// When a program interact directly with user
								// through shell, its running mode is called foreground
								// Background programs is programs that don't interact with
								// user though shell. If user want to see its output,
								// or configuration, they have to open file created by that program,
								// usually called log file, conf file,...
}

PCB::PCB(int id)
{
    joinsem = new Semaphore("[PCB] Join semaphore", 0);
    exitsem = new Semaphore("[PCB] Exit semaphore", 0);
    mutex = new Semaphore("[PCB] Mutex ", 1);

    exitcode = 0;
    numwait = 0;

    thread = NULL;
    pid = id;
	isForeground = true;

    if (id != 0)
        parentID = currentThread->getPID();
    else parentID = 0;
}

PCB::~PCB()
{
    delete joinsem;
    delete exitsem;
    delete mutex;

    if (thread != NULL)
    {
        thread->freeSpace();
        thread->Finish();
    }
}

int PCB::Exec(char *filename, int pid)
{
    mutex->P();

    thread = new Thread(filename);
    if (thread == NULL)
    {
        printf("PCB::exec() -> Out of memory to create new thread !\n");
        mutex->V();
        return -1;
    }

    thread->processID = pid;
	parentID = currentThread->processID;
	thread->Fork(_StartProcess, pid);
	mutex->V();

	return pid;
}

int PCB::GetID()
{
    return pid;
}

int PCB::GetNumWait()
{
    return numwait;
}

void PCB::JoinWait()
{
    joinsem->P();
}

void PCB::ExitWait()
{
    exitsem->P();
}

void PCB::JoinRelease()
{
    joinsem->V();
}

void PCB::ExitRelease()
{
    exitsem->V();
}

void PCB::IncNumWait()
{
    mutex->P();
    numwait++;
    mutex->V();
}

void PCB::DecNumWait()
{
    mutex->P();
    if (numwait > 0) numwait--;
    mutex->V();
}

void PCB::SetExitCode(int ec)
{
    exitcode = ec;
}

int PCB::GetExitCode()
{
    return exitcode;
}

void PCB::SetFileName(char* fn)
{
    strcpy(pName, fn);
}

char* PCB::GetFileName()
{
    return pName;
}


void _StartProcess(int pid)
{
	char *getName = pTab->GetName(pid);

	AddrSpace *space = new AddrSpace(getName);

	// Load memory of created process into memory
	currentThread->space = space;
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	// Switch to user mode to execute the process
   	machine->Run();

   	ASSERT(FALSE);
    if (getName != NULL) delete getName;
}




/*********************************

           Process Table

**********************************/




/**********************************

			Semaphore

**********************************/



/**********************************

		Semaphore Table

**********************************/
