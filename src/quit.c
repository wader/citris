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

/* $Id: quit.c 910 2005-09-14 21:38:13Z mattias $ */


/* This is the quit system.

   It's aim is to gracefully shutdown the application
   under not so favorable circumstances.

   It consists of a stock quitevent handler and
   a "registy" of functions to call on exit
   The exit system is a better version of atexit()
   where the number of exit calls is not limited and
   callbacks are removable.

   TODO: This!
   Eventually I will add a state "pop" function here where
   some errors can be handled gracefully, as opposed to simply
   exiting or continuing with only a small report on stderr as
   it's done know
*/
#include "global.h"

#include <SDL/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "quit.h"

/* This is the _stock_ quitevent handler
   It halts the application by terminating the eventloop,
   stateloop, et. al. 
   We do _not_ call exit, the loops should have a chance to
   finish themselves.
   
   If this handler is called from within a custom handler
   it must be called last!!
   i.e. return quitevent_eh(event);
*/

int quithandler_eh(SDL_Event * event)
{
    assert(event->type == SDL_QUIT);

    fprintf(stderr,"TODO: Fix me! The out of date quiteventhandler!!!!!\n");
    fflush(stderr);

    return FALSE;
}

/* End callbacks with argument */
struct quitcb_entry
{
    quit_cb func;
    void *arg;
};

/* Global registry of callbacks 
   It should operate in a LIFO manner

   You just gotta love all these static
   registries!!!!! :)

   Man if I have to threadsafe this code!
   Should be about as messy as having oral
   sex with an alligator
 */
struct quitcb_entry *exit_callbacks = NULL;
int num_callbacks = 0;

/* A hint to see if quit_atexit has succesfully
   been atexit() registered */
int atexit_success = 0;

/* This callback is registered with atexit()
   (un)standard only guarantes a small number of 
   functions to be called successfully via atexit()
   To be safe this should be the ONYL function
   added that way!!!
*/
static void quit_atexit(void)
{
    assert(1 <= num_callbacks || NULL != exit_callbacks);

    while(0 <= --num_callbacks)
        exit_callbacks[num_callbacks].func(exit_callbacks[num_callbacks].arg);
}

/* Add a callback to the registry
 */
int quit_addcb(quit_cb func, void *arg)
{
    void *temp;

    assert(NULL != func);

    if(!atexit_success)
    {
        /* First time around 
           
           This branch may also be reached if
           enough callbacks are added and subsequently
           removed from the registry. It should not matter
           as long as we only add quit_atexit using atexit
           a few times
         */

        if(0 > atexit(quit_atexit))
        {
            /* We do _not_ exit in this branch 
               since the error might not be permament
               and if it is the use will se alot
               of WARNING messages
            */
            fprintf(stderr,"WARNING: Could not atexit() handler\n");
            fflush(stderr);
        }
        else
        {
#if 0
            fprintf(stderr,"Sucessfully registered quit_atexit()\n");
#endif
            atexit_success = 1;
        }
    }

    /* Reallocate memory for one more quitcb_entry */
    temp = realloc(exit_callbacks, (num_callbacks+1) * sizeof(struct quitcb_entry));
    if(NULL == temp)
    {
        perror("Couldn't realloc exit_callbacks\n");
        
        return FALSE;
    }
    exit_callbacks = temp;

    /* Set the members */
    exit_callbacks[num_callbacks].func = func;
    exit_callbacks[num_callbacks].arg = arg;
    
    num_callbacks++;
    
    return TRUE;
}

/* This function is used to remove callbacks from the registry
 */
int quit_removecb(quit_cb func)
{
    int i;
    
    /* Should this really be an assertion?
       might be OK just to return */
    assert(NULL != func);

    for(i=0; i < num_callbacks && exit_callbacks[i].func != func; i++)
        ;
    
    /* Check if the callback was found */
    if(i == num_callbacks)
        return FALSE; 
    
    /* Check that the callback is in 
       the middle of the registry 
       
       if it's last, do nothing
    */
    if(i < num_callbacks - 1)
    {
        /* memmove the remaining stuff */
        /* This preserves the order and thus the LIFO behaviour of the registry */
        memmove(exit_callbacks + i, exit_callbacks + i + 1, num_callbacks - (i + 1)); 
    }
    
    /* We don not free any memory
       This is very little memory and it will be 
       corrected on the next call to _addcb or
       freed if we exit 
       
       It should however be freed if we've just removed
       the last callback. It isn't, for the same reason as
       stated above, necessary, bu think it this case
       it's good style :)
    */
    if(0 == --num_callbacks)
    {
        free(exit_callbacks);
        exit_callbacks = NULL;
    }

    return TRUE;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
