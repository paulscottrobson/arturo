/**
 * @file       fileio.c
 *
 * @brief      File I/O (FSYS Level interface)
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "ff.h"

//
//                              Current open files.
//
static FIL files[FIO_MAX_HANDLES];

/**
 * @brief      Map FatFS errors onto our FSYS Error types
 *
 * @param[in]  r     FatFS error
 *
 * @return     FIO Error Code
 */
static int _FSYSMapError(FRESULT r) {
    int err;

    switch(r) {
        case FR_OK:                                                             // No error.
            err = FIO_OK;break;
        case FR_NO_FILE:                                                        // File/Path not found
        case FR_NO_PATH:
            err = FIO_ERR_NOTFOUND;break;
        case FR_INVALID_NAME:                                                   // Input is wrong.
        case FR_INVALID_OBJECT:
        case FR_INVALID_PARAMETER:
            err = FIO_ERR_COMMAND;break;
        default:                                                                // Everything else.
            err = FIO_ERR_SYSTEM;break;
            break;
    }
    return err;
}

/**
 * @brief      Reset all file objects etc.
 */
void FSYSInitialise(void) {
}


/**
 * @brief      Get file information. Fills structure if provided
 *
 * @param      name  The name of the file
 * @param      info  The structure containing the information
 *
 * @return     Error code if non zero
 */
int FSYSFileInformation(char *name,FIOInfo *info) {
    FILINFO ffi;
    FRESULT r = f_stat(name,&ffi);                                                  // Read the information
    if (r == FR_OK && info != NULL) {                                               // Information received.
        info->isDirectory = (ffi.fattrib & AM_DIR) != 0;                            // Directory flag
        info->length = info->isDirectory ? 0 : ffi.fsize;                           // Length if file, 0 for directory.
    }
    return _FSYSMapError(r);
}

/**
 * @brief      Create a new file, deleting any currently existing file
 *
 * @param      name  The name of the file
 *
 * @return     Error code if nonzero
 */
int FSYSCreateFile(char *name) {
    FIL fRec;
    FRESULT fr = f_open(&fRec,name,FA_CREATE_ALWAYS|FA_WRITE);                  // Try to create a file
    if (fr != FR_OK) return _FSYSMapError(fr);                                  // Creation failed.
    f_close(&fRec);                                                             // Close it.
    return FIO_OK;
}


/**
 * @brief      Delete a file
 *
 * @param      name  The name of the file
 *
 * @return     Error code if non zero
 */
int FSYSDeleteFile(char *name) {
    FRESULT fr = f_unlink(name);
    if (fr == FR_OK) return FIO_OK;                                             // Delete worked okay.
    if (fr == FR_NO_FILE || fr == FR_NO_PATH) return FIO_OK;                    // Does not exist
    return _FSYSMapError(fr);
}


/**
 * @brief      Create a new directory if it does not already exist
 *
 * @param      name  The name of the directory
 *
 * @return     Error code if nonzero
 */
int FSYSCreateDirectory(char *name) {
    FRESULT fr = f_mkdir(name);
    if (fr == FR_OK) return FIO_OK;                                             // Create worked okay.
    if (fr == FR_EXIST) return FIO_OK;                                          // It already exists, not an error
    return _FSYSMapError(fr);
}


/**
 * @brief      Change to a new directory
 *
 * @param      directory  The directory to change to
 *
 * @return     Error code if non-zero
 */
int FSYSChangeDirectory(char *directory) {
    FRESULT fr = f_chdir(directory);
    return _FSYSMapError(fr);
}

/**
 * @brief      Delete directory, ignoring if it does not exist/not empty
 *
 * @param      name  The name of the directory
 *
 * @return     Error code if non-zero
 */
int FSYSDeleteDirectory(char *name) {
    return FSYSDeleteFile(name);                                                // Same as delete file :)
}

/**
 * @brief      Open File in R/W mode, rewind to start
 *
 * @param[in]  handle  The handle to use
 * @param      name    The name of the file.
 *
 * @return     Error code if non-zero
 */
