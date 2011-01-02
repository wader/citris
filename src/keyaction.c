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

/* $Id: keyaction.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <SDL.h>

#include "tetris.h"
#include "keyaction.h"

/* TODO: Organize and/or remove globals */
extern tetris *game;
extern SDL_Surface *screen;
extern void update(void);


/* Theese are all of the available actions
   that can occur from key presses and a
   utility function to map a action name into
   a function pointer
   
   With the notable exception of ESC
   which is handled separately

   All names are lower case and are prefixed as follows:
   game_  : Action on the playing field
   syswm_ : Misc stuff that interacts with the system
   (more to come)
*/

typedef struct {
    key_action action;
    const char *name;
} keyactions;

const keyactions keyaction_list[] = 
{
    {game_block_clockw, "block_clockwise"},
    {game_block_cclockw, "block_counterclockwise"},
    {game_block_rotate, "block_rotate"},
    {game_block_down, "block_down"},
    {game_block_drop, "block_drop"},
    {syswm_alter_fullscreen , "alter_fullscreen"},
    {menu_up , "menu_up"},
    {menu_down , "menu_down"},
    {menu_select , "menu_select"},
    {NULL, NULL}
};

/* Mapping from name to function pointer */
key_action keyaction_name2action(const char *name)
{
    const keyactions *iterator = keyaction_list;

    assert(NULL != name);
  
    for(iterator = keyaction_list; iterator->name != NULL; iterator++)
        if(0 == strcmp(name, iterator->name))
            return iterator->action;
    
    return NULL;
}

/* Mapping from action to name */
const char *keyaction_action2name(key_action action)
{
    const keyactions *iterator = keyaction_list;

    assert(NULL != action);

    for(iterator = keyaction_list; iterator->action != NULL; iterator++)
        if(iterator->action == action) 
            return iterator->name;

    return NULL;
}


void game_block_clockw(void)
{
    tetris_shape_move(game, 0, -1, 0);
    update();
}

void game_block_cclockw(void)
{
    tetris_shape_move(game, 0, 1, 0);
    update();
}

void game_block_rotate(void)
{
    tetris_shape_move(game, 1, 0, 0);
    update();
}

void game_block_down(void)
{
    tetris_shape_move_down(game);
    update();
}

void game_block_drop(void)
{
    while(tetris_shape_move_down(game))
        ;
    update();
}

void syswm_alter_fullscreen(void)
{
    Uint32 new_flags;
    SDL_Surface *temp;
  
    if(screen->flags & SDL_FULLSCREEN)
    {
        /* Switch from fullscreen to windowed */
        new_flags = screen->flags & (~SDL_FULLSCREEN);
    }
    else
    {
        /* Switch from windowed to fullscreen */
        new_flags = screen->flags | SDL_FULLSCREEN;
    }
  
    temp = SDL_SetVideoMode(screen->w, screen->h, screen->format->BitsPerPixel, new_flags);
  
    if(NULL == temp || temp->flags != new_flags)
    {
        fprintf(stderr, "Couldn't change videomode\n");

        return;
    }

    screen = temp;

    update();
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
