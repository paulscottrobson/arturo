/**
 * @file       soundsystem.c
 *
 * @brief      Header file, 4 channel 8 bit soundsystem
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

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


