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

/* $Id: util.c 910 2005-09-14 21:38:13Z mattias $ */


#ifdef WIN32
#include <direct.h>
#endif
#ifndef WIN32
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "global.h"
#include "tetris.h"
#include "util.h"
#include "quit.h"

/* File to read random data from */
FILE *urandom = NULL;

char *start_dir;
char *data_dir;

/* Potential locations of data */
const char *directories[] =
{
#ifdef WIN32
    ".",
#else
    DATA_DIR,
#endif
#ifndef NDEBUG
    "../data",
    "./data",
    "..",
    ".", 
#endif
    NULL
};

/* The hint file */
#define HINT_FILE "citris.dir"


static void util_cleanup(void *data);


#ifndef _GNU_SOURCE
static char *get_current_dir_name()
{
    char *temp_pwd = NULL;
    char *temp_alc = NULL;
    int op_len = 0;
	
    while(NULL == temp_pwd)
    {
        op_len += 32;
        temp_pwd = temp_alc;
        temp_alc = realloc(temp_alc, op_len);
        if(NULL == temp_alc)
        {
            free(temp_pwd);
            return NULL;
        }
        temp_pwd = getcwd(temp_alc, op_len);
    }

    return temp_pwd;
}
#endif


int util_init(void)
{
    int i;
    struct stat s;

    /* save current working directory */
    start_dir = get_current_dir_name();
    if(start_dir == NULL)
    {
        perror("get_current_dir_name: start_dir");

        return FALSE;
    }
    
    data_dir = NULL;
    
    for(i = 0; directories[i] != NULL; i++)
    {
        /* to support relative paths, we chdir to start dir before each test */
        if(chdir(start_dir) == -1)
        {
            perror("chdir: start_dir");
            
            return FALSE;
        }

        /* test directory, move to next on error */
        if(chdir(directories[i]) == -1)
            continue;

        /* stat hint file */
        if(stat(HINT_FILE, &s) != -1)
        {
            data_dir = get_current_dir_name();
            if(data_dir == NULL)
            {
                perror("get_current_dir_name: data_dir");

                return FALSE;
            }
            
            break;
        }
    }
    
    /* chdir to start directory */
    if(0 > chdir(start_dir))
    {
        perror("chdir: start_dir");
        
        return FALSE;
    }
    
    if(data_dir == NULL)
    {
        fprintf(stderr, "Could not find citris data directory\n");
        
        return FALSE;
    }
    
    /* Open /dev/urandom */
    if(NULL == (urandom = fopen("/dev/urandom", "r")))
    {
        /* Check if there is no urandom or if there was an error */
        if(ENOENT != errno)
        {
            perror("fopen: /dev/urandom");
            
            return FALSE;
        }
        else 
        {
            /* Since there is no urandom urand() will
               just be a wrapper for rand()	    

               We'll seed rand with teh current time to
               gain some randomicity
            */
            time_t seed = time(NULL);
            
            srand((int)seed);
        }
    }

    quit_addcb(util_cleanup, NULL);

    return TRUE;
}


/* Reads an int from urandom or calls rand() if there is no urandom */
static int urand()
{
    int random_int = 0;

    if(NULL == urandom)
    {
        /* No /dev/urandom */
        random_int = rand();
    }
    else
    {
        /* Using urandom */
        do
        {
            if(0 == fread(&random_int, 1, sizeof(random_int), urandom))
            {
                perror("fread: urandom");

                break;
            }

        /* Check that we are within bounds */
        } while(random_int > RAND_MAX || random_int < 0); 
    }

    return random_int;
}


/* Get a random int between low and high */
int random_int(int low, int high)
{
    assert(low < high);

    return low + (int)(((float)high)*urand() / (RAND_MAX + 1.0));
}

/* finds shortest directions to p from c, allowing it wrap around */
int mod_shortest_direction(int max, int c, int p)
{
    if(c > p)
        return -mod_shortest_direction(max, p, c);
    else
    {
        if(p - c < max / 2)
            return 1;
        else
            return -1;
    }
}

static void util_cleanup(void *data)
{
    if(NULL != urandom)
	    fclose(urandom);
    
    free(start_dir);
    free(data_dir);
}

/* ex: set tabstop=4 expandtab: */
/* -*- Mode: C; tab-width: 4 -*- */
