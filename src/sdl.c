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

/* $Id: sdl.c 910 2005-09-14 21:38:13Z mattias $ */


#ifdef WIN32
#include <direct.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL.h>
#include <SDL/SDL_ttf.h>

#include "global.h"
#include "eventengine.h"
#include "states.h"
#include "timer.h"
#include "util.h"
#include "quit.h"
#include "shapes.h"
#include "game.h"
#include "intro.h"
#include "extro.h"
#include "menu.h"
#include "gameover.h"
#include "gamefield.h"
#include "ai.h"

SDL_Surface *screen;

/* Struct to keep init's and cleanups in */
typedef struct
{
    state_init_f init;
    state_cleanup_f cleanup;
} states_ic;

/* Quick access for all inits and cleanups */
const states_ic all_states_ic[NUM_STATES] =
{
    {game_init, game_cleanup},
    /*{intro_init, intro_cleanup},*/
    /*{extro_init, extro_cleanup},*/
    {menu_init, menu_cleanup},
    {gameover_init, gameover_cleanup}
};

static struct run_state all_states[NUM_STATES];
			      
int debughandler_eh(SDL_Event * event)
{
    switch (event->type)
    {
        case SDL_ACTIVEEVENT:
        case SDL_SYSWMEVENT:
        case SDL_VIDEORESIZE:
        case SDL_VIDEOEXPOSE:
            fprintf(stderr, "Got videoevent\n");
            break;;
        case SDL_QUIT:
            fprintf(stderr, "Got quitevent\n");
            break;;
        default:
            fprintf(stderr, "Debughandler got unknown event\n");
            return FALSE;
    }

    fflush(stderr);

    return TRUE;
}

/* wrapper to fit into quit cb system */
static void sdl_cleanup(void *data)
{
    SDL_Quit();
}

/* wrapper */
static void ttf_cleanup(void *data)
{
    TTF_Quit();
}

int main(int argc, char **argv)
{
    int i;
  
    /* WARNING: Never commit SDL_INIT_NOPARACHUTE to cvs!! */
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    /*if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) != 0) */
    {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }
    quit_addcb(sdl_cleanup, NULL);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); 
    SDL_WM_SetCaption("Citris " VERSION, NULL);

    //screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        fprintf(stderr, "SDL_SetVidMode: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }

    if(TTF_Init() != 0)
    {
        fprintf(stderr, "TTF_Init: %s\n", SDL_GetError());

        return EXIT_FAILURE;
    }
    quit_addcb(ttf_cleanup, NULL);

    /* various inits, random, [start,data]_dir, also adds a cleanup quit cb */
    if(!util_init())
    {
        fprintf(stderr, "util_init: failed\n");
        
        return EXIT_FAILURE;
    }
    
    /* must be after util_init */
    /* all init functions wants game data in current working directory */
    if(chdir(data_dir) == -1)
    {
        perror(data_dir);
        
        return EXIT_FAILURE;
    }
    
    /* currently a dummy, adds a cleanup quit cb */
    if(!timed_func_init())
    {
        fprintf(stderr, "timed_func_init: failed\n");
        
        return EXIT_FAILURE;
    }

    /* translate shape data, also adds a cleanup quit cb */
    if(!shapes_init())
    {
        fprintf(stderr, "shapes_init: failed\n");

        return EXIT_FAILURE;
    }
    
    /* sets inital gamefield render settings */
    if(!gamefield_init())
    {
        fprintf(stderr, "gamefield_init: failed\n");

        return EXIT_FAILURE;
    }
    
    /* sets inital gamefield render settings */
    if(!ai_init())
    {
        fprintf(stderr, "ai_init: failed\n");

        return EXIT_FAILURE;
    }
    
    /* init event engine, also adds a cleanup quit cb */
    if(!event_engine_init())
    {
        fprintf(stderr, "event_engine_init: failed\n");
        
        return EXIT_FAILURE;
    }

    /* Init all used states */
    for(i = 0; i < ARRAY_ELEMENTS(all_states_ic); i++)
    {
        if(NULL == all_states_ic[i].init(&all_states[i]))
        {
            fprintf(stderr, "state %d init: failed\n", i);

            return EXIT_FAILURE;
        }
    }

    /* chdir back to where we come from */
    if(chdir(start_dir) == -1)
    {
        perror(start_dir);

        return EXIT_FAILURE;
    }

    /* Start the first state */
    if(!state_loop(all_states, STATE_MENU))
        fprintf(stderr, "state_loop: failed\n");
    
    /* Cleanup all used states */
    for(i = 0; i < ARRAY_ELEMENTS(all_states_ic); i++)
        all_states_ic[i].cleanup(&all_states[i]);
    
    return EXIT_SUCCESS;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
