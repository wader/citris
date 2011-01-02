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

/* $Id: gameover.c 910 2005-09-14 21:38:13Z mattias $ */


#include "global.h"

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ai.h"
#include "gamefield.h"
#include "game.h"
#include "eventengine.h"
#include "timer.h"
#include "update.h"
#include "keyhandler.h"
#include "states.h"
#include "quit.h"

static SDL_Surface *text_gameover;

static int gameover_end;

static int gameover_keymouse_eh(SDL_Event *event)
{
    assert(event != NULL);
    assert(event->type == SDL_KEYUP || event->type == SDL_MOUSEBUTTONUP);

    gameover_end = TRUE;

    return TRUE;
}

int gameover_statechange(run_state new)
{
    SDL_Surface *fader;
    SDL_Rect r;
    
    assert(NULL != new);
    
    /* Add a new timed event for this function */
    /*timed_func_add(
            SDL_GetTicks()+1,
            menu_moveblock_cb,
            NULL
            );*/

    /* FIXME: magic color constants */
    fader = SDL_CreateRGBSurface(
            SDL_HWSURFACE | SDL_SRCALPHA,
            screen->w, screen->h, screen->format->BitsPerPixel,
            0, 0, 0, 0
            );
    SDL_FillRect(fader, NULL, SDL_MapRGB(fader->format, 0x4a, 0, 0));
    SDL_SetAlpha(fader, SDL_SRCALPHA, 0x8e);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
    gamefield_render(screen, game);
    SDL_BlitSurface(fader, NULL, screen, NULL);
    SDL_FreeSurface(fader);

    r.x = screen->w/2 - text_gameover->w/2;
    r.y = screen->h/2 - text_gameover->h/2;
    r.w = text_gameover->w;
    r.h = text_gameover->h;
    SDL_BlitSurface(text_gameover, NULL, screen, &r);

    update();

    return TRUE;
}


static state gameover_run(void)
{
    gameover_end = FALSE;
    
    while(!gameover_end)
        event_engine_run(EVENTMODE_RUN, 25);

    return STATE_MENU;
}

static void gameover_update(void)
{
    SDL_Flip(screen);
}

run_state gameover_init(run_state rstate)
{
    SDL_Color c;
    TTF_Font *font;
    
    /* Store a pointer to menu_main in data */
    rstate->data = NULL;
    
    /* Set remaining functions */
    rstate->run = gameover_run;
    rstate->extra = gameover_statechange;
    rstate->update = gameover_update;
    
    /* Initialise event handlers */
    event_handler_add(quithandler_eh, SDL_QUIT);
    event_handler_add(updatehandler_eh, SDL_USEREVENT);
    event_handler_add(gameover_keymouse_eh, SDL_KEYUP);
    event_handler_add(gameover_keymouse_eh, SDL_MOUSEBUTTONUP);
    
    if(NULL == (rstate->eventengine = event_engine_save()))
        return NULL;
    
    font = TTF_OpenFont("font.ttf", screen->h/8);
    if(font == NULL)
    {
        fprintf(stderr,"failed to load font: %s\n", SDL_GetError());

        return FALSE;
    }
    
    c.r = 200;
    c.g = 200;
    c.b = 200;
    text_gameover = TTF_RenderText_Blended(font, "Game over", c);
   
    TTF_CloseFont(font);
    
    event_engine_clear(); //TODO: Fix eventengine so this isnt necessary
    
    return rstate;
}

void gameover_cleanup(run_state rstate)
{
    SDL_FreeSurface(text_gameover);
    free(rstate->eventengine);
}

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
