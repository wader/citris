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

/* $Id: menu.c 910 2005-09-14 21:38:13Z mattias $ */


/* Da aweinspiring menu system 
   
   Lets use bitmaps for now

   A menu entry consists of:
   Menu entry surface(filename) when active(mouse over/keynaved to)
   Menu entry surface(filename) when inactive(all other times)
   Function to call when the entry is selected
   
   The position in which they are displayed is decided 
   by the order they occur in indata
*/
#include "global.h"

#include <SDL/SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ai.h"
#include "gamefield.h"
#include "game.h"
#include "eventengine.h"
#include "timer.h"
#include "tetris.h"
#include "update.h"
#include "keyhandler.h"
#include "states.h"
#include "quit.h"


typedef void (*selected_f)();
typedef struct
{
    char *text;
    selected_f selected;
    int hidden;
    SDL_Rect rect;
    SDL_Surface *surface;
} menu_entry;

static SDL_Surface *background;
static SDL_Surface *fader;
static SDL_Surface *logo;
static SDL_Surface *setting_on;
static SDL_Surface *setting_off;

/* changed by menu select funtion if it wants to go to a new state */
static state state_to_go = STATE_ERROR;

static void menu_new_game();
static void menu_continue_game();
static void menu_reset_game();
static void menu_upside_down_field();
static void menu_rotate_field();
static void menu_exit();

static menu_entry menu_entries[] =
{
    {"New game", menu_new_game, FALSE, {0}, NULL},
    {"Continue game", menu_continue_game, FALSE, {0}, NULL},
    {"Reset game", menu_reset_game, FALSE, {0}, NULL},
    {"Upside down field", menu_upside_down_field, FALSE, {0}, NULL},
    {"Rotate field", menu_rotate_field, FALSE, {0}, NULL},
    {"Exit", menu_exit, FALSE, {0}, NULL}
};


/* Active entry */
static int selected_entry = -1;

static int ai_playing = FALSE;


/* The keymap for the menu */
static keymap default_keymap[] = 
{
    {"UP",         "menu_up"},
    {"DOWN",       "menu_down"},
    {"RETURN",     "menu_select"},
    {"ALT+RETURN", "alter_fullscreen"},
    {NULL, NULL}
};

static void menu_moveblock_cb(void *data)
{
    tetris_shape_move_down(game);

    if(!game->playing)
    {
        tetris_reset(game);
        tetris_start(game);
    }
    
    timed_func_add(
            SDL_GetTicks() + tetris_level_speeds[(game->level < TETRIS_LEVELS ? game->level : TETRIS_LEVELS-1)],
            menu_moveblock_cb,
            NULL
            );
    
    update();
}

static void menu_ai_start()
{
    tetris_reset(game);
    tetris_start(game);
    ai_playing = TRUE;
    ai_start();
    timed_func_add(
            SDL_GetTicks() + tetris_level_speeds[(game->level < TETRIS_LEVELS ? game->level : TETRIS_LEVELS-1)],
            menu_moveblock_cb,
            NULL
            );
}

static int menu_find_coord(int x, int y)
{
    int i;
    
    for(i = 0; i < ARRAY_ELEMENTS(menu_entries); i++)
        if(menu_entries[i].hidden == FALSE &&
           menu_entries[i].rect.x <= x &&
           menu_entries[i].rect.y <= y &&
           menu_entries[i].rect.w > (x - menu_entries[i].rect.x) &&
           menu_entries[i].rect.h > (y - menu_entries[i].rect.y))
            return i;

    return -1;
}

static int menu_mousemove_eh(SDL_Event *event)
{
    int s;

    assert(NULL != event);
    assert(SDL_MOUSEMOTION == event->type);

    s = menu_find_coord(event->motion.x, event->motion.y);

    if(selected_entry != s)
    {
        selected_entry = s;
        update();
    }
    
    return TRUE;
}

static int menu_mousebutton_eh(SDL_Event *event)
{
    assert(NULL != event);
    assert(SDL_MOUSEBUTTONUP == event->type);
   
    if(selected_entry != -1)
        menu_entries[selected_entry].selected();
    
    return TRUE;
}

static void menu_new_game()
{
    state_to_go = STATE_GAME;
}
		  
static void menu_continue_game()
{
    state_to_go = STATE_GAME;
}

static void menu_reset_game()
{
    menu_ai_start();
    update();
}

static void menu_upside_down_field()
{
    gamefield_upside_down_field = !gamefield_upside_down_field;
    update();
}

static void menu_rotate_field()
{
    gamefield_rotate_field = !gamefield_rotate_field;
    update();
}

static void menu_exit()
{
    state_to_go = STATE_EXIT;
}

