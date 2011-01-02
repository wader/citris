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

/* $Id: eventengine.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef EVENTENGINE_H
#define EVENTENGINE_H

#include <SDL.h>

/* proto for the eventhandler functions */
typedef int(*event_handler_f) (SDL_Event *);


/* This is test to see if a specific SDL_Event is ignorable */
#define EVENT_IGNORABLE(a) ((a) >= SDL_USEREVENT || ( (a) > SDL_ACTIVEEVENT && (a) < SDL_QUIT))

/* Eventengine functions */

int event_engine_clear(void);
int event_engine_init(void);
int event_handler_add(event_handler_f handler, int type);
int event_handler_remove(event_handler_f handler, int type);

/* throttle causes the event loop to take a minimum of throttle
   time to complete a run through the loop
*/

#define EVENTMODE_WAIT 1
#define EVENTMODE_RUN  0
#define NO_THROTTLE    0

int event_engine_run(int event_mode, int throttle);


/* This function restores a set of handlers
   saved with _save
   
   It appends to contents of the saved data 
   to the current handlers
*/
int event_engine_restore(void *storage);


/* This function saves all current event handlers
   It does _not_ remove och alter the register
   in any way, a copy is returned

 */
void *event_engine_save(void);



#endif              /* EVENTENGINE_H */
