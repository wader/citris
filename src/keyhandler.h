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

/* $Id: keyhandler.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include <SDL.h>
#include "keyaction.h"

/* A keymapping */
typedef struct {
  char *key_name;
  char *action_name;
} keymap;

/* Sets the active map for the key eventhandler 
   Pass NULL to clear the map
*/
void keyhandler_setmap(key_action **actions);


/* Create or append to a key_action function pointer map */
key_action **keyhandler_createmap(key_action **fptr_map, keymap *keymapping);


/* Clean up a map */
void keyhandler_clearmap(key_action **fptr_map);


/* The event handler for keyboard events */
int keyhandler_eh(SDL_Event * event);

#endif
