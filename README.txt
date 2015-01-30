/****************************************************************************
*                 ASMLibrary
* Copyright (c) 2008-2011 by Yao Wei, all rights reserved.
* Author:      	Yao Wei
* 
****************************************************************************/


===================
Dependencies
===================
	-- opencv (1.0 or later for Windows/Linux/MacOSX/Android)
 
===================
How to build your program
===================

1. Windows
Open src/build.dsw(sln) and src/fit.dsw(sln) under the enviroment of Microsoft VS.

2. Linux/MacOSX
Goto linux/macosx directory and type make command.

================
A Quick Tutorial
================

To get farmiliarised quickly with the library, execute the following steps:

1. Build active shape models 
    Prepare: For building model, you should have several pairs of images and annotations. 
             The imm database which you can get from AAM-API's homepage is an alternative. 
             Besides the aam-api's asf format, we also support pts format which you can get 
             detais from FRANCK database annotated by Cootes.
    
    Usage: build [options] train_path image_ext point_ext cascade_name model_file    
    options:
          -i   interpolate or not (default 0)
          -l   profile length of half side (default 8)
          -p   level of parymid (default 3)
          -t   percentage of shape PCA (default 0.975)
          -T   type of sampling profile (default PROFILE_1D, not implemented otherwise)
    train_path    the traingset path which contains some pairs of images and annotations
    image_ext     image format, support jpg, png, bmp, etc.
    point_ext     point format, support asf, pts
    cascade_name  the opencv face detect xml file
    model_file	  the output mode file

    Examples:
 
    # Build asmmodel from 240-images of imm-database which you can download from Stagmman's homepage.
    > build -i 0 -p 3 -l 8 -t 0.98 ../immdatabase bmp asf haarcascade_frontalface_alt2.xml aamapi.amf

    # Build asmmodel from FRANCK-database which you can download from Cootes's homepage.
    > build -i 1 -p 4 -l 8 ../franck jpg pts haarcascade_frontalface_alt2.xml franck.amf		

 Notice: the file format of shape data are borrowed from Cootes and Stagmman, and you can generate your 
 pts file by using Cootes's am_tools and asf file by using Stagmman's aam-api.
 Notice: Linux library doesn't support building asm models.

2. Fit using active shape models

   Usage: fit -m model_file -h cascade_file {-i image_file | -v video_file | -c camara_idx} -n n_iteration

   Examples:
   # Image alignment on an image using 30 iterations
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -i aa.jpg -n 30

   # Face tracking on a video file
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -v bb.avi -n 25

   # Face tracking on live camara
   > fit -m my.amf -h haarcascade_frontalface_alt2.xml -c 0

====================
Citing asmlibrary
====================

Please cite the following or equivalent reference in any publicly available text that uses asmlibrary: 

@article{
    author={Yao Wei}, 
    title={Research on Facial Expression Recognition and Synthesis}, 
    journal={Master Thesis, Department of Computer Science and Technology, Nanjing University}, 
    year={Feb 2009}, 
    note={\url{http://code.google.com/p/asmlibrary}} 
} 

====================
Question
====================

When posting your question, you should read the API carefully, then check if your problem has been discussed in previous topics. Anyway, you are welcome to enjoy this compact face alignment/tracking library, and I will be appreciated if you report bugs to me.
