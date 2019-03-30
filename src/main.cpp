#include <string.h>

#include "bufs_func.hpp"
#include "bufs_error.hpp"
#include "bufs_misc.hpp"

//#define BUFS_VERSION 1

int main(int argc, char* argv[]){

	// TODO Signal handler (see bufs_misc.cpp)

	if(argc < 2){
		getSomeHelp(1);
	};
	
	try{
		// argument check
		if(!strcmp(argv[1], "v")){
			getSomeHelp(0);
		}
		if(!strcmp(argv[1], "h")){
			getSomeHelp(2);
		}
		if(!strcmp(argv[1], "l")){
			if(argv[2] == nullptr){
				throw ENOFSPECIF;
			}
			BUFS_List(argv[2]);
		}
		if(!strcmp(argv[1], "x")){
			if(argv[2] == nullptr || argv[3] == nullptr){
				throw ENOFSPECIF;
			}
			BUFS_Dump(argv[2], argv[3]);
		}
		if(!strcmp(argv[1], "c")){
			if(argv[2] == nullptr){
				throw ENOFSPECIF;
			}
			if(argv[3] == nullptr){
				throw ENODSPECIF;
			}
			BUFS_Build(argv[2], argv[3]);
		}

	} catch(int err){
		errorCheck(err);
	};

};
