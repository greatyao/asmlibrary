CPPFLAGS = -I/usr/local/include/opencv -O2 -Wall -pg -g -MD
PROGRAMS = fit 

all: $(PROGRAMS)

vjfacedetect.o: ../src/vjfacedetect.cpp
	g++  $(CPPFLAGS)  -c -o vjfacedetect.o ../src/vjfacedetect.cpp	
video_camera.o: ../src/video_camera.cpp
	g++  $(CPPFLAGS)  -c -o video_camera.o ../src/video_camera.cpp	
demo_fit.o: ../src/demo_fit.cpp
	g++  $(CPPFLAGS)  -c -o demo_fit.o ../src/demo_fit.cpp

fit: demo_fit.o vjfacedetect.o video_camera.o
	g++ -pg -o fit vjfacedetect.o video_camera.o demo_fit.o  -L/usr/local/lib -lopencv_video -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_objdetect -L./ -lasmlibrary -framework OpenCL -framework AppKit -framework QuartzCore -framework QTKit -framework Cocoa
clean:
	rm -f *.o $(PROGRAMS)
