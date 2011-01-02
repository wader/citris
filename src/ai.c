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

/*
 * This Tetris AI implementation is based on an algorithm invented by
 * Pierre Dellacherie <dellache@club-internet.fr>
 *
 * When writing my implementation I sneaked at a C++ implementation by
 * Colin P. Fahey <cpfahey@earthlink.net>
 *
 * You can read more about this algorithm and other Tetris AI related topics
 * at Colin's Tetris page:
 * http://www.colinfahey.com/2003jan_tetris/2003jan_tetris.htm
 * 
 * Algorithm notes:
 * Empty rows (above pile) has zero transition counts on a circular field.
 * Priority is not implemented, it has less or no meaning on a circular field.
 */

/* $Id: ai.c 910 2005-09-14 21:38:13Z mattias $ */


#include "global.h"
#include "tetris.h"
#include "game.h"
#include "util.h"
#include "update.h"
#include "timer.h"
#include "quit.h"

static tetris *sandbox;

static void ai_best_move(int *x, int *frame);
static int ai_tetris_transitions_column(tetris *t, int x);
static int ai_tetris_transitions_row(tetris *t, int y);
static int ai_tetris_shape_blocks_eliminated(tetris *t);
static int ai_tetris_complete_lines(tetris *t);
static int ai_tetris_buried_holes(tetris *t, int x);
static int ai_tetris_blanks(tetris *t, int x, int y);
static int ai_tetris_wells(tetris *t, int x);
static int ai_tetris_shape_ymin(tetris *t);
static int ai_tetris_shape_ymax(tetris *t);


static void ai_move(void *data)
{
    static int x;
    static int frame;
    static int dx;
    static int dframe;

    if(game->playing)
    {
        if(game->new)
        {
            game->new = FALSE;
            
            x = 0;
            frame = 0;
            ai_best_move(&x, &frame);

            dx = mod_shortest_direction(game->width, game->x, x);
            dframe = mod_shortest_direction(game->shape_array[game->shape]->frames, game->frame, frame);
            /*
            printf("ai: cx=%d cf=%d | ax=%d af=%d | dx=%d df=%d\n",
                    game->x, game->frame,
                    x, frame,
                    dx, dframe
                    );
            */
        }

        if(x != game->x)
            tetris_shape_move(game, 0, dx, 0);

        if(frame != game->frame)
            tetris_shape_move(game, dframe, 0, 0);

        /* FIXME: this tries to make AI more human :) */
        if(x == game->x)
        {
            if(random_int(0, 3) == 0)
                while(tetris_shape_move_down(game))
                    ;
            else if(random_int(0, 5) == 0)
                tetris_shape_move_down(game);
        }
        
        update();
    }

    timed_func_add(SDL_GetTicks() + random_int(100, 300), ai_move, NULL);
}

void ai_start()
{
    timed_func_add(SDL_GetTicks() + 1, ai_move, NULL);
}

static void ai_cleanup(void *data)
{
    tetris_free(sandbox);
    
    /* FIXME: remove timer, cant remove individual timers yet, menu.c clears all insted */
}

int ai_init()
{
    sandbox = tetris_alloc(GAME_WIDTH, GAME_HEIGHT);
    if(sandbox == NULL)
    {
        fprintf(stderr, "tetris_alloc: sandbox: failed\n");

        return FALSE;
    }

    quit_addcb(ai_cleanup, NULL);

    return TRUE;
}

