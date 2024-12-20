# Kernel

*This documentation is preliminary*

## Introduction

The kernel provides software support for the Olimex RP2040PC and (hopefully) the Olimex Neo6502 hardware to allow them to be used as ARM machines. It is hoped that RP2350PC devices will be supported too.

It provides raw hardware connection, and some external support routines.

It is planned to be the basic of my ARM/RISCV machine, but can also be used as a platform to turn the devices into (say) an emulator.

## Graphics

Currently only a single mode is provided, which is initialised at the start, this is an 8 colour 640x480 mode, which operates using bitplanes rather like an Amiga. The first bitplane is red, the second green, the third blue.  This occupies slightly under half the memory of the RP2040

Information on this can be obtained from the function *DVIGetModeInformation()* which returns a pointer to a *DVIModeInformation* structure.

The mode can be changed by *DVISetMode()* but at present there is only one mode supported.

### Notes

There is in development a 320x240x256 byte mode, which will be selectable by a *DVISetMode()* function but this is not currently working with PicoDVI, and I'm not sure why.

Other modes should be theoretically possible.

## File Storage

A USB key plugged into the board (RP2040PC) or external hub (Neo6502) is exposed as a FAT32 storage device. The keys ID is displayed during the bootup sequence.

The key is accessed via the fatfs library documented [here](http://elm-chan.org/fsw/ff/00index_e.html) , an example of usage can be found in the function *TESTShowUSBRootDirectory* in test_app.c which dumps the root directory.  The functions have close mapping to the standard C library equivalents.

Note that findfirst/next are not enabled, directory read should be done using open and close (as in the example)

It is not required to have a USB key, however this will slow the boot down. When the hardware starts, there is a delay loop which is waiting for the USB system to stabilise, which takes about a second. This will time out after a couple of seconds.

## Keyboard

The kernel supports a single USB keyboard. USB keyboards send USB packets to the hardware, these are intercepted and processed, and can be either handled in a raw sequence of key up/down data *or* can be processed for you, giving an easier standardised keyboard interface. This is covered later.

The 'processing' is on by default.

As with the Neo6502 pressing Ctrl + Alt + Alt Gr simultaneously resets the machine.

## Sound

An RP2040PC has two sound channels at present they are separated by default, and the test_app.c shows an example of the required driver function.

The concept is that a sampling function is called to get the next sample at a frequency of *SNDGetSampleFrequency()* , and you can figure out what you want the output level to be. In the example in test_app.c channel 1 (right) returns white noise, channel 0 (left) a square wave beep at 440Hz.

The name of the function that is called to get the sample is related to the one described in the "Main function" section below. If *ARTURO_RUN_FUNCTION* is set to TESTApplication then the function called to get the sample will be called TESTApplication_GetSample and should be declared and used as in test_app.c

The returned value should be -128 .. 127 and can be samples or similar. 

## Timer

A function *TMRRead()* returns the time since power up which clocks at 100Hz. This is not used in the sample code. 

This should be used as the basis for time centric code, e.g. sprite moving speeds.

## 50Hz Tick

In your main program, there should be a timer tick handler, this is the IF (HASTICK50_FIRED()) part of the test_app.c. This will fire at 50Hz, the speed is not guaranteed.

This should always call *USBUpdate()* which processes USB messages, and if you are using the full keyboard system *KBDCheckTimer()* which is responsible for auto repeating keys.  If you are not using the full keyboard system this function is a no-operation.

On the demo it also writes a full stop.

This is deliberately not done using interrupts as a way of avoiding any issues. I didn't write PicoDVI or tinyusb and it seems to me to be a good idea not to poke them.

## Application source

Your application code can go in user_code and user_code/include ; anything placed in user_code should be picked up and compiled.

This includes the file testing/test_app.c ; this should be removed by the linker.

## Configuration

Configuration is done using the **config.make** file, which allows you to change various settings.

### Main function

The primary function setting is the *ARTURO_RUN_FUNCTION* setting. This specifies the C function that is run to actually execute the code, and is currently set to "TestApplication" which causes the function *TestApplication()* to be called once initialisation is complete.

If you change this to run something else, then the TestApplication() and associated functions should be removed by the linker.

This function should not be returned. Code is running on core 0, core 1 is running the DVI code.

### Keyboard Handler

The standard keyboard manager can be disabled using *ARTURO_PROCESS_KEYS* which is 1 by default. Setting this to zero means that the developer will have to provide his or her own *USBKeyboardEvent()* handler which will be sent key transitions asynchronously.

### Mono Sound

The sound can be set to function as a single channel using *ARTURO_MONO_SOUND* which is 0 by default, when non-zero the RP2040PC stereo channels are combined into a single channel.

### Default Locale

The default locale can be set by changing *ARTURO_KBD_LOCALE*, which is "gb" by default. This can be changed at any time using the *LOCSetLocale()* function

## Console support

There is a very simple debug console which is accessed via *CONWrite()* which outputs a single character, and *CONWriteString()* which works like printf (e.g. has variable arguments). These are printed out on the DVI display in a simple fashion. Note that currently \r should be used for newline.

## Keyboard Support

Keyboard support converts the raw USB interface into an easily useable keyboard device. It supports locales, which can be extended, and providing *KBDCheckTimer()* is called, auto repeating keys.

It is also possible to access the keyboard current state allowing the keys to be used for gaming. The external functions are

| Function          | Purpose                                                      |
| ----------------- | ------------------------------------------------------------ |
| KBDGetStateArray  | Returns a bit array representing the state of the keyboard (e.g. one for each bit, indicating whether the key is currently down) |
| KBDGetModifiers   | Returns a byte showing the current state of the modifiers (Shift, Alt etc.) |
| KBDIsKeyAvailable | Returns true if there is a key in the keyboard queue.        |
| KBDGetKey         | Returns and removes the next key in the keyboard queue, returns 0 if the queue is empty. Control constants are in control_codes.h. This value should be standard ASCII. |
| KBDEscapePressed  | Returns true when the escape key has been pressed, optionally can reset it on reading this. |

## Graphics Support

tbc

## Mouse Support

Mouse support automatically converts Mouse information to a useable format. It is not possible at present to manually process the mouse USB messages.

| Function        | Purpose                                                      |
| --------------- | ------------------------------------------------------------ |
| MSEGetState     | Returns the position, button state, and scroll wheel position |
| MSEMousePresent | Returns true if the mouse is physically connected.           |
| MSESetPosition  | Sets the mouse position.                                     |

## Gamepad Support

Gamepad support converts the gamepad information to a simple process.  Up to four gamepads can be supported. USB messages cannot be processed directly. The following functions are used to access GamePad status.

| Function           | Purpose                                                      |
| ------------------ | ------------------------------------------------------------ |
| CTLControllerCount | Returns the number of controllers plugged in.                |
| CTLReadController  | Reads the state of a specific controller by number (0..n-1). Returns a CTLState pointer, which has members dx and dy (for directional control) and booleans a,b,x,y representing the buttons in the SNES arrangement. |

### Gamepad Drivers

There is a huge plethora of joystick designs and types. This is why support is limited to a directional pad and A,B,X,Y, which allows pretty much any post-Atari joystick to be used if it can be plugged in. 

In the USB system keyboards and mice are standard devices and they behave in the same way. They may have additional information, and layouts and buttons, but they can be handled with the same code and localisation of the keyboard.

Gamepads come in the 'other devices' class and have to be handled individually. It is recommended to use the Olimex SNES style PC joypads, (081F:E401) https://www.olimex.com/Products/Retro-Computers/USB-GAMEPAD/ but if you have another device it can be added, and some Chinese SNES pad clones from Amazon may work, or may be made to work.

Similar types of device with different VID:PID values may work if those values are simply added to the dispatcher in gamepad_drivers.c ; but anything else will require a new driver.

This involves adding a new section to gamepad_drivers like this:

		case CTL_DEVICE_TYPE_ID(0x081F,0xE401):
			retVal = CTLDriverSNESType(command,cs,msg);
			break;

with a new driver function (e.g. CTLDriverXBoxUSBPad say) and one or more VID/PID pairings in the case line. The declaration for the driver goes in gamepad_drivers.h with the same signature as CTLDriverSNESType. 

VID/PID of unknown devices are displayed at boot time, they may be something else, so you can check against online lists, in Linux you can use *lsusb* to determine what they are, it's probably possible in Windows in one of the thirteen variations of the Control Panel that's been there since Windows ran on an x86.

The driver itself can go in a file in the hardware/gamepad_drivers subdirectory (which cmake will pick up automatically). The sample one (*snes_usb.c*) is well commented to explain what is going on. It has to respond to two messages. CTLM_REGISTER simply announces its presence and type.

CTLM_UPDATE handles the contents of the received USB report and updates dx,dy,a,b,x and y in the provided CTLState structure. This is (in this example) done with the bits from the report.

The six million dollar question is, what are those bits ? Most of this you have to discover experimentally. In *CTLUpdateController* in *gamepad.c* there is an if (false) command, which will look something like this:

	if (false) {  																	
		CONWriteString("%d : ",len);
		for (int i = 0;i < len;i++) CONWriteString("%d:%02x ",i,report[i]);
		CONWriteString("\r");
	}

If this is set to true, then reports from unknown devices are dumped on the console. From this it should be possible to figure out which bits in the report correspond to which buttons. 

## Sound Support

tbc

------

*Paul Robson paul@robsons.org.uk*

*Last revised 21st December 2024*

## I