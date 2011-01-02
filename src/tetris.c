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
 */

/* $Id: tetris.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "util.h"
#include "tetris.h"

static int tetris_field_shape_is_valid_position(tetris *t, int shape, int frame, int x, int y);
static void tetris_field_copy_line(tetris *t, int from, int to);
static void tetris_field_move_down(tetris *t, int stop);
static void tetris_field_clear(tetris *t);
static void tetris_shape_next_random(tetris *t);
static void tetris_shape_next(tetris *t);

/* score values like on Game Boy Tetris (not sure) */
int tetris_score_lines[4] = {40,100,300,1200};

/* level speeds, thank to David Necas <yeti@monoceros.physics.muni.cz> */
/* FIXME: calculate at run time? check Davids perl script? */
int tetris_level_speeds[TETRIS_LEVELS] = {1000,886,785,695,616,546,483,428,379,336,298,264,234,207,183,162,144,127,113,100};


/* FIXME: precalc somehow insted? */
/* TODO: this is wrong?! or not? offset and stuff, hmm */
int tetris_shape_frame_width(tetris *t, int shape, int frame)
{
    int i,j,width;

    width = 0;
    
    for(i=0;i < t->shape_array[shape]->height;i++)
        for(j=0;j < t->shape_array[shape]->width;j++)
            if(tetris_shape_block_get(t,shape,frame,j,i) > 0 && j > width)
                width = j;

    return width+1; /* +1, translate from zero based position count */
}

/* returns block number at a given position */
/* 0 is transparent */
/* position is relative to upper left corner in frame */
int tetris_shape_block_get(tetris *t, int shape, int frame, int x, int y)
{
    return
        t->shape_array[shape]->data
        [
	    /* frames are align "left to right" not "top to down" */
	    (y*t->shape_array[shape]->frames*t->shape_array[shape]->width)+ /* offset to line */
	    (frame*t->shape_array[shape]->width)+ /* offset on line to frame */
	    x /* offset in frame "line" */
        ];
}

int tetris_shape_is_valid_position(tetris *t, int shape, int frame, int x, int y)
{
    /* we trust lazy evaluation, if first first expression is false the */
    /* second is skipped. if not we would probably crash */
    return
        tetris_field_shape_is_inside(t,shape,frame,x,y) &&
        tetris_field_shape_is_valid_position(t,shape,frame,x,y);
}

/* all paramters should be relative, -1, 0 or +1 */
/* move down should not be done directly with this function, insted use tetris_shape_move_down */
int tetris_shape_move(tetris *t, int frame, int x, int y)
{
    /* remove shape from field while validating new position */
    tetris_field_shape_clear(t,t->shape,t->frame,t->x,t->y);

    if(
        tetris_shape_is_valid_position(
            t,
            t->shape,
            MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,frame),
            t->x+x,
            t->y+y
            )
        )
    {
        t->frame = MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,frame);
        t->x = MOD_ROTATE(t->width,t->x,x);
        
        t->y += y;
        /* put it back at new position */
        tetris_field_shape_draw(t,t->shape,t->frame,t->x,t->y);
        
        return TRUE;
    }
    
    /* put it back at old position */
    tetris_field_shape_draw(t,t->shape,t->frame,t->x,t->y);

    return FALSE;
}

/* TODO: level clamp? */
int tetris_shape_move_down(tetris *t)
{
    int l;
    
    /* try to move down */
    if(tetris_shape_move(t,0,0,1))
        return TRUE;

    /* we are at bottom, leave shape on field (by doing nothing) and check for lines */
    l = tetris_field_collapse_lines(t);
    t->lines += l;
    t->score += l*l;
    t->level = t->lines/10;
    tetris_shape_next(t);
    
    return FALSE;
}

