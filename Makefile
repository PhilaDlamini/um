#
# Makefile for the UM HW
#
CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic -O2 $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lcii40-O2 -l40locality -lm -lbitpack -lum-dis -lcii


all: um lab

um: um.o
		$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

lab: umlab.o umlabwrite.o
		$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o
