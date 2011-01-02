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

/* $Id: keyaction.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef KEYACTION_H
#define KEYACTION_H

/* Prototype for the action functions */
typedef void (*key_action)(void);

/* Get key_action mapping from action name */
key_action keyaction_name2action(const char *name);

/* Mapping from action to name */
const char *keyaction_action2name(key_action action);


/* The actions registered so far */
void game_block_clockw(void);
void game_block_cclockw(void);
void game_block_rotate(void);
void game_block_down(void);
void game_block_drop(void);
void syswm_alter_fullscreen(void);
void menu_up(void);
void menu_down(void);
void menu_select(void);


#endif



