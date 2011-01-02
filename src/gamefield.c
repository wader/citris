/*
 * Copyright (C)2002 Mattias Wadman <mattias@sudac.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 * 
 */

/* $Id: gamefield.c 910 2005-09-14 21:38:13Z mattias $ */


#include "global.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <assert.h>
#include <string.h>

#include "game.h"
#include "gamefield.h"
#include "eventengine.h"
#include "tetris.h"
#include "update.h"
#include "keyhandler.h"
#include "mousemove.h"
#include "states.h"
#include "util.h"
#include "quit.h"



static SDL_Surface *last_surface;
static tetris *last_tetris;
static TTF_Font *font;
static SDL_Surface *text_score;
static SDL_Surface *text_level;
static SDL_Surface *text_lines;
static int last_gamefield_upside_down_field;
static int last_gamefield_rotate_field;


static const unsigned char shape_to_color[8][3] =
{
    {0, 0, 0}, /* transparent, not drawn */
    {0, 0, 255}, /* shape 1, blue */
    {0, 255, 0}, /* shape 2, green */
    {255, 0, 0}, /* ... */
    {255, 255, 255},
    {255, 255, 0},
    {0, 255, 255},
    {255, 0, 255}
};

static void gamefield_cleanup(void *data)
{
    SDL_FreeSurface(last_surface);
    tetris_free(last_tetris);
    TTF_CloseFont(font);
    SDL_FreeSurface(text_score);
    SDL_FreeSurface(text_level);
    SDL_FreeSurface(text_lines);
}

int gamefield_init()
{
    SDL_Color c;

    last_surface = SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            screen->w, screen->h, screen->format->BitsPerPixel,
            0, 0, 0, 0
            );
    if(last_surface == NULL)
    {
        fprintf(stderr, "SDL_CreateRGBASurface: last_suface: %s\n", SDL_GetError());

        return FALSE;
    }
    SDL_FillRect(last_surface, NULL, SDL_MapRGB(last_surface->format, 0, 0, 0));
    
    last_tetris = tetris_alloc(GAME_WIDTH, GAME_HEIGHT); /* FIXME: take from game? game not allocated yet :( */
    if(last_tetris == NULL)
    {
        fprintf(stderr, "tetris_alloc: last_tetris: failed\n");
        
        return FALSE;
    }
    
    font = TTF_OpenFont("font.ttf", screen->h/30);
    if(font == NULL)
    {
        fprintf(stderr, "TTF_OpenFont: %s\n", SDL_GetError());
        
        return FALSE;
    }

    c.r = 150;
    c.g = 150;
    c.b = 150;
    text_score = TTF_RenderText_Blended(font, "Score:", c);
    text_level = TTF_RenderText_Blended(font, "Level:", c);
    text_lines = TTF_RenderText_Blended(font, "Lines:", c);
    if(text_score == NULL || text_level == NULL || text_lines == NULL)
    {
        fprintf(stderr, "TTF_RenderText_Blended: text_*: %s\n", SDL_GetError());
        
        return FALSE;
    }

    gamefield_upside_down_field = FALSE;
    gamefield_rotate_field = FALSE;

    quit_addcb(gamefield_cleanup, NULL);

    return TRUE;
}

/* number of pixels between nodes in polygon */
#define DRAW_PIE_SEGMENT_FINENESS 8
/* use a constant array for coordinates, we dont want to call malloc or alloca for each draw */
#define DRAW_PIE_SEGMENT_MAX_POLYGONS 16

