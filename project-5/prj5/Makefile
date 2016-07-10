SUBMIT = cachesim.h cachesim.c cachesim_driver.c Makefile
CXXFLAGS := -g -Wall -std=c99 -lm
CXX=gcc

all: cachesim

cachesim: cachesim.o cachesim_driver.o
	$(CXX) -o cachesim cachesim.o cachesim_driver.o $(CXXFLAGS)

clean:
	rm -f cachesim result.txt *.o

submit: clean
	tar zcvf bonus-submit.tar.gz $(SUBMIT)

#Experimental summer 2016
run:
	chmod 700 ./run_script.sh && \
	./run_script.sh > result.txt && \
	diff result.txt solution.txt
