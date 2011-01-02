AM_CFLAGS = -Wall

bin_PROGRAMS = citris

citris_CPPFLAGS = -DDATA_DIR=\"$(pkgdatadir)\"
citris_CFLAGS = $(AM_CFLAGS) $(SDL_CFLAGS) 
citris_LDADD = $(SDL_LIBS)
citris_SOURCES = \
global.h \
sdl.c \
eventengine.c \
eventengine.h \
mousemove.c \
mousemove.h \
tetris.c \
tetris.h \
timer.c \
timer.h \
keyhandler.c \
keyhandler.h \
keyaction.c \
keyaction.h \
keynames.c \
keynames.h \
util.c \
util.h \
update.c \
update.h \
states.c\
states.h\
gamefield.c \
gamefield.h \
game.c \
game.h \
gameover.c \
gameover.h \
shapes.c \
shapes.h \
ai.c \
ai.h \
intro.c \
intro.h \
extro.c \
extro.h \
menu.c \
menu.h \
quit.c \
quit.h

