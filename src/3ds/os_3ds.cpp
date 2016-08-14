
#include <sys/stat.h>
#include <stdio.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include "SDL_3ds.h"

#define TICKS_PER_MSEC (268123.480)

u8 language = 1;

bool init=false;

const char * error = "";

int SDL_Init(int flag) //returns 0 on success, -1 on failure 
{
    if(!init) {
		sf2d_init();
		sf2d_set_3D(false);
		sf2d_set_vblank_wait(true);
		sf2d_set_clear_color(RGBA8(9, 101, 101, 255));
		
		osSetSpeedupEnable(true);
	//	consoleInit(GFX_BOTTOM, NULL);

		
		romfsInit();
		cfguInit();
		soundInit();
	//	consoleInit (GFX_BOTTOM, NULL);

		// Read the language field from the config savegame.
		CFGU_GetSystemLanguage(&language);
		
	// In case game dir doesn't exist
		mkdir("/3ds", 0777);
		mkdir("/3ds/Njam3ds", 0777);
		init= true;
	}
    return 0;
}


void SDL_Quit()
{
	soundClose();
	cfguExit();
    romfsExit();
	sf2d_fini();

}

const char * SDL_GetError()
{
    return error;
}


unsigned int SDL_GetTicks()
{
    return svcGetSystemTick()/TICKS_PER_MSEC;
}

void SDL_Delay(int ms)
{
	svcSleepThread(ms*1000000);
}

// stubbed win functions
void SDL_WM_SetCaption(const char* caption,void* p) {return;}
void SDL_WM_SetIcon(void* i, void* p) {return;}
void SDL_ShowCursor(int s) {return;}

int SDL_PollEvent(int event)
{
	int ret = 0;
	hidScanInput();
	ret = hidKeysDown();
	if (ret) event = SDL_KEYDOWN;
	if (ret & KEY_TOUCH) event |= SDL_MOUSEBUTTONDOWN | SDL_FINGERDOWN;
	if(!aptMainLoop()) event |= SDL_QUIT;
	return ret;
}

int getTouchX(){
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.px;
}

int getTouchY(){
	touchPosition pos;	
	hidTouchRead(&pos);
	return pos.py;
}

int SDL_GetKeyState(void* p)
{
//	hidScanInput();
//	return hidKeysHeld();
	return hidKeysDown();
}

int WaitKey()
{
	do hidScanInput();
    while (!hidKeysDown());
	
	return hidKeysDown(); 	
}

int SDL_GetMouseState(int * x, int * y)
{
	*x = getTouchX();
	*y = getTouchY();
	return 0;
}

int getLanguage(void)
{
	return language;
}

void setLanguage(int languageID)
{
	if (languageID>5) language = 1;
	else if (languageID<1) language = 5;
	else language = languageID;
 
}