static void ai_best_move(int *x, int *frame)
{
    int i, j, k;
    float rating_best = -1.0e20f; /* very low */
    float rating = 0;
    float rating_landing_height = 0;
    int rating_row_transitions = 0;
    int rating_column_transitions = 0;
    int rating_buried_holes = 0;
    int rating_wells = 0;
    int rating_completed_lines = 0;
    int rating_eroded = 0;

    /* try all rotations and valid start points */
    for(i = 0; i < game->shape_array[game->shape]->frames; i++)
    {
        /* width + (shape width - 1), make use of field wrap */
        for(j = 0; j < game->width + (game->shape_array[game->shape]->width - 1); j++)
        {
            tetris_copy(sandbox, game);
            
            tetris_field_shape_clear(sandbox, sandbox->shape, sandbox->frame, sandbox->x, sandbox->y);

            if(!tetris_field_shape_is_inside(sandbox, sandbox->shape, i, j, sandbox->y) ||
               !tetris_shape_is_valid_position(sandbox, sandbox->shape, i, j, sandbox->y))
                continue;

            sandbox->x = j;
            sandbox->frame = i;
            
            tetris_field_shape_draw(sandbox, sandbox->shape, sandbox->frame, sandbox->x, sandbox->y);

            rating_row_transitions = 0;
            rating_column_transitions = 0;
            rating_buried_holes = 0;
            rating_wells = 0;
            rating_completed_lines = 0;
            rating_eroded = 0;
            
            /* drop it */
            while(tetris_shape_move(sandbox, 0, 0, 1))
                ;
          
            /* FIXME: check if this calculation is right, think it off by one :) */
            /* top is zero, so invert */
            rating_landing_height = sandbox->height -
                                    (ai_tetris_shape_ymin(sandbox) + ai_tetris_shape_ymax(sandbox))
                                    /2.0;

            rating_completed_lines = ai_tetris_complete_lines(sandbox);
            if(rating_completed_lines > 0)
            {
                rating_eroded = rating_completed_lines * ai_tetris_shape_blocks_eliminated(sandbox);
                tetris_field_collapse_lines(sandbox);
            }

            for(k = 0; k < sandbox->height; k++)
                rating_row_transitions += ai_tetris_transitions_row(sandbox, k);

            for(k = 0; k < sandbox->width; k++)
            {
                rating_column_transitions += ai_tetris_transitions_column(sandbox, k);
                rating_buried_holes += ai_tetris_buried_holes(sandbox, k);
                rating_wells += ai_tetris_wells(sandbox, k);
            }

            rating = 0;
            rating += -1.0f * rating_landing_height;
            rating +=  1.0f * rating_eroded;
            rating += -1.0f * rating_row_transitions;
            rating += -1.0f * rating_column_transitions;
            rating += -4.0f * rating_buried_holes;
            rating += -1.0f * rating_wells;
            
            if(rating > rating_best)
            {
                rating_best = rating;
                *x = j;
                *frame = i;
           
                /*
                printf("best=%f x=%d f=%d | x=%d frame=%i best=%f rating=%f eroded=%d row=%d col=%d bur=%d wel=%d\n",
                        rating_best, *x, *frame, j, i, rating_best, rating,
                        rating_eroded,
                        rating_row_transitions,
                        rating_column_transitions,
                        rating_buried_holes,
                        rating_wells
                        );
                */
            }
        }
    }
}

static int ai_tetris_transitions_column(tetris *t, int x)
{
    int i, j = 0;

    for(i = 0; i < t->height-1; i++)
        if((tetris_field_block_get(t, x, i) > 0) != (tetris_field_block_get(t, x, i+1) > 0))
            j++;

    /* above top is "unoccupied" */
    if(tetris_field_block_get(t, x, 0) > 0)
        j++;
    
    /* below bottom is "occupied" */
    if(tetris_field_block_get(t, x, t->height-1) == 0)
        j++;
    
    return j;
}

static int ai_tetris_transitions_row(tetris *t, int y)
{
    int i, j = 0;

    /* width+1 to "wrap" around the cirular field */
    for(i = 0; i < t->width+1; i++)
        if((tetris_field_block_get(t, i, y) > 0) != (tetris_field_block_get(t, i+1, y) > 0))
            j++;
    
    return j;
}

static int ai_tetris_shape_blocks_eliminated(tetris *t)
{
    int i, j, k = 0;

    for(i = 0; i < t->shape_array[t->shape]->height; i++)
        for(j = 0; j < t->shape_array[t->shape]->width; j++)
            if(tetris_field_is_valid_position(t, t->x+j, t->y+i) &&
               tetris_shape_block_get(t, t->shape, t->frame, j, i) > 0 &&
               tetris_field_is_line(t, t->y+i))
                k++;
    return k;
}

static int ai_tetris_complete_lines(tetris *t)
{
    int i, j = 0;

    for(i = 0; i < t->height; i++)
        if(tetris_field_is_line(t, i))
            j++;
    
    return j;
}

static int ai_tetris_buried_holes(tetris *t, int x)
{
    int i, j = 0, k = 0;

    for(i = 0; i < t->height; i++)
        if(tetris_field_block_get(t, x, i) > 0)
            j = 1;
        else
            k += j;
    
    return k;
}

static int ai_tetris_blanks(tetris *t, int x, int y)
{
    int i, j = 0;
    
    for(i = y; i < t->height; i++)
        if(tetris_field_block_get(t, x, i) > 0)
            break;
        else
            j++;

    return j;
}

static int ai_tetris_wells(tetris *t, int x)
{
    int i, j = 0;

    for(i = 0; i < t->height; i++)
        if(tetris_field_block_get(t, x-1, i) > 0 &&
           tetris_field_block_get(t, x+1, i) > 0)
            j += ai_tetris_blanks(t, x, i);

    return j;
}

static int ai_tetris_shape_ymin(tetris *t)
{
    int i, j, k;

    k = t->y + t->shape_array[t->shape]->height;
    
    for(i = 0; i < t->shape_array[t->shape]->height; i++)
        for(j = 0; j < t->shape_array[t->shape]->width; j++)
            if(tetris_shape_block_get(t, t->shape, t->frame, j, i) > 0 && k < t->y + i)
                k = t->y + i;

    return k;
}

static int ai_tetris_shape_ymax(tetris *t)
{
    int i, j, k;

    k = t->y;
    
    for(i = 0; i < t->shape_array[t->shape]->height; i++)
        for(j = 0; j < t->shape_array[t->shape]->width; j++)
            if(tetris_shape_block_get(t, t->shape, t->frame, j, i) > 0 && k > t->y + i)
                k = t->y + i;

    return k;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