/* checks if shape is on some occupied block in field */
/* should only be called in combination with tetris_field_shape_is_inside, it does no boundary checks */
static int tetris_field_shape_is_valid_position(tetris *t, int shape, int frame, int x, int y)
{
    int i,j;

    for(i=0;i < t->shape_array[shape]->height;i++)
        for(j=0;j < t->shape_array[shape]->width;j++)
            if(
                tetris_shape_block_get(t,shape,frame,j,i) > 0 &&
                tetris_field_block_get(t,x+j,y+i) > 0
                )
                return FALSE;

    return TRUE;
}

/* checks if shape boundaries is inside field */
/* does not care if shape is on occupied blocks in field */
int tetris_field_shape_is_inside(tetris *t, int shape, int frame, int x, int y)
{
    int i,j;
    
    for(i=0;i < t->shape_array[shape]->height;i++)
        for(j=0;j < t->shape_array[shape]->width;j++)
            if(
                tetris_shape_block_get(t,shape,frame,j,i) > 0 &&
                !tetris_field_is_valid_position(t,x+j,y+i)
                )
                return FALSE;

    return TRUE;
}

int tetris_field_block_get(tetris *t, int x, int y)
{
    return t->field[y*t->width+(MOD_ROTATE(t->width,0,x))];
}

int tetris_field_is_valid_position(tetris *t, int x, int y)
{
    if(y < 0 || y >= t->height)
        return FALSE;
    //if(x < 0 || x >= t->width || y < 0 || y >= t->height)
    //  return FALSE;

    return TRUE;
}

int tetris_field_is_line(tetris *t, int line)
{
    int i;

    for(i=0;i < t->width;i++)
        if(tetris_field_block_get(t,i,line) == 0)
            return FALSE;
    
    return TRUE;
}

/* copying from a row outside field will make destination row filled with 0 */
static void tetris_field_copy_line(tetris *t, int from, int to)
{
    int i;

    if(tetris_field_is_valid_position(t,0,from))
        for(i=0;i < t->width;i++)
            t->field[to*t->width+i] = t->field[from*t->width+i];
    else        
        for(i=0;i < t->width;i++)
            t->field[to*t->width+i] = 0;
}

/* move entire field above stop down one line */
static void tetris_field_move_down(tetris *t, int stop)
{
    int i;

    for(i=stop;i >= 0;i--)
        tetris_field_copy_line(t,i-1,i);
}

int tetris_field_collapse_lines(tetris *t)
{
    int i,lines;

    lines = 0;
    i = t->height-1; /* bottom row */
    
    while(i >= 0) /* until and including top row */
        if(tetris_field_is_line(t,i))
        {
            tetris_field_move_down(t,i);
            lines++;
        }
        else
            /* only decrease if no line was found, if a row was found the rows above */
            /* is moved down and the current line becoms a new line. */
            i--;
    
    return lines;
}

static void tetris_field_clear(tetris *t)
{
    int i,j;

    for(i=0;i < t->height;i++)
        for(j=0;j < t->width;j++)
            t->field[i*t->width+j] = 0;
}

void tetris_field_shape_draw(tetris *t, int shape, int frame, int x, int y)
{
    int i,j,c;
    
    for(i=0;i < t->shape_array[shape]->height;i++)
        for(j=0;j < t->shape_array[shape]->width;j++)
        {
            c = tetris_shape_block_get(t,shape,frame,j,i);
        
            /* sanity, we dont want to poke outside field */
            /* don't draw transparent blocks */
            if(tetris_field_is_valid_position(t,x+j,y+i) && c > 0)
                t->field[(y+i)*t->width+((t->width+x+j)%t->width)] = c;
        }
}

void tetris_field_shape_clear(tetris *t, int shape, int frame, int x, int y)
{
    int i,j;
    
    for(i=0;i < t->shape_array[shape]->height;i++)
        for(j=0;j < t->shape_array[shape]->width;j++)
            /* sanity, we dont want to poke outside field */
            /* ony remove non transparent blocks occupied by the shape */
            if(
                tetris_field_is_valid_position(t,x+j,y+i) &&
                tetris_shape_block_get(t,shape,frame,j,i) > 0
                )
                t->field[(y+i)*t->width+((t->width+x+j)%t->width)] = 0;
}

