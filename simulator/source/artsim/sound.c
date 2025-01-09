/**
 * @file 
 *
 * @brief      
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


//
//		Name:		sound.cpp
//		Purpose:	SoundSupport library for SDL.
//		Created:	25th December 2024.
//		Author:		qxxxb (https://github.com/qxxxb/sdl2-beeper)
//					Paul Robson (paul@robsons.org.uk)
//



#include <artsim.h>

static SDL_AudioDeviceID audioDevice;
static SDL_AudioSpec audioSpec;

void (*m_writeData)(uint8_t* ptr, double data);
int (*m_calculateOffset)(int sample, int channel);


// 
// 		Calculate the offset in bytes from the start of the audio stream to the
// 		memory address at `sample` and `channel`.
//
// 		Channels are interleaved.
//


static int calculateOffset_s16(int sample, int channel) {
	return
		(sample * sizeof(int16_t) * audioSpec.channels) +
		(channel * sizeof(int16_t));
}

static int calculateOffset_f32(int sample, int channel) {
	return
		(sample * sizeof(float) * audioSpec.channels) +
		(channel * sizeof(float));
}


//
// 		Convert a normalized data value (range: 0.0 .. 1.0) to a data value matching
// 		the audio format.
//


static void writeData_s16(uint8_t* ptr, double data) {
	int16_t* ptrTyped = (int16_t*)ptr;
	double range = (double)INT16_MAX - (double)INT16_MIN;
	double dataScaled = data * range / 2.0;
	*ptrTyped = dataScaled;
}

static void writeData_f32(uint8_t* ptr, double data) {
	float* ptrTyped = (float*)ptr;
	*ptrTyped = data;
}


//
//						Callback when requesting buffer be filled
//


static void audioCallback(void* userdata,uint8_t* stream,int len) {
	// Unused parameters
	(void)userdata;
	(void)len;

	// Write data to the entire buffer by iterating through all samples and
	// channels.
	for (int sample = 0; sample < audioSpec.samples; ++sample) {
		#if ARTURO_PROCESS_SOUND==1
		double data = ((double)SNDGetChannelSample(0))/128.0;
		#else		
		double data = ((double)ApplicationGetChannelSample(0))/128.0;
		#endif
		// Write the same data to all channels
		for (int channel = 0; channel < audioSpec.channels; ++channel) {
			int offset = m_calculateOffset(sample, channel);
			uint8_t* ptrData = stream + offset;
			m_writeData(ptrData, data);
		}
	}
}


//
//								Open a sound device
//


void SOUNDOpen(void) {
	char *formatName = "<unknown>";

	// First define the specifications we want for the audio device
	SDL_AudioSpec desiredSpec;
	SDL_zero(desiredSpec);

	// Commonly used sampling frequency
	desiredSpec.freq = 44100;

	// Currently this program supports two audio formats:
	// - AUDIO_S16: 16 bits per sample
	// - AUDIO_F32: 32 bits per sample
	//
	// We need this flexibility because some sound cards do not support some
	// formats.

	// Higher bit depth means higher resolution the sound, lower bit depth
	// means lower resolution for the sound. Since we are just playing a simple
	// sine wave, 16 bits is fine.
	desiredSpec.format = AUDIO_S16;

	// Smaller buffer means less latency with the sound card, but higher CPU
	// usage. Bigger buffers means more latency with the sound card, but lower
	// CPU usage. 512 is fairly small, since I don't want a delay before a beep
	// is played.
	desiredSpec.samples = 512;

	// Since we are just playing a simple sine wave, we only need one channel.
	desiredSpec.channels = 1;

	// Set the callback (pointer to a function) to our callback. This function
	// will be called by SDL2 in a separate thread when it needs to write data
	// to the audio buffer. In other words, we don't control when this function
	// is called; SDL2 manages it.
	desiredSpec.callback = audioCallback;

	// When we open the audio device, we tell SDL2 what audio specifications we
	// desire. SDL2 will try to get these specifications when opening the audio
	// device, but sometimes the audio device does not support some of our
	// desired specifications. In that case, we have to be flexible and adapt
	// to what the audio device supports. The obtained specifications that the
	// audio device supports will be stored in `audioSpec`

	audioDevice = SDL_OpenAudioDevice(
		NULL, // device (name of the device, which we don't care about)
		0, // iscapture (we are not recording sound)
		&desiredSpec, // desired
		&audioSpec, // obtained
		0 // allowed_changes (allow any changes between desired and obtained)
	);

	if (audioDevice == 0) {
		SDL_Log("Failed to open audio: %s", SDL_GetError());
		// TODO: throw exception
	} else {
		switch (audioSpec.format) {
			case AUDIO_S16:
				m_writeData = writeData_s16;
				m_calculateOffset = calculateOffset_s16;
				formatName = "AUDIO_S16";
				break;
			case AUDIO_F32:
				m_writeData = writeData_f32;
				m_calculateOffset = calculateOffset_f32;
				formatName = "AUDIO_F32";
				break;
			default:
				SDL_Log("Unsupported audio format: %i", audioSpec.format);
				// TODO: throw exception
		}
		printf("Format %s %dHz\n",formatName,audioSpec.freq);

		// std::cout << "[Beeper] frequency: " << audioSpec.freq << std::endl;
		// std::cout << "[Beeper] format: " << formatName << std::endl;

		// std::cout
		// 	<< "[Beeper] channels: "
		// 	<< (int)(audioSpec.channels)
		// 	<< std::endl;

		// std::cout << "[Beeper] samples: " << audioSpec.samples << std::endl;
		// std::cout << "[Beeper] padding: " << audioSpec.padding << std::endl;
		// std::cout << "[Beeper] size: " << audioSpec.size << std::endl;
	}
}


//
//								End the Sound system
//


void SOUNDClose(void) {
	SDL_CloseAudioDevice(audioDevice);
}


//
//								Start playing sound
//


void SOUNDPlay(void) {
	SDL_PauseAudioDevice(audioDevice, 0);
}


//
//								Stop playing sound
//


void SOUNDStop(void) {
	SDL_PauseAudioDevice(audioDevice, 1);
}


//
//      Function that returns the sample rate in Hz of the implementeing hardware
//


int SNDGetSampleFrequency(void) {
    return audioSpec.freq;
}
