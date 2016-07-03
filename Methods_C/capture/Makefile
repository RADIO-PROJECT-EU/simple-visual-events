
CC = gcc

INC_OPTS=-I/usr/include/opencv -I/usr/include -I/cygdrive/c/Program\ Files/OpenCV/cv/include -I/cygdrive/c/Program\ Files/OpenCV/bin -I/cygdrive/c/Program\ Files/OpenCV/cxcore/include  -I/cygdrive/c/Program\ Files/OpenCV/otherlibs/highgui
LIB_OPTS=-L/cygdrive/c/Program\ Files/OpenCV/lib

radio_3:
	- rm radio_capture_show
	gcc -o radio_capture_show radio_capture_show.c $(INC_OPTS)  $(LIB_OPTS) -lcv -lcxcore -lhighgui -lm



