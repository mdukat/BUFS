#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include "bufs_func.hpp"
#include "bufs_error.hpp"

unsigned char getNumberOfFiles(char* path){	// TODO check if there are <256 files in directory, throw exception
	DIR* dir;
	struct dirent *edir;
	unsigned char no = 0;

	dir = opendir(path);	// TODO handle errors of opendir
	if(dir != NULL){
		while(edir = readdir(dir)){
			no++;
		}
	}
	closedir(dir);
	return no-2;
};

void checkFile(FILE* fileInput, unsigned char* pFileNumber){
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
			printf("FOUND, DUMPING...\t");		// TODO printf is buffered
			pFileContent = new char[fileSize];
			fread(pFileContent, fileSize, 1, pFileInput);	// read file content
			pFileOutput = fopen(pFileName, "wb");
			fwrite(pFileContent, fileSize, 1, pFileOutput);
			fclose(pFileOutput);
			delete[] pFileContent;
			delete[] pFileName;
			printf("OK!\n");			// TODO printf is buffered
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
	DIR* dir;
	struct dirent *edir;
	FILE* inputFile;
	FILE* outputFile;
	unsigned int fileSize;
	char* fileContent;
	unsigned char fileNameSize;
	char* dirAndFile;

	printf("%s -> %s...\n", dirPath, imagePath);

	outputFile = fopen(imagePath, "wb");

	// write header
	unsigned int header = 0x53465542; // 42 55 46 53 - BUFS
	fwrite(&header, sizeof(unsigned int), 1, outputFile);

	unsigned char filesNumber = getNumberOfFiles(dirPath);
	fwrite(&filesNumber, sizeof(unsigned char), 1, outputFile);

	dir = opendir(dirPath);
	if(dir != NULL){
		while(edir = readdir(dir)){
			dirAndFile = new char[strlen(dirPath)+strlen(edir->d_name)];
			strcpy(dirAndFile, dirPath);
			strcat(dirAndFile, edir->d_name);

			if(strcmp(edir->d_name, "..") == 0 || strcmp(edir->d_name, ".") == 0){	// directory fix
			}else{

			// UI
			printf("%s...\t", edir->d_name);	// TODO printf is buffered, so is not writing this one to stdout before '\n'
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
	closedir(dir);
	exit(0);
};

// TODO BUFS_Check - checks correctness of image
// check size of files (header length vs real length)
// check how many files are in the image, vs how many in header