int FSYSOpen(int handle,char *name) {
    FRESULT fr = f_open(&files[handle],name,FA_READ|FA_WRITE|FA_OPEN_EXISTING);     // Open read/write
    if (fr != FR_OK) return _FSYSMapError(fr);                                      // Error occurred of some sort.
    fr = f_lseek(&files[handle],0);                                                 // Rewind
    return _FSYSMapError(fr);
};

/**
 * @brief      Close the file
 *
 * @param[in]  handle  The handle of the file
 *
 * @return     Error code if non-zero
 */
int FSYSClose(int handle) {
    return _FSYSMapError(f_close(&files[handle]));                                  // Close file.
}


/**
 * @brief      Read data from the file. Returns error or # of bytes read.
 *
 * @param[in]  handle  The handle of the file
 * @param      data    The location of the data to be read to
 * @param[in]  size    The size in bytes
 *
 * @return     +ve bytes read, or Error code if non-zero
 */
int FSYSRead(int handle,void *data,int size) {
    UINT count;
    FRESULT fr = f_read(&files[handle],data,size,&count);                           // Attempt to read data from file.
    if (fr != FR_OK) return _FSYSMapError(fr);                                      // Error occurred of some sort.
    return (int)count;                                                              // Bytes read.
}


/**
 * @brief      Write data to the file.
 *
 * @param[in]  handle  The handle of the file
 * @param      data    The data to write
 * @param[in]  size    The size in bytes
 *
 * @return     Error code if non-zero
 */
int FSYSWrite(int handle,void *data,int size) {
    UINT count;
    FRESULT fr = f_write(&files[handle],data,size,&count);                          // Attempt to write data to file.
    if (fr != FR_OK) return _FSYSMapError(fr);                                      // Error occurred of some sort.
    if (count != size) return FIO_ERR_SYSTEM;                                       // Write did not complete.
    return FIO_OK;                                                                  // It's okay.
}


/**
 * @brief      Check end of file
 *
 * @param[in]  handle  The handle to check
 *
 * @return     0 if more data, +ve if EOF or Error code if non-zero
 */
int FSYSEndOfFile(int handle) {
    return f_eof(&files[handle]) ? FIO_EOF : FIO_OK;                                // Seems not to be able to error :)
}


/**
 * @brief      Gets and/or sets position in file.
 *
 * @param[in]  handle       The handle
 * @param[in]  newPosition  The new position in the file if >= 0
 *
 * @return     the position in the file at the start.
 */
int FSYSGetSetPosition(int handle,int newPosition) {
    int current = f_tell(&files[handle]);                                           // Where are we now (cannot error in fatfs)
    if (newPosition >= 0) {                                                         // Moving ?
        FRESULT fr = f_lseek(&files[handle],newPosition);                           // Move it and check
        if (fr != FR_OK) return _FSYSMapError(fr);
    }
    return current;
}

static DIR currentDirectory;

/**
 * @brief      Opens directory to be read
 *
 * @param      directory  The directory to be read
 *
 * @return     Error code if non-zero
 */
int FSYSOpenDirectory(char *directory) {
    return _FSYSMapError(f_opendir(&currentDirectory,directory));                   // Open directory
}


/**
 * @brief      Read next directory entry
 *
 * @param      fileName  The file name read from the directory
 *
 * @return     Error code if non-zero
 */
int FSYSReadDirectory(char *fileName) {
    FILINFO fi;
    FRESULT fr = f_readdir(&currentDirectory,&fi);                                  // Read next
    if (fr != FR_OK) return _FSYSMapError(fr);                                      // Error
    if (fi.fname[0] == '\0') return FIO_EOF;                                        // End of directory data.
    fi.fname[MAX_FILENAME_SIZE] = '\0';                                             // Truncate file name.
    strcpy(fileName,fi.fname);
    return FIO_OK;
}


/**
 * @brief      Close directory being read
 *
 * @return     Error code if non-zero
 */
int FSYSCloseDirectory(void) {
    return _FSYSMapError(f_closedir(&currentDirectory));
}
