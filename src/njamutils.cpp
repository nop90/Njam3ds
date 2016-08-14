/*-----------------------------------------------------------------------------
Njamutils.cpp

Various utility functions to simplify programming.
See njamutils.h for function list.

Copyright 2003 Milan Babuskov

This file is part of Njam (http://njam.sourceforge.net).

Njam is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Njam is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Njam in file COPYING; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
-----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "3ds/SDL_3ds.h"
#include "njamutils.h"
#include "njamfont.h"
//-----------------------------------------------------------------------------
void NjamSetRect(SDL_Rect& rect, int x, int y, int w, int h)
{
 	rect.x = x;
 	rect.y = y;
 	rect.w = w;
 	rect.h = h;
}
//-----------------------------------------------------------------------------
SDLKey NjamGetch(bool Wait)
{
	if (Wait) 
		return WaitKey();
	else 
		hidScanInput();
	return hidKeysHeld();
}
//-----------------------------------------------------------------------------
int NjamRandom(int MaxValue)
{
 	return ((int)((double)MaxValue*rand()/(RAND_MAX+1.0)));	// according to rand man page
}
//-----------------------------------------------------------------------------
// general logging function
LogFile::LogFile(const char *Text, bool Create)
{
        printf("%s", Text);
/*    FILE *fptr;
    if (Create)
        fptr = fopen("log.txt", "w+");
    else
        fptr = fopen("log.txt", "a+");

    if (fptr)
    {
        fprintf(fptr, "%s", Text);
        fclose(fptr);
    }
*/
}
//-----------------------------------------------------------------------------
// Return the pixel value at (x, y)
// NOTE: The surface must be locked before calling this!
Uint32 GetPixel(SDL_Surface *surface, int x, int y)
{
	return getPixel(surface, x, y);
}
//-----------------------------------------------------------------------------
// Set the pixel at (x, y) to the given value
// NOTE: The surface must be locked before calling this!
void PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    setPixel(surface, x, y, pixel);
}
//-----------------------------------------------------------------------------
bool SurfaceEffect(SDL_Surface *surface, SDL_Rect& r, tEffect Effect)
{
    int x, y;
	if ( SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) < 0 )
	{
		fprintf(stderr, "njamutils.cpp: Can't lock surface: %s\n", SDL_GetError());
		return false;
	}

	Uint8 r1, g1, b1;
	for (int x = r.x; x < r.w+r.x; x++)
	{
		for (int y = r.y; y < r.h+r.y; y++)
		{
			SDL_GetRGB(GetPixel(surface, x, y), surface->format, &r1, &g1, &b1);
			int val = (r1 + g1 + b1) / 3;
			Uint32 color=0;

			if (Effect == fxBlackWhite)
				color = SDL_MapRGB(surface->format, (unsigned char)(val/1.1), (unsigned char)val, (unsigned char)(val/1.1));
			else if (Effect == fxDarken)
				color = SDL_MapRGB(surface->format, r1/2, g1/2, b1/2);
			PutPixel(surface, x, y, color);
		}
	}

    if ( SDL_MUSTLOCK(surface) )
        SDL_UnlockSurface(surface);

	return true;
}
//-----------------------------------------------------------------------------
#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(__DECC) && defined(VMS)
#include <unixlib.h>
static char *vms_to_unix_buffer = NULL;
static int convert_vms_to_unix(char *vms_dir_name)
{
	vms_to_unix_buffer = vms_dir_name;
}
#endif

char *NjamGetHomeDir()
{
	struct passwd *pw;

	if (!(pw = getpwuid(getuid())))
	{
		fprintf(stderr, "Who are you? Not found in passwd database!!\n");
		return NULL;
	}

#if defined(__DECC) && defined(VMS)
	/* Convert The OpenVMS Formatted "$HOME" Directory Path Into Unix
	   Format. */
	decc$from_vms(pw->pw_dir, convert_vms_to_unix, 1);
	return vms_to_unix_buffer;
#else
	return pw->pw_dir;
#endif
}
//-----------------------------------------------------------------------------
int NjamCheckAndCreateDir(const char *name)
{
	struct stat stat_buffer;

	if (stat(name, &stat_buffer))
	{
		/* error check if it doesn't exist or something else is wrong */
		if (errno == ENOENT)
		{
			/* doesn't exist letts create it ;) */
#ifdef BSD43
			if (mkdir(name, 0775))
#else
				if (mkdir(name, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH))
#endif
				{
					fprintf(stderr, "Error creating dir %s", name);
					perror(" ");
					return -1;
				}
		}
		else
		{
			/* something else went wrong yell about it */
			fprintf(stderr, "Error opening %s", name);
			perror(" ");
			return -1;
		}
	}
	else
	{
		/* file exists check it's a dir otherwise yell about it */
#ifdef BSD43
		if (!(S_IFDIR & stat_buffer.st_mode))
#else
			if (!S_ISDIR(stat_buffer.st_mode))
#endif
			{
				fprintf(stderr,"Error %s exists but isn't a dir\n", name);
				return -1;
			}
	}
	return 0;
}
#endif
