/**
 * @file       fileio.h
 *
 * @brief      Header file, file I/O system level
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

//
//      New errors. The numbers refer to FatFS errors, these are combined.
//
#define FIO_OK              (0)                                                     // No error
#define FIO_ERR_SYSTEM      (-1)                                                    // Internal system error
#define FIO_ERR_NOTFOUND    (-2)                                                    // Directory or File not found
#define FIO_ERR_COMMAND     (-3)                                                    // Bad input of some sort
#define FIO_ERR_MAXFILES    (-4)                                                    // Too many files open
#define FIO_ERR_HANDLE      (-5)                                                    // Bad handle / not open
#define FIO_ERR_READONLY    (-6)                                                    // Writing to read opened file.

#define FIO_EOF             (1)                                                     // End of file / Directory lsit.

#define MAX_FILENAME_SIZE   (32)                                                    // Max size of filename base

typedef struct _FIO_Information {
    char name[MAX_FILENAME_SIZE+1];                                                 // Name goes here 
    int  length;                                                                    // Byte size
    bool isDirectory;                                                               // True if directory    
} FIOInfo;

#define FIO_MAX_HANDLES     (8)                                                     // Maximum number of supported files.

//
//      Do not use these, use the FIO versions instead.
//
void    FSYSInitialise(void);

int     FSYSOpen(int handle,char *name);                                            // Open an existing file
int     FSYSClose(int handle);                                                      // Close an open file.
int     FSYSRead(int handle,void *data,int size);                                   // Read bytes from a file.
int     FSYSWrite(int handle,void *data,int size);                                  // Write bytes to a file.
int     FSYSGetSetPosition(int handle,int newPosition);                             // Read and optionally set position.

int     FSYSFileInformation(char *name,FIOInfo *info);                              // Check if file exists/get information.
int     FSYSCreateFile(char *name);                                                 // Create an empty file of that name.
int     FSYSCreateDirectory(char *name);                                            // Create a directory of that name
int     FSYSDeleteFile(char *name);                                                 // Delete a file
int     FSYSDeleteDirectory(char *name);
int     FSYSChangeDirectory(char *directory);                                       // Change to a new directory

int     FSYSOpenDirectory(char *directory);                                         // Open a directory
int     FSYSReadDirectory(char *fileName);                                          // Read next directory entry
int     FSYSCloseDirectory(void);                                                   // Close directory

