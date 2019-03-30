#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "bufs_func.hpp"
#include "bufs_error.hpp"

unsigned char getNumberOfFiles(char* path){
	DIR* dir;
	struct dirent *edir;
	unsigned char no = 0;

	dir = opendir(path);
	if(dir != NULL){
		while( (edir = readdir(dir)) != NULL ){
			no++;
		}
		closedir(dir);
		dir = NULL;
	}else{
		throw(EOPENDIR);
	};
	if(no-2 >=256){
		throw(ETMFILES);
	};
	return no-2;
};

void checkFile(FILE* fileInput, unsigned char* pFileNumber){
	// Checks if image file is BUFS file

	char* strBuffer = new char[5];
	fread(strBuffer, 4, 1, fileInput);
	if(strncmp(strBuffer, "BUFS", 4) != 0){
		throw ENOBUFS;
	};
	delete[] strBuffer;

	fread(pFileNumber, 1, 1, fileInput);
	if(*pFileNumber <= 0){
		throw ENOFILES;
	}else{
		printf("Found %d files!\n", *pFileNumber);
	};
};

void BUFS_List(char* imagePath){
	// List files in image

	FILE* pFileInput;
	unsigned char iFileNumber;
	unsigned char iFileCounter = 0;

	unsigned char fileNameSize;
	unsigned int fileSize;
	char* pFileName;

	pFileInput = fopen(imagePath, "rb");

	checkFile(pFileInput, &iFileNumber);

	while(iFileCounter < iFileNumber){	// read files
		fread(&fileNameSize, 1, 1, pFileInput);		// read file name size

		pFileName = new char[fileNameSize+1];
		fread(pFileName, fileNameSize, 1, pFileInput);	// read file name
		pFileName[fileNameSize] = '\0';

		fread(&fileSize, sizeof(unsigned int), 1, pFileInput);	// read file size

		printf("%u Bytes\t\t(%u) %s\n", fileSize, fileNameSize, pFileName);

		fseek(pFileInput, fileSize, SEEK_CUR);	// seek to next file

		// cleanup
		delete[] pFileName;
		iFileCounter++;
	};
	fclose(pFileInput);
	exit(0);
};

void BUFS_Dump(char* imagePath, char* pFileToFind){
	// Dump (extract) file from BUFS image
	// testSize - Additional test to check if everything is fine
	// TODO Add "DumpAll" function

	FILE* pFileInput;
	FILE* pFileOutput;
	char* pFileContent;
	char* pFileName;
	unsigned char iFileNumber;
	unsigned char iFileCounter = 0;
	unsigned char fileNameSize = 0;
	unsigned int fileSize = 0;

	pFileInput = fopen(imagePath, "rb");

	checkFile(pFileInput, &iFileNumber);

	while(iFileCounter < iFileNumber){ // read files
		fread(&fileNameSize, 1, 1, pFileInput);		// read file name size

		pFileName = new char[fileNameSize+1];		// read file name
		fread(pFileName, fileNameSize, 1, pFileInput);
		pFileName[fileNameSize] = '\0';
		fread(&fileSize, sizeof(int), 1, pFileInput);	// read file content size
		printf("File %s...\n", pFileName);


		if(strncmp(pFileName, pFileToFind, fileNameSize) == 0){
			printf("FOUND, DUMPING...\t");
			fflush(stdout);

			pFileContent = new char[fileSize];
			fread(pFileContent, fileSize, 1, pFileInput);	// read file content
			pFileOutput = fopen(pFileName, "wb");
			fwrite(pFileContent, fileSize, 1, pFileOutput);
			fclose(pFileOutput);
			delete[] pFileContent;
			delete[] pFileName;
			printf("OK!\n");
			break;
		};

		fseek(pFileInput, fileSize, SEEK_CUR);
		delete[] pFileName;
		iFileCounter++;
	};
	fclose(pFileInput);
	exit(0);
};

