#include <stdio.h>
#include <stdlib.h>
#include <csignal>

#include "bufs_error.hpp"
#include "bufs_misc.hpp"

#define BUFS_VERSION 1

const char* bufsHelpPageString = "\
BUFS - Backup Filesystem\n\
Made by Mateusz d3s Dukat\n\
./bufs [hvxltc] image_file [input_dir]/[output_file]\n\
\t h - Show this help\n\
\t v - Show version\n\
\t x image_file output_file - Extract output_file from image_file\n\
\t l image_file - List files in image\n\
\t t image_file - Test image for errors [with big images, takes a lot of time]\n\
\t c image_file input_dir - Create image from files in input_dir\n\n\
Have a nice day UwU\n";

void errorCheck(int errorNo){
	printf("ERROR: ");
	int errorValue;
	switch(errorNo){
		case ENOBUFS:
			printf("Selected file is not BUFS image!");
			errorValue = ENOBUFS;
			break;
		case ENOFILES:
			printf("Files count not found in header!");
			errorValue = ENOFILES;
			break;
		case ENOFSPECIF:
			printf("No input/output file specified!");
			errorValue = ENOFSPECIF;
			break;
		case ENODSPECIF:
			printf("No input directory specified!");
			errorValue = ENODSPECIF;
			break;
		case ETMFILES:
			printf("Too much files in directory!");
			errorValue = ETMFILES;
			break;
		case EOPENDIR:
			printf("Error with opening directory!");
			errorValue = EOPENDIR;
			break;
		default:
			printf("Undefined error!");
			errorValue = 123;
	};
	printf("\nExiting...\n");
	exit(errorValue);
};

void getSomeHelp(int helpCase){
	if(helpCase == 0){		// -v (version)
		printf("BUFS - Backup Filesystem\nMade by Mateusz d3s Dukat\nVersion: %d\n", BUFS_VERSION);
	}else if(helpCase == 1){	// quick help 
		printf("./bufs [hvxltc] image_file [input_dir]/[output_file]\n");
	}else if(helpCase == 2){	// long help
		printf("%s", bufsHelpPageString); // bufs_misc.hpp
	};
	exit(0);
};

void signalHandler(int signum){
	if(signum == SIGABRT){
		exit(SIGABRT); // Quick-fix, occurs at end of function so we dont care
	};

	exit(signum);
};
