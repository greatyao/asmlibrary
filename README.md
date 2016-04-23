# Avtive Shape Model C++ Library (ASMLibrary)


## Dependencies
- opencv (1.0 or later for Windows/Linux/MacOSX/Android)
 
## How to build your program
- Windows

    Open src/build.dsw(sln) and src/fit.dsw(sln) under the enviroment of Microsoft VS.

- Linux/MacOSX

    Goto linux/macosx directory and type make command.

## Quick Tutorial

### Prepare: 
- For model training, you should have several pairs of images and annotations. ASMLibrary supports pts and asf format.
- Download the imm dataset from AAM-API's homepage [link: IMM Dataset](http://www2.imm.dtu.dk/pubdb/views/publication_details.php?id=922)
- Download FRANCK dataset annotated by Cootes [link: Cootes's Dataset](http://personalpages.manchester.ac.uk/staff/timothy.f.cootes/tfc_software.html)
- Download helen dataset from this [link: Helen Dataset](http://ibug.doc.ic.ac.uk/resources/facial-point-annotations/)
- You can generate your pts file by using Cootes's am_tools and asf file by using Stagmman's aam-api.
 

### Training 
   > build -i 1 -p 4 -l 8 ../franck jpg pts haarcascade_frontalface_alt2.xml franck.amf

 
### Fitting

- Image alignment on an image using 30 iterations
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -i aa.jpg -n 30

- Face tracking on a video file
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -v bb.avi -n 25

- Face tracking on live camara
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -c 0


## Question
if you have any question, contact me at njustyw@gmail.com, THANKS.