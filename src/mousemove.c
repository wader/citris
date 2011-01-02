/*
 * Copyright (C)2003 Mattias From <morphriz@yahoo.com>
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

/* $Id: mousemove.c 910 2005-09-14 21:38:13Z mattias $ */


#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "global.h"
#include "tetris.h"
#include "game.h"
#include "update.h"
#include "util.h"


/* The divisor that regulates how much
   mouse movement is reqired to move
   the mouse
*/
#define MOUSE_STEP_INTERVAL 50


int mmovehandler_eh(SDL_Event * event)
{
    int x, y, position, direction;
    float angle;

    assert(SDL_MOUSEMOTION == event->type);
    
    /* distance from center */
    x = event->motion.x - screen->w/2;
    /* invert y, vertical coordinate is reversed on screen */
    y = screen->h/2 - event->motion.y;

    /* FIXME: only when in origo? this seams to work fine anyway :) */
    if(x == 0 || y == 0)
        return TRUE;
    
    /* angle from the x-coodinate in radians */
    angle = atan((float)y/x);
    
    /* translate to "whole" circle angle */
    if(x < 0)
        angle += (float)M_PI;
    else if(x > 0 && y < 0)
        angle += (float)M_PI*2;
    
    /* translate to game grid x position */
    position = (angle*game->width)/(M_PI*2);
    
    /* try to center mouse pointer onto shape */
    /* TODO: calc with angle insted, offset = (((M_PI*2)/game->width)*tetris_shape_frame_width(game,game->shape,game->frame))/2; */
    position = MOD_ROTATE(game->width, position, -(tetris_shape_frame_width(game,game->shape,game->frame) / 2));
    
    /* do we have to move? */
    if(game->x == position)
        return TRUE;
 
    /* find direction of shortest way to the current position */
    direction = mod_shortest_direction(game->width, game->x, position);

    /* move in steps util reaching position */
    while(game->x != position)
        /* stop if something is in our way */
        if(!tetris_shape_move(game, 0, direction, 0))
            break;
    
    /* redraw game screen */
    update();
    
    return TRUE;
}

#ifdef UNDEFINED
int mmovehandler_eh(SDL_Event * event)
{
    int i;

    /* This is used by the _PeepEvents trick */
    const int numevents = 10;
    SDL_Event events[numevents];
    int incoming_events = 0;

    /* The direction to move */
    int direction = 1;

    /* The total move distance */
    Sint32 distance = 0;

    /* The remainder of all divisions 
       are stored in remainder since
       we otherwise would discard all
       mousemovment smaller than 
       MOUSE_STEP_INTERVAL
    */
    static Sint32 remainder = 0;

    assert(event != NULL && event->type == SDL_MOUSEMOTION);

    /* This is the _PeepEvent trick

    The point is that the mousemotion of SDL does not
    have functionality to set a mimimum mousemove
    that would be considerd an event. Thus we get 
    loads and loads of tiny, tiny events to handle.

    Let _PeekEvents() get all pending SDL_MOUSEMOTION
    events and add them up. This will result in one
    big move at a time instead of loads of 1 pixel
    moves

    Of course this may result in bad feel since 
    to quick large moves in opposite directions
    may cancel each other and not be displayed to user

    This can be fixed by only adding keep adding while the sign
    is the same and create a new motion event with the sum of the
    rest and _PostEvent() it
    TODO: The above
    */
    incoming_events = SDL_PeepEvents(events,
                     numevents,
                     SDL_GETEVENT,
                     SDL_EVENTMASK(SDL_MOUSEMOTION));
    if (incoming_events < 0) {
    //Error
    fprintf(stderr, "Error in SDL_PeekEvents(): %s\n", SDL_GetError());
    return FALSE;
    }

#if 1
    /* Calculate the inital distance to move
    */
    distance = event->motion.xrel + event->motion.yrel + remainder;

    /* Add all relative movements to distance    */
    for (i = 0; i < incoming_events; i++) {
    distance += events[i].motion.xrel + events[i].motion.yrel;
    }
#else
    /* This approach is faster but might have unforseen effects, 
       just testing 

       Sum of relative distances should be the same as end - start
    */
    if (incoming_events) {
    --incoming_events;

    distance = (events[incoming_events].motion.x - event->motion.x) +
        (events[incoming_events].motion.y - event->motion.y);

    }

    distance += remainder;

#endif

#if 0
    fprintf(stderr,
        "In mmovehandler_eh(): distance = %d  remainder = %d\n",
        distance, distance % MOUSE_STEP_INTERVAL);
#endif

    if (distance < 0) {
    direction = -1;
    distance *= -1;
    }


    if (MOUSE_STEP_INTERVAL > distance) {
    remainder = direction * distance;

    return TRUE;
    }

    do {
    //Note: negative direction for move
    //      gives clockwise on mouseright
    tetris_shape_move(game, 0, -direction, 0);
    distance -= MOUSE_STEP_INTERVAL;
    } while (distance > MOUSE_STEP_INTERVAL);

    remainder = distance;

    update();
    return TRUE;
}
#endif

int mbuttonhandler_eh(SDL_Event * event)
{
    assert(event != NULL);
    assert(event->type == SDL_MOUSEBUTTONUP);
    assert(event->button.state == SDL_RELEASED);

    switch(event->button.button)
    {
        case SDL_BUTTON_LEFT:
            //Rotate shape
            tetris_shape_move(game, 1, 0, 0);
            break;
        case SDL_BUTTON_RIGHT:
            //Drop the shape
            while (tetris_shape_move_down(game));
            break;
        case SDL_BUTTON_MIDDLE:
            tetris_shape_move_down(game);
            break;
        default:
            /* Something is wrong */
            return FALSE;
    }

    update();
    
    return TRUE;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
