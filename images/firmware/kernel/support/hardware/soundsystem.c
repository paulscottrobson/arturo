/**
 * @file       soundsystem.c
 *
 * @brief      Waveform generator, default sound system.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

#define CHANNEL_COUNT   (4)

struct _ChannelStatus {
    int limit;
    int wrapper;
    int state;
    int soundType;
    int volume;
    int samplePos;
} audio[CHANNEL_COUNT];



/**
 * @brief      Return number of supported Channels
 *
 * @return     Channels availbale
 */
int SNDGetChannelCount(void) {
    return CHANNEL_COUNT;
}


//
//                               
//


/**
 * @brief      Mute all channels
 */
void SNDMuteAllChannels(void) {
    for (int i = 0;i < CHANNEL_COUNT;i++) {
        struct _ChannelStatus *cs = &audio[i];
        cs->limit = cs->wrapper = cs->state = cs->soundType = cs->volume = 0;
    }
}


//
//              Get the next sample for the driver provided hardware rate.
//


/**
 * @brief      Get the next sound sample
 *
 * @param[in]  channel  dummy ?.
 *
 * @return     Sound level -128 .. 127
 */
int8_t SNDGetChannelSample(int channel) {

    static int activeCount = 0;

    int level = 0;                                                                  // Summative limit
    int channelsActive = activeCount;                                               // We have a very simple form of AGC, more than one channel scales volume
    activeCount = 0;                                                                // Reset the count

    for (int i = 0;i < CHANNEL_COUNT;i++) {                                         // Scan the channels
        struct _ChannelStatus *cs = &audio[i];
        if (cs->volume != 0) {                                                      // Channel on.
            activeCount++;                                                          // Bump active count
            if (cs->wrapper-- == 0) {                                               // Time to change the output level.
                cs->wrapper = cs->limit;                                            // Fix up the new limit.
                cs->state ^= 0xFF;
                switch (cs->soundType) {
                    case SNDTYPE_NOISE:
                        level += (rand() & 0xFF)-0x80;
                        break;
                    default:                                                        // Square wave
                        level += cs->state ? cs->volume : -cs->volume;break;
                }
            }
        }
    }
    if (channelsActive > 1) {                                                       // If >= 2 channels scale output by 75% to reduce clipping.
        level = level * 3 / 4;
    }
    if (level < -127) level = -127;                                                 // Clip into range
    if (level > 127) level = 127;
    return level;
}


//
//                                  Play note on channel
//


/**
 * @brief      Play a square wave on the channe l ?
 *
 * @param[in]  channel  The channel to change the note.
 * @param      c        Channel to update.
 */
void SNDUpdate(int channel,SNDCHANNEL *c) {
    if (channel >= CHANNEL_COUNT) return;
    if (c->frequency != 0) {
        audio[channel].limit = SNDGetSampleFrequency()/c->frequency/2;
        audio[channel].wrapper = 0;
        audio[channel].soundType = c->type;
        audio[channel].volume = c->volume;
        audio[channel].samplePos = 0;
    } else {
        audio[channel].volume = 0;
    }
}

