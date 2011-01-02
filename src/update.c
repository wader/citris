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

/* $Id: update.c 910 2005-09-14 21:38:13Z mattias $ */


#include "SDL/SDL.h"
#include <stdio.h>
#include <assert.h>

#include "global.h"
#include "update.h"

static update_f perform_update = NULL;

/* This is the global function that registers 
   updateevents
 */
void update()
{
    static SDL_UserEvent post_update =
    {
        SDL_USEREVENT,
        0,
        NULL,
        NULL
    };
    
    if(0 > SDL_PushEvent((SDL_Event *)&post_update))
        fprintf(stderr, "Error in SDL_PushEvent(): %s\n", SDL_GetError());
}

/* Set the updatefunction */
void update_setfunc(update_f upf)
{
    assert(NULL != upf);

    perform_update = upf;
}

/* The handler for updateevents
   Removes _all_ pending updateevents
 */
int updatehandler_eh(SDL_Event * event)
{
    /* This is used to discard all update
       events on the queue, only necessary to perform once
    */
    static SDL_Event events[10];

    /* No updatefunction warrants an abort */
    assert(perform_update != NULL);
    assert(event != NULL);

#if 0
    {
	int retval = SDL_PeepEvents(events, 10, SDL_GETEVENT,
				SDL_EVENTMASK(SDL_USEREVENT));
	if (retval)
	    fprintf(stderr, "updatehandler_eh: Removed %d updateevents\n", retval);
    }
#else
    SDL_PeepEvents(events, 10, SDL_GETEVENT, SDL_EVENTMASK(SDL_USEREVENT));
#endif
   
    perform_update();

    return TRUE;
}
