
#ifndef __SDL_3DS_H__
#define __SDL_3DS_H__


#include <stdlib.h>
#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>


#define SDL_INIT_VIDEO 1
#define SDL_INIT_AUDIO 2
#define SDL_INIT_TIMER 4
#define SDL_ANYFORMAT 0
typedef int Uint32;
typedef u8 Uint8;


typedef u32 SDLKey;


int SDL_Init(int flag); //returns 0 on success, -1 on failure 
void SDL_Quit();

unsigned int SDL_GetTicks();
void SDL_Delay(int ms);

// stubbed win functions
void SDL_WM_SetCaption(const char* caption,void* p);
void SDL_WM_SetIcon(void* i, void* p);

const char * SDL_GetError();

#define SDL_Event int

#define SDL_QUIT 1<<1
#define SDL_KEYDOWN 1<<2
#define SDL_FINGERDOWN 1<<3
#define SDL_MOUSEBUTTONDOWN KEY_TOUCH

#define SDL_DISABLE 0


int SDL_PollEvent(int event);
int getTouchX();
int getTouchY();

int SDL_GetKeyState(void* p);

int WaitKey();

int SDL_GetMouseState(int * x, int * y);


#define SDL_DISABLE 0
#define SDL_ENABLE 1
void SDL_ShowCursor(int s);

int getLanguage(void);
void setLanguage(int languageID);


#define SDLK_ENTER	KEY_A
#define SDLK_RETURN	KEY_A
#define SDLK_ESCAPE	KEY_START
#define SDLK_LEFT	KEY_LEFT
#define SDLK_RIGHT	KEY_RIGHT
#define SDLK_UP		KEY_UP
#define SDLK_DOWN	KEY_DOWN
#define SDLK_p 		KEY_B


#define SDLK_DELETE KEY_Y
#define SDLK_SPACE	KEY_A
#define SDLK_F1 	0
#define SDLK_F4 	0

#define SDLK_BACKSPACE 0
#define SDLK_CAPSLOCK 0
#define SDLK_PAGEUP 0
#define SDLK_PAGEDOWN 0
#define SDLK_LSHIFT 0
#define SDLK_LCTRL	0
#define SDLK_l		0
#define SDLK_k		0
#define SDLK_g		0
#define SDLK_f		0
#define SDLK_r		0
#define SDLK_z		0
#define SDLK_w		0
#define SDLK_s		0
#define SDLK_a		0
#define SDLK_d		0
#define SDLK_x		0
#define SDLK_c		0
#define SDLK_u		0
#define SDLK_y		0
#define SDLK_n		0
#define SDLK_m 		0
#define SDLK_t 		0
#define SDLK_o 		0
#define SDLK_SEMICOLON 0

#define KMOD_ALT 0
#define KMOD_CTRL 0

#define SDLK_0 0
#define SDLK_1 0
#define SDLK_2 0
#define SDLK_3 0
#define SDLK_4 0
#define SDLK_5 0
#define SDLK_6 0
#define SDLK_7 0
#define SDLK_8 0
#define SDLK_9 0

#define SDLK_KP0 0
#define SDLK_KP1 0
#define SDLK_KP2 0
#define SDLK_KP3 0
#define SDLK_KP4 0
#define SDLK_KP5 0
#define SDLK_KP6 0
#define SDLK_KP7 0
#define SDLK_KP8 0
#define SDLK_KP9 0

//******************* Video

typedef struct SDL_Surface {
        u32 flags;                           /* Read-only */
//        SDL_PixelFormat *format;                /* Read-only */
        int format;                /* Read-only */
        int w, h;                               /* Read-only */
//        Uint16 pitch;                           /* Read-only */
//        void *pixels;                           /* Read-write */
		sf2d_texture* texture;
//		int colorKey;

//        SDL_Rect clip_rect;                     /* Read-only */
//        int refcount;                           /* Read-mostly */
} SDL_Surface;

typedef struct{
  int x, y;
  int w, h;
} SDL_Rect;

typedef struct{
  int x, y;
} SDL_Point;


