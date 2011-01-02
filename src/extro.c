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

/* $Id: extro.c 910 2005-09-14 21:38:13Z mattias $ */


/*
 Extro, identical to intro for now
 */


#include "global.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "states.h"
#include "eventengine.h"
#include "quit.h"


/* State local info */
static SDL_Surface *extro_image = NULL;
const char *EXTRO_IMAGE_FILE = "extro.png";
volatile int end_extro = 0;

/* The eventhandler */
static int keymouse_eh(SDL_Event *event)
{
    assert(NULL != event);
    assert(event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN);

    end_extro = 1;

    return TRUE;
}


/* Update the screen, for this state */
static void extro_update(void)
{
    assert(NULL != extro_image);

#ifndef NDEBUG
    if(0 > SDL_BlitSurface(extro_image, NULL, screen, NULL))
        fprintf(stderr, "TODO: I need to exit here, how do I do that??\n");
    else
        SDL_Flip(screen);
#else
    SDL_Flip(screen);
#endif
}



/* State run function */
static state extro_run(void)
{
    assert(!end_extro && extro_image != NULL);
    
    /* Blit the surface to screen */
    if(0 > SDL_BlitSurface(extro_image, NULL, screen, NULL))
    {
        fprintf(stderr, "TODO: I need to exit here, how do I do that??\n");
    }
    
    extro_update();

    /* Main loop */
    while (!end_extro && event_engine_run(EVENTMODE_RUN, 100))
    {
#if 0
        static long loop = 0;
        printf("In main extro loop: iteration %ld\n", ++loop);
#endif
    }
    
    /* Leaving the game */
    return STATE_EXIT; 
}


/* State initialiser */
run_state extro_init(run_state rstate)
{
    assert(NULL != rstate);

    /* Load the extro image */
    extro_image = IMG_Load(EXTRO_IMAGE_FILE);
    if(NULL == extro_image)
    {
        fprintf(stderr,"Couldn't load extro datafile: %s\n",SDL_GetError());
        
        return NULL;
    }

    /* Set the function pointers */
    rstate->update = extro_update;
    rstate->run = extro_run;
    rstate->extra = NULL; //No extra state init

    /* Set up the eventengine */
    event_handler_add(updatehandler_eh, SDL_USEREVENT);
    event_handler_add(keymouse_eh, SDL_KEYDOWN);
    event_handler_add(keymouse_eh, SDL_MOUSEBUTTONDOWN);
    event_handler_add(quithandler_eh, SDL_QUIT);
    
    if(NULL == (rstate->eventengine = event_engine_save()))
        return NULL;
    
    event_engine_clear(); //TODO: Fix eventengine so this isnt necessary
    
    return rstate;
}

/* Clean up extro structures */
void extro_cleanup(run_state rstate)
{
    assert(extro_image != NULL);
    assert(rstate != NULL);
    
    SDL_FreeSurface(extro_image);
    free(rstate->eventengine);
    
    return;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
