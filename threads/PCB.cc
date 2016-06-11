#include "PCB.h"
#include "system.h"
#include "thread.h"
PCB::PCB(){
	//create root process
	this->pid=0;
	this->parentID=-1;
	this->numwait=0;
	this->exitcode=0;

	this->joinsem= new Semaphore("PCB-JOIN",0);
	this->exitsem= new Semaphore("PCB-EXIT",0);
	this->multex=new Semaphore("PCB-MULTEX",1);
}
PCB::PCB(int id){
	this->pid=id;
	this->parentID=currentThread->pid;
	this->numwait=0;
	this->exitcode=0;

	this->joinsem= new Semaphore("PCB-JOIN",0);
	this->exitsem= new Semaphore("PCB-EXIT",0);
	this->multex=new Semaphore("PCB-MULTEX",1);
}
PCB::~PCB(){
	//delete this->joinsem;
	//delete this->exitsem;
	//delete this->multex;
	//if(!thr)
		//delete this->thr;
}



void StartProcess_2(int id)
{
	char* fileName = pTab->GetFileName(id);

    OpenFile *executable = fileSystem->Open(fileName);
    AddrSpace *space;

    if (executable == NULL) {
		printf("PCB:Unable to open file %s\n", fileName);
		delete executable;
		return;
    }
    

    space = new AddrSpace(executable);


    currentThread->space = space;


    delete executable;			// close file

    space->InitRegisters();		// set the initial register values
    space->RestoreState();		// load page table register

    machine->Run();			// jump to the user progam

}


int PCB::Exec(char* filename,int pid){
	this->multex->P();

	this->thr = new Thread(filename);
	
	if(!thr){
		printf("PCB:Not enough space for new thread");
		this->multex->V();
		return -1;
	}
	thr->pid=pid;
	thr->parentid=currentThread->pid;

	pTab->GetProcess(pid)->SetFileName(filename);

	thr->Fork(StartProcess_2,pid);

	this->multex->V();
	return pid;
}

int PCB::GetID(){
	return this->pid;
}
int PCB::GetParentID(){
	return this->parentID;
}
int PCB::GetNumWait(){
	return this->numwait;
}
void PCB::JoinWait(){
	this->joinsem->P();
}
void PCB::ExitRelease(){
	this->exitsem->V();
}

void PCB::ExitWait(){
	this->exitsem->P();
}
void PCB::JoinRelease(){
	this->joinsem->V();
}

void PCB::IncNumWait(){
	this->multex->P();
	this->numwait++;
	this->multex->V();
}
void PCB::DecNumWait(){
	this->multex->P();
	this->numwait=this->numwait-1;
	if(this->numwait<0)
		this->numwait=0;
	this->multex->V();
}
void PCB::SetExitCode(int ex){
	this->exitcode=ex;
}
int PCB::GetExitCode(){
	return this->exitcode;
}
void PCB::SetFileName(char* fn){
	strcpy(this->filename,fn);
}
char* PCB::GetFileName(){
	return this->filename;
}