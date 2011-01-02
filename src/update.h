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

/* $Id: update.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef UPDATE_H
#define UPDATE_H

#include "SDL/SDL.h"

/* A type for updatefunctions */
typedef void (*update_f)(void);


/* This is the global function that registers 
   updateevents
 */
void update();


/* Set the updatefunction */
void update_setfunc(update_f upf);


/* The handler for updateevents
   Removes _all_ pending updateevents
 */
int updatehandler_eh(SDL_Event * event);


#endif
