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

/* $Id: states.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef STATES_H
#define STATES_H

#include "keyhandler.h"
#include "update.h"

/* Available states */
typedef enum {
    STATE_ERROR= -2,
    STATE_EXIT = -1,
    STATE_GAME = 0,
/*    STATE_INTRO = 1, */
/*    STATE_EXTRO,*/
    STATE_MENU,
    STATE_GAMEOVER,

    NUM_STATES
} state;


/* Pointer type for states
 */
typedef struct run_state * run_state;

/* Prototype for user defined statechange functions */
typedef int (*state_change_f)(run_state new);

/* Protoype for the per state functions
   that acts as "main loop" for the state

   Return the next state to enter
 */
typedef state (*state_run_f)(void);

/* Proto for state inits and cleanups */
typedef run_state (*state_init_f)(run_state);
typedef void (*state_cleanup_f)(run_state);

/* Main state loop
   Where it all happens
 */
int state_loop(const run_state states, state to_run);



/* Data common to most states */
struct run_state 
{
    /* Mandatory members */
    void *eventengine;
    update_f update;
    state_run_f run;
    
    /* Non mandatory members */
    key_action **keymap;
    state_change_f extra;
    void *data;
};


#endif
