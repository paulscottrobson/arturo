// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      superboard.h
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      6th January 2025
//      Reviewed :  No
//      Purpose :   Superboard header
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once

uint8_t SuperRead(uint16_t a);
void SuperWrite(uint16_t a,uint8_t d);
uint8_t ReadKeyboard(uint8_t keyboardPort);
void ScreenPaint(uint16_t addr,uint8_t c);
