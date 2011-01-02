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

/* $Id: game.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef GAME_H
#define GAME_H

#include "states.h"
#include "tetris.h"

#define GAME_WIDTH  20
#define GAME_HEIGHT 15

extern tetris *game;


/* This initializes a game state
 */
run_state game_init(run_state rstate);

/* Cleanup all game data */
void game_cleanup(run_state rstate);


#endif
