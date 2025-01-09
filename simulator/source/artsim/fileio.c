

//
//		Name : 		fileio.c
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		25th December 2024
//		Reviewed :	No
//		Purpose :	File I/O (FSYS Level interface)
//					This level does not check things like valid handles. It's purpose
//					is to precisely perform the required task.
//



#include "artsim.h"
#include "sys/stat.h"
#include "dirent.h"


//
//								Current open files.
//


static FILE *files[FIO_MAX_HANDLES];


//
//							Reset all file objects etc.
//


void FSYSInitialise(void) {
}


//
//							Mep errno to our errors
//


static int _FSYSMapError(void) {
	int e;
	switch(errno) {
		case 0:
			e = 0;break;
		case ENOENT:
			e = FIO_ERR_NOTFOUND;break;
		default:
			e = FIO_ERR_SYSTEM;break;
	}
	return e;
}


//
//							Convert file to an internal file name.
//


static char *_FSYSMapName(char *name) {
	static char buffer[256];
	mkdir("storage",0777);  													// Creates the working directory
	strcpy(buffer,"storage/");strcat(buffer,name);  								// Convert filename to working file name.
	return(buffer);
}


//
//				Get file information. Fills structure if provided and no error.
//


int FSYSFileInformation(char *name,FIOInfo *info) {

	DIR *f = opendir(_FSYSMapName(name));  										// Directory check
	if (f != NULL) {
		closedir(f);  															// Found, close it
		if (info != NULL) {  													// Fill in if required.
			info->length = 0;info->isDirectory = -1; 
		}
		return FIO_OK;  			
	}
	struct stat ffi;
	if (stat(_FSYSMapName(name),&ffi) != 0) return _FSYSMapError();  			// Try for file, Stat failed
	if (info != NULL) {
		info->isDirectory = false;
		info->length = info->isDirectory ? 0 : ffi.st_size;    					// Length if file, 0 for directory.
	}
	return FIO_OK;
}


//
//					Create a new file, deleting any currently existing file
//


int FSYSCreateFile(char *name) {
	FILE *f = fopen(_FSYSMapName(name),"w");  									// Creates an empty file.
	if (f == NULL) return _FSYSMapError();  									// Create filed.
	fclose(f);  																// Close the new file.
	return FIO_OK;
}  	


//
//						Delete file, ignoring if it does not exist
//


int FSYSDeleteFile(char *name) {
	if (unlink(_FSYSMapName(name)) == 0) return FIO_OK; 						// Delete worked okay.
	if (errno == ENOENT) return FIO_OK;											// Didn't exist, that's okay.
	return _FSYSMapError();	
}


//
//					Create a new directory if it does not already exist
//


int FSYSCreateDirectory(char *name) {  											
	if (mkdir(_FSYSMapName(name),0777) == 0) return FIO_OK; 					// Create directory okay
	if (errno == EEXIST) return FIO_OK;  										// It already exists, not an error
	return _FSYSMapError();
}


//
//				Delete directory, ignoring if it does not exist/not empty
//


int FSYSDeleteDirectory(char *name) {
	if (rmdir(_FSYSMapName(name)) == 0) return FIO_OK;  							// Removal okay
	if (errno == ENOENT || errno == EEXIST || 
						errno == ENOTEMPTY || errno == ENOTDIR) return FIO_OK;  	// Not present, not empty etc.
	return _FSYSMapError();
}  											


//
//								Change to a new directory
//


int FSYSChangeDirectory(char *directory) {
	if (chdir(_FSYSMapName(directory)) == 0) return FIO_OK;
	return _FSYSMapError();
}


//
//							Open File in R/W mode, rewind to start
//


int FSYSOpen(int handle,char *name) {
	files[handle] = fopen(_FSYSMapName(name),"r+");  								// Open read/write
	if (files[handle] == NULL) return _FSYSMapError();   							// Open failed.
	if (fseek(files[handle],0,SEEK_SET) < 0) return _FSYSMapError();  				// Rewind it.
	return FIO_OK;
};


//
//										Close the file
//


int FSYSClose(int handle) {
	if (fclose(files[handle]) < 0) return _FSYSMapError();  						// Close file.
	return FIO_OK;
}


//
//				Read data from the file. Returns error or # of bytes read.
//


int FSYSRead(int handle,void *data,int size) {
	int bytesRead = fread(data,1,size,files[handle]);  								// Attempt to read data from file.
	if (bytesRead < 0) return _FSYSMapError();  									// Read failed.
	return bytesRead;
}


//
//								Write data to the file. 
//


int FSYSWrite(int handle,void *data,int size) {
	int bytesWritten = fwrite(data,1,size,files[handle]);  							// Attempt to write.
	if (bytesWritten < 0) return _FSYSMapError(); 									// Error occurred
	if (bytesWritten != size) return FIO_ERR_SYSTEM;  								// For some reason, did not write.
	return FIO_OK;  																// It's okay.
}


//
//		Check end of file. Return -ve on error, 0 if more data, +ve if eof
//


int FSYSEndOfFile(int handle) {
	return feof(files[handle]) ? FIO_EOF : FIO_OK;  								// Seems not to be able to error :)
}


//
//				Returns current position, and sets new position if >= 0
//


int FSYSGetSetPosition(int handle,int newPosition) {
	int current = ftell(files[handle]);  											// Where are we now ?
	if (current < 0) return _FSYSMapError();  										// Failed.
	if (newPosition >= 0) {  														// Moving ?
		if (fseek(files[handle],newPosition,SEEK_SET) < 0) return _FSYSMapError();  // Move and check.
	}
	return current;
}  	


//
//								Open Directory for reading
//


static DIR* currentDirectory;

int FSYSOpenDirectory(char *directory) {
	directory = _FSYSMapName(directory);  											// Map directory
	currentDirectory = opendir(directory);  										// Open directory.
	return (currentDirectory == NULL) ? _FSYSMapError() : FIO_OK;  					// Return OK, or error.
}


//
//			Read next directory entry. This does not exclude .. or . at this level.
// 			Returns FIO_EOF if there is no directory information to read
//


int FSYSReadDirectory(char *fileName) {
	struct dirent *next = readdir(currentDirectory); 								// Read next entry.
	if (next == NULL) {  															// Read failed.
		if (errno == 0) return FIO_EOF;  											// End of directory
		return _FSYSMapError();  													// All other errors.
	}
	next->d_name[MAX_FILENAME_SIZE] = '\0';  										// Truncate file name.
	strcpy(fileName,next->d_name);
	return FIO_OK;
}  	


//
//								Close directory being read
//


int FSYSCloseDirectory(void) {
	return closedir(currentDirectory) < 0 ? _FSYSMapError() : FIO_OK;
}
