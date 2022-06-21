# ReferenceFinder 3.0
# Makefile for Unix, Linux and related systems
# Author: Carlos A. Furuti

# Adjust these macros for your particular system
#   (default values are appropriate for GNU tools)
CC = g++
CFLAGS = -O3 -Wpedantic -ansi --std=c++14
# System-specific additional libraries
LIBS = 
# Destination directory for executables
EXEDIR=../../../linux
PROGRAM=$(EXEDIR)/ReferenceFinder.exe
# Optional objects (depend on CALCINPUT)
OPTOBJS = parser.o lexer.o

OBJS = ReferenceFinder.o ReferenceFinder_console.o FindDivisions.o HTMLStreamDgmr.o $(OPTOBJS)

# Main rule
$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS)
ReferenceFinder.o: ReferenceFinder.cpp ReferenceFinder.h
	$(CC) $(CFLAGS) -c ReferenceFinder.cpp
ReferenceFinder_console.o: ReferenceFinder_console.cpp ReferenceFinder.h parser.h
	$(CC) $(CFLAGS) -c ReferenceFinder_console.cpp
HTMLStreamDgmr.o: HTMLStreamDgmr.cpp HTMLStreamDgmr.h ReferenceFinder.h
	$(CC) $(CFLAGS) -c HTMLStreamDgmr.cpp

FindDivisions.o: FindDivisions.cpp FindDivisions.h
	$(CC) $(CFLAGS) -c FindDivisions.cpp

lexer.o: lexer.cpp lexer.h
	$(CC) $(CFLAGS) -c lexer.cpp
parser.o: parser.cpp parser.h lexer.h
	$(CC) $(CFLAGS) -c parser.cpp

clean:
	-rm -f $(OBJS) 
cleanall:
	-rm -f $(OBJS) $(PROGRAM)
