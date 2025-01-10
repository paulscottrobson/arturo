/**
 * @file       fileio,c
 *
 * @brief      FSYS level file interface
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */



#include "artsim.h"
#include "sys/stat.h"
#include "dirent.h"


//
//                              Current open files.
//
static FILE *files[FIO_MAX_HANDLES];



/**
 * @brief      Reset all file objects etc.
 */
void FSYSInitialise(void) {
}
/**
 * @brief      Map the internal error value onto FIO type error
 *
 * @return     FIO Error type or zero.
 */
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


/**
 * @brief      Convert a file name to a local system file name
 *
 * @param      name  The name of the file on the virtual file system
 *
 * @return     Filename on the real file system.
 */
static char *_FSYSMapName(char *name) {
    static char buffer[256];
    mkdir("storage",0777);                                                          // Creates the working directory
    strcpy(buffer,"storage/");strcat(buffer,name);                                  // Convert filename to working file name.
    return(buffer);
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

    DIR *f = opendir(_FSYSMapName(name));                                       // Directory check
    if (f != NULL) {
        closedir(f);                                                            // Found, close it
        if (info != NULL) {                                                     // Fill in if required.
            info->length = 0;info->isDirectory = -1; 
        }
        return FIO_OK;              
    }
    struct stat ffi;
    if (stat(_FSYSMapName(name),&ffi) != 0) return _FSYSMapError();             // Try for file, Stat failed
    if (info != NULL) {
        info->isDirectory = false;
        info->length = info->isDirectory ? 0 : ffi.st_size;                     // Length if file, 0 for directory.
    }
    return FIO_OK;
}

/**
 * @brief      Create a new file, deleting any currently existing file
 *
 * @param      name  The name of the file
 *
 * @return     Error code if nonzero
 */
int FSYSCreateFile(char *name) {
    FILE *f = fopen(_FSYSMapName(name),"w");                                    // Creates an empty file.
    if (f == NULL) return _FSYSMapError();                                      // Create filed.
    fclose(f);                                                                  // Close the new file.
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
    if (unlink(_FSYSMapName(name)) == 0) return FIO_OK;                         // Delete worked okay.
    if (errno == ENOENT) return FIO_OK;                                         // Didn't exist, that's okay.
    return _FSYSMapError(); 
}

/**
 * @brief      Create a new directory if it does not already exist
 *
 * @param      name  The name of the directory
 *
 * @return     Error code if nonzero
 */
int FSYSCreateDirectory(char *name) {                                           
    if (mkdir(_FSYSMapName(name),0777) == 0) return FIO_OK;                     // Create directory okay
    if (errno == EEXIST) return FIO_OK;                                         // It already exists, not an error
    return _FSYSMapError();
}

/**
 * @brief      Delete directory, ignoring if it does not exist/not empty
 *
 * @param      name  The name of the directory
 *
 * @return     Error code if non-zero
 */
int FSYSDeleteDirectory(char *name) {
    if (rmdir(_FSYSMapName(name)) == 0) return FIO_OK;                              // Removal okay
    if (errno == ENOENT || errno == EEXIST || 
                        errno == ENOTEMPTY || errno == ENOTDIR) return FIO_OK;      // Not present, not empty etc.
    return _FSYSMapError();
}                                           

/**
 * @brief      Change to a new directory
 *
 * @param      directory  The directory to change to
 *
 * @return     Error code if non-zero
 */
int FSYSChangeDirectory(char *directory) {
    if (chdir(_FSYSMapName(directory)) == 0) return FIO_OK;
    return _FSYSMapError();
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
    files[handle] = fopen(_FSYSMapName(name),"r+");                                 // Open read/write
    if (files[handle] == NULL) return _FSYSMapError();                              // Open failed.
    if (fseek(files[handle],0,SEEK_SET) < 0) return _FSYSMapError();                // Rewind it.
    return FIO_OK;
};

/**
 * @brief      Close the file
 *
 * @param[in]  handle  The handle of the file
 *
 * @return     Error code if non-zero
 */
int FSYSClose(int handle) {
    if (fclose(files[handle]) < 0) return _FSYSMapError();                          // Close file.
    return FIO_OK;
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
    int bytesRead = fread(data,1,size,files[handle]);                               // Attempt to read data from file.
    if (bytesRead < 0) return _FSYSMapError();                                      // Read failed.
    return bytesRead;
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
    int bytesWritten = fwrite(data,1,size,files[handle]);                           // Attempt to write.
    if (bytesWritten < 0) return _FSYSMapError();                                   // Error occurred
    if (bytesWritten != size) return FIO_ERR_SYSTEM;                                // For some reason, did not write.
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
    return feof(files[handle]) ? FIO_EOF : FIO_OK;                                  // Seems not to be able to error :)
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
    int current = ftell(files[handle]);                                             // Where are we now ?
    if (current < 0) return _FSYSMapError();                                        // Failed.
    if (newPosition >= 0) {                                                         // Moving ?
        if (fseek(files[handle],newPosition,SEEK_SET) < 0) return _FSYSMapError();  // Move and check.
    }
    return current;
}   

static DIR* currentDirectory;

/**
 * @brief      Opens directory to be read
 *
 * @param      directory  The directory to be read
 *
 * @return     Error code if non-zero
 */
int FSYSOpenDirectory(char *directory) {
    directory = _FSYSMapName(directory);                                            // Map directory
    currentDirectory = opendir(directory);                                          // Open directory.
    return (currentDirectory == NULL) ? _FSYSMapError() : FIO_OK;                   // Return OK, or error.
}

/**
 * @brief      Read next directory entry
 *
 * @param      fileName  The file name read from the directory
 *
 * @return     Error code if non-zero
 */
int FSYSReadDirectory(char *fileName) {
    struct dirent *next = readdir(currentDirectory);                                // Read next entry.
    if (next == NULL) {                                                             // Read failed.
        if (errno == 0) return FIO_EOF;                                             // End of directory
        return _FSYSMapError();                                                     // All other errors.
    }
    next->d_name[MAX_FILENAME_SIZE] = '\0';                                         // Truncate file name.
    strcpy(fileName,next->d_name);
    return FIO_OK;
}   

/**
 * @brief      Close directory being read
 *
 * @return     Error code if non-zero
 */
int FSYSCloseDirectory(void) {
    return closedir(currentDirectory) < 0 ? _FSYSMapError() : FIO_OK;
}
