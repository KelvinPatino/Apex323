#include<stdio.h> 
#include <stdlib.h>
#include <string.h>
#define MAX_PATH = 50

void menu(){
	printf("\nSince you didn't run the program with arguments, here's the menu to help you out:)\n");
	//menu function placeholder for isaac to complete
}

void parseCommandLine(int argc, char *argv[]){

	//programatically print each argument in the command line
	for (int i = 0; i < argc; i++){
		printf("argv[%d]=%s\n",i,argv[i]);
	}

	if (argc == 1){
		menu();
	}
}

//main function with command line argument count and array of command line arguments
//argc is the number of arguments and *argv[] is the array of arguments themselves
int main(int argc, char *argv[]) {
	printf("Hello World\n");

	parseCommandLine(argc, argv);{
	return 0;

}



}
