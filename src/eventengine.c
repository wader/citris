/*
 * Copyright (C)2000 Mattias From <sirfrom@geocities.com>
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

/* $Id: eventengine.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <SDL.h>

#include "global.h"
#include "eventengine.h"
#include "timer.h"
#include "quit.h"


#ifdef _REENTRANT
#warning "This software is not reentrant"
#endif

/* Array of dynamic event_handler_f arrays
   and how many of each has been installed
*/
static event_handler_f *event_register[SDL_NUMEVENTS];
static int event_numhands[SDL_NUMEVENTS];


/* This clearfunction can be called many times
   Should be called everytime the running mode
   of the game changes. 
*/
int event_engine_clear(void)
{
    int i;

    for(i = 0; i < SDL_NUMEVENTS; i++)
    {
        if(event_numhands[i])
        {
            free(event_register[i]);
            event_numhands[i] = 0;
        }
        
        event_register[i] = NULL;

        if(EVENT_IGNORABLE(i))
            SDL_EventState(i, SDL_IGNORE);
    }

    return TRUE;
}

static void event_engine_cleanup(void *data)
{
    event_engine_clear();
}

/* Simple init, just does a clear for now..
   might do some adding of videorelated timing
   in here
*/
int event_engine_init(void)
{
    int i;

    for(i = 0; i < SDL_NUMEVENTS; i++)
    {
        event_numhands[i] = 0;
        event_register[i] = NULL;
        if(EVENT_IGNORABLE(i))
            SDL_EventState(i, SDL_IGNORE);
    }

    quit_addcb(event_engine_cleanup, NULL);

    return TRUE;
}

/* Add an event handler for a given event
 */
int event_handler_add(event_handler_f handler, int type)
{
    int numregs = event_numhands[type];
    void *temp;

    assert(handler);
    assert(type < SDL_NUMEVENTS);

    if(!event_numhands[type] && EVENT_IGNORABLE(type))
        SDL_EventState(type, SDL_ENABLE);

    temp = realloc(
            event_register[type],
            ++event_numhands[type] * sizeof(event_handler_f)
            );

    if(NULL == temp)
        return FALSE;

    event_register[type] = temp;
    event_register[type][numregs] = handler;

    return TRUE;
}

/* Remove a specific handler
 */
int event_handler_remove(event_handler_f handler, int type)
{
    int i;

    assert(handler);

    if(!event_register[type] || !event_numhands[type])
        return FALSE;

    /* Special case: one handler */
    if(event_numhands[type] == 1)
    {
        if(event_register[type][0] != handler)
            return FALSE;

        free(event_register[type]);

        event_register[type] = NULL;
        event_numhands[type] = 0;

        SDL_EventState(type, SDL_IGNORE);
        
        return TRUE;
    }

    /* more than one handler */
    for(i = 0; i < event_numhands[type]; i++)
        if(event_register[type][i] == handler)
        {
            /* Move the last handler to the pos of the removed
               TODO: Preserve the order of the handlers
            */
            if(i < (event_numhands[type] - 1))
                event_register[type][i] = event_register[type][event_numhands[type] - 1];

            event_register[type] = realloc(
                    event_register[type],
                    event_numhands[type] * sizeof(event_handler_f)
                    );

            if(!event_register[type])
                return FALSE;

            return TRUE;
        }

    return FALSE;
}

/* Take care of incoming events 

WARNING: Posting events inside eventhandlers on a regular basis
may result in infinte loops

TODO: Fix the infite loop bug described above
*/
int event_engine_run(int event_mode, int throttle)
{
    SDL_Event event;
    int i;
    Uint32 trigticks = 0, ticks = 0;

    do
    {
        if(throttle)
        {
            ticks = SDL_GetTicks();
            trigticks = ticks + throttle;
        }

        while(SDL_PollEvent(&event))
        {
#ifdef DEBUG
            static long iter = 0;
            fprintf(stderr, "In eventengine_run: iteration %ld\n", ++iter);
#endif
            i = 0;

            while(i < event_numhands[event.type])
            {
                /* Exit if the eventhandler returns false */
                if(!(event_register[event.type][i] &&
                     event_register[event.type][i](&event)))
                    return FALSE;

                i++;
            }
        }

        /* This function calls all timed functions */
        timed_func_call();

        if(throttle)
        {
            if(trigticks > (ticks = SDL_GetTicks()))
                SDL_Delay(trigticks - ticks);
        }
        
        /* Sets up the event loop according to current event mode */
    } while (event_mode);

    return TRUE;
}

/* This function saves all current event handlers
   It does _not_ remove och alter the register
   in any way, a copy is returned
*/
void *event_engine_save(void)
{
    void *retval = NULL;
    int *numhands_storage;
    event_handler_f *function_storage;
    int size = sizeof(event_numhands);
    int i;


    for(i = 0; i < SDL_NUMEVENTS; i++)
        size += sizeof(event_handler_f) * event_numhands[i];

    /* size is not the total size of the integer array
       event_numhands and all the function pointers */
    retval = malloc(size);
    if(NULL == retval)
    {
        perror("malloc");
        return NULL;
    }

    /* Copy the integers */
    numhands_storage = retval;
    memcpy(numhands_storage, event_numhands, sizeof(event_numhands));

    /* Copy the pointers */
    function_storage = (void *) (numhands_storage + SDL_NUMEVENTS);
    for(i = 0; i < SDL_NUMEVENTS; i++)
        if (event_numhands[i]) {
            /* The actual pointers are copied, not their storage in
               the event_register array */
            memcpy(
                function_storage,
                event_register[i],
                sizeof(event_handler_f) * event_numhands[i]
                );
            function_storage += event_numhands[i];
        }

    return retval;
}

/* This function restores a set of handlers
   saved with _save
   
   It appends to contents of the saved data 
   to the current handlers
*/
int event_engine_restore(void *storage)
{
    int i, j;

    int *numhands = storage;
    event_handler_f *handlers = NULL;

    assert(NULL != storage);

    handlers = (void *) (numhands + SDL_NUMEVENTS);

    for(i = 0; i < SDL_NUMEVENTS; i++)
        for(j = 0; j < numhands[i]; j++)
        {
            event_handler_add(*handlers, i);
            handlers++;
        }

    return TRUE;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
