#include "../userprog/syscall.h"

int main()
{
	int i = 0;

	for(i = 0; i < 256; i++){
		PrintInt(i);
		PrintString(".\t");
		PrintChar(i);
		PrintChar('\n');
	}

	return 0;
}
