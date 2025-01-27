/**
 * @file       fileio.c
 *
 * @brief      File I/O wrapper for the FSYS interface
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
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
 * @return     Error code if non-zero
 */
int FIOOpen(char *fileName) {
    int i = 0;
    int res;
    while (i < FIO_MAX_HANDLES && files[i].isInUse) i++;                            // Find an unused handle.
    if (i == FIO_MAX_HANDLES) return FIO_ERR_MAXFILES;                              // None found.
    files[i].isInUse = true;
    files[i].isReadOnly = false;
    res = FSYSOpen(i,fileName);
    if (res<0) {
      files[i].isInUse = false;
      i = -1;
    }
    return i;
}


/**
 * @brief      Close file
 *
 * @param[in]  handle  The handle
 *
 * @return     Error code if non-zero
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
 * @return     Error code if non-zero 
 */
int FIORead(int handle,void *data,int size) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    return FSYSRead(handle,data,size);
}


/**
 * @brief      Write data to an open file.
 * @param[in]  handle  The handle
 * @param      data    The data
 * @param[in]  size    The size
 *
 * @return     Error code if non-zero
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
 * @return     Position from the start or Error Code
 */
int FIOGetSetPosition(int handle,int newPosition) {
    if (!VALID_AND_OPEN(handle)) return FIO_ERR_HANDLE;                             // Bad handle
    return FSYSGetSetPosition(handle,newPosition);
}


/**
 * @brief      Create a File
 *  *
 * @param      fileName  The file name
 *
 * @return     Handle or Error code if non-zero
 */
int FIOCreateFile(char *fileName) {
    return FSYSCreateFile(fileName);
}

/**
 * @brief      Create a directory
 *
 * @param      fileName  The name of the directory to create
 *
 * @return     Error code if non-zero
 */
int FIOCreateDirectory(char *fileName) {
    return FSYSCreateDirectory(fileName);
}

/**
 * @brief      Delete a file
 *
 * @param      fileName  The filename
 *
 * @return     Error code if non-zero
 */
int FIODeleteFile(char *fileName) {
    return FSYSDeleteFile(fileName);
}

/**
 * @brief      Delete Directory
 *
 * @param      fileName  The file name to directory
 *
 * @return     Error code if non-zero
 */
int FIODeleteDirectory(char *fileName) {
    return FSYSDeleteDirectory(fileName);
}

/**
 * @brief      Change to a new directory
 *
 * @param      fileName  The change (.. /dir dir etc)
 *
 * @return     Error code if non-zero
 */
int FIOChangeDirectory(char *fileName) {
    return FSYSChangeDirectory(fileName);
}

/**
 * @brief      Get information on a file
 *
 * @param      name  The name of the file
 * @param      info  The information structure
 *
 * @return     Error code if non-zero
 */
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


/**
 * @brief      Open a directory
 *
 * @param      directory  The directory to read
 *
 * @return     Error code if non-zero
 */
int FIOOpenDirectory(char *directory) {
    return FSYSOpenDirectory(directory);
}

/**
 * @brief      Read next directory 
 *
 * @param[in]  handle    The handle
 * @param      fileName  The file name read
 *
 * @return     Error code if non-zero
 */
int FIOReadDirectory(int handle,char *fileName) {
    return FSYSReadDirectory(fileName);
}

/**
 * @brief      Close a directory
 *
 * @param[in]  handle  The handle
 *
 * @return     Error code if non-zero
 */
int FIOCloseDirectory(int handle) {
    return FSYSCloseDirectory();
}
