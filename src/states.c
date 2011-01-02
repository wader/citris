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

/* $Id: states.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdio.h>
#include <assert.h>

#include "global.h"
#include "keyhandler.h"
#include "eventengine.h"
#include "update.h"

#include "states.h"


/* This function changes the most
   common elements of a state
   It also calls a the per state
   change function if one is defined
*/
static int state_change(const run_state new)
{
    assert(NULL != new);

    /* Clear the event engine
       Do this first to minimise the number of events that
       end up in the wrong state

       When the ev.eng. is cleared all ignorable events are 
       _filtered_ out
    */
    if(!event_engine_clear())
        return FALSE;
  
    /* Call the extra state change function
       This is to guarantee that data that
       might be needed by the update function
       are in order
    */
    if(NULL != new->extra && !new->extra(new))
        return FALSE;
  
    /* Set the update function */
    update_setfunc(new->update);

    /* Set the keymapping */
    keyhandler_setmap(new->keymap);

    /* Set (and thus reenable) the event engine */
    if(!event_engine_restore(new->eventengine))
        return FALSE;

    /* Post an update to guarantee
       that the scren is redrawn
     */
    update();

    return TRUE;
}

/* Main state loop
   Where it all happens
 */
int state_loop(const run_state states, state to_run)
{
    /* Check so we don't start off with
       strange values */
    assert(to_run > STATE_ERROR);
    assert(to_run < NUM_STATES);
    assert(states != NULL);

    while(to_run > STATE_ERROR && to_run < NUM_STATES)
        switch(to_run)
        {
            /* Error inside a state */
            case NUM_STATES:
            case STATE_ERROR:
                break;
            
            /* Exit from stateloop
               The only way to return true
            */
            case STATE_EXIT:
                return TRUE;
                break;

            /* A running game 
             */
            case STATE_GAME:
            /*case STATE_INTRO:*/
            /*case STATE_EXTRO:*/
            case STATE_MENU:
            case STATE_GAMEOVER:
                if(!state_change(states + to_run))
                    break;
                to_run = states[to_run].run();
                break;
            default:
                fprintf(stderr, "State %d not implemented", to_run);
                return FALSE;
        }
    
    return FALSE;
}



/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */

