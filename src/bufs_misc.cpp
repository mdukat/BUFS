#include <stdio.h>
#include <stdlib.h>

#include "bufs_error.hpp"
#include "bufs_misc.hpp"

#define BUFS_VERSION 1

const char* bufsHelpPageString = "\
BUFS - Backup Filesystem\n\
Made by Mateusz d3s Dukat\n\
./bufs [hvxlc] image_file [input_dir]/[output_file]\n\
\t h - Show this help\n\
\t v - Show version\n\
\t x image_file output_file - Extract output_file from image_file\n\
\t l image_file - List files in image\n\
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
		printf("./bufs [hvxlc] image_file [input_dir]/[output_file]\n");
	}else if(helpCase == 2){	// long help
		printf("%s", bufsHelpPageString); // bufs_misc.hpp
	};
	exit(0);
};

// TODO Signal handler

//	KNOWN BUG - SIGABRT from one of functions of glibc (occured in bufs_func.cpp:BUFS_Write())
/*	strace dump:
write(1, "Writed 57 files to test.img.\n", 29Writed 57 files to test.img.
) = 29
writev(2, [{iov_base="corrupted size vs. prev_size", iov_len=28}, {iov_base="\n", iov_len=1}], 2corrupted size vs. prev_size
) = 29
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fe29f1c5000
rt_sigprocmask(SIG_UNBLOCK, [ABRT], NULL, 8) = 0
rt_sigprocmask(SIG_BLOCK, ~[RTMIN RT_1], [], 8) = 0
getpid()                                = 8146
gettid()                                = 8146
tgkill(8146, 8146, SIGABRT)             = 0
rt_sigprocmask(SIG_SETMASK, [], NULL, 8) = 0
--- SIGABRT {si_signo=SIGABRT, si_code=SI_TKILL, si_pid=8146, si_uid=1000} ---
+++ killed by SIGABRT (core dumped) +++
*/
