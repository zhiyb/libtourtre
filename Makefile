CC = gcc
CPPFLAGS = -I./include
CFLAGS = -ansi -pedantic -Wall -fPIC -O2 -Wno-unused-but-set-variable $(DEBUG_FLAGS)

AR = ar
ARFLAGS = -r

all : shared static

shared : libtourtre.so

static : libtourtre.a

objs =  src/tourtre.o     \
	src/ctArc.o       \
	src/ctBranch.o    \
	src/ctComponent.o \
	src/ctNode.o      \
	src/ctQueue.o     \
	src/ctNodeMap.o

libtourtre.a : $(objs)
	$(AR) $(ARFLAGS) $@ $^
	
libtourtre.so : $(objs)
	$(CC) -shared -o $@ $^

src/tourtre.o : src/tourtre.c include/tourtre.h src/ctMisc.h include/ctArc.h include/ctNode.h src/ctComponent.h include/ctNode.h src/ctQueue.h src/ctAlloc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/ctNodeMap.o : src/ctNodeMap.c src/ctNodeMap.h include/ctNode.h src/ctQueue.h src/sglib.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/%.o : src/%.c include/tourtre.h src/ctMisc.h include/%.h 
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean :
	-rm -rf src/*.o libtourtre.a libtourtre.so doc/html

	
# src/test : 	libtourtre.a test/test.c
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -o test/test test/test.c -L. -ltourtre
# 
# test: src/test
# 	test/test
