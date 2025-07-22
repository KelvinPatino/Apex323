#include <stdio.h>
#include <stdlib.h>



#define MAX_PATH 500

void Menu(char *program){

int choice;

fprintf(stdout, "Program intentions: read a cover image and secret message\n");
fprintf(stdout, "embed data in a 24-bit image using differential value encoding\n");
fprintf(stdout, "resulting pixel value applies secret message, then  allows for extraction to process\n");
fprintf(stdout, "Enter 1 to get information or 9 to exit: ");
scanf("%d", &choice);

if(choice == 1){
fprintf(stdout, "To Hide:\n");
fprintf(stdout, "\t%s -hide -c <cover file> -m<msg file> [-s <stego file>] [-b <number of bits>]\n\n", program);

fprintf(stdout, "To Extract:\n");
fprintf(stdout, "\t%s -extract -s <cove file> [-m <message file>] [-b <number of bits>]\n\n", program);

fprintf(stdout, "Parameters:\n");
fprintf(stdout, "\tSet number of bits to hide per pixel:	-b ( 1 to 8 def:1 )\n\n");

}
if(choice == 9){

exit(0);
	}
}

int main(){

char program[] = "Menu.c";
Menu(program);
return 0;

}


















