#include "STable.h"
STable::STable(){
	this->bm=new BitMap(MAX_SEMAPHORE);
	for(int i=0;i<MAX_SEMAPHORE;i++)
		semTab[i]=NULL;
}
STable::~STable(){
	for(int i=0;i<MAX_SEMAPHORE;i++)
		if(semTab[i]!=NULL)
			delete semTab[i];
}
int STable::Create(char* name, int init){
	if(bm->NumClear()==0)
		ASSERT(false);//stop, not enough space

	for(int i=0;i<MAX_SEMAPHORE;i++)
		if(strcmp(semTab[i]->GetName(),name)==0)
			return -1;

	semTab[bm->Find()]=new Sem(name,init);
	return 0;
}
int STable::Wait(char* name){
	for(int i=0;i<MAX_SEMAPHORE;i++)
		if(semTab[i]!=NULL && strcmp(semTab[i]->GetName(),name)==0)
			{
				semTab[i]->wait();
				return 0;
			}
	return -1;
}
int STable::Signal(char* name){
	for(int i=0;i<MAX_SEMAPHORE;i++)
		if(semTab[i]!=NULL && strcmp(semTab[i]->GetName(),name)==0)
			{
				semTab[i]->signal();
				return 0;
			}
	return -1;
}
int STable::FindFreeSlot(int id){
	for(int i=0;i<MAX_SEMAPHORE;i++)
		if(bm->Test(i))
			return i;
	return -1;
}