static void menu_move(int d)
{
    if(selected_entry == -1)
    {
        if(d < 0)
            selected_entry = 0;
        else
            selected_entry = ARRAY_ELEMENTS(menu_entries)-1;
    }
    
    /* move selection to next non-hidden entry and rotate if necessary */
    while(TRUE)
    {
        selected_entry = MOD_ROTATE(ARRAY_ELEMENTS(menu_entries), selected_entry, d);
        if(menu_entries[selected_entry].hidden == FALSE)
            break;
    }

    update();
}

void menu_up(void)
{
    menu_move(-1);
}

void menu_down(void)
{
    menu_move(1);
}

void menu_select(void)
{
    if(selected_entry != -1)
        menu_entries[selected_entry].selected();
}

int menu_statechange(run_state new)
{
    assert(NULL != new);
    
    selected_entry = -1;
    
    if(!game->playing)
    {
        menu_ai_start();
    }
    else
    {
        SDL_FillRect(background, NULL, SDL_MapRGB(background->format, 0, 0, 0));
        gamefield_render(background, game);
        SDL_BlitSurface(fader, NULL, background, NULL);
    }

    update();

    return TRUE;
}

static state menu_run(void)
{
    state_to_go = STATE_MENU;

    while(STATE_MENU == state_to_go)
        event_engine_run(EVENTMODE_RUN, 25);

    if(ai_playing)
    {
        tetris_reset(game);
        timed_func_clear(); /* FIXME: this clears the ai timer */
        ai_playing = FALSE;
    }

    return state_to_go;
}

static void menu_update(void)
{
    int i;
    int ypos;
    SDL_Rect r;

    ypos = screen->h/9;

    SDL_FillRect(background, NULL, SDL_MapRGB(background->format, 0, 0, 0));
    gamefield_render(background, game);
    SDL_BlitSurface(fader, NULL, background, NULL);

    SDL_BlitSurface(background, NULL, screen, NULL);
        
    r.x = screen->w/2 - logo->w/2;
    r.y = ypos;
    SDL_BlitSurface(logo, NULL, screen, &r);
    
    ypos += logo->h;

    ypos += screen->h/20;

    
    for(i = 0; i < ARRAY_ELEMENTS(menu_entries); i++)
    {
        if((menu_entries[i].selected == menu_new_game && !ai_playing) ||
           (menu_entries[i].selected == menu_continue_game && ai_playing) ||
           (menu_entries[i].selected == menu_reset_game && ai_playing))
        {
            menu_entries[i].hidden = TRUE;

            continue;
        }
        
        menu_entries[i].hidden = FALSE;
        if(menu_entries[i].selected == menu_upside_down_field ||
           menu_entries[i].selected == menu_rotate_field)
        {
            menu_entries[i].rect.x = screen->w/2 - menu_entries[i].surface->w/2 - setting_off->w/2;
            menu_entries[i].rect.w = menu_entries[i].surface->w + setting_off->w;
        }
        else
        {
            menu_entries[i].rect.x = screen->w/2 - menu_entries[i].surface->w/2;
            menu_entries[i].rect.w = menu_entries[i].surface->w;
        }
        menu_entries[i].rect.y = ypos;
        menu_entries[i].rect.h = menu_entries[i].surface->h;
         
        if(i == selected_entry)
        {
            SDL_Surface *marker;
            SDL_Rect r;
            
            marker = SDL_CreateRGBSurface(
                    SDL_HWSURFACE | SDL_SRCALPHA,
                    menu_entries[i].rect.w+20, menu_entries[i].surface->h, menu_entries[i].surface->format->BitsPerPixel,
                    128, 128, 128, 128 /* alpha voodoo, 128 is optimized */
                    );
            
            filledCircleRGBA(
                    marker, /* surface */
                    marker->h/2, marker->h/2, /* origin */
                    marker->h/2, /* radius */
                    200, 200, 200, /* color */
                    SDL_ALPHA_OPAQUE /* alpha */
                    );
            aacircleRGBA(
                    marker, /* surface */
                    marker->h/2, marker->h/2, /* origin */
                    marker->h/2, /* radius */
                    200, 200, 200, /* color */
                    SDL_ALPHA_OPAQUE /* alpha */
                    );
            filledCircleRGBA(
                    marker, /* surface */
                    marker->w - 1 - marker->h/2, marker->h/2, /* origin */
                    marker->h/2, /* radius */
                    200, 200, 200, /* color */
                    SDL_ALPHA_OPAQUE /* alpha */
                    );
            aacircleRGBA(
                    marker, /* surface */
                    marker->w - marker->h/2, marker->h/2, /* origin */
                    marker->h/2, /* radius */
                    200, 200, 200, /* color */
                    SDL_ALPHA_OPAQUE /* alpha */
                    );
            boxRGBA(
                    marker,
                    marker->h/2,
                    0,
                    marker->w - marker->h/2,
                    marker->h-1,
                    200, 200, 200,
                    SDL_ALPHA_OPAQUE
                    );

            r.x = screen->w/2 - (marker->w/2);
            r.y = menu_entries[i].rect.y;
            r.w = marker->w;
            r.h = marker->h;
            
        
            SDL_BlitSurface(marker, NULL, screen, &r);
        
            SDL_FreeSurface(marker);
        }
        

        if(0 > SDL_BlitSurface(menu_entries[i].surface, NULL, screen, &menu_entries[i].rect))
        {
            fprintf(stderr, "Couldn't blit surface: %s\n", SDL_GetError());
            return;
        }
        
        if(menu_entries[i].selected == menu_upside_down_field ||
           menu_entries[i].selected == menu_rotate_field)
        {
            r.x = menu_entries[i].rect.x + menu_entries[i].surface->w;
            r.y = ypos;
        
            if((menu_entries[i].selected == menu_upside_down_field && gamefield_upside_down_field) ||
               (menu_entries[i].selected == menu_rotate_field && gamefield_rotate_field))
                SDL_BlitSurface(setting_on, NULL, screen, &r);
            else
                SDL_BlitSurface(setting_off, NULL, screen, &r);
        }
        
        ypos += menu_entries[i].surface->h;
    }
    
    SDL_Flip(screen);
}