static void tetris_shape_next_random(tetris *t)
{
    t->next_shape = random_int(0,t->shape_count);
    t->next_frame = random_int(0,t->shape_array[t->next_shape]->frames);
}

static void tetris_shape_next(tetris *t)
{
    t->new = TRUE;
    
    t->shape = t->next_shape;
    t->frame = t->next_frame;
    tetris_shape_next_random(t);
    //t->x = (t->width/2)-(t->shape_array[t->shape]->width/2);
    //t->x = random_int(0,t->width);
    t->y = 0;

    /* version 1: put shape as high as possible allowing it to rotate */
    /* this is useless beacuse of how the frame data look like, it already solves it */
    /*
    while(
        tetris_field_shape_is_inside(t,t->shape,MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,0),t->x,t->y-1) &&
        tetris_field_shape_is_inside(t,t->shape,MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,1),t->x,t->y-1) &&
        tetris_field_shape_is_inside(t,t->shape,MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,2),t->x,t->y-1) &&
        tetris_field_shape_is_inside(t,t->shape,MOD_ROTATE(t->shape_array[t->shape]->frames,t->frame,3),t->x,t->y-1)
        )
        t->y--;
    */
   
    /* version 2: put shape as high as possible not careing about rotation */
    while(tetris_field_shape_is_inside(t, t->shape, t->frame, t->x, t->y-1))
        t->y--;

    /* game over if start position is not valid. we trust lazy evalution, if */
    /* first expression is true the next is not evaluated, if it would we could crash */
    if(
        !tetris_field_shape_is_inside(t,t->shape,t->frame,t->x,t->y) ||
        !tetris_shape_is_valid_position(t,t->shape,t->frame,t->x,t->y)
        )
        t->playing = FALSE;
    
    /* draw it, even if it's not valid position, the player probably want to see what shape made him/her lose ;) */
    tetris_field_shape_draw(t,t->shape,t->frame,t->x,t->y);
}

void tetris_shape_array_set(tetris *t, shape **array, int count)
{
    t->shape_array = array;
    t->shape_count = count;
}

void tetris_reset(tetris *t)
{
    tetris_field_clear(t);
    t->score = 0;
    t->lines = 0;
    t->level = 0;
    t->playing = FALSE;
}

void tetris_start(tetris *t)
{
    tetris_shape_next_random(t);
    t->playing = TRUE;
    tetris_shape_next(t);
}

/* TODO: custome speed and scores too? */
tetris *tetris_alloc(int width, int height)
{
    tetris *t;

    t = malloc(sizeof(tetris));
    if(t == NULL)
        return NULL;

    t->field = malloc(width*height*sizeof(int));
    if(t->field == NULL)
    {
        free(t);

        return NULL;
    }
    
    t->width = width;
    t->height = height;
    t->x = 0;
    t->y = 0;
    t->shape = 0;
    t->frame = 0;
    t->score = 0;
    t->lines = 0;
    t->level = 0;
    t->next_shape = 0;
    t->next_frame = 0;
    t->shape_array = NULL;
    t->shape_count = 0;
    t->playing = FALSE;
    
    return t;
}

/* NOTE: does not preserve shape_array pointer, not really needed anyway */
void tetris_copy(tetris *dest, tetris *src)
{
    int *field = dest->field;
    
    memcpy(dest, src, sizeof(tetris));
    dest->field = field;
    memcpy(dest->field, src->field, sizeof(int)*src->width*src->height);
}

void tetris_free(tetris *t)
{
    free(t->field);
    free(t);
}

/* vim: set ts=4 sw=4 sts=4 et: */
/* -*- Mode: C; tab-width: 4 -*- */