static void draw_pie_segment(
    SDL_Surface *surface, /* surface */
    int ox, int oy,	/* origo */
    float d1, float d2,	/* start and stop angle, in radian */
    float r1, float r2,	/* start and stop radius, in pixels */
    int cr, int cg, int cb,	/* color */
    int ca	/* alpha */
)
{
    int i, n;
    float d, c;
    /* n "quad"-segements need (n+1)*2 coordinates, try on paper and you will see :) */
    Sint16 xv[(DRAW_PIE_SEGMENT_MAX_POLYGONS + 1) * 2];
    Sint16 yv[(DRAW_PIE_SEGMENT_MAX_POLYGONS + 1) * 2];

    /* number of polygons used to draw segment */
    n = abs(((d1 - d2) * r2)) / DRAW_PIE_SEGMENT_FINENESS;

    /* keep number of polygons in range 1 to max polygons */
    n = CLAMP(n, 1, DRAW_PIE_SEGMENT_MAX_POLYGONS);

    /* delta angle */
    d = (d2 - d1) / n;

    /* start angle */
    c = d1;

    for(i = 0; i < n + 1; i++)
    {
        /* invert y, vertical coordinate is reversed on screen */
        xv[i] = ox + cos(c) * r2;
        yv[i] = oy + -sin(c) * r2;
        xv[(n + 1) * 2 - 1 - i] = ox + cos(c) * r1;
        yv[(n + 1) * 2 - 1 - i] = oy + -sin(c) * r1;

        /* increse with delta */
        c += d;
    }

    filledPolygonRGBA(surface, xv, yv, (n + 1) * 2, cr, cg, cb, ca);
    aapolygonRGBA(surface, xv, yv, (n + 1) * 2, cr * 0.7, cg * 0.7, cb * 0.7, ca);
}

