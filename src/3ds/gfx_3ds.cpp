
#include <3ds.h>
#include <stdio.h>
#include "SDL_3ds.h"


bool drawing,drawing2;
int offset;
float scaleY;
float scaleX;

void endTopFrame(void)
{
	if(drawing)
	{ 
		if (offset)
		{
			sf2d_draw_rectangle(0, 0, 40, 240, RGBA8(0, 0, 0, 0xff)); 
			sf2d_draw_rectangle(360, 0, 40, 240, RGBA8(0, 0, 0, 0xff));
		}
		sf2d_end_frame();
	}
	drawing = false;
}

void endBottomFrame(void)
{
	if(drawing2)
		sf2d_end_frame();
	drawing2 = false;
}

void SDL_Flip(void * p)
{
	endTopFrame();
	endBottomFrame();
	sf2d_swapbuffers();
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, int flag)
{
	drawing=false;
	drawing2=false;
	if(flag & SDL_FULLSCREEN)
	{
		offset = 0;
		scaleY = 0.4; 
		scaleX = 0.5;
	} else {
		offset = 1;
		scaleY = 0.4; 
		scaleX = 0.4; 
	}
	if (width == 320) return (SDL_Surface*) 2;
	else return (SDL_Surface*) 1;
}

SDL_Surface* IMG_Load(const char* f)
{
    SDL_Surface* s;
	s = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	if(!s) return NULL;
	s->texture = sfil_load_PNG_file(f, SF2D_PLACE_RAM);

	if (s->texture) {
		s->w = s->texture->width;
		s->h = s->texture->height;
		sf2d_texture_set_params(s->texture, GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)); 
		s->flags = 0;
		s->format = 0;
		return s;
	} else {
		free(s);
		return NULL;
	} 
}

SDL_Surface* SDL_LoadBMP(const char* f)
{
    SDL_Surface* s;
	s = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	if(!s) return NULL;
	s->texture = sfil_load_BMP_file(f, SF2D_PLACE_RAM);

	if (s->texture) {
		s->w = s->texture->width;
		s->h = s->texture->height;
		sf2d_texture_set_params(s->texture, GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)); 
		s->flags = 0;
		s->format = 0;
		return s;
	} else {
		free(s);
		return NULL;
	} 
}


SDL_Surface* SDL_CreateRGBSurface(int type, int width, int height, int bpp, int a, int b, int c, int d)
{
    SDL_Surface* s;
	s = (SDL_Surface*) malloc(sizeof(SDL_Surface));
	if(!s) return NULL;
	s->texture = sf2d_create_texture (width, height,  TEXFMT_RGBA8, SF2D_PLACE_RAM);
	if (s->texture) {
		s->w = s->texture->width;
		s->h = s->texture->height;
		s->flags = 0;
		sf2d_texture_set_params(s->texture, GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR)); 
		s->format = 0;
		sf2d_texture_tile32(s->texture);
		return s;
	} else {
		free(s);
		return NULL;
	} 
}


void SDL_FreeSurface(SDL_Surface* s)
{
    if(s)
	{
		if((s==(SDL_Surface*) 1) || (s==(SDL_Surface*) 2)) s=NULL;
		else
		{
			if(s->texture) sf2d_free_texture(s->texture);
			free(s);
		}
	}
}


void SDL_SetColorKey(SDL_Surface* s, int flag, u32 color)
{
 //   s->colorKey= color;
}

u32 SDL_MapRGB(int flag , u8 r, u8 g, u8 b)
{
    return RGBA8(r, g, b, 0xff);
}

u32 SDL_MapRGBA(int flag , u8 r, u8 g, u8 b, u8 a)
{
    return RGBA8(r, g, b, a);
}


void SDL_FillRect(SDL_Surface* s, SDL_Rect* rect, u32 color)
{
    if (s==(SDL_Surface*) 1) 
	{

		if(drawing2) endBottomFrame();
		if(!drawing)
		{
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			drawing=true;
		} 
		if(rect) sf2d_draw_rectangle(rect->x*scaleX+40*offset, rect->y*scaleY, rect->w*scaleX, rect->h*scaleY, color);
		else  sf2d_draw_rectangle(40*offset, 0, 320 + 80 * (1-offset), 240, color);
	} else if (s==(SDL_Surface*) 2)
	{
		if(drawing) endTopFrame();
		if(!drawing2)
		{
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			drawing2=true;
		} 
		if(rect) sf2d_draw_rectangle(rect->x*scaleY, rect->y*scaleY, rect->w*scaleY, rect->h*scaleY, color);
		else  sf2d_draw_rectangle(0, 0, 320, 240, color);
	} else {
		if(s) 
			if(s->texture){ 
				int i,j;
				int dx = (rect)?rect->x:0;
				int dy = (rect)?rect->y:0;
				int dw = (rect)?rect->w:s->texture->width;
				int dh = (rect)?rect->h:s->texture->height;
				if (dw > s->texture->width) dw = s->texture->width;
				if (dh > s->texture->height) dh = s->texture->height;
				for (i=dx;i< dw; i++)
					for (j=dy;j< dh; j++) 
						sf2d_set_pixel(s->texture,i,j,color); 		
				sf2d_texture_tile32(s->texture);
			}
	}
}