void BUFS_Build(char* imagePath, char* dirPath){
	// Build BUFS image from specified directory Path

	DIR* dir;
	struct dirent *edir;
	FILE* inputFile;
	FILE* outputFile;
	unsigned int fileSize;
	char* fileContent;
	unsigned char fileNameSize;
	char* dirAndFile;

	printf("%s -> %s...\n", dirPath, imagePath);
	// TODO Check if in path are directories (error/ignore)
	// TODO Fix path string if "/" is not last (based on fread() error)

	outputFile = fopen(imagePath, "wb");

	// write header
	unsigned int header = 0x53465542; // 42 55 46 53 - BUFS
	fwrite(&header, sizeof(unsigned int), 1, outputFile);

	unsigned char filesNumber = getNumberOfFiles(dirPath);
	fwrite(&filesNumber, sizeof(unsigned char), 1, outputFile);

	dir = opendir(dirPath);
	if(dir != NULL){
		while( (edir = readdir(dir)) != NULL ){
			dirAndFile = new char[strlen(dirPath)+strlen(edir->d_name)];
			strcpy(dirAndFile, dirPath);
			strcat(dirAndFile, edir->d_name);

			if(strcmp(edir->d_name, "..") == 0 || strcmp(edir->d_name, ".") == 0){	// directory fix
			}else{

			// UI
			printf("%s...\t", edir->d_name);
			fflush(stdout);

			// get values
			fileNameSize = strlen(edir->d_name);
			
			inputFile = fopen(dirAndFile, "r");
			fseek(inputFile, 0, SEEK_END);
			fileSize = ftell(inputFile);
			fseek(inputFile, 0, SEEK_SET);
			fileContent = new char[fileSize];
			fread(fileContent, fileSize, 1, inputFile);
			fclose(inputFile);

			// write everything to new image
			fwrite(&fileNameSize, sizeof(unsigned char), 1, outputFile);
			fwrite(edir->d_name, fileNameSize, 1, outputFile);
			fwrite(&fileSize, sizeof(unsigned int), 1, outputFile);
			fwrite(fileContent, fileSize, 1, outputFile);

			// UI
			printf("OK!\n");
			
			// cleanup
			delete[] fileContent;
			}
			delete[] dirAndFile;
		}
	}

	fclose(outputFile);
	printf("Writed %d files to %s.\n", filesNumber, imagePath);
	
	//closedir(dir);	<- SIGABRT fix
	exit(0);
};

// BUFS_Check - checks correctness of image
// check size of files (header length vs real length)
// check how many files are in the image, vs how many in header

void BUFS_Check(char* imagePath){
	FILE* image;
	char* cBuffer;
	unsigned int* uiBuffer;
	unsigned char fileCount = 0;
	unsigned char fileCounter = 0;
	unsigned int fileSize = 0;
	unsigned char nameSize = 0;
	int sigBreak = 0;

	time_t timeStart;
	time_t timeNow;
	time_t timeStartBUFS_Check = time(NULL);
	unsigned int timeI;
	float MBperSecond;
	float donePercent;

	image = fopen(imagePath, "rb");

	// Header test
	printf("Header test...\t");
	fflush(stdout);
	uiBuffer = new unsigned int;
	fread(uiBuffer, 4, 1, image);
	if(*uiBuffer == 0x53465542){
		printf("PASS\n");
	}else{
		printf("ERROR\n");
	};
	delete uiBuffer;

	// Get file count
	fread(&fileCount, 1, 1, image);
	//printf("Image says it has %d files...\n", fileCount);

	// Files check
	while(fileCounter < fileCount){
		fread(&nameSize, 1, 1, image);
		//fseek(image, nameSize, SEEK_SET);

		
		cBuffer = new char[nameSize+1];
		fread(cBuffer, nameSize, 1, image);
		cBuffer[nameSize] = '\0';
		printf("\t\t%d) %s...\t\r", fileCounter+1, cBuffer);
		fflush(stdout);
		delete[] cBuffer;

		fread(&fileSize, 4, 1, image);
		cBuffer = new char;
		timeStart = time(NULL);
		timeI = 0;
		for(unsigned int i = 0; i<fileSize; i++){
			if(fread(cBuffer, 1, 1, image) == 0){
				printf("\nFile read error on %lx!\n", ftell(image));
				sigBreak = 1;
				break;
			};
			
			timeI++;
			timeNow = time(NULL);
			if(timeNow == timeStart+1){	// every second, check speed of read and percent
				MBperSecond = (float)timeI / 1000000.0f;	// since we read every byte, every second we can calculate new value
				donePercent = ((float)i / (float)fileSize) * 100.0f;
				printf("%.2fMB/s %.2f%%\r", MBperSecond, donePercent);
				fflush(stdout);
				timeStart = timeNow;
				timeI = 0;
			};
		}
		delete cBuffer;
		if(sigBreak != 0){
			break;
		};

		fileCounter++;
		printf("PASS            \n");
	};

	if(fileCounter == fileCount){
		printf("File Count\tPASS\n");
	}else{
		printf("File Count\tFAIL\n");
	};
	fclose(image);
	printf("BUFS Test took exactly %ld seconds\n", time(NULL)-timeStartBUFS_Check);
};