void gamefield_render(SDL_Surface *surface, tetris *t)
{
    int i, j, v;
    int center_x = surface->w / 2;
    int center_y = surface->h / 2;
    int inner_radius = surface->h / 26;
    int height_delta = (center_y - inner_radius) / t->height;
    int preview_offset_x = (surface->w / 7) * 6, preview_offset_y = (surface->h / 12);
    int preview_block = surface->w / 32;
    char buffer[256];
    SDL_Surface *text_surface;
    SDL_Color c;
    SDL_Rect r;


    /* draw field blocks */
    for(i = 0; i < t->height; i++)
        for(j = 0; j < t->width; j++)
        {
            int x, y, lx, ly;
            
            if(gamefield_upside_down_field)
                y = i;
            else
                y = t->height - i - 1;
            
            if(last_gamefield_upside_down_field)
                ly = i;
            else
                ly = last_tetris->height - i - 1;

            if(gamefield_rotate_field)
                /* make shape appear at bottom on screen and offset so that shape is in the center */
                x = (t->width + t->x - j) - (t->width/4 + 1 - tetris_shape_frame_width(t, t->shape, t->frame) / 2);
            else
                x = j;
            
            if(last_gamefield_rotate_field)
                lx = (last_tetris->width + last_tetris->x - j) - (last_tetris->width/4 + 1 - tetris_shape_frame_width(last_tetris, last_tetris->shape, last_tetris->frame) / 2);
            else
                lx = j;
            
            /* only draw if block has changed color */
            v = tetris_field_block_get(t, x, y);
            if(v != tetris_field_block_get(last_tetris, lx, ly))
                draw_pie_segment(
                        last_surface,
                        center_x, center_y,
                        ((float)j / t->width) * M_PI * 2,
                        ((float)(j + 1) / t->width) * M_PI * 2,
                        inner_radius + i * height_delta,
                        inner_radius + (i + 1) * height_delta,
                        shape_to_color[v][0] * (((float) i / (t->height * 1.3)) + 0.25),
                        shape_to_color[v][1] * (((float) i / (t->height * 1.3)) + 0.25),
                        shape_to_color[v][2] * (((float) i / (t->height * 1.3)) + 0.25),
                        SDL_ALPHA_OPAQUE
                        );
        }
   
    /* save current state */
    last_gamefield_upside_down_field = gamefield_upside_down_field;
    last_gamefield_rotate_field = gamefield_rotate_field;
    tetris_copy(last_tetris, game);
   
    SDL_BlitSurface(last_surface, NULL, surface, NULL);
    
    /* draw grid lines */
    for(j = 0; j < t->width; j++)
    {
    	float x, y;

    	x = cos(((float)j / t->width) * M_PI * 2);
    	y = sin(((float)j / t->width) * M_PI * 2);

    	aalineRGBA(
                surface,
                center_x + x * inner_radius,
                center_y + y * inner_radius,
                center_x + x * (inner_radius + height_delta * (t->height)),
                center_y + y * (inner_radius + height_delta * (t->height)),
                80, 80, 80, 128 //SDL_ALPHA_OPAQUE
                );
    }
    
    /* draw grid circles */
    for(j = 0; j < t->height + 1; j++)
	    aacircleRGBA(
                surface,
                center_x, center_y,
                inner_radius + (height_delta * j),
                80, 80, 80, 128 //SDL_ALPHA_OPAQUE
                );
                
    /* draw next shape preview */
    for(j = 0; j < game->shape_array[game->next_shape]->width; j++)
        for(i = 0; i < game->shape_array[game->next_shape]->height; i++)
        {
            v = tetris_shape_block_get(game, game->next_shape, game->next_frame, j, i);
                
            if(v != 0)
            {
                boxRGBA(
                        surface,
                        preview_offset_x + preview_block * j,
                        preview_offset_y + preview_block * i,
                        preview_offset_x + (preview_block * (j + 1)),
                        preview_offset_y + (preview_block * (i + 1)),
                        shape_to_color[v][0] * 0.7,
                        shape_to_color[v][1] * 0.7,
                        shape_to_color[v][2] * 0.7,
                        SDL_ALPHA_OPAQUE
                        );
                
                boxRGBA(
                        surface,
                        preview_offset_x + preview_block * j + 1,
                        preview_offset_y + preview_block * i + 1,
                        preview_offset_x + (preview_block * (j + 1)) - 1,
                        preview_offset_y + (preview_block * (i + 1)) - 1,
                        shape_to_color[v][0],
                        shape_to_color[v][1],
                        shape_to_color[v][2],
                        SDL_ALPHA_OPAQUE
                        );
            }
        }
    

    /* draw score, level, lines texts */
    c.r = 255;
    c.g = 255;
    c.b = 255;
    
    r.y = screen->h/14;
    r.x = screen->w/7 - text_score->w;
    r.w = text_score->w;
    r.h = text_score->h;
    SDL_BlitSurface(text_score, NULL, surface, &r);
    sprintf(buffer, "%d", t->score);
    text_surface = TTF_RenderText_Blended(font, buffer, c);
    r.x = screen->w/7 + 5;
    r.w = text_surface->w;
    r.h = text_surface->h;
    SDL_BlitSurface(text_surface, NULL, surface, &r);
    SDL_FreeSurface(text_surface);
    
    r.y += text_score->h;
    r.x = screen->w/7 - text_level->w;
    r.w = text_level->w;
    r.h = text_level->h;
    SDL_BlitSurface(text_level, NULL, surface, &r);
    sprintf(buffer, "%d", t->level);
    text_surface = TTF_RenderText_Blended(font, buffer, c);
    r.x = screen->w/7 + 5;
    r.w = text_surface->w;
    r.h = text_surface->h;
    SDL_BlitSurface(text_surface, NULL, surface, &r);
    SDL_FreeSurface(text_surface);
    
    r.y += text_level->h;
    r.x = screen->w/7 - text_lines->w;
    r.w = text_lines->w;
    r.h = text_lines->h;
    SDL_BlitSurface(text_lines, NULL, surface, &r);
    sprintf(buffer, "%d", t->lines);
    text_surface = TTF_RenderText_Blended(font, buffer, c);
    r.x = screen->w/7 + 5;
    r.w = text_surface->w;
    r.h = text_surface->h;
    SDL_BlitSurface(text_surface, NULL, surface, &r);
    SDL_FreeSurface(text_surface);
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
