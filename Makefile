CC=g++
DEBUG=-g

TOPDIR=.

.PHONY: clean gapp destroy


SRCS=main.cpp light.cpp camera.cpp objects.cpp scene.cpp utils.cpp

EXTLIB=-lGL -lGLEW -lglfw -lgomp -fopenmp

raytracer: 
	$(CC) $(SRCS) $(EXTLIB) -o raytracer

clean:
	-rm raytracer