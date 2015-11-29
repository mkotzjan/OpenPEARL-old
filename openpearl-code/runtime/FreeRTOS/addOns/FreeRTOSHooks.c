#include <stdio.h>
void vApplicationStackOverflowHook(){
	printf("Stack Overflow\n");
	for(;;);
}

void vApplicationMallocFailedHook(){
	printf("malloc failed!\n");
	for(;;);
}
