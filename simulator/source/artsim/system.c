/**
 * @file       system.c
 *
 * @brief      Open SDL, Close SDL, Process polling.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include <artsim.h>

static SDL_Window *mainWindow = NULL;
static SDL_Surface *mainSurface = NULL;

static int startTime = 0,endTime = 0,frameCount = 0;

static void SYSUpdateMouse(void);

#define RED(x) ((((x) >> 8) & 0xF) * 17)
#define GREEN(x) ((((x) >> 4) & 0xF) * 17)
#define BLUE(x) ((((x) >> 0) & 0xF) * 17)

/**
 * @brief      Get elapsed time since start
 *
 * @return     time in 1khz ticks
 */
int TMRReadTimeMS(void) {
    return SDL_GetTicks();
}

/**
 * @brief      Open the main window and start everything off
 *
 * @param[in]  muteSound  Mute sound on simulator
 *
 */
void SYSOpen(bool muteSound) {

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_GAMECONTROLLER) < 0)    {   // Try to initialise SDL Video and Audio
        exit(printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError()));
    }
    mainWindow = SDL_CreateWindow("Artsim", SDL_WINDOWPOS_UNDEFINED,                // Try to create a window
                            SDL_WINDOWPOS_UNDEFINED, 640*AS_SCALE+16,480*AS_SCALE+16, SDL_WINDOW_SHOWN );
    if (mainWindow == NULL) {
        exit(printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ));
    }

    mainSurface = SDL_GetWindowSurface(mainWindow);                                 // Get a surface to draw on.

    CTLFindControllers();                                                           // Have to be done after SDL Initialisation.
    MSEInitialise();
    SOUNDOpen();
    SNDMuteAllChannels();                                                           // Mute all channels
    if (!muteSound) SOUNDPlay();

    SDL_ShowCursor(SDL_DISABLE);                                                    // Hide mouse cursor
    startTime = TMRReadTimeMS();
}


static int isRunning = -1;                                                          // Is app running

/**
 * @brief      Check the SDL2 message queue, update mouse and keyboard, update display.
 *
 * @return     { description_of_the_return_value }
 */
int SYSPollUpdate(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {                                                 // While events in event queue.
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {     // Exit if ESC/Ctrl+ESC pressed tbc,
            int ctrl = ((SDL_GetModState() & KMOD_LCTRL) != 0);                     // If control pressed
            if (ctrl == 0) isRunning = 0;                                           // Exit
        }
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {                 // Handle other keys, which may go up or down.
            KBDProcessEvent(event.key.keysym.scancode,SDL_GetModState(),event.type == SDL_KEYDOWN);
        }
        if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN      // Mouse button/position update
                                                        || event.type == SDL_MOUSEBUTTONUP) {
            MSEEnableMouse();
            SYSUpdateMouse();
        }
        if (event.type == SDL_MOUSEWHEEL) {                                         // Handle scroll wheel events.
            int dy = event.wheel.y;
            if (event.wheel.type == SDL_MOUSEWHEEL_FLIPPED) dy = -dy;
            MSEUpdateScrollWheel(dy);
        }
        if (event.type == SDL_QUIT) {                                               // Exit on Alt+F4 etc.
            isRunning = 0;
        }
    }
    frameCount++;
    RNDRender(mainSurface);
    SDL_UpdateWindowSurface(mainWindow);                                            // And update the main window.  
    return isRunning;
}

/**
 * @brief      Close down everything
 */
void SYSClose(void) {
    endTime = TMRReadTimeMS();
    SDL_DestroyWindow(mainWindow);                                                  // Destroy working window
    SOUNDStop();
    SDL_CloseAudio();                                                               // Shut audio up.
    SDL_Quit();                                                                     // Exit SDL.
    printf("Frame Rate %.2f\n",frameCount/((endTime-startTime)/1000.0));
}


/**
 * @brief      Draw arectangle on the working surface
 *
 * @param      rc      Rectangle dimensions
 * @param[in]  colour  Fill Colour
 */
void SYSRectangle(SDL_Rect *rc,int colour) {
    SDL_FillRect(mainSurface,rc,SDL_MapRGB(mainSurface->format,RED(colour),GREEN(colour),BLUE(colour)));
}


/**
 * @brief      Convert mouse data to correct format and update mouse system
 */
static void SYSUpdateMouse(void) {
    int x,y;
    SDL_Rect r;
    int xScale,yScale,xWidth,yWidth;
    struct DVIModeInformation *dm = DVIGetModeInformation();
    Uint32 sbut = SDL_GetMouseState(&x,&y);
    r.x = 0;r.y = 0;r.w = 640*AS_SCALE;r.h = 480*AS_SCALE;
    xScale = AS_SCALE * 640 / dm->width;
    yScale = AS_SCALE * 480 / dm->height;
    if (x >= r.x && y >= r.y && x < r.x+r.w && y < r.y+r.h) {
        x = (x - r.x) / xScale;y = (y - r.y) / yScale;
        int buttons = 0;
        if (sbut & SDL_BUTTON(1)) buttons |= 0x1;
        if (sbut & SDL_BUTTON(3)) buttons |= 0x2;
        if (sbut & SDL_BUTTON(2)) buttons |= 0x4;
        //printf("%x %d,%d\n",buttons,x,y);
        MSESetPosition(x & 0xFFFF,y & 0xFFFF);
        MSEUpdateButtonState(buttons & 0xFF);
    }
}