/*
void filledEllipseRGBA(SDL_Surface* s, int x, int y, int rx, int ry, u8 r, u8 g, u8 b, u8 a)
{
    if (s==(SDL_Surface*) 1) 
	{
		if(!drawing)
		{
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			drawing=true;
		} 
		sf2d_draw_fill_circle(x*scalepos+40*offset, y, rx*scalewidth, RGBA8(r, g, b, a));
	} else if (s==(SDL_Surface*) 3) 
	{
		if(drawing) endTopFrame();
		if(!drawing2)
		{
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			drawing2=true;
		} 
		sf2d_draw_fill_circle(x, y, rx, RGBA8(r, g, b, a));
	}
}
*/

int SDL_BlitSurface(SDL_Surface* s, SDL_Rect * src, SDL_Surface* d, SDL_Rect * dst)
{
	u32 pixel;
    if(d==(SDL_Surface*) 1)
	{
		if(drawing2) endBottomFrame();
		if(!drawing)
		{
			sf2d_start_frame(GFX_TOP, GFX_LEFT);
			drawing=true;
		} 

		if(src) {
			if(dst) 
				sf2d_draw_texture_part_scale(s->texture, (int)(dst->x*scaleX+40*offset),dst->y*scaleY, src->x*scaleY, src->y*scaleY, src->w*scaleY, src->h*scaleY, 1.0 +0.25*(1-offset),1);
			else
				sf2d_draw_texture_part_scale(s->texture, 40*offset,0, src->x*scaleY, src->y*scaleY, src->w*scaleY, src->h*scaleY, 1.0 +0.25*(1-offset),1);
		} else {
			if(dst) 
				sf2d_draw_texture_scale(s->texture, (int)(dst->x*scaleX+40*offset),dst->y*scaleY, 1.0 +0.25*(1-offset),1);
			else
				sf2d_draw_texture_scale(s->texture, 40*offset,0, 1.0 +0.25*(1-offset),1);
		}
	} else if(d==(SDL_Surface*) 2) 
	{
		if(drawing) endTopFrame();
		if(!drawing2)
		{
			sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			drawing2=true;
		} 

		if(src) {
			if(dst) 
				sf2d_draw_texture_part_scale(s->texture, dst->x*scaleY,dst->y*scaleY, src->x*scaleY, src->y*scaleY, src->w*scaleY, src->h*scaleY, 1, 1);
			else
				sf2d_draw_texture_part_scale(s->texture, 0,0, src->x*scaleY, src->y*scaleY, src->w*scaleY, src->h*scaleY, 1, 1);
		} else {
			if(dst) 
				sf2d_draw_texture_scale(s->texture, dst->x*scaleY,dst->y*scaleY, 1, 1);
			else
				sf2d_draw_texture_scale(s->texture, 0,0, 1, 1);
		}
	} else {
		if(s && d) 
			if(s->texture && d->texture){ 
				int i,j;

				int sx = (src)?src->x:0;
				int sy = (src)?src->y:0;
				int dx = (dst)?dst->x:0;
				int dy = (dst)?dst->y:0;
				int sw = (src)?src->w:s->texture->width;
				int sh = (src)?src->h:s->texture->height;
				if (sx+sw > s->texture->width) sw = s->texture->width - sx;
				if (sy+sh > s->texture->height) sh = s->texture->height - sy;
				for (i=0;i< sw; i++)
					for (j=0;j< sh; j++) {
						pixel = sf2d_get_pixel ( s->texture,i+sx,j+sy);
						if (pixel & 0xff) sf2d_set_pixel(d->texture,i+dx,j+dy,pixel); 		
					}
				sf2d_texture_tile32(d->texture);
			}
	}
	return 0;
}

u32 getPixel(SDL_Surface *surface, int x, int y)
{
	return sf2d_get_pixel(surface->texture, x, y);
}

void setPixel(SDL_Surface *surface, int x, int y, u32 color)
{
	sf2d_set_pixel(surface->texture, x, y, color);
}

void SDL_GetRGB(u32 c, int format, u8* r, u8* g, u8* b)
{
	*r = RGBA8_GET_R(c);
	*g = RGBA8_GET_G(c);
	*b = RGBA8_GET_B(c);
}


int SDL_MUSTLOCK(SDL_Surface *surface)
{
	return 0;
}

int SDL_LockSurface(SDL_Surface *surface)
{
	return 0;
}

void SDL_UnlockSurface(SDL_Surface *surface){}
