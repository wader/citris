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

/* $Id: game.c 910 2005-09-14 21:38:13Z mattias $ */


#include "global.h"

#include <SDL/SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "game.h"
#include "shapes.h"
#include "gamefield.h"
#include "eventengine.h"
#include "timer.h"
#include "tetris.h"
#include "update.h"
#include "keyhandler.h"
#include "mousemove.h"
#include "states.h"
#include "util.h"
#include "quit.h"


tetris *game = NULL;

static keymap default_keymap[] = 
{
	{"UP",         "block_rotate"},
	{"DOWN",       "block_down"},
	{"SPACE",      "block_drop"},
	{"LEFT",       "block_clockwise"},
	{"RIGHT",      "block_counterclockwise"},
	{"ALT+RETURN", "alter_fullscreen"},
	{NULL, NULL}  
};

static void game_moveblock_cb(void *data)
{
    tetris_shape_move_down(game);

    update();
    
    /* Add a new timed event for this function */
    timed_func_add(
            SDL_GetTicks() + tetris_level_speeds[(game->level < TETRIS_LEVELS ? game->level : TETRIS_LEVELS-1)],
            game_moveblock_cb,
            NULL
            );
}

static void game_update(void)
{
    /* whole surface to black */
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    gamefield_render(screen, game);
    SDL_Flip(screen);
}
    

/* This function should do all
   that is necessary every time 
   the game is restarted
*/
static int game_statechange(run_state new)
{
    assert(game != NULL);
   
    /* Reinitialize the game if it isn't running */
    if(!game->playing)
    {
        /* Reset the field, score and position etc */
        tetris_reset(game);

        /* Start the game */
        tetris_start(game);
    }
    
    /* First add of callback to move block */
    timed_func_add(
            SDL_GetTicks() + tetris_level_speeds[(game->level < TETRIS_LEVELS ? game->level : TETRIS_LEVELS-1)],
            game_moveblock_cb,
            NULL
            );
    
    return TRUE;
}

/* Thu "main" loop of this state

 */
static state game_run(void)
{
    while (game->playing && event_engine_run(EVENTMODE_RUN, 25))
    {
#if 0
        static long loop = 0;
        
        printf("In main game loop: iteration %ld\n", ++loop);
#endif
    }

    /* Remove the moveblock timer */
    timed_func_clear();
   
    if(game->playing)
        return STATE_MENU;
    else
        return STATE_GAMEOVER; 
}

/* This initializes a game state

 */
run_state game_init(run_state rstate)
{
    /* Create a keymap */
    if(NULL == (rstate->keymap = keyhandler_createmap(NULL, default_keymap)))
        return NULL;

    /* Set the function pointers */
    rstate->update = game_update;
    rstate->run = game_run;
    rstate->extra = game_statechange;
    
    /* Just a precausion */
    rstate->data = NULL;
    
    /* Allocate a game initialized in game_statechange */
    game = tetris_alloc(GAME_WIDTH, GAME_HEIGHT);
    if(game == NULL)
        return NULL;
    
    tetris_shape_array_set(game, shape_array, ARRAY_ELEMENTS(shape_array));

    /* Set up the event engine */
    event_handler_add(keyhandler_eh, SDL_KEYDOWN);
    event_handler_add(mmovehandler_eh, SDL_MOUSEMOTION);
    event_handler_add(mbuttonhandler_eh, SDL_MOUSEBUTTONUP);
    event_handler_add(quithandler_eh, SDL_QUIT);
    event_handler_add(updatehandler_eh, SDL_USEREVENT);

    if(NULL == (rstate->eventengine = event_engine_save()))
        return NULL;
	
    event_engine_clear(); //TODO: Fix eventengine so this isnt necessary
  
    return rstate;
}

/* Cleanup all game data */
void game_cleanup(run_state rstate)
{
#if 0 
    fprintf(stderr,"Cleaning up game\n");
    fflush(stderr);
#endif
    tetris_free(game);

    keyhandler_clearmap(rstate->keymap);
    free(rstate->eventengine);
    
    return;
}



/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
