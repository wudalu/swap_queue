GCC=g++
CFLAGS= -Wall -g
MAKE=gmake
LIBS= /usr/local/lib

all:swap_queue

swap_queue:main.o aoi_thread.o swap_queue.o 
	$(GCC) -o $@ $^ -L/$(LIBS) -lpthread -g

%.o:%.cpp
	$(GCC) -c $(CFLAGS) $^ -o $@ 

.PHONY:clean
clean:
	-rm -rf *.o 
	-rm swap_queue
	-rm -rf *core*
