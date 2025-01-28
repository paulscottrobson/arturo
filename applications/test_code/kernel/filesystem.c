/**
 * @file       filesystem.c
 *
 * @brief      Check File System functions work
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "testapp.h"

/**
 * @brief      Check file system functionality
 */
void DemoApp_CheckFileIO(void) {
	FIOInfo fi;int e;char *s;
	char buffer[42];

	//
	//      Get some stuff about files
	//
	CONWriteString("\r\nFSYSFileInformation\r\n");
	s = "inline.bas";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
	s = "inline.xxx";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
	s = "aadir";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
	s = "abdir";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d\r\n",e,s,fi.length,fi.isDirectory);

	//
	//      Create files and delete some
	//
	CONWriteString("\r\nFSYSCreateFile/FSYSDeleteFile\r\n");
	e = FSYSCreateFile("newfile.1");  CONWriteString("Create %d ",e);
	e = FSYSCreateFile("newfile.2");  CONWriteString("Create %d ",e);
	e = FSYSDeleteFile("newfile.1");  CONWriteString("Delete %d ",e);
	e = FSYSDeleteFile("newfile.3");  CONWriteString("Delete %d\r\n",e);

	//
	//      Create directories and delete some
	//
	CONWriteString("\r\nFSYSCreateDirectory/FSYSDeleteDirectory\r\n");
	e = FSYSCreateDirectory("newDirectory.1");  CONWriteString("Create %d ",e);
	e = FSYSCreateDirectory("newDirectory.2");  CONWriteString("Create %d ",e);
	e = FSYSDeleteDirectory("newDirectory.1");  CONWriteString("Delete %d ",e);
	e = FSYSDeleteDirectory("newDirectory.3");  CONWriteString("Delete %d\r\n",e);

	//
	//         File I/O, write hello world to a test file.
	//
	CONWriteString("\r\nFile R/W\r\n");
	e = FSYSCreateFile("test.file");CONWriteString(" %d",e);
	e = FSYSOpen(0,"test.file");CONWriteString(" %d",e);
	e = FSYSWrite(0,"Hello world",5);CONWriteString(" %d",e);
	e = FSYSClose(0);CONWriteString(" %d\n",e);

	//
	//          Read bits into a buffer, also checks positioning and close
	//
	strcpy(buffer,"_________");
	e = FSYSOpen(0,"test.file");CONWriteString(" %d",e);
	e = FSYSRead(0,buffer,2);CONWriteString(" %d %s",e,buffer);    
	e = FSYSRead(0,buffer+3,3);CONWriteString(" %d %s",e,buffer);
	e = FSYSGetSetPosition(0,1);CONWriteString(" %d",e);
	e = FSYSRead(0,buffer+7,1);CONWriteString(" %d %s",e,buffer);
	e = FSYSClose(0);CONWriteString(" %d\n",e);

	//
	//          Directory listing.
	//
	CONWriteString("\r\nFSYSOpenDirectory\r\n");
	e = FSYSOpenDirectory("/");CONWriteString(" %d",e);
	while (e = FSYSReadDirectory(buffer),e == 0) CONWriteString(" %s",buffer);
	CONWriteString(" %d",e);
	e = FSYSCloseDirectory();CONWriteString(" %d",e);
}
