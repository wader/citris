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

/* $Id: shapes.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "tetris.h"
#include "quit.h"

shape shape1 =
{
    1, 2, 2, /* frames, width, height */
    "11"
    "11"
};

shape shape2 =
{
    2, 3, 3,
    " 2 " "22 "
    "22 " " 22"
    "2  " "   "
};

shape shape3 =
{
    2, 3, 3,
    "3  " " 33"
    "33 " "33 "
    " 3 " "   "
};

shape shape4 =
{
    2, 4, 4,
    " 4  " "    "
    " 4  " "    "
    " 4  " "4444"
    " 4  " "    "
};

shape shape5 =
{
    4, 3, 3,
    " 5 " "   " "55 " "  5"
    " 5 " "555" " 5 " "555"
    " 55" "5  " " 5 " "   "
};

shape shape6 =
{
    4, 3, 3,
    " 6 " "6  " " 66" "   "
    " 6 " "666" " 6 " "666"
    "66 " "   " " 6 " "  6"
};

shape shape7 =
{
    4, 3, 3,
    " 7 " " 7 " "   " " 7 "
    "777" " 77" "777" "77 "
    "   " " 7 " " 7 " " 7 "
};

shape *shape_array[7] = {
    &shape1,
    &shape2,
    &shape3,
    &shape4,
    &shape5,
    &shape6,
    &shape7
};


static void shapes_cleanup()
{
    int i;
    
    for(i = 0; i < ARRAY_ELEMENTS(shape_array); i++) 
        free(shape_array[i]->data);
}

int shapes_init()
{
	int i, j;
    shape *s;
    
    /* Translate "human readable" shape data into numbers */
    /* "1" -> 1, ... */
    /* Exception: " " -> 0 */
    for(j = 0; j < ARRAY_ELEMENTS(shape_array); j++) 
    {
        s = shape_array[j];
        
        /* some (newer) gcc versions put string constant in read-only memory */
        s->data = strdup(s->data);
        if(s->data == NULL)
            return FALSE;

        for(i = 0; i < s->height*s->width*s->frames; i++)
            if(*(s->data+i) == ' ')
                *(s->data+i) = 0;
            else
                *(s->data+i) = *(s->data+i) - '0';
    }

    quit_addcb(shapes_cleanup, NULL);

    return TRUE;
}

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