/* Intilize the menu items 
   Load grafix and setup run_state
*/
run_state menu_init(run_state rstate)
{
    int i;
    SDL_Color c;
    TTF_Font *font;
   
    font = TTF_OpenFont("font.ttf", screen->h/16);
    if(font == NULL)
    {
        fprintf(stderr,"Couldn't load font: %s\n", SDL_GetError());
        return NULL;
    }
    
    c.r = 180;
    c.g = 180;
    c.b = 180;
    for(i = 0; i < ARRAY_ELEMENTS(menu_entries); i++)
    {
        menu_entries[i].surface = TTF_RenderText_Blended(
                                        font,
                                        menu_entries[i].text, 
                                        c
                                        );
    }
    
    c.r = 80;
    c.g = 200;
    c.b = 80;
    setting_on = TTF_RenderText_Blended(font, "On", c);
    setting_off = TTF_RenderText_Blended(font, "Off", c);
    
    TTF_CloseFont(font);

    /* Create the keymap */
    if(NULL == (rstate->keymap = keyhandler_createmap(NULL, default_keymap)))
        return NULL;

    /* Store a pointer to menu_main in data */
    rstate->data = NULL;
    
    /* Set remaining functions */
    rstate->run = menu_run;
    rstate->extra = menu_statechange;
    rstate->update = menu_update;
    
    /* Initialise event handlers */
    event_handler_add(keyhandler_eh, SDL_KEYDOWN);
    event_handler_add(quithandler_eh, SDL_QUIT);
    event_handler_add(updatehandler_eh, SDL_USEREVENT);
    event_handler_add(menu_mousemove_eh, SDL_MOUSEMOTION);
    event_handler_add(menu_mousebutton_eh, SDL_MOUSEBUTTONUP);
    
    if(NULL == (rstate->eventengine = event_engine_save()))
        return NULL;
    
    /* surface used for menu background */
    background = SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            screen->w, screen->h, screen->format->BitsPerPixel,
            0, 0, 0, 0
            );

    /* fader surface, used to make background a bit faded to black */
    fader = SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            background->w, background->h, background->format->BitsPerPixel,
            0, 0, 0, 0
            );

    /* FIXME: magic color constants */
    SDL_FillRect(fader, NULL, SDL_MapRGB(fader->format, 0, 0, 0x4a));
    //SDL_SetAlpha(fader, SDL_SRCALPHA, 0x8e);
    SDL_SetAlpha(fader, SDL_SRCALPHA, 128);
    
    /* FIXME: constant for image name */
    if(NULL == (logo = IMG_Load("menu_logo.png")))
    {
        fprintf(stderr,"Couldn't load datafile: %s\n", SDL_GetError());
        return NULL;
    }
    
    event_engine_clear(); //TODO: Fix eventengine so this isnt necessary
    
    return rstate;
}

void menu_cleanup(run_state rstate)
{
    int i;

    assert(rstate != NULL);
    
    for(i = 0; i < ARRAY_ELEMENTS(menu_entries); i++)
        SDL_FreeSurface(menu_entries[i].surface);
    
    SDL_FreeSurface(background);
    SDL_FreeSurface(fader);
    SDL_FreeSurface(logo);

    /* Free keymap and eventengine */
    keyhandler_clearmap(rstate->keymap);
    free(rstate->eventengine);
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
