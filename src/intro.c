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

/* $Id: intro.c 910 2005-09-14 21:38:13Z mattias $ */


/* This is the game intro, where we
   say "Nice of you to start his
   mighty fine ol' game, now go play"

   It only handles two events
   _mousebutton adn _keydown (Which key doesnt matter)
   and then leaves the intro state

   This is where all loading and precomputing
   should be done. In the future we should probably
   have a progressmeter and exit when all loading is
   done
   
   Splashscreen?
   
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
static SDL_Surface *intro_image = NULL;
const char *INTRO_IMAGE_FILE = "intro.png";
volatile int end_intro = 0;

/* The eventhandler */
static int keymouse_eh(SDL_Event *event)
{
    assert(NULL != event);
    assert(event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN);

    /* Signal that the player don't want to 
       look at the ultra-cool-supernice-megafine
       intro anymore
    */
    end_intro = 1;

    return TRUE;
}


/* Update the screen, for this state */
static void intro_update(void)
{
    assert(screen != NULL);
    assert(intro_image != NULL);

#ifndef NDEBUG
    if(0 > SDL_BlitSurface(intro_image, NULL, screen, NULL))
    {
        fprintf(stderr, "TODO: I need to exit here, how do I do that??\n");
    }
    else
        SDL_Flip(screen);
#else
    SDL_Flip(screen);
#endif
}



/* State run function */
static state intro_run(void)
{
    assert(!end_intro && intro_image != NULL);
    
    /* Blit the surface to screen */
    if(0 > SDL_BlitSurface(intro_image, NULL, screen, NULL))
    {
        fprintf(stderr, "TODO: I need to exit here, how do I do that??\n");
    }
    
    intro_update();

    /* Main loop */
    while (!end_intro && event_engine_run(EVENTMODE_RUN, 100)) 
    {
#if 0
        static long loop = 0;
        printf("In main intro loop: iteration %ld\n", ++loop);
#endif
    }
    
    return STATE_MENU;
}


/* State initialiser */
run_state intro_init(run_state rstate)
{
    assert(rstate != NULL);

    /* Load the intro image */
    intro_image = IMG_Load(INTRO_IMAGE_FILE);
    if(NULL == intro_image)
    {
        fprintf(stderr,"Couldn't load intro datafile: %s\n",SDL_GetError());
        
        return NULL;
    }

    /* Set the function pointers */
    rstate->update = intro_update;
    rstate->run = intro_run;
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

/* Clean up intro structures */
void intro_cleanup(run_state rstate)
{
    assert(intro_image != NULL);
    assert(rstate != NULL);
    
    SDL_FreeSurface(intro_image);
    free(rstate->eventengine);
    
    return;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
