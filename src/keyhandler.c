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

/* $Id: keyhandler.c 910 2005-09-14 21:38:13Z mattias $ */


#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL.h>

#include "keyaction.h"
#include "keynames.h"
#include "keyhandler.h"

/* number of mod key combinations */
#define MOD_COMBS 4

key_action *keyconfig[MOD_COMBS] = {NULL};

/* These convert modifiers into indices
   and the other way around

   SHIFT is a key not a mod..
*/
#define MOD_TO_INDEX(a) ((a & KMOD_ALT ? 2 : 0) | (a & KMOD_CTRL ? 1 : 0))

/* TODO: used? */
const SDLMod INDEX_TO_MOD[] =
{
    KMOD_NONE,
    KMOD_CTRL,
    KMOD_ALT,
    KMOD_ALT | KMOD_CTRL
};

/* Sets the active map for the key eventhandler 
   Pass NULL to clear the map
*/
void keyhandler_setmap(key_action **actions)
{
    int i;
  
    if(actions == NULL)
    {
        for(i=0; i < MOD_COMBS; i++)
            keyconfig[i] = NULL;

        return;
    }

    for(i=0; i < MOD_COMBS; i++)
        keyconfig[i] = actions[i];
}

/* Create or append to a key_action function pointer map */
key_action **keyhandler_createmap(key_action **fptr_map, keymap *keymapping)
{
    int i;

    assert(keymapping != NULL);

    if(NULL == fptr_map)
    {
        fptr_map = malloc(MOD_COMBS * sizeof(key_action *));
        if(NULL == fptr_map)
        {
            perror("malloc");
            return NULL;
        }
    
        for(i=0; i < MOD_COMBS; i++)
            fptr_map[i] = NULL;
    }
    
    /* Assign values */
    while(keymapping->action_name != NULL)
    {
        SDLKey key;
        SDLMod mod;
        key_action action;

        /* Get the keysym/keymod for this key */
        if(SDLK_LAST == keyname_name2key(keymapping->key_name, &key, &mod))
        {
            fprintf(stderr, "Could not find the key %s\n", keymapping->key_name);
            keymapping++;
            continue;
        }

        /* Get the keyaction for this key */
        action = keyaction_name2action(keymapping->action_name);
        if(NULL == action)
        {
            fprintf(stderr, "Could not find keyaction called %s\n", keymapping->action_name);
            keymapping++;
            continue;
        }

        /* Allocate memory if necessary */
        if(NULL == fptr_map[MOD_TO_INDEX(mod)])
        {
            fptr_map[MOD_TO_INDEX(mod)] = malloc(sizeof(key_action) * SDLK_LAST);
            if(NULL == fptr_map[MOD_TO_INDEX(mod)])
            {
                perror("malloc");
                return NULL;
            }
          
            for(i=0; i < SDLK_LAST; i++)
                fptr_map[MOD_TO_INDEX(mod)][i] = NULL;
        }

        fptr_map[MOD_TO_INDEX(mod)][key] = action;
        keymapping++;
    } 

    return fptr_map;
}


/* Free map resources */
void keyhandler_clearmap(key_action **fptr_map)
{
    int i;

    assert(fptr_map != NULL);

    for(i=0; i < MOD_COMBS; i++)
        if(NULL != fptr_map[i])
            free(fptr_map[i]);

    free(fptr_map);
}


int keyhandler_eh(SDL_Event * event)
{
    assert(NULL != event);
    assert(event->type == SDL_KEYDOWN);

    /* Check if there are any keys that handle current sym + mod combination */
    if(NULL != keyconfig[MOD_TO_INDEX(event->key.keysym.mod)])
    {
        /* There are keys defined for this modifier */
        if(NULL != keyconfig[MOD_TO_INDEX(event->key.keysym.mod)][event->key.keysym.sym])
        {
            /* There is a handle for the key in this modset */
            keyconfig[MOD_TO_INDEX(event->key.keysym.mod)][event->key.keysym.sym]();
            return TRUE;
        }
    }

    /* ESC is important enough to warrant
       special treatment 
       If ESC without mod is not handled
       in the current config, post a quitevent
    */
    if(SDLK_ESCAPE == event->key.keysym.sym &&
       (event->key.keysym.mod & (KMOD_ALT|KMOD_CTRL)) == KMOD_NONE
       )
    {
        SDL_QuitEvent quit_event = { SDL_QUIT };
#if 0
        fprintf(stderr, "Posting quitevent\n");
#endif
        if(0 > SDL_PushEvent((SDL_Event *) & quit_event))
        {
            fprintf(stderr, "Unable to shut down cleanly\n");
            exit(EXIT_FAILURE);
        }
    }

    return TRUE;
}

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
