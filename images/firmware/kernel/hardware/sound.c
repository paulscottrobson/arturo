/**
 * @file       sound.c
 *
 * @brief      PWM Audio support for Neo6502
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "dvi.h"

static int sampleFrequency = -1;
static bool combineSoundChannels = false;

/**
 * @brief      Returns the sample rate of the underlying hardware
 *
 * @return     Frequency in Khz
 */
int SNDGetSampleFrequency(void) {
    if (sampleFrequency < 0) {                                                      // Only do this once.
        sampleFrequency = DVI_TIMING.bit_clk_khz * 1024 / SAMPLE_DIVIDER / 255;     // the 255 is the wrap interrupt count.
    }
    return sampleFrequency;
}

/**
 * @brief      PWM Interrupt handler
 */
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN_L));                              // Acknowledge interrupt
    #if ARTURO_PROCESS_SOUND==1
    uint8_t sample0 = SNDGetChannelSample(0)+128;                                   // Get sample 1, copy to Left Pin
    #else
    uint8_t sample0 = ApplicationGetChannelSample(0)+128;                           // Get sample 1, copy to Left Pin
    #endif

    pwm_set_gpio_level(AUDIO_PIN_L,sample0);
    if (AUDIO_HARDWARE_CHANNELS == 2) {                                             // If there are 2 channels.
        uint8_t sample1 = sample0;
        if (!combineSoundChannels) {                                                // Reuse sample 1 if combined, otherwise get sample 2
            #if ARTURO_PROCESS_SOUND==1
            uint8_t sample1 = SNDGetChannelSample(0)+128;                           // Get sample 1, copy to Left Pin
            #else
            uint8_t sample1 = ApplicationGetChannelSample(0)+128;                   // Get sample 1, copy to Left Pin
            #endif
        }
        pwm_set_gpio_level(AUDIO_PIN_R,sample1);
    }
}


/**
 * @brief      Initialise a Pico PWM channel
 *
 * @param[in]  pin              The pin to use
 * @param[in]  enableInterrupt  Enable interrupts on this pin ?
 */
static void _SND_Initialise_Channel(int pin,bool enableInterrupt) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    int pin_slice = pwm_gpio_to_slice_num(pin);
    if (enableInterrupt) {                                                          // Only enable interrupt on 1 channel.
        // Setup PWM interrupt to fire when PWM cycle is complete
        pwm_clear_irq(pin_slice);
        // set the handle function above
        irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
        irq_set_enabled(PWM_IRQ_WRAP, true);
        pwm_set_irq_enabled(pin_slice, true);
    }
    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, SAMPLE_DIVIDER);
    pwm_config_set_wrap(&config, 255);
    pwm_init(pin_slice, &config, true);
    //
    pwm_set_gpio_level(pin, 0);
}

/**
 * @brief      Initialise the whole sound system
 *
 * @param[in]  _combineChannels  Play 2 channels as one.
 */
void SNDInitialise(bool _combineChannels) {
    combineSoundChannels = _combineChannels;                                        // Record if we are supposed to add them.
    _SND_Initialise_Channel(AUDIO_PIN_L,true);                                      // Initialise 1 or 2 channels.
    if (AUDIO_HARDWARE_CHANNELS == 2) {
        _SND_Initialise_Channel(AUDIO_PIN_R,false);
    }
}

