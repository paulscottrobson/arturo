// ***************************************************************************************
// ***************************************************************************************
//
//		Name : 		soundsystem.h
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		3rd January 2025
//		Reviewed :	No
//		Purpose :	Multi channel sound system header
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once

typedef struct _sound_channel {
    uint8_t type;
    int  frequency;
    int  volume;
} SNDCHANNEL;

#define SNDTYPE_NOISE  	(0)
#define SNDTYPE_SQUARE  (1)

int SNDGetChannelCount(void);
void SNDMuteAllChannels(void);
int8_t SNDGetChannelSample(int channel);
void SNDUpdate(int channel,SNDCHANNEL *c);


