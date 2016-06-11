#include "SC.h"
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <string.h>
#include <malloc.h>
#include "utility.h"
#include "../filesys/FileTable.h"
#define MaxFileLength 32

// System call ngat he thong
void Syscall_Halt()
{
    printf("Syscall_Halt called by user, caller pid = %d\n", currentThread->pid);
    interrupt->Halt();
}

// Chong tran buffer cho console
void consoleFlush()
{
    char null;
    while (synchcons->Read(&null, 1));
}

// Chuyen doi tu char[] sang int
int char2int(char* buffer, int bufferSize)
{
    int result = 0;
    int dec = 1;

    for (int i = bufferSize-1; i >= 1; i--)
    {
        result += dec * (buffer[i] - '0');
        dec *= 10;
    }

    //Xu ly ki tu dau tien
    if (buffer[0] >='0' && buffer[0] <= '9')
        result += dec*(buffer[0] - '0');
    else if (buffer[0] == '-')
        result = -result;

    return result;
}

// Tinh gia tri tuyet doi cua mot so
int abs(int a)
{
    return (a < 0) ? -a : a;
}

// Ham chuyen tu int qua char*
// Modified version from http://www.strudel.org.uk/itoa/
char* int2char(int val, int base)
{
    if (val == 0)
    {
        static char c0 = '0';
        return &c0;
    }

    static char buffer[BUFFER_SIZE];
    int absVal = abs(val);

    int i = BUFFER_SIZE - 2;

    for(; absVal && i ; --i, absVal /= base)
        buffer[i] = "0123456789abcdef"[absVal % base];

    if (val < 0)
        buffer[i--] = '-';

    return &buffer[i+1];
}

// Kiem tra char[] co phai la mot int hop le hay khong ?
int isIntBuffer(char* buffer, int bufferSize)
{
    // char[] rong khong duoc xem la chuoi int hop le
    if ((bufferSize == 0) || (buffer == NULL))
        return 0;

    // Kiem tra so am/duong
    if (buffer[0] != '+' && buffer[0] != '-' && (buffer[0] < '0' || buffer[0]>'9'))
        return 0;

    for (int i = 1; i < bufferSize; i++)
        if ((buffer[i] < '0') || (buffer[i] > '9'))
            return 0;

    return 1;
}

// System call tru 2 so
void Syscall_Sub()
{
    DEBUG('a',"SC_Sub called\n");

    // Doc argument tu thanh ghi $a0 ($4)
    int op1 = machine->ReadRegister(4);

    // Doc argument tu thanh ghi $a1 ($5)
    int op2 = machine->ReadRegister(5);

    int result = op1 - op2;

    // Tra ket qua ve thanh ghi $v0 ($2)
    machine->WriteRegister(2, result);
}

// System call doc mot so int
void Syscall_ReadInt()
{
    DEBUG('a',"SC_ReadInt called\n");

    // Khoi tao buffer luu du lieu tu console
    // Dung malloc de dung vung nho heap, chong tran vung nho stack
    char *buffer = (char*)malloc(BUFFER_SIZE);

    // Doc tu console 20 byte
    int bufferSize = synchcons->Read(buffer, BUFFER_SIZE);

    // Bao ve buffer overflow tren console (giong scanf_s, gets_s,...)
    // Chong tan cong: 111...1111rm -rf /
    if (bufferSize == BUFFER_SIZE)
        consoleFlush();

    if (isIntBuffer(buffer, bufferSize) == 0)
    {
        // Tra ve 0 vao thanh ghi $v0 ($2)
        machine->WriteRegister(2, 0);
        return;
    }

    // Chuyen doi buffer nhap vao sang so
    int number = char2int(buffer, bufferSize);

    // Giai phong buffer
    free(buffer);

    // Tra ket qua ve thanh ghi $v0 ($2)
    machine->WriteRegister(2, number);
}

/*void Syscall_PrintInt()
{
    DEBUG('a',"SC_PrintInt called\n");
    //Doc argument
    int number = machine->ReadRegister(4);
    //number -> string
    char buffer[11];
    int p=10;

    bool negative=(number<0)?true:false;

    while(number!=0)
    {
        buffer[p--]=abs(number % 10)+'0';
        number=number/10;
    }
    if(negative) buffer[p--]='-';

    if(p==10)
        buffer[p--]='0';

    //Print
    synchcons->Write(buffer+p+1,11-p+1);
    return;
}*/

