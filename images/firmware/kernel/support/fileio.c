

//
//		Name : 		fileio.c
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		21st December 2024
//		Reviewed :	No
//		Purpose :	File I/O mapping.
//



#include "common.h"

static struct _FileInfo {
	bool isInUse;
	bool isReadOnly;	
} files[FIO_MAX_HANDLES];

#define VALID_AND_OPEN(n) ((n) >= 0 && (n) < FIO_MAX_HANDLES && files[n].isInUse)


//
//									Initialise support I/O
//


void FIOInitialise(void) {
	FSYSInitialise();
	for (int i = 0;i < FIO_MAX_HANDLES;i++) files[i].isInUse = false;
}


//
//									Open file read/write
//


int FIOOpen(char *fileName) {
	int i = 0;
	while (i < FIO_MAX_HANDLES && files[i].isInUse) i++; 							// Find an unused handle.
	if (i == FIO_MAX_HANDLES) return FIO_ERR_MAXFILES; 								// None found.
	files[i].isInUse = true;
	files[i].isReadOnly = false;
	return FSYSOpen(i,fileName); 
}


//
//										Close file
//


int FIOClose(int handle) {
	if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;  							// Bad handle
	files[handle].isInUse = false;  												// About to close it.
	return FSYSClose(handle);
}


//
//										Read Data
//


int FIORead(int handle,void *data,int size) {
	if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;  							// Bad handle
	return FSYSRead(handle,data,size);
}


//
//										Write data
//


int FIOWrite(int handle,void *data,int size) {
	if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;  							// Bad handle
	if (files[handle].isReadOnly) return FIO_ERR_READONLY;  						// Read only (not actually implemented)
	return FSYSWrite(handle,data,size);
}


//
//							 Get and optionally set position
//


int FIOGetSetPosition(int handle,int newPosition) {
	if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;  							// Bad handle
	return FSYSGetSetPosition(handle,newPosition);
}


//
//									These are direct wrappers
//


int FIOCreateFile(char *fileName) {
	return FSYSCreateFile(fileName);
}

int FIOCreateDirectory(char *fileName) {
	return FSYSCreateDirectory(fileName);
}

int FIODeleteFile(char *fileName) {
	return FSYSDeleteFile(fileName);
}

int FIODeleteDirectory(char *fileName) {
	return FSYSDeleteDirectory(fileName);
}

int FIOChangeDirectory(char *fileName) {
	return FSYSChangeDirectory(fileName);
}

int FIOFileInformation(char *name,FIOInfo *info) {
	if (info != NULL) {  															// FSYSFileInformation doesn't clear FIOInfo
		strcpy(info->name,"?");
		info->length = 0;
		info->isDirectory = false;
	}
	return FSYSFileInformation(name,info); 
}								


//
//		This implementation has a 'fake' handle, but it's actually ignored and this
//		implementation can only do one directory scan at a time.
//


int FIOOpenDirectory(char *directory) {
	return FSYSOpenDirectory(directory); 
} 		

int FIOReadDirectory(int handle,char *fileName) {
	return FSYSReadDirectory(fileName); 
}

int FIOCloseDirectory(int handle) {
	return FSYSCloseDirectory(); 
}
