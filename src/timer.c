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

/* $Id: timer.c 910 2005-09-14 21:38:13Z mattias $ */


#include <SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "timer.h"
#include "quit.h"


struct _timed_func
{
    Uint32 time;
    timercb_f function;
    void *data;
    struct _timed_func *next;
};


static timed_func queue = NULL;

void timed_func_add(Uint32 time, timercb_f function, void *data)
{
    timed_func new = NULL;

    assert(NULL != function);
    
    new = malloc(sizeof(struct _timed_func));
    if(NULL == new)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(EXIT_FAILURE);
    }

    new->time = time;
    new->function = function;
    new->data = data;

    /* Add the new entry to the queue */
    if(NULL != queue)
    {
        timed_func temp = queue;

        /* Check if this will be added first */
        if(time < queue->time)
        {
            new->next = queue;
            queue = new;

            return;
        }

        while(temp->next != NULL && time > temp->next->time)
            temp = temp->next;

        /* temp is now the last node with smaller time than new */
        new->next = temp->next;
        temp->next = new;
    }
    else
    {
        /* We're adding the first entry in the queue */
        new->next = NULL;
        queue = new;
    }
}


/* Call all functions with time less than current tick value */
void timed_func_call()
{
    timed_func temp;

    while(queue != NULL && queue->time < SDL_GetTicks())
    {
        queue->function(queue->data);
        
        temp = queue;
        queue = queue->next;
        free(temp);
    }
}

void timed_func_clear()
{
    while(queue != NULL)
    {
        timed_func temp;
        
        temp = queue;
        queue = queue->next;
        free(temp);
    }

    queue = NULL;
}

static void timed_func_cleanup(void *data)
{
    timed_func_clear();
}

int timed_func_init()
{
    quit_addcb(timed_func_cleanup, NULL);

    return TRUE;
}

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
