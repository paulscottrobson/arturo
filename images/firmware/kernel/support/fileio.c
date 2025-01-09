/**
 * @file   fileio.c
 *
 * @brief      File I/O wrapper for the FSYS interface
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */



#include "common.h"

static struct _FileInfo {
    bool isInUse;
    bool isReadOnly;
} files[FIO_MAX_HANDLES];

#define VALID_AND_OPEN(n) ((n) >= 0 && (n) < FIO_MAX_HANDLES && files[n].isInUse)



/**
 * @brief      Initialise support I/O
 */
void FIOInitialise(void) {
    FSYSInitialise();
    for (int i = 0;i < FIO_MAX_HANDLES;i++) files[i].isInUse = false;
}



/**
 * @brief      Open file read/write
 *
 * @param      fileName  The file name
 *
 * @return     ECINZ
 */
int FIOOpen(char *fileName) {
    int i = 0;
    while (i < FIO_MAX_HANDLES && files[i].isInUse) i++;                            // Find an unused handle.
    if (i == FIO_MAX_HANDLES) return FIO_ERR_MAXFILES;                              // None found.
    files[i].isInUse = true;
    files[i].isReadOnly = false;
    return FSYSOpen(i,fileName);
}


/**
 * @brief      Close file
 *
 * @param[in]  handle  The handle
 *
 * @return     ECINZ
 */
int FIOClose(int handle) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    files[handle].isInUse = false;                                                  // About to close it.
    return FSYSClose(handle);
}

/**
 * @brief      Read data from open file
 *
 * @param[in]  handle  Handle on which to place.
 * @param      data    The data
 * @param[in]  size    The size
 *
 * @return     ECINZ 
 */
int FIORead(int handle,void *data,int size) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    return FSYSRead(handle,data,size);
}


/**
 * @brief      Write data tp am p[emo;e/]
 *
 * @param[in]  handle  The handle
 * @param      data    The data
 * @param[in]  size    The size
 *
 * @return     { description_of_the_return_value }
 */
int FIOWrite(int handle,void *data,int size) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    if (files[handle].isReadOnly) return FIO_ERR_READONLY;                          // Read only (not actually implemented)
    return FSYSWrite(handle,data,size);
}


/**
 * @brief      Get and enhances anyway
 *
 * @param[in]  handle       Handle
 * @param[in]  newPosition  The new position or -1 to leave
 *
 * @return     Position from the start or EVIN
 */
int FIOGetSetPosition(int handle,int newPosition) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    return FSYSGetSetPosition(handle,newPosition);
}


/**
 * @brief      FIOCre
 *  *
 * @param      fileName  The file name
 *
 * @return     { description_of_the_return_value }
 */
int FIOCreateFile(char *fileName) {
    return FSYSCreateFile(fileName);
}

/**
 * @brief      { function_description }
 *
 * @param      fileName  The file name
 *
 * @return     { description_of_the_return_value }
 */
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
    if (info != NULL) {                                                             // FSYSFileInformation doesn't clear FIOInfo
        strcpy(info->name,"?");
        info->length = 0;
        info->isDirectory = false;
    }
    return FSYSFileInformation(name,info);
}


//
//      This implementation has a 'fake' handle, but it's actually ignored and this
//      implementation can only do one directory scan at a time.
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