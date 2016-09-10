MC_NAME = $(shell uname -m)
LC_MACH_NAME = $(shell echo $(MC_NAME) | tr '[A-Z]' '[a-z]')
ifeq ($(LC_MACH_NAME), i686)
ASMLIBS = -lasmlibrary 
else
ASMLIBS = -lasmlibrary64 
endif

OPENCVCFLAGS = `pkg-config --cflags opencv`
CPPFLAGS = $(OPENCVCFLAGS) -O2 -Wall -pg -g -MD
OPENCVLIBS = `pkg-config --libs opencv`
PROGRAMS = fit 

all: $(PROGRAMS)

vjfacedetect.o: ../src/vjfacedetect.cpp
	g++  $(CPPFLAGS)  -c -o vjfacedetect.o ../src/vjfacedetect.cpp	
video_camera.o: ../src/video_camera.cpp
	g++  $(CPPFLAGS)  -c -o video_camera.o ../src/video_camera.cpp	
demo_fit.o: ../src/demo_fit.cpp
	g++  $(CPPFLAGS)  -c -o demo_fit.o ../src/demo_fit.cpp

# asmlibrary.so must be linked after opencv_core.so opencv_imgproc.so, otherwise it will segment fault!!!
fit: demo_fit.o vjfacedetect.o video_camera.o
	g++ -pg -o fit vjfacedetect.o video_camera.o demo_fit.o $(OPENCVLIBS) -Wl,-rpath=./ -L./ $(ASMLIBS) 

clean:
	rm -f *.o $(PROGRAMS)
