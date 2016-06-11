#ifndef _SC_H_
#define _SC_H_

// Do dai thong thuong cua mot buffer
#define BUFFER_SIZE     20

// Do dai toi da cua mot buffer
#define MAX_BUFFER_SIZE BUFFER_SIZE*10

// System call ngat he thong
void Syscall_Halt();

// System call tru 2 so
void Syscall_Sub();

// System call doc mot so int
void Syscall_ReadInt();

// System call in mot so int
void Syscall_PrintInt();

// System call doc mot ki tu
void Syscall_ReadChar();

// System call in ra mot ki tu
void Syscall_PrintChar();

// System call doc mot chuoi
void Syscall_ReadString();

// System call in ra mot chuoi
void Syscall_PrintString();

void Syscall_Create();

void Syscall_Read();

void Syscall_Write();

void Syscall_CloseFile();

void Syscall_Open();

void Syscall_Seek();


/***************MINH****************/
void Syscall_Exec();

void Syscall_Join();

void Syscall_Exit();

void Syscall_CreateSemaphore();

void Syscall_Up();

void Syscall_Down();

#endif
