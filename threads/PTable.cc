#include "PTable.h"
#include "system.h"
PTable::PTable(){
	this->bm=new BitMap(MAX_PROCESS);
	for(int i=0;i<MAX_PROCESS;i++)
		pcb[i]=NULL;
	bmsem = new Semaphore("BMSEM",1);	
	
	//root process
	bm->Mark(0);
	pcb[0]=new PCB(); //special void constructor, save just only for root process
	pSize=1;
	pcb[0]->SetFileName("root");
}	
PTable::PTable(int size){
	//nothing for now...
}
PTable::~PTable(){
	for(int i=0;i<MAX_PROCESS;i++)
		if(pcb[i]!=NULL)
			delete pcb[i];
	delete bm;
	delete bmsem;
	pSize=0;
}
int PTable::ExecUpdate(char* name){
	this->bmsem->P();

	OpenFile* p = fileSystem->Open(name);

	if(!p){
		printf("------------------Can't open file,ptable:\n");
		printf("%s\n",name);
		this->bmsem->V();
		return -1;
	}
	if(strcmp(name,pcb[currentThread->pid]->GetFileName())==0){
		printf("------------------Can't execute itself,ptable\n");
		this->bmsem->V();
		return -1;
	}

	if(bm->NumClear()==0){
		printf("------------------Can't execute more process,ptable\n");
		this->bmsem->V();
		return -1;
	}

	int slot = this->bm->Find();
	pcb[slot] = new PCB(slot);

	int ret=pcb[slot]->Exec(name,slot);

	this->bmsem->V();
	return ret;
}
int PTable::ExitUpdate(int ex){
	if(currentThread->pid==0){
		printf("Main process exited\n");
		interrupt->Halt();
		return 0;
	}

	pcb[currentThread->pid]->SetExitCode(ex);
	if(IsExist(currentThread->parentid)){
		pcb[currentThread->parentid]->DecNumWait();
		pcb[currentThread->parentid]->JoinRelease();
	}
	pcb[currentThread->pid]->ExitWait();
	
	Remove(currentThread->pid);
	return ex;
}
int PTable::JoinUpdate(int id){
	if(id<0 || id>=MAX_PROCESS || currentThread->pid != pcb[id]->GetParentID())
	{
		printf("------------------Co loi, PTABle:JoinUpdate\n");
		return -1;
	}
	pcb[currentThread->pid]->IncNumWait();
	pcb[currentThread->pid]->JoinWait();
	int ex = pcb[id]->GetExitCode();
	pcb[currentThread->pid]->ExitRelease();
	return ex;
}

int PTable::GetFreeSlot(){
	bmsem->P();
	for(int i=0;i<MAX_PROCESS;i++)
		if(bm->Test(i)){
			bmsem->V();
			return i;
		}
	bmsem->V();	
	return -1;
}
bool PTable::IsExist(int pid){
	bmsem->P();
	for(int i=0;i<MAX_PROCESS;i++)
		if(pcb[i] != NULL && pcb[i]->GetID()==pid){
			bmsem->V();
			return true;
		}
	bmsem->V();
	return false;
}
void PTable::Remove(int pid){
	this->bm->Clear(pid);
	if(pcb[pid] != NULL)
		delete pcb[pid];
}
char* PTable::GetFileName(int id){
	return pcb[id]->GetFileName();
}

PCB* PTable::GetProcess(int pid){
	return pcb[pid];
}