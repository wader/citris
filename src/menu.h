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

/* $Id: menu.h 910 2005-09-14 21:38:13Z mattias $ */


#ifndef MENU_H
#define MENU_H

/* State initialiser */
void menu_cleanup(run_state rstate);

/* State cleanup */
run_state menu_init(run_state rstate);


#endif

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
