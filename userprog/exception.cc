// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "SC.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2); // what syscall?
    switch(which)
    {
    case NoException:
        break;

    case PageFaultException:
        printf("Page Fault Exception, System's Halting...\n");
        interrupt->Halt();
        break;

    case ReadOnlyException:
        printf("Read Only Exception, System's halting...\n");
        interrupt->Halt();
        break;

    case BusErrorException:
        printf("Bus Error Exception, System's halting...\n");
        interrupt->Halt();
        break;

    // System call chua xu li duoc
    case AddressErrorException:
        printf("Address Error Exception, System's halting...\n");
        interrupt->Halt();
        break;

    case OverflowException:
        printf("Overflow Exception, System's halting...\n");
        interrupt->Halt();
        break;

    case IllegalInstrException:
        printf("Illegal Instruction Exception, System's halting...\n");
        interrupt->Halt();
        break;

    case NumExceptionTypes:
        printf("Num Exception Types, System's halting...\n");
        interrupt->Halt();
        break;

    case SyscallException:
        switch(type)
        {
        // System call ngat he thong
        case SC_Halt:
            Syscall_Halt();
            break;

        // System call tru 2 so
        case SC_Sub:
            Syscall_Sub();
            break;

        // System call doc mot so int
        case SC_ReadInt:
            Syscall_ReadInt();
            break;

        // System call in ra mot so int
        case SC_PrintInt:
            Syscall_PrintInt();
            break;

        // System call doc mot ki tu
        case SC_ReadChar:
            Syscall_ReadChar();
            break;

        // System call in ra mot ki tu
        case SC_PrintChar:
            Syscall_PrintChar();
            break;

        // System call doc mot chuoi
        case SC_ReadString:
            Syscall_ReadString();
            break;

        // System call in ra mot chuoi
        case SC_PrintString:
            Syscall_PrintString();
            break;

        case SC_Create:
            Syscall_Create();
            break;

        case SC_Open:
            Syscall_Open();
            break;   

        case SC_Read:
            Syscall_Read();
            break;

        case SC_Write:
            Syscall_Write();
            break;

        case SC_CloseFile:
            Syscall_CloseFile();
            break;

        case SC_Seek:
            Syscall_Seek();
            break;


        /********************MINH*****************/

        case SC_Exec:
            Syscall_Exec();
            break;

        case SC_Join:
            Syscall_Join();
            break;

        case SC_Exit:
            Syscall_Exit();
            break;

        // QUY
        case SC_CreateSemaphore:
            Syscall_CreateSemaphore();
            break;

        case SC_Up:
            Syscall_Up();
            break;

        case SC_Down:
            Syscall_Down();
            break;
        }

        // Advance program counters.
        machine->registers[PrevPCReg] = machine->registers[PCReg]; // for debugging, in case we are jumping into lala-land
        machine->registers[PCReg] = machine->registers[NextPCReg];
        machine->registers[NextPCReg] += 4;

        break;
    }
}
