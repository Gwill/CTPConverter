IDIR =include
LIBS =lib/*.so
CC=g++
CFLAGS=-I$(IDIR) $(LIBS) -g -std=gnu++11 -lzmq  



all : generate compile

generate: template/*tpl
	python generate.py

compile : *.cpp include/*.h
	$(CC) *.cpp  $(CFLAGS) -o converter

clean:
	rm -f *.o *.orig include/*.orig *.pyc *.con *.pk converter