// System call in mot so int
void Syscall_PrintInt()
{
    DEBUG('a',"SC_PrintInt called\n");

    //Doc argument tu thanh ghi $a0 ($4)
    int number = machine->ReadRegister(4);

    // Tao buffer luu so can xuat ra
    char* buffer = int2char(number, 10);            // static char buffer[]

    // Ghi buffer ra man hinh console
    synchcons->Write(buffer, strlen(buffer));
}

// System call doc mot ki tu
void Syscall_ReadChar()
{
    DEBUG('a',"SC_ReadChar called\n");

    char c;
    synchcons->Read(&c, 1); //Doc tu console

    // Bao ve buffer overflow tren console (giong scanf_s, gets_s,...)
    // Chong tan cong: 111...1111rm -rf /
    consoleFlush();

    // Ghi ki tu doc duoc vao thanh ghi ket qua $v0 ($2)
    machine->WriteRegister(2, c);
}

// System call in mot ki tu
void Syscall_PrintChar()
{
    DEBUG('a',"SC_PrintChar called\n");

    //Doc tu thanh ghi tham so $a0 ($4)
    char a = machine->ReadRegister(4);

    //Ghi ki tu ra man hinh console
    synchcons->Write(&a, 1);
}

// System call doc mot chuoi
void Syscall_ReadString()
{
    DEBUG('a',"SC_ReadString called\n");

    // Doc len dia chi va do dai buffer do nguoi lap trinh dua vao
    int address = machine->ReadRegister(4);
    int bufferSize = machine->ReadRegister(5);

    // Khoi tao buffer ung voi do dai chuoi do lap trinh vien cung cap
    char* buffer = (char*)malloc(bufferSize);

    // Nap buffer vua tao tu buffer cua console va lay do dai buffer that su
    int realBufferSize = synchcons->Read(buffer, bufferSize);

    // Bao ve buffer overflow tren console (giong scanf_s, gets_s,...)
    // Chong tan cong: 111...1111rm -rf /
    if (realBufferSize == bufferSize)
        consoleFlush();

    // Ghi buffer vua doc tu console vao memory
    for(int i = 0; i < realBufferSize; i++)
        machine->WriteMem(address+i, 1, buffer[i]);

    // Giai phong buffer
    free(buffer);
}

// System call in ra mot chuoi
void Syscall_PrintString()
{
    DEBUG('a',"SC_PrintString called\n");

    // Lay dia chi cua chuoi can in
    int address = machine->ReadRegister(4);

    int dummy;

    // Doc va xuat mot day cac o nho cho den khi gap '\0'
    while ((machine->ReadMem(address++, 1, &dummy) && dummy != '\0'))
    {
        char c = dummy;
        synchcons->Write(&c, 1);
    }
}



char* User2System(int virtAddr, int limit)
{
    int i;
    int oneChar;
    char* kernelBuf = NULL;

    kernelBuf = new char[limit + 1];
    if(kernelBuf == NULL)
        return kernelBuf;

    memset(kernelBuf, 0, limit + 1);

    for(i = 0; i < limit; i++)
    {
        machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;

        if(oneChar == 0)
            break;
    }
    return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
    if(len < 0) return -1;
    if(len == 0) return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int) buffer[i];
        machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    }while(i < len && oneChar != 0);

    return i;
}

void Syscall_Create()
{
    // Dia chi ao
    int virtAddr;
    // Ten file
    char* filename;
    char dbgFile='a';

    DEBUG(dbgFile, "\n SC_Create call...");
    DEBUG(dbgFile, "\n Reading virtual address of filename");

    virtAddr = machine->ReadRegister(4);

    DEBUG(dbgFile, "\n Reading filename");

    filename = User2System(virtAddr, MaxFileLength + 1);

    if(filename == NULL)
    {
        printf("\nNot enough memory in system");
        DEBUG(dbgFile, "\nNot enough memory in system");
        machine->WriteRegister(2, -1);
        delete filename;
        return;
    }

    DEBUG(dbgFile, "\nFinish reading filename");

    if(!fileSystem->Create(filename, 0))
    {
        printf("\nError create file %s", filename);
        machine->WriteRegister(2, -1);
        delete filename;
        return;
    }
    machine->WriteRegister(2, 0);

    delete filename;
    return;
}

