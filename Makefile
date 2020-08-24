# 
# Makefile : swift host application
# 
# Description: 
# This Makefile generates the Host Application binary for the ARM CortexA9
#  
# Rev. 0.1
# 
# Author: Andrea Guerrieri <andrea.guerrieri@epfl.ch (C) 2017
#
# Copyright: See COPYING file that comes with this distribution
#
# 

CROSS_COMPILE ?=

AS		= $(CROSS_COMPILE)as
#CC		= $(CROSS_COMPILE)gcc
CC		= $(CROSS_COMPILE)g++
CD		= cd
#LD		= $(CC) -nostdlib
LD		= $(CC)
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
AWK		= awk
GENKSYMS	= scripts/genksyms/genksyms
DEPMOD		= /sbin/depmod
KALLSYMS	= scripts/kallsyms
PERL		= perl
CHECK		= sparse
DOXYGEN		= doxygen


APP = convert


SRCDIR=./src
OBJDIR=./obj
BINDIR=./bin
LIBDIR=./lib
DOCSDIR=./docs

SWIFT_API_DOXY = .swift_api.cfg
SWIFT_STRUCTURE_DOXY = .swift_application_structure.cfg
SWIFT_SAMPLE_DOXY = .swift_user_application.cfg

#CC=g++
#CFLAGS=-O3 -Wall
#CFLAGS=-O0 -g -Wall
#CFLAGS=pthread -lm -O0 -g -static-libgcc -Wall -static -fpermissive
# DEFINE1=-DUSE_PL_MEMORY
#DEFINE2=-D_USE_TCP
DEFINES=${DEFINE1} ${DEFINE2} ${DEFINE3}

#CFLAGS=-O3 -g -Wall -fpermissive $(DEFINES) 
#CFLAGS=-std=c++14 -Wall -fPIC -Wno-sign-compare -Wno-unused-function -O3
CFLAGS=`pkg-config libgvc --cflags` -std=c++14 -Wall -fPIC -Wno-sign-compare -Wno-unused-function -O3
LFLAGS=`pkg-config libgvc --libs`
#LFLAGS=-lpthread -lm -static-libgcc 
#-static
IDIR=$(LIBDIR)

$(BINDIR)/$(APP) :: \
		$(OBJDIR)/test.o $(OBJDIR)/Dataflow.o $(OBJDIR)/DFnetlist.o $(OBJDIR)/DFlib.o \
		$(OBJDIR)/DFnetlist_channel_width.o $(OBJDIR)/DFnetlist_Connectivity.o\
		$(OBJDIR)/DFnetlsit_BasicBlocks.o \
		$(OBJDIR)/DFnetlist_check.o $(OBJDIR)/DFnetlist_read_dot.o $(OBJDIR)/DFnetlist_write_dot.o \
		$(OBJDIR)/DFnetlist_write_blif.o
		$(CC) $(CFLAGS) $? -o $@ $(LDIR) $(LFLAGS) \

$(OBJDIR)/DFnetlist.o :: $(SRCDIR)/DFnetlist.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)
	
$(OBJDIR)/Dataflow.o :: $(SRCDIR)/Dataflow.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFlib.o :: $(SRCDIR)/DFlib.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlsit_BasicBlocks.o :: $(SRCDIR)/DFnetlsit_BasicBlocks.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_Connectivity.o :: $(SRCDIR)/DFnetlist_Connectivity.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_channel_width.o :: $(SRCDIR)/DFnetlist_channel_width.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_check.o :: $(SRCDIR)/DFnetlist_check.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_read_dot.o :: $(SRCDIR)/DFnetlist_read_dot.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_write_dot.o :: $(SRCDIR)/DFnetlist_write_dot.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/DFnetlist_write_blif.o :: $(SRCDIR)/DFnetlist_write_blif.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)

$(OBJDIR)/test.o :: $(SRCDIR)/test.cpp
	$(CC) $(CFLAGS) -c $? -o $@ -I $(IDIR)
	
	
docs :: 
	$(CD) $(DOCSDIR) && \
	$(DOXYGEN) $(SWIFT_API_DOXY) && \
	$(DOXYGEN) $(SWIFT_STRUCTURE_DOXY) &&\
	$(DOXYGEN) $(SWIFT_SAMPLE_DOXY)

clean ::
	rm -rf $(BINDIR)/* $(OBJDIR)/*.o $(LIBDIR)/*.o
