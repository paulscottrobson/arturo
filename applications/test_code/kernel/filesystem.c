// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      filesystem.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      27th December 2024
//      Reviewed :  No
//      Purpose :   Check File System
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include <libraries.h>
#include "testapp.h"

// ***************************************************************************************
//
//                          This tests the USB key is working.
//
// ***************************************************************************************

void DemoApp_CheckFileIO(void) {
    FIOInfo fi;int e;char *s;
    char buffer[42];

    CONWriteString("\rFSYSFileInformation\r");
    s = "inline.bas";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
    s = "inline.xxx";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
    s = "aadir";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d ",e,s,fi.length,fi.isDirectory);
    s = "abdir";e = FSYSFileInformation(s,&fi);CONWriteString("(%d) %s %d %d\r",e,s,fi.length,fi.isDirectory);

    CONWriteString("\rFSYSCreateFile/FSYSDeleteFile\r");
    e = FSYSCreateFile("newfile.1");  CONWriteString("Create %d ",e);
    e = FSYSCreateFile("newfile.2");  CONWriteString("Create %d ",e);
    e = FSYSDeleteFile("newfile.1");  CONWriteString("Delete %d ",e);
    e = FSYSDeleteFile("newfile.3");  CONWriteString("Delete %d\r",e);

    CONWriteString("\rFSYSCreateDirectory/FSYSDeleteDirectory\r");
    e = FSYSCreateDirectory("newDirectory.1");  CONWriteString("Create %d ",e);
    e = FSYSCreateDirectory("newDirectory.2");  CONWriteString("Create %d ",e);
    e = FSYSDeleteDirectory("newDirectory.1");  CONWriteString("Delete %d ",e);
    e = FSYSDeleteDirectory("newDirectory.3");  CONWriteString("Delete %d\r",e);

    CONWriteString("\rFile R/W\r");
    e = FSYSCreateFile("test.file");CONWriteString(" %d",e);
    e = FSYSOpen(0,"test.file");CONWriteString(" %d",e);
    e = FSYSWrite(0,"Hello world",5);CONWriteString(" %d",e);
    e = FSYSClose(0);CONWriteString(" %d\n",e);

    strcpy(buffer,"_________");
    e = FSYSOpen(0,"test.file");CONWriteString(" %d",e);
    e = FSYSRead(0,buffer,2);CONWriteString(" %d %s",e,buffer);    
    e = FSYSRead(0,buffer+3,3);CONWriteString(" %d %s",e,buffer);
    e = FSYSGetSetPosition(0,1);CONWriteString(" %d",e);
    e = FSYSRead(0,buffer+7,1);CONWriteString(" %d %s",e,buffer);
    e = FSYSClose(0);CONWriteString(" %d\n",e);

    CONWriteString("\rFSYSOpenDirectory\r");
    e = FSYSOpenDirectory("/");CONWriteString(" %d",e);
    while (e = FSYSReadDirectory(buffer),e == 0) CONWriteString(" %s",buffer);
    CONWriteString(" %d",e);
    e = FSYSCloseDirectory();CONWriteString(" %d",e);
}
