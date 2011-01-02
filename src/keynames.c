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

/* $Id: keynames.c 910 2005-09-14 21:38:13Z mattias $ */


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "SDL/SDL.h"

#include "global.h"


typedef struct
{
    const char *name;
    SDLKey sym;
} keynames ;


const keynames keyname_list[] = 
{
	{ "0" , SDLK_0 } ,
	{ "1" , SDLK_1 } ,
	{ "2" , SDLK_2 } ,
	{ "3" , SDLK_3 } ,
	{ "4" , SDLK_4 } ,
	{ "5" , SDLK_5 } ,
	{ "6" , SDLK_6 } ,
	{ "7" , SDLK_7 } ,
	{ "8" , SDLK_8 } ,
	{ "9" , SDLK_9 } ,
	{ "AMPERSAND" , SDLK_AMPERSAND } ,
	{ "ASTERISK" , SDLK_ASTERISK } ,
	{ "AT" , SDLK_AT } ,
	{ "BACKQUOTE" , SDLK_BACKQUOTE } ,
	{ "BACKSLASH" , SDLK_BACKSLASH } ,
	{ "BACKSPACE" , SDLK_BACKSPACE } ,
	{ "BREAK" , SDLK_BREAK } ,
	{ "CAPSLOCK" , SDLK_CAPSLOCK } ,
	{ "CARET" , SDLK_CARET } ,
	{ "CLEAR" , SDLK_CLEAR } ,
	{ "COLON" , SDLK_COLON } ,
	{ "COMMA" , SDLK_COMMA } ,
	{ "COMPOSE" , SDLK_COMPOSE } ,
	{ "DELETE" , SDLK_DELETE } ,
	{ "DOLLAR" , SDLK_DOLLAR } ,
	{ "DOWN" , SDLK_DOWN } ,
	{ "END" , SDLK_END } ,
	{ "EQUALS" , SDLK_EQUALS } ,
	{ "ESCAPE" , SDLK_ESCAPE } ,
	{ "EURO" , SDLK_EURO } ,
	{ "EXCLAIM" , SDLK_EXCLAIM } ,
	{ "F1" , SDLK_F1 } ,
	{ "F10" , SDLK_F10 } ,
	{ "F11" , SDLK_F11 } ,
	{ "F12" , SDLK_F12 } ,
	{ "F13" , SDLK_F13 } ,
	{ "F14" , SDLK_F14 } ,
	{ "F15" , SDLK_F15 } ,
	{ "F2" , SDLK_F2 } ,
	{ "F3" , SDLK_F3 } ,
	{ "F4" , SDLK_F4 } ,
	{ "F5" , SDLK_F5 } ,
	{ "F6" , SDLK_F6 } ,
	{ "F7" , SDLK_F7 } ,
	{ "F8" , SDLK_F8 } ,
	{ "F9" , SDLK_F9 } ,
	{ "FIRST" , SDLK_FIRST } ,
	{ "GREATER" , SDLK_GREATER } ,
	{ "HASH" , SDLK_HASH } ,
	{ "HELP" , SDLK_HELP } ,
	{ "HOME" , SDLK_HOME } ,
	{ "INSERT" , SDLK_INSERT } ,
	{ "KP0" , SDLK_KP0 } ,
	{ "KP1" , SDLK_KP1 } ,
	{ "KP2" , SDLK_KP2 } ,
	{ "KP3" , SDLK_KP3 } ,
	{ "KP4" , SDLK_KP4 } ,
	{ "KP5" , SDLK_KP5 } ,
	{ "KP6" , SDLK_KP6 } ,
	{ "KP7" , SDLK_KP7 } ,
	{ "KP8" , SDLK_KP8 } ,
	{ "KP9" , SDLK_KP9 } ,
	{ "KP_DIVIDE" , SDLK_KP_DIVIDE } ,
	{ "KP_ENTER" , SDLK_KP_ENTER } ,
	{ "KP_EQUALS" , SDLK_KP_EQUALS } ,
	{ "KP_MINUS" , SDLK_KP_MINUS } ,
	{ "KP_MULTIPLY" , SDLK_KP_MULTIPLY } ,
	{ "KP_PERIOD" , SDLK_KP_PERIOD } ,
	{ "KP_PLUS" , SDLK_KP_PLUS } ,
	{ "LALT" , SDLK_LALT } ,
	{ "LCTRL" , SDLK_LCTRL } ,
	{ "LEFT" , SDLK_LEFT } ,
	{ "LEFTBRACKET" , SDLK_LEFTBRACKET } ,
	{ "LEFTPAREN" , SDLK_LEFTPAREN } ,
	{ "LESS" , SDLK_LESS } ,
	{ "LMETA" , SDLK_LMETA } ,
	{ "LSHIFT" , SDLK_LSHIFT } ,
	{ "LSUPER" , SDLK_LSUPER } ,
	{ "MENU" , SDLK_MENU } ,
	{ "MINUS" , SDLK_MINUS } ,
	{ "MODE" , SDLK_MODE } ,
	{ "NUMLOCK" , SDLK_NUMLOCK } ,
	{ "PAGEDOWN" , SDLK_PAGEDOWN } ,
	{ "PAGEUP" , SDLK_PAGEUP } ,
	{ "PAUSE" , SDLK_PAUSE } ,
	{ "PERIOD" , SDLK_PERIOD } ,
	{ "PLUS" , SDLK_PLUS } ,
	{ "POWER" , SDLK_POWER } ,
	{ "PRINT" , SDLK_PRINT } ,
	{ "QUESTION" , SDLK_QUESTION } ,
	{ "QUOTE" , SDLK_QUOTE } ,
	{ "QUOTEDBL" , SDLK_QUOTEDBL } ,
	{ "RALT" , SDLK_RALT } ,
	{ "RCTRL" , SDLK_RCTRL } ,
	{ "RETURN" , SDLK_RETURN } ,
	{ "RIGHT" , SDLK_RIGHT } ,
	{ "RIGHTBRACKET" , SDLK_RIGHTBRACKET } ,
	{ "RIGHTPAREN" , SDLK_RIGHTPAREN } ,
	{ "RMETA" , SDLK_RMETA } ,
	{ "RSHIFT" , SDLK_RSHIFT } ,
	{ "RSUPER" , SDLK_RSUPER } ,
	{ "SCROLLOCK" , SDLK_SCROLLOCK } ,
	{ "SEMICOLON" , SDLK_SEMICOLON } ,
	{ "SLASH" , SDLK_SLASH } ,
	{ "SPACE" , SDLK_SPACE } ,
	{ "SYSREQ" , SDLK_SYSREQ } ,
	{ "TAB" , SDLK_TAB } ,
	{ "UNDERSCORE" , SDLK_UNDERSCORE } ,
	{ "UNDO" , SDLK_UNDO } ,
	{ "UNKNOWN" , SDLK_UNKNOWN } ,
	{ "UP" , SDLK_UP } ,
	{ "WORLD_0" , SDLK_WORLD_0 } ,
	{ "WORLD_1" , SDLK_WORLD_1 } ,
	{ "WORLD_10" , SDLK_WORLD_10 } ,
	{ "WORLD_11" , SDLK_WORLD_11 } ,
	{ "WORLD_12" , SDLK_WORLD_12 } ,
	{ "WORLD_13" , SDLK_WORLD_13 } ,
	{ "WORLD_14" , SDLK_WORLD_14 } ,
	{ "WORLD_15" , SDLK_WORLD_15 } ,
	{ "WORLD_16" , SDLK_WORLD_16 } ,
	{ "WORLD_17" , SDLK_WORLD_17 } ,
	{ "WORLD_18" , SDLK_WORLD_18 } ,
	{ "WORLD_19" , SDLK_WORLD_19 } ,
	{ "WORLD_2" , SDLK_WORLD_2 } ,
	{ "WORLD_20" , SDLK_WORLD_20 } ,
	{ "WORLD_21" , SDLK_WORLD_21 } ,
	{ "WORLD_22" , SDLK_WORLD_22 } ,
	{ "WORLD_23" , SDLK_WORLD_23 } ,
	{ "WORLD_24" , SDLK_WORLD_24 } ,
	{ "WORLD_25" , SDLK_WORLD_25 } ,
	{ "WORLD_26" , SDLK_WORLD_26 } ,
	{ "WORLD_27" , SDLK_WORLD_27 } ,
	{ "WORLD_28" , SDLK_WORLD_28 } ,
	{ "WORLD_29" , SDLK_WORLD_29 } ,
	{ "WORLD_3" , SDLK_WORLD_3 } ,
	{ "WORLD_30" , SDLK_WORLD_30 } ,
	{ "WORLD_31" , SDLK_WORLD_31 } ,
	{ "WORLD_32" , SDLK_WORLD_32 } ,
	{ "WORLD_33" , SDLK_WORLD_33 } ,
	{ "WORLD_34" , SDLK_WORLD_34 } ,
	{ "WORLD_35" , SDLK_WORLD_35 } ,
	{ "WORLD_36" , SDLK_WORLD_36 } ,
	{ "WORLD_37" , SDLK_WORLD_37 } ,
	{ "WORLD_38" , SDLK_WORLD_38 } ,
	{ "WORLD_39" , SDLK_WORLD_39 } ,
	{ "WORLD_4" , SDLK_WORLD_4 } ,
	{ "WORLD_40" , SDLK_WORLD_40 } ,
	{ "WORLD_41" , SDLK_WORLD_41 } ,
	{ "WORLD_42" , SDLK_WORLD_42 } ,
	{ "WORLD_43" , SDLK_WORLD_43 } ,
	{ "WORLD_44" , SDLK_WORLD_44 } ,
	{ "WORLD_45" , SDLK_WORLD_45 } ,
	{ "WORLD_46" , SDLK_WORLD_46 } ,
	{ "WORLD_47" , SDLK_WORLD_47 } ,
	{ "WORLD_48" , SDLK_WORLD_48 } ,
	{ "WORLD_49" , SDLK_WORLD_49 } ,
	{ "WORLD_5" , SDLK_WORLD_5 } ,
	{ "WORLD_50" , SDLK_WORLD_50 } ,
	{ "WORLD_51" , SDLK_WORLD_51 } ,
	{ "WORLD_52" , SDLK_WORLD_52 } ,
	{ "WORLD_53" , SDLK_WORLD_53 } ,
	{ "WORLD_54" , SDLK_WORLD_54 } ,
	{ "WORLD_55" , SDLK_WORLD_55 } ,
	{ "WORLD_56" , SDLK_WORLD_56 } ,
	{ "WORLD_57" , SDLK_WORLD_57 } ,
	{ "WORLD_58" , SDLK_WORLD_58 } ,
	{ "WORLD_59" , SDLK_WORLD_59 } ,
	{ "WORLD_6" , SDLK_WORLD_6 } ,
	{ "WORLD_60" , SDLK_WORLD_60 } ,
	{ "WORLD_61" , SDLK_WORLD_61 } ,
	{ "WORLD_62" , SDLK_WORLD_62 } ,
	{ "WORLD_63" , SDLK_WORLD_63 } ,
	{ "WORLD_64" , SDLK_WORLD_64 } ,
	{ "WORLD_65" , SDLK_WORLD_65 } ,
	{ "WORLD_66" , SDLK_WORLD_66 } ,
	{ "WORLD_67" , SDLK_WORLD_67 } ,
	{ "WORLD_68" , SDLK_WORLD_68 } ,
	{ "WORLD_69" , SDLK_WORLD_69 } ,
	{ "WORLD_7" , SDLK_WORLD_7 } ,
	{ "WORLD_70" , SDLK_WORLD_70 } ,
	{ "WORLD_71" , SDLK_WORLD_71 } ,
	{ "WORLD_72" , SDLK_WORLD_72 } ,
	{ "WORLD_73" , SDLK_WORLD_73 } ,
	{ "WORLD_74" , SDLK_WORLD_74 } ,
	{ "WORLD_75" , SDLK_WORLD_75 } ,
	{ "WORLD_76" , SDLK_WORLD_76 } ,
	{ "WORLD_77" , SDLK_WORLD_77 } ,
	{ "WORLD_78" , SDLK_WORLD_78 } ,
	{ "WORLD_79" , SDLK_WORLD_79 } ,
	{ "WORLD_8" , SDLK_WORLD_8 } ,
	{ "WORLD_80" , SDLK_WORLD_80 } ,
	{ "WORLD_81" , SDLK_WORLD_81 } ,
	{ "WORLD_82" , SDLK_WORLD_82 } ,
	{ "WORLD_83" , SDLK_WORLD_83 } ,
	{ "WORLD_84" , SDLK_WORLD_84 } ,
	{ "WORLD_85" , SDLK_WORLD_85 } ,
	{ "WORLD_86" , SDLK_WORLD_86 } ,
	{ "WORLD_87" , SDLK_WORLD_87 } ,
	{ "WORLD_88" , SDLK_WORLD_88 } ,
	{ "WORLD_89" , SDLK_WORLD_89 } ,
	{ "WORLD_9" , SDLK_WORLD_9 } ,
	{ "WORLD_90" , SDLK_WORLD_90 } ,
	{ "WORLD_91" , SDLK_WORLD_91 } ,
	{ "WORLD_92" , SDLK_WORLD_92 } ,
	{ "WORLD_93" , SDLK_WORLD_93 } ,
	{ "WORLD_94" , SDLK_WORLD_94 } ,
	{ "WORLD_95" , SDLK_WORLD_95 } ,
	{ "a" , SDLK_a } ,
	{ "b" , SDLK_b } ,
	{ "c" , SDLK_c } ,
	{ "d" , SDLK_d } ,
	{ "e" , SDLK_e } ,
	{ "f" , SDLK_f } ,
	{ "g" , SDLK_g } ,
	{ "h" , SDLK_h } ,
	{ "i" , SDLK_i } ,
	{ "j" , SDLK_j } ,
	{ "k" , SDLK_k } ,
	{ "l" , SDLK_l } ,
	{ "m" , SDLK_m } ,
	{ "n" , SDLK_n } ,
	{ "o" , SDLK_o } ,
	{ "p" , SDLK_p } ,
	{ "q" , SDLK_q } ,
	{ "r" , SDLK_r } ,
	{ "s" , SDLK_s } ,
	{ "t" , SDLK_t } ,
	{ "u" , SDLK_u } ,
	{ "v" , SDLK_v } ,
	{ "w" , SDLK_w } ,
	{ "x" , SDLK_x } ,
	{ "y" , SDLK_y } ,
	{ "z" , SDLK_z } ,
	{ NULL , SDLK_LAST }
};