void Syscall_Open()
{
    // Dia chi ao
    int virtAddr = machine->ReadRegister(4);
    // Kieu mo file
    int type = machine->ReadRegister(5);

    // Lay ten file can mo
    char *name = User2System(virtAddr, 50);

    if(name == NULL) // Loi
        machine->WriteRegister(2, -1);
    else{
        // Stdin <=> 0
        if (strcmp(name, "stdin") == 0)
            machine->WriteRegister(2, 0);
            
        // Stdout <=> 1
        else if(strcmp(name, "stdout") == 0)
            machine->WriteRegister(2, 1);
        else // Khong phai la stdin hoac stdout
        {
            OpenFile*  file = fileSystem->Open(name);
            // File khong ton tai
            if(file == NULL) 
                machine->WriteRegister(2, -1);
            else
            {
                // Mo file tu bang File
                FileDescriptionTable* desTable = fileSystem->getDesTable();

                // Dang ki file vao bang file duoc mo
                machine->WriteRegister(2, desTable->registerFile(file, type));
            }
        }

        delete[]name;
    }
}

void Syscall_Read()
{
    // Dia chi ao
    int virtAddr = machine->ReadRegister(4);
    // So byte can doc
    int numByte = machine->ReadRegister(5);
    // Vi tri file trong bang file
    OpenFileId id = machine->ReadRegister(6);
    int nRead = -1; // So ki tu doc duoc

    char* buffer = new char[numByte];
    if(buffer == NULL)
        machine->WriteRegister(2, -1);
    else
    {
        // Kiem tra ID thuoc bang file va file khong phai la stdout (id = 1)
        if (id < 0 || id > 9 || id == 1)
            machine->WriteRegister(2, -1);
        else if(id == 0)
        {
            // stdin
            nRead = synchcons->Read(buffer, numByte);
            if(nRead < numByte) // So byte doc duoc nho hon so byte can doc (file co kich thuoc nho)
                machine->WriteRegister(2, -2);
        }
        else
        {
            FileDescriptionTable* desTable = fileSystem->getDesTable();

            if(desTable->registered(id) == false) // Khong ton tai file id trong bang file
                machine->WriteRegister(2, -1);
            else
            {   
                OpenFile* file = desTable->getOpenFile(id);

                if(desTable->canRead(id)) // Thuc chat thi file nao cung doc duoc!
                {
                    nRead = file->Read(buffer, numByte);
                }
                else
                {
                    machine->WriteRegister(2, -1);
                }
            }
        }   

        // Tra ve so byte doc duoc
        machine->WriteRegister(2, nRead);

        System2User(virtAddr, nRead, buffer);

        delete buffer;
    }
}

//
void Syscall_Write()
{
    // Dia chi ao
    int virtAddr = machine->ReadRegister(4);
    // So byte can viet
    int numByte = machine->ReadRegister(5);
    // Vi tri file trong bang file
    OpenFileId id = machine->ReadRegister(6);

    int nWritten = -1; // So ki tu viet duoc

    char* buffer = User2System(virtAddr, numByte);

    if(buffer == NULL)
        machine->WriteRegister(2, -1);
    else
    {
        if(id < 0 || id > 9 || id == 0) // Vi tri file khong co trong bang file hoac la STDIN
            machine->WriteRegister(2, -1);
        else if(id == 1)
        {
            //STDin
            nWritten = synchcons->Write(buffer, numByte);
            if(nWritten < numByte) // So byte viet duoc nho hon so byte can viet
                machine->WriteRegister(2, -2);
        }
        else
        {
            FileDescriptionTable* desTable = fileSystem->getDesTable();

            // Khong co file trong bang file
            if(desTable->registered(id) == false)
                machine->WriteRegister(2, -1);
            else
            {
                OpenFile* file = desTable->getOpenFile(id);

                if(desTable->canWrite(id)) // Neu file co the viet duoc
                    nWritten = file->Write(buffer, numByte);
                else
                    machine->WriteRegister(2, -1);
            }
        }

        // Tra ve so byte viet duoc
        machine->WriteRegister(2, nWritten);

        delete buffer;
    }
}

