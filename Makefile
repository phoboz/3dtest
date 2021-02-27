include	Makefile.config

PROJECT=reargunner2k
SRCC=main.c imath.c tritab.c
SRCS=
SRCH=
OBJS=$(SRCC:.c=.o) $(SRCS:.s=.o) 
OTHEROBJS=
RMVLIBS=display.o collision.o interrupt.o paula.o protracker.o render.o rmvlib.a

include	Makefile.example