#define SDL_SRCCOLORKEY 1
#define SDL_FULLSCREEN 1
#define SDL_DOUBLEBUF 2
#define SDL_HWSURFACE 4
#define SDL_SWSURFACE 8

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, int flags);

SDL_Surface* IMG_Load(const char* f);
SDL_Surface* SDL_LoadBMP(const char* f);
SDL_Surface* SDL_CreateRGBSurface(int type, int width, int height, int bpp, int a, int b, int c, int d);

void SDL_FreeSurface(SDL_Surface* s);

void SDL_SetColorKey(SDL_Surface* s, int flag, u32 color);

u32 SDL_MapRGB(int flag , u8 r, u8 g, u8 b);
u32 SDL_MapRGBA(int flag , u8 r, u8 g, u8 b, u8 a);

void SDL_FillRect(SDL_Surface* s, SDL_Rect* rect, u32 color);
void filledEllipseRGBA(SDL_Surface* s, int x, int y, int rx, int ry, u8 r, u8 g, u8 b, u8 a);

int SDL_BlitSurface(SDL_Surface* s, SDL_Rect * src, SDL_Surface* d, SDL_Rect * dst);

void SDL_Flip(void * p);

u32 getPixel(SDL_Surface *surface, int x, int y);
void setPixel(SDL_Surface *surface, int x, int y, u32 color);
void SDL_GetRGB(u32 c, int format, u8* r, u8* g, u8* b);

int SDL_MUSTLOCK(SDL_Surface *surface);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);

//******************** Audio

#define FSOUND_FREE 1
#define NUMSFX (45)

#define AUDIO_S16 SOUND_FORMAT_16BIT
#define MIX_MAX_VOLUME 1
#define MIX_DEFAULT_FORMAT SOUND_FORMAT_16BIT



typedef struct FSOUND_SAMPLE
{
	u8* data;
	u32 size;
	u32 format;
	bool used;
	bool loop;
} FSOUND_SAMPLE;


typedef FSOUND_SAMPLE FMUSIC_MODULE;

typedef FSOUND_SAMPLE Mix_Chunk;
typedef FSOUND_SAMPLE Mix_Music;


void soundInit();
void soundClose();

int FSOUND_Init(u32 freq, u32 bpd, u32 unkn);

int FSOUND_GetSFXMasterVolume(); //
int FMUSIC_GetMasterVolume(FMUSIC_MODULE* s); 


void FMUSIC_SetMasterVolume(FMUSIC_MODULE* s, u8 volume);
void FMUSIC_FreeSong(FMUSIC_MODULE* s);
void FSOUND_SetSFXMasterVolume(u8 volson);

void initSFX(FSOUND_SAMPLE* s);
void loadSFX(FSOUND_SAMPLE* s, const char* filename, u32 format);

void FSOUND_PlaySound(int chl,FSOUND_SAMPLE* s);
void FMUSIC_StopSong(FMUSIC_MODULE* s);
void FMUSIC_SetLooping(FMUSIC_MODULE* s, bool flag);
void FMUSIC_PlaySong(FMUSIC_MODULE* s);

FSOUND_SAMPLE* FSOUND_Sample_Load(int flag, const char * f,int a, int b, int c);
FMUSIC_MODULE* FMUSIC_LoadSong(const char * f); 

Mix_Chunk * Mix_LoadWAV(const char * f);
Mix_Music * Mix_LoadMUS(const char *f);

int Mix_PlayChannel(int channel, Mix_Chunk *s, int loops);
int Mix_PlayMusic(Mix_Music * s , int loops );
int Mix_PlayingMusic(void);


void Mix_FreeChunk(Mix_Chunk* s);
void Mix_FreeMusic(Mix_Music* s);
void Mix_HaltMusic();


void FSOUND_Sample_Free(FSOUND_SAMPLE* s);

void FSOUND_Close();
void Mix_CloseAudio();

int Mix_OpenAudio( int audio_rate, u16 audio_format, int audio_channels, int bufsize );

void Mix_VolumeMusic( int vol );

const char * Mix_GetError();

int Mix_PausedMusic();
void Mix_PauseMusic();
void Mix_ResumeMusic();



#endif