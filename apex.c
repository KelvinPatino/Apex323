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

	//if the only commandline argument is the program itself, 
	//print the menu to help the user out
	if (argc == 1){
		menu();
	}

	//else, if there are multiple command line arguments, iterate through them to determine 
	//the next course of action
	
	// count = 1 so that we start parsing from argument 1, because argument 0 is the program name
	cnt = 1;

	// loop through all the command line arguments
	while(cnt < argc){
		//if current argument is "-c", we can expect a cover file next
		if(strcasecmp((argv[cnt], "-c") == 0){ 
				//move to the file name argument
				cnt ++;
				//if no file name is provided, that's an error
				if (cnt == argc){
				fprintf(stderr, "\n\nError - no file name following <%s> parameter.\n\n", argv[cnt -1]);
				exit(-1);
				}
			//this is where me and isaac stopped, this section is to be finished tomorrow. 
				
				

				


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