SDLKey keyname_name2key(const char *name, SDLKey *sym, SDLMod *mod)
{
    const keynames *temp = keyname_list;

    assert(name != NULL);
    assert(sym != NULL);
    assert(mod != NULL);
    
    /*  Map a possible name prefix into keymod */
    *mod = KMOD_NONE;
    while(TRUE)
    {
        if(strncmp("ALT+",name,4) == 0)
        {
            *mod |= KMOD_ALT;
            name += 4;
        }
        else if(strncmp("CTRL+",name,5) == 0)
        {
            *mod |= KMOD_CTRL;
            name += 5;
        }
        else
            break;
    }
    
    /* Map a name into keysym */
    while(temp->name != NULL)
    {
        /* If the first letter match: check the entire name */
        if(name[0] == temp->name[0] && 0 == strcmp(name, temp->name))
        {
            *sym = temp->sym;

            return temp->sym;
        }

        temp++;
    }

    return SDLK_LAST;
}

/* Maps a keysym/keymod into its common name */
const char *keyname_key2name(SDLKey key, SDLMod mod)
{
    /* This array is only used if a modifier is active */
    static char keyname[128] = { '\0' };

    const keynames *temp = keyname_list;

    while(NULL != temp->name && temp->sym == key)
	    temp++;

    /* Return if the key wasn't found
       or if there is no mod(then the
       name in the array can be used as
       return value)
    */
    if(NULL == temp->name || KMOD_NONE == ((mod & (KMOD_ALT | KMOD_CTRL))))
	    return temp->name; 
  
    /* Use the static array to concatenate a mod + key
       name */
    keyname[0] = '\0';

    if(mod & KMOD_ALT)
	    strcat(keyname, "ALT+");
  
    if(mod & KMOD_CTRL)
	    strcat(keyname, "CTRL+");
  
    strncat(keyname, temp->name, 118); //118=128 - NUL_len - ALT_len - CTRL_len

    return keyname;
}


/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