void Syscall_Seek()
{
    // Vi tri file trong bang file
    OpenFileId id = machine->ReadRegister(4);

    // Vi tri muon seek den
    int pos = machine->ReadRegister(5);

    //char a;

    // id khong thuoc bang file HOAC (stdin va stdout)
    if(id < 2 || id > 9)
        machine->WriteRegister(2, -1);
    else
    {
        FileDescriptionTable* desTable = fileSystem->getDesTable();

        // Neu bangfile[id] chua co file duoc dang ki
        if(desTable->registered(id) == false)
            machine->WriteRegister(2, -1);
        else // nguoc lai
        {
            OpenFile* file = desTable->getOpenFile(id);

            // Vi tri khong hop le
            if(pos < -1)
            {
                printf("Vi tri khong hop le\n");
                machine->WriteRegister(2, -1);
            }
            // di chuyen den cuoi file;
            else if(pos == -1)
            {
                file->Seek(file->Length());
                machine->WriteRegister(2,file->Length());
            }
            else if (pos <= file->Length()) // di chuyen den vi tri bat ki ton tai trong file
            {  
                file->Seek(pos);
                machine->WriteRegister(2, pos);
            }
            else // Vi tri muon seek den lon hon kich thuoc file
            {
                printf("Kich thuoc file khong du\n");
                machine->WriteRegister(2, -1);
            }
        }
    }
}

void Syscall_CloseFile()
{
    // Vi tri file trong bang file
    OpenFileId id = machine->ReadRegister(4);
    // Kiem tra dong file duoc khong
    bool ret = fileSystem->getDesTable()->remove(id);
    // Tra ve ket qua
    if(ret){
        machine->WriteRegister(2, 0);
    }
    else
        machine->WriteRegister(2, -1);
}

/* 

Quy 's code above and bottom

*/

void Syscall_CreateSemaphore()
{
    int virtAddr = machine->ReadRegister(4);

    int semval = machine->ReadRegister(5);

    char *name = User2System(virtAddr, 50);

    if(name == NULL)
        machine->WriteRegister(2, -1);

    // semTab->Create(name, semval)

    //STable *sTab = new STable();


    if(semTab->Create(name, semval))
        machine->WriteRegister(2, 1);
    else machine->WriteRegister(2, -1);

    // Ghi vao thanh ghi 2
}

void Syscall_Up()
{
    int virtAddr = machine->ReadRegister(4);

    char *name = User2System(virtAddr, 50);

    if (name == NULL)
        machine->WriteRegister(2, -1);

    // Kiem tra name trong bang sTab

    if(semTab->Signal(name))
        machine->WriteRegister(2, 1);
    else machine->WriteRegister(2, -1);


    // Signal cua Stable

    // Ghi vao thanh ghi 2
}

void Syscall_Down()
{
    int virtAddr = machine->ReadRegister(4);

    char *name = User2System(virtAddr, 50);

    if (name == NULL)
        machine->WriteRegister(2, -1);

    // Kiem tra name trong bang STab

    // Goi phuong thuc Wait cua lop STable
    if(semTab->Wait(name))
        machine->WriteRegister(2, 1);
    else
        machine->WriteRegister(2, -1);

    // Luu ket qua thuc hien vao thanh ghi r2
}



/****************************
*
*           MINH 's code
*
*****************************/

void Syscall_Exec(){

    int virtAddr = machine->ReadRegister(4);
    char* name=User2System(virtAddr,100);//(int virtAddr, int limit)
    if(name==NULL){
        printf("Khong mo duoc file");
        machine->WriteRegister(2,-1);
        return;
    }
    machine->WriteRegister(2,pTab->ExecUpdate(name));
}

void Syscall_Join(){
    int pid = machine->ReadRegister(4);
    machine->WriteRegister(2,pTab->JoinUpdate(pid));
}

void Syscall_Exit(){
    int exitStatus = machine->ReadRegister(4);
    machine->WriteRegister(2,pTab->ExitUpdate(exitStatus));
}
