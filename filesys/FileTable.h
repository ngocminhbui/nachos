#ifndef _FILETABLE_
#define _FILETABLE_

#define OnlyRead 1
#define ReadAndWrite 0
#define NotSet -1
typedef int OpenFileId;


class FileDescriptionTable{
  private:
    OpenFile* fileTable[10];
    int permission[10];
  public:
    FileDescriptionTable(){
      memset(fileTable,NULL,sizeof(OpenFile*)*10);
      memset(permission,0,sizeof(int)*10);

      //stdin
      fileTable[0]=(OpenFile*)NULL;
      permission[0]=OnlyRead;

      //stdout
      fileTable[1]=(OpenFile*)NULL;
      permission[1]=ReadAndWrite;

    }

    OpenFile* getOpenFile(OpenFileId id)
    {
      return fileTable[id];
    }

    OpenFileId registerFile(OpenFile* f, int mode){
      // Kiem tra tu id = 2 cho den 9, co vi tri trong nao khong
      bool test = false;
      for(int i = 2; i < 10; i++)
      {
        if(fileTable[i]==NULL)
          test = true;
      }

      // Neu co vi tri trong, chon vi tri trong dau tien de dang ki file moi vao
      if(test == true)
      {
        for(int i=2;i<10;i++)
        {
          if(fileTable[i]==NULL){
            fileTable[i]=f;
            permission[i]=mode;
            // Vi tri file nho nhat trong bang file
            return i;
          }
        }
      }

      // Neu khong co cho trong, return -1
      return -1;
    }
    bool registered(OpenFileId id){
      return fileTable[id];
    }
    bool canRead(OpenFileId id){
      return true;
    }
    bool canWrite(OpenFileId id){
      return permission[id]==ReadAndWrite;
    }
    bool remove(OpenFileId id){
      if(id < 0 || id > 9 || fileTable[id] == NULL)
        return false;
      
      fileTable[id]=NULL;
      permission[id]=0;
      return true;
    }
    ~FileDescriptionTable(){
      for(int i=0;i<10;i++)
        if(fileTable[i]!=NULL)
          delete fileTable[i];
    }
  }; 

#endif