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

/* $Id: tetris.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef __TETRIS_H__
#define __TETRIS_H__

/* modulus magic, used for rotating an index */
#define MOD_ROTATE(max,current,delta) ((max+current+delta)%max)

#define TETRIS_WIDTH	10
#define TETRIS_HEIGHT	18
#define TETRIS_LEVELS	20


struct _shape {
    int frames;
    int width;
    int height;
    char *data;
};

typedef struct _shape shape;

struct _tetris {
    int *field;
    int width;
    int height;
    int x;
    int y;
    int shape;
    int frame;
    unsigned int score;
    unsigned int lines;
    unsigned int level;
    int next_shape;
    int next_frame;
    shape **shape_array;
    int shape_count;
    int playing;

    /* FIXME: dummy unsed to indicate new shape to AI */
    int new;
};

typedef struct _tetris tetris;

tetris *tetris_alloc(int width, int height);
void tetris_free(tetris * t);
void tetris_start(tetris * t);
void tetris_reset(tetris * t);
void tetris_copy(tetris *dest, tetris *src);
void tetris_shape_array_set(tetris * t, shape ** array, int count);

int tetris_shape_move(tetris * t, int frame, int x, int y);
int tetris_shape_move_down(tetris * t);
int tetris_shape_is_valid_position(tetris *t, int shape, int frame, int x, int y);
int tetris_shape_block_get(tetris * t, int shape, int frame, int x, int y);
int tetris_shape_frame_width(tetris *t, int shape, int frame);

void tetris_field_shape_clear(tetris *t, int shape, int frame, int x, int y);
void tetris_field_shape_draw(tetris *t, int shape, int frame, int x, int y);
int tetris_field_shape_is_inside(tetris *t, int shape, int frame, int x, int y);
int tetris_field_collapse_lines(tetris *t);
int tetris_field_block_get(tetris * t, int x, int y);
//#define tetris_field_block_get(t, x, y) (t->field[(y)*t->width+(MOD_ROTATE(t->width,0,x))])
int tetris_field_is_valid_position(tetris *t, int x, int y);
int tetris_field_is_line(tetris *t, int line);

int tetris_score_lines[4];
int tetris_level_speeds[TETRIS_LEVELS];

int tetris_next_frame; /* start frame for next shape */

#endif
